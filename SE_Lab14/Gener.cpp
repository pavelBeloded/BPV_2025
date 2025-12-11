#include "stdafx.h"
#include "Gener.h"
#include <fstream>
#include <stack>
#include <string>
#include <algorithm>

namespace Gener
{
	enum StackItemType {
		ITEM_INT_VAL,
		ITEM_INT_ADDR,
		ITEM_STR_VAL,
		ITEM_STR_ADDR
	};

	std::stack<StackItemType> stack_state;

	std::string SafeName(const char* name) {
		std::string s = name;
		std::replace(s.begin(), s.end(), '$', '_');
		return s;
	}

	void Head(std::ofstream* stream)
	{
		*stream << ".586\n";
		*stream << ".model flat, stdcall\n";
		*stream << "option casemap :none\n";

		*stream << "\n; --- Libs ---\n";
		*stream << "includelib msvcrt.lib\n";
		*stream << "includelib legacy_stdio_definitions.lib\n";

		*stream << "\n; --- Externs ---\n";
		*stream << "ExitProcess PROTO :DWORD\n";
		*stream << "printf PROTO C :DWORD, :VARARG\n";
		*stream << "strlen PROTO C :DWORD\n";
		*stream << "sprintf PROTO C :DWORD, :DWORD, :VARARG\n";

		// Прототипы твоих функций из User Lib (должны быть реализованы где-то или заглушены)
		// Если они в .lib, то всё ок. Если нет - линкер упадет.
		// Предполагаем, что они есть (раз ASM собрался).
		*stream << "touint PROTO :DWORD\n";
		*stream << "sub_str PROTO :DWORD, :DWORD, :DWORD\n";

		*stream << "\n.stack 4096\n";
	}

	void Data(std::ofstream* stream, IT::IdTable& id)
	{
		*stream << "\n.data\n";

		*stream << "\tfmt_num db \"%d\", 0\n";
		*stream << "\tfmt_num_nl db \"%d\", 10, 0\n";
		*stream << "\tfmt_str_nl db \"%s\", 10, 0\n";
		*stream << "\tmsg_start db \"[ASM] Program Started\", 10, 0\n";

		*stream << "\tcommon_buf db 256 dup(0)\n";
		*stream << "\tdate_buf db \"12.12.2025\", 0\n";

		*stream << "\tret_stack dd 1024 dup(?)\n";
		*stream << "\tret_ptr dd 0\n";

		for (int i = 0; i < id.size; i++)
		{
			if (id.table[i].idtype == IT::F) continue;

			std::string name = SafeName(id.table[i].id);

			if (id.table[i].iddatatype == IT::STR)
			{
				std::string val = id.table[i].value.vstr.str;
				std::replace(val.begin(), val.end(), '\"', '\'');

				if (val.empty())
					*stream << "\t" << name << "_s db 0\n";
				else
					*stream << "\t" << name << "_s db \"" << val << "\", 0\n";

				*stream << "\t" << name << " dd offset " << name << "_s\n";
			}
			else
			{
				*stream << "\t" << name << " dd " << id.table[i].value.vint << "\n";
			}
		}
	}

	void Code(std::ofstream* stream, CodeGen::ByteCode& code, IT::IdTable& id)
	{
		*stream << "\n.code\n";
		*stream << "main PROC\n";
		*stream << "\tinvoke printf, offset msg_start\n";

		for (size_t i = 0; i < code.size(); i++)
		{
			CodeGen::Instruction& instr = code[i];
			*stream << "M" << i << ": ";

			switch (instr.op)
			{
			case CodeGen::CMD_PUSH:
			{
				IT::Entry& e = id.table[instr.target];
				if (e.idtype == IT::L) {
					if (e.iddatatype == IT::INT) {
						*stream << "push " << e.value.vint << "\n";
						stack_state.push(ITEM_INT_VAL);
					}
					else {
						*stream << "push offset " << SafeName(e.id) << "_s\n";
						stack_state.push(ITEM_STR_VAL);
					}
				}
				else {
					*stream << "push offset " << SafeName(e.id) << "\n";
					if (e.iddatatype == IT::INT) stack_state.push(ITEM_INT_ADDR);
					else stack_state.push(ITEM_STR_ADDR);
				}
				break;
			}

			case CodeGen::CMD_POP:
			{
				if (instr.target == LT_TI_NULLIDX)
				{
					StackItemType r_type = stack_state.top(); stack_state.pop();
					*stream << "pop eax\n";
					if (r_type == ITEM_INT_ADDR || r_type == ITEM_STR_ADDR) *stream << "mov eax, [eax]\n";

					StackItemType l_type = stack_state.top(); stack_state.pop();
					*stream << "pop ebx\n";
					*stream << "mov [ebx], eax\n";

					*stream << "push eax\n";
					if (r_type == ITEM_STR_VAL || r_type == ITEM_STR_ADDR) stack_state.push(ITEM_STR_VAL);
					else stack_state.push(ITEM_INT_VAL);
				}
				else
				{
					*stream << "pop eax\n";
					*stream << "mov " << SafeName(id.table[instr.target].id) << ", eax\n";
				}
				break;
			}

			case CodeGen::CMD_ADD:
			case CodeGen::CMD_SUB:
			case CodeGen::CMD_MUL:
			case CodeGen::CMD_DIV:
			case CodeGen::CMD_MOD:
			{
				StackItemType r_type = stack_state.top(); stack_state.pop();
				*stream << "pop ebx\n";
				if (r_type == ITEM_INT_ADDR) *stream << "mov ebx, [ebx]\n";

				StackItemType l_type = stack_state.top(); stack_state.pop();
				*stream << "pop eax\n";
				if (l_type == ITEM_INT_ADDR) *stream << "mov eax, [eax]\n";

				if (instr.op == CodeGen::CMD_ADD) *stream << "add eax, ebx\n";
				else if (instr.op == CodeGen::CMD_SUB) *stream << "sub eax, ebx\n";
				else if (instr.op == CodeGen::CMD_MUL) *stream << "imul eax, ebx\n";
				else if (instr.op == CodeGen::CMD_DIV) { *stream << "cdq\n"; *stream << "idiv ebx\n"; }
				else if (instr.op == CodeGen::CMD_MOD) { *stream << "cdq\n"; *stream << "idiv ebx\n"; *stream << "mov eax, edx\n"; }

				*stream << "push eax\n";
				stack_state.push(ITEM_INT_VAL);
				break;
			}

			case CodeGen::CMD_CMPE:
			case CodeGen::CMD_CMPNE:
			case CodeGen::CMD_CMPG:
			case CodeGen::CMD_CMPL:
			{
				StackItemType r_type = stack_state.top(); stack_state.pop();
				*stream << "pop ebx\n";
				if (r_type == ITEM_INT_ADDR) *stream << "mov ebx, [ebx]\n";

				StackItemType l_type = stack_state.top(); stack_state.pop();
				*stream << "pop eax\n";
				if (l_type == ITEM_INT_ADDR) *stream << "mov eax, [eax]\n";

				*stream << "cmp eax, ebx\n";
				*stream << "mov eax, 0\n";

				if (instr.op == CodeGen::CMD_CMPE) *stream << "sete al\n";
				else if (instr.op == CodeGen::CMD_CMPNE) *stream << "setne al\n";
				else if (instr.op == CodeGen::CMD_CMPG) *stream << "setg al\n";
				else if (instr.op == CodeGen::CMD_CMPL) *stream << "setl al\n";

				*stream << "push eax\n";
				stack_state.push(ITEM_INT_VAL);
				break;
			}

			case CodeGen::CMD_PRINT:
			{
				StackItemType type = stack_state.top(); stack_state.pop();
				*stream << "pop eax\n";

				if (type == ITEM_STR_VAL || type == ITEM_STR_ADDR) {
					if (type == ITEM_STR_ADDR) *stream << "mov eax, [eax]\n";
					*stream << "invoke printf, offset fmt_str_nl, eax\n";
				}
				else {
					if (type == ITEM_INT_ADDR) *stream << "mov eax, [eax]\n";
					*stream << "invoke printf, offset fmt_num_nl, eax\n";
				}
				break;
			}

			case CodeGen::CMD_JMP:
				*stream << "jmp M" << instr.target << "\n";
				break;

			case CodeGen::CMD_JZ:
			{
				StackItemType type = stack_state.top(); stack_state.pop();
				*stream << "pop eax\n";
				if (type == ITEM_INT_ADDR) *stream << "mov eax, [eax]\n";
				*stream << "cmp eax, 0\n";
				*stream << "je M" << instr.target << "\n";
				break;
			}

			case CodeGen::CMD_CALL:
				*stream << "mov ebx, ret_ptr\n";
				*stream << "mov dword ptr [ret_stack + ebx*4], offset Ret_M" << i << "\n";
				*stream << "inc ret_ptr\n";
				*stream << "jmp M" << instr.target << "\n";
				*stream << "Ret_M" << i << ":\n";
				*stream << "push eax\n";
				stack_state.push(ITEM_INT_VAL);
				break;

			case CodeGen::CMD_CALL_LIB:
			{
				IT::Entry& func = id.table[instr.target];
				std::string fname = func.id;

				if (fname == "length") {
					StackItemType t = stack_state.top(); stack_state.pop();
					*stream << "pop eax\n";
					if (t == ITEM_STR_ADDR) *stream << "mov eax, [eax]\n";

					*stream << "invoke strlen, eax\n";
					*stream << "push eax\n";
					stack_state.push(ITEM_INT_VAL);
				}
				else if (fname == "tostr") {
					StackItemType t = stack_state.top(); stack_state.pop();
					*stream << "pop eax\n";
					if (t == ITEM_INT_ADDR) *stream << "mov eax, [eax]\n";

					*stream << "invoke sprintf, offset common_buf, offset fmt_num, eax\n";
					*stream << "push offset common_buf\n";
					stack_state.push(ITEM_STR_VAL);
				}
				else if (fname == "date") {
					*stream << "push offset date_buf\n";
					stack_state.push(ITEM_STR_VAL);
				}
				// --- ИСПРАВЛЕНИЕ: ДОБАВЛЕН SUBSTR ---
				else if (fname == "substr") {
					// Стек: STR, START, LEN (TOP)
					// Pop Len
					StackItemType t3 = stack_state.top(); stack_state.pop();
					*stream << "pop ecx\n";
					if (t3 == ITEM_INT_ADDR) *stream << "mov ecx, [ecx]\n";

					// Pop Start
					StackItemType t2 = stack_state.top(); stack_state.pop();
					*stream << "pop ebx\n";
					if (t2 == ITEM_INT_ADDR) *stream << "mov ebx, [ebx]\n";

					// Pop Str
					StackItemType t1 = stack_state.top(); stack_state.pop();
					*stream << "pop eax\n";
					if (t1 == ITEM_STR_ADDR) *stream << "mov eax, [eax]\n";

					// invoke sub_str, str, start, len
					*stream << "invoke sub_str, eax, ebx, ecx\n";
					*stream << "push eax\n";
					stack_state.push(ITEM_STR_VAL);
				}
				// --- ИСПРАВЛЕНИЕ: ДОБАВЛЕН TOUINT ---
				else if (fname == "touint") {
					StackItemType t = stack_state.top(); stack_state.pop();
					*stream << "pop eax\n";
					if (t == ITEM_STR_ADDR) *stream << "mov eax, [eax]\n";

					*stream << "invoke touint, eax\n";
					*stream << "push eax\n";
					stack_state.push(ITEM_INT_VAL);
				}
				else {
					*stream << "push 0\n";
					stack_state.push(ITEM_INT_VAL);
				}
				break;
			}

			case CodeGen::CMD_RET:
				*stream << "cmp ret_ptr, 0\n";
				*stream << "je quit_program\n";
				*stream << "dec ret_ptr\n";
				*stream << "mov ebx, ret_ptr\n";
				*stream << "mov eax, [ret_stack + ebx*4]\n";
				*stream << "jmp eax\n";
				break;
			}

			*stream << "\n";
		}

		*stream << "quit_program:\n";
		*stream << "invoke ExitProcess, 0\n";
		*stream << "main ENDP\n";
		*stream << "end main\n";
	}

	void CodeGeneration(CodeGen::ByteCode& bytecode, IT::IdTable& idtable, const wchar_t* out_file)
	{
		std::ofstream stream(out_file);
		if (!stream.is_open()) return;
		Head(&stream);
		Data(&stream, idtable);
		Code(&stream, bytecode, idtable);
		stream.close();
	}
}