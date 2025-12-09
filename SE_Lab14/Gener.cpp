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

	void Head(std::ofstream* stream)
	{
		*stream << ".586\n";
		*stream << ".model flat, stdcall\n";
		*stream << "option casemap :none\n";

		*stream << "\n; --- Libs ---\n";
		*stream << "includelib kernel32.lib\n";
		*stream << "includelib libucrt.lib\n";

		*stream << "\n; --- WinAPI ---\n";
		*stream << "ExitProcess PROTO :DWORD\n";
		*stream << "GetStdHandle PROTO :DWORD\n";
		*stream << "WriteConsoleA PROTO :DWORD, :DWORD, :DWORD, :DWORD, :DWORD\n";

		*stream << "\n; --- User Lib ---\n";
		*stream << "str_len PROTO :DWORD\n";
		*stream << "tostr PROTO :DWORD\n";
		*stream << "touint PROTO :DWORD\n";
		*stream << "date PROTO\n";
		*stream << "get_time PROTO\n";
		*stream << "sub_str PROTO :DWORD, :DWORD, :DWORD\n";

		*stream << "\n.stack 4096\n";
	}

	void Data(std::ofstream* stream, IT::IdTable& id)
	{
		*stream << "\n.data\n";
		*stream << "\tconsole_handle dd 0\n";
		*stream << "\tnewline db 13, 10, 0\n";
		*stream << "\tbuffer db 256 dup(0)\n";

		for (int i = 0; i < id.size; i++)
		{
			if (id.table[i].idtype == IT::F || id.table[i].idtype == IT::P)
				continue;

			if (id.table[i].iddatatype == IT::STR)
			{
				std::string val = id.table[i].value.vstr.str;
				std::replace(val.begin(), val.end(), '\"', '\'');

				*stream << "\t" << id.table[i].id << "_s db \"" << val << "\", 0\n";
				*stream << "\t" << id.table[i].id << " dd " << id.table[i].id << "_s\n";
			}
			else  
			{
				*stream << "\t" << id.table[i].id << " dd " << id.table[i].value.vint << "\n";
			}
		}
	}

	void Code(std::ofstream* stream, CodeGen::ByteCode& code, IT::IdTable& id)
	{
		*stream << "\n.code\n";

		*stream << "\nprint_str PROC uses eax ebx ecx edx, pstr:DWORD\n";
		*stream << "\tpush -11\n";
		*stream << "\tcall GetStdHandle\n";
		*stream << "\tmov console_handle, eax\n";
		*stream << "\tmov edx, pstr\n";
		*stream << "\txor ecx, ecx\n";
		*stream << "calc_len:\n";
		*stream << "\tcmp byte ptr [edx+ecx], 0\n";
		*stream << "\tje print_now\n";
		*stream << "\tinc ecx\n";
		*stream << "\tjmp calc_len\n";
		*stream << "print_now:\n";
		*stream << "\tpush 0\n";
		*stream << "\tpush 0\n";
		*stream << "\tpush ecx\n";
		*stream << "\tpush pstr\n";
		*stream << "\tpush console_handle\n";
		*stream << "\tcall WriteConsoleA\n";
		*stream << "\tret\n";
		*stream << "print_str ENDP\n";

		*stream << "\noutnum PROC uses eax ebx ecx edx, number:DWORD\n";
		*stream << "\tmov eax, number\n";
		*stream << "\tmov ecx, 0\n";
		*stream << "\tmov ebx, 10\n";          
		*stream << "div_loop:\n";
		*stream << "\txor edx, edx\n";
		*stream << "\tdiv ebx\n";
		*stream << "\tadd dl, '0'\n";
		*stream << "\tpush edx\n";
		*stream << "\tinc ecx\n";
		*stream << "\ttest eax, eax\n";
		*stream << "\tjnz div_loop\n";
		*stream << "print_loop:\n";
		*stream << "\tpop eax\n";
		*stream << "\tmov buffer[0], al\n";
		*stream << "\tpush ecx\n";
		*stream << "\tinvoke print_str, addr buffer\n";
		*stream << "\tpop ecx\n";
		*stream << "\tloop print_loop\n";
		*stream << "\tret\n";
		*stream << "outnum ENDP\n";

		*stream << "\nprint_newline PROC\n";
		*stream << "\tinvoke print_str, addr newline\n";
		*stream << "\tret\n";
		*stream << "print_newline ENDP\n";

		*stream << "\nmain PROC\n";

		for (size_t i = 0; i < code.size(); i++)
		{
			CodeGen::Instruction& instr = code[i];
			*stream << "M" << i << ": ";

			switch (instr.op)
			{
			case CodeGen::CMD_PUSH:
			{
				IT::Entry& e = id.table[instr.target];

				if (e.idtype == IT::L)
				{
					if (e.iddatatype == IT::INT) {
						*stream << "push " << e.value.vint << "\n";
						stack_state.push(ITEM_INT_VAL);
					}
					else {
						*stream << "push " << e.id << "\n";
						stack_state.push(ITEM_STR_VAL);
					}
				}
				else
				{
					*stream << "push offset " << e.id << "\n";
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
					*stream << "mov " << id.table[instr.target].id << ", eax\n";
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

				if (type == ITEM_STR_VAL || type == ITEM_STR_ADDR)
				{
					if (type == ITEM_STR_ADDR) *stream << "mov eax, [eax]\n";
					*stream << "invoke print_str, eax\n";
				}
				else
				{
					if (type == ITEM_INT_ADDR) *stream << "mov eax, [eax]\n";
					*stream << "invoke outnum, eax\n";
				}
				*stream << "invoke print_newline\n";
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
				*stream << "call M" << instr.target << "\n";
				stack_state.push(ITEM_INT_VAL);
				break;

			case CodeGen::CMD_CALL_LIB:
			{
				IT::Entry& func = id.table[instr.target];
				std::string fname = func.id;

				int args_count = 0;
				if (fname == "time") { fname = "get_time"; args_count = 0; }
				else if (fname == "date") { args_count = 0; }
				else if (fname == "length") { fname = "str_len"; args_count = 1; }
				else if (fname == "tostr") { args_count = 1; }
				else if (fname == "touint") { args_count = 1; }
				else if (fname == "substr") { fname = "sub_str"; args_count = 3; }

				if (args_count == 1) {
					StackItemType t = stack_state.top(); stack_state.pop();
					*stream << "pop eax\n";
					if (t == ITEM_INT_ADDR || t == ITEM_STR_ADDR) *stream << "mov eax, [eax]\n";
					*stream << "push eax\n";
				}
				else if (args_count == 3) {
					StackItemType t3 = stack_state.top(); stack_state.pop();  
					StackItemType t2 = stack_state.top(); stack_state.pop();  
					StackItemType t1 = stack_state.top(); stack_state.pop();  

					*stream << "pop ecx\n";  
					if (t3 == ITEM_INT_ADDR) *stream << "mov ecx, [ecx]\n";

					*stream << "pop ebx\n";  
					if (t2 == ITEM_INT_ADDR) *stream << "mov ebx, [ebx]\n";

					*stream << "pop eax\n";  
					if (t1 == ITEM_STR_ADDR) *stream << "mov eax, [eax]\n";

					*stream << "push ecx\n";  
					*stream << "push ebx\n";  
					*stream << "push eax\n";  
				}

				*stream << "call " << fname << "\n";
				*stream << "push eax\n";

				if (func.iddatatype == IT::STR) stack_state.push(ITEM_STR_VAL);
				else stack_state.push(ITEM_INT_VAL);
				break;
			}

			case CodeGen::CMD_RET:
				*stream << "push 0\ncall ExitProcess\n";
				break;
			}

			*stream << "\n";
		}

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