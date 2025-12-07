#include "stdafx.h"
#include "Executor.h"
#include "Error.h"
#include <stack>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

extern "C" {
	int   __stdcall length(char* s);
	char* __stdcall tostr(int n);
	int   __stdcall touint(char* s);
	char* __stdcall date();
	char* __stdcall get_time();
	char* __stdcall substr(char* s, int start, int len);
}

namespace Executor {

	struct Value {
		IT::IDDATATYPE type;
		union {
			int i_val;
			char s_val[255];
		};
		int var_idx = -1;
	};

	struct StackFrame {
		size_t returnIP;
		size_t stackSize;
	};

	void Execute(CodeGen::ByteCode& code, IT::IdTable& idtable) {
		std::stack<Value> stack;
		std::stack<StackFrame> callStack;
		size_t ip = 0;

		// std::cout << "\n--- EXECUTOR START ---\n"; // Убираем шум

		while (ip < code.size()) {
			CodeGen::Instruction& instr = code[ip];

			switch (instr.op) {

			case CodeGen::CMD_PUSH: {
				IT::Entry e = IT::GetEntry(idtable, (int)instr.target);
				Value v;
				v.type = e.iddatatype;
				v.var_idx = (int)instr.target;

				if (e.idtype == IT::L) {
					if (e.iddatatype == IT::INT) v.i_val = e.value.vint;
					else strcpy_s(v.s_val, e.value.vstr.str);
					v.var_idx = -1;
				}
				else {
					if (e.iddatatype == IT::INT) v.i_val = e.value.vint;
					else strcpy_s(v.s_val, e.value.vstr.str);
				}
				stack.push(v);
				break;
			}

			case CodeGen::CMD_POP: {
				if (instr.target != LT_TI_NULLIDX) {
					if (stack.empty()) throw ERROR_THROW(701);
					Value val = stack.top(); stack.pop();
					IT::Entry& varEntry = idtable.table[instr.target];
					if (varEntry.iddatatype == IT::INT) varEntry.value.vint = val.i_val;
					else { varEntry.value.vstr.len = strlen(val.s_val); strcpy_s(varEntry.value.vstr.str, val.s_val); }
				}
				else {
					if (stack.size() < 2) throw ERROR_THROW(701);
					Value val = stack.top(); stack.pop();
					Value target = stack.top(); stack.pop();

					if (target.var_idx == -1) return; // Ошибка, но молчим или кидаем throw

					IT::Entry& varEntry = idtable.table[target.var_idx];

					// УБРАЛИ DEBUG ВЫВОД ЗДЕСЬ
					if (varEntry.iddatatype == IT::INT) varEntry.value.vint = val.i_val;
					else {
						varEntry.value.vstr.len = strlen(val.s_val);
						strcpy_s(varEntry.value.vstr.str, val.s_val);
					}
				}
				break;
			}

			case CodeGen::CMD_ADD: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = a.type; res.var_idx = -1;
				if (a.type == IT::INT) res.i_val = a.i_val + b.i_val;
				else { strcpy_s(res.s_val, a.s_val); strcat_s(res.s_val, b.s_val); }
				stack.push(res); break;
			}
			case CodeGen::CMD_SUB: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = IT::INT; res.var_idx = -1; res.i_val = a.i_val - b.i_val;
				stack.push(res); break;
			}
			case CodeGen::CMD_MUL: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = IT::INT; res.var_idx = -1; res.i_val = a.i_val * b.i_val;
				stack.push(res); break;
			}
			case CodeGen::CMD_DIV: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				if (b.i_val == 0) throw ERROR_THROW(1); // Деление на 0
				Value res; res.type = IT::INT; res.var_idx = -1; res.i_val = a.i_val / b.i_val;
				stack.push(res); break;
			}
			case CodeGen::CMD_MOD: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				if (b.i_val == 0) throw ERROR_THROW(1);
				Value res; res.type = IT::INT; res.var_idx = -1; res.i_val = a.i_val % b.i_val;
				stack.push(res); break;
			}
			case CodeGen::CMD_CMPE: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = IT::INT; res.var_idx = -1;
				if (a.type == IT::INT) res.i_val = (a.i_val == b.i_val); else res.i_val = (strcmp(a.s_val, b.s_val) == 0);
				stack.push(res); break;
			}
			case CodeGen::CMD_CMPNE: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = IT::INT; res.var_idx = -1;
				if (a.type == IT::INT) res.i_val = (a.i_val != b.i_val); else res.i_val = (strcmp(a.s_val, b.s_val) != 0);
				stack.push(res); break;
			}
			case CodeGen::CMD_CMPL: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = IT::INT; res.var_idx = -1; res.i_val = (a.i_val < b.i_val);
				stack.push(res); break;
			}
			case CodeGen::CMD_CMPG: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = IT::INT; res.var_idx = -1; res.i_val = (a.i_val > b.i_val);
				stack.push(res); break;
			}
			case CodeGen::CMD_CMPLE: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = IT::INT; res.var_idx = -1; res.i_val = (a.i_val <= b.i_val);
				stack.push(res); break;
			}
			case CodeGen::CMD_CMPGE: {
				Value b = stack.top(); stack.pop(); Value a = stack.top(); stack.pop();
				Value res; res.type = IT::INT; res.var_idx = -1; res.i_val = (a.i_val >= b.i_val);
				stack.push(res); break;
			}

								   // --- ВОТ ЭТО ОСТАВЛЯЕМ (ВЫВОД ПРОГРАММЫ) ---
			case CodeGen::CMD_PRINT: {
				if (stack.empty()) throw ERROR_THROW(701);
				Value v = stack.top(); stack.pop();
				if (v.type == IT::INT) std::cout << v.i_val << std::endl;
				else std::cout << v.s_val << std::endl;
				break;
			}
								   // -------------------------------------------

			case CodeGen::CMD_JMP: { ip = instr.target; continue; }
			case CodeGen::CMD_JZ: {
				if (stack.empty()) throw ERROR_THROW(701);
				Value v = stack.top(); stack.pop();
				if (v.i_val == 0) { ip = instr.target; continue; }
				break;
			}
			case CodeGen::CMD_CALL: {
				callStack.push({ ip + 1, stack.size() });
				ip = instr.target;
				continue;
			}
			case CodeGen::CMD_RET: {
				Value retVal;
				bool hasRet = !stack.empty();
				if (hasRet) retVal = stack.top();

				if (callStack.empty()) return; // Выход из main

				StackFrame frame = callStack.top();
				callStack.pop();
				ip = frame.returnIP;

				// Очистка стека (если нужно)
				// while (stack.size() > frame.stackSize) stack.pop(); // Раскомментируй, если хочешь жесткой очистки

				if (hasRet) stack.push(retVal);
				continue;
			}
			case CodeGen::CMD_CALL_LIB: {
				IT::Entry libFunc = IT::GetEntry(idtable, (int)instr.target);
				std::string name = libFunc.id;
				Value res; res.var_idx = -1;
				if (name == "date") { char* d = date(); strcpy_s(res.s_val, d); res.type = IT::STR; }
				else if (name == "time") { char* t = get_time(); strcpy_s(res.s_val, t); res.type = IT::STR; }
				else if (name == "length") { Value arg = stack.top(); stack.pop(); res.i_val = length(arg.s_val); res.type = IT::INT; }
				else if (name == "tostr") { Value arg = stack.top(); stack.pop(); char* s = tostr(arg.i_val); strcpy_s(res.s_val, s); res.type = IT::STR; }
				else if (name == "touint") { Value arg = stack.top(); stack.pop(); res.i_val = touint(arg.s_val); res.type = IT::INT; }
				else if (name == "substr") { Value l = stack.top(); stack.pop(); Value s = stack.top(); stack.pop(); Value str = stack.top(); stack.pop(); char* resS = substr(str.s_val, s.i_val, l.i_val); strcpy_s(res.s_val, resS); res.type = IT::STR; }
				stack.push(res);
				break;
			}
			}
			ip++;
		}
	}
}