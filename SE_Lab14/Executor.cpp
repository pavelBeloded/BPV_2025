#include "stdafx.h"
#include "Executor.h"
#include "Error.h"
#include <stack>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>

// Внешние функции C++ для эмуляции библиотеки
extern "C" {
	int   __stdcall str_len(char* s);
	char* __stdcall tostr(int n);
	unsigned int __stdcall touint(char* s); // <-- unsigned
	char* __stdcall date();
	char* __stdcall get_time();
	char* __stdcall sub_str(char* s, int start, int len);
	void  __stdcall pause_prog();
}

namespace Executor {

	struct Value {
		IT::IDDATATYPE type;
		union {
			unsigned int i_val; // <--- UNSIGNED
			char s_val[255];
		};
		int var_idx = -1;          
	};

	struct StackFrame {
		size_t returnIP;
		size_t stackSize;
	};

	std::string DumpStack(const std::vector<Value>& debugStack, IT::IdTable& idtable) {
		std::stringstream ss;
		ss << "   Stack [" << debugStack.size() << "]: ";
		for (const auto& v : debugStack) {
			if (v.var_idx != -1) {
				ss << "{" << IT::GetEntry(idtable, v.var_idx).id << "} ";
			}
			else {
				if (v.type == IT::INT) ss << v.i_val << " "; // Выведет корректное unsigned число
				else ss << "'" << v.s_val << "' ";
			}
		}
		return ss.str();
	}

	void Execute(CodeGen::ByteCode& code, IT::IdTable& idtable, Log::LOG log) {
		std::vector<Value> stack;
		std::stack<StackFrame> callStack;
		size_t ip = 0;

		Log::WriteLine(log, "\n--- EXECUTOR START ---", nullptr);

		while (ip < code.size()) {
			CodeGen::Instruction& instr = code[ip];

            // ... (Логирование команд оставляем как было, оно не зависит от типа) ... 
            // Для краткости я его пропущу, вставь свой старый блок логирования

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
				stack.push_back(v);
				break;
			}

			case CodeGen::CMD_POP: {
				if (instr.target != LT_TI_NULLIDX) {
					if (stack.empty()) throw ERROR_THROW(701);
					Value val = stack.back(); stack.pop_back();
					IT::Entry& varEntry = idtable.table[instr.target];

					if (varEntry.iddatatype == IT::INT) varEntry.value.vint = val.i_val;
					else {
						varEntry.value.vstr.len = strlen(val.s_val);
						strcpy_s(varEntry.value.vstr.str, val.s_val);
					}
				}
				else {
					if (stack.size() < 2) throw ERROR_THROW(701);
					Value val = stack.back(); stack.pop_back();
					Value target = stack.back(); stack.pop_back();

					if (target.var_idx != -1) {
						IT::Entry& varEntry = idtable.table[target.var_idx];
						if (varEntry.iddatatype == IT::INT) {
							varEntry.value.vint = val.i_val;
						}
						else {
							varEntry.value.vstr.len = strlen(val.s_val);
							strcpy_s(varEntry.value.vstr.str, val.s_val);
						}
					}
				}
				break;
			}

			case CodeGen::CMD_ADD: {
				Value b = stack.back(); stack.pop_back();
				Value a = stack.back(); stack.pop_back();
				Value res; res.type = a.type; res.var_idx = -1;
				if (a.type == IT::INT) res.i_val = a.i_val + b.i_val; // Unsigned overflow is safe in C++
				else { strcpy_s(res.s_val, a.s_val); strcat_s(res.s_val, b.s_val); }
				stack.push_back(res);
				break;
			}

			case CodeGen::CMD_SUB: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				Value res; res.type = IT::INT; res.var_idx = -1; 
                // Unsigned underflow: 1 - 2 -> 4294967295
                res.i_val = a.i_val - b.i_val;
				stack.push_back(res); break;
			}
			case CodeGen::CMD_MUL: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				Value res; res.type = IT::INT; res.var_idx = -1; 
                res.i_val = a.i_val * b.i_val;
				stack.push_back(res); break;
			}
			case CodeGen::CMD_DIV: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				if (b.i_val == 0) throw ERROR_THROW(1);         
				Value res; res.type = IT::INT; res.var_idx = -1; 
                res.i_val = a.i_val / b.i_val; // Unsigned division
				stack.push_back(res); break;
			}
			case CodeGen::CMD_MOD: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				if (b.i_val == 0) throw ERROR_THROW(1);
				Value res; res.type = IT::INT; res.var_idx = -1; 
                res.i_val = a.i_val % b.i_val;
				stack.push_back(res); break;
			}

            // Сравнения для unsigned работают корректно сами по себе
			case CodeGen::CMD_CMPE: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				Value res; res.type = IT::INT; res.var_idx = -1;
				if (a.type == IT::INT) res.i_val = (a.i_val == b.i_val); else res.i_val = (strcmp(a.s_val, b.s_val) == 0);
				stack.push_back(res); break;
			}
			case CodeGen::CMD_CMPNE: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				Value res; res.type = IT::INT; res.var_idx = -1;
				if (a.type == IT::INT) res.i_val = (a.i_val != b.i_val); else res.i_val = (strcmp(a.s_val, b.s_val) != 0);
				stack.push_back(res); break;
			}
			case CodeGen::CMD_CMPL: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				Value res; res.type = IT::INT; res.var_idx = -1; 
                res.i_val = (a.i_val < b.i_val);
				stack.push_back(res); break;
			}
			case CodeGen::CMD_CMPG: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				Value res; res.type = IT::INT; res.var_idx = -1; 
                res.i_val = (a.i_val > b.i_val);
				stack.push_back(res); break;
			}
			case CodeGen::CMD_CMPLE: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				Value res; res.type = IT::INT; res.var_idx = -1; 
                res.i_val = (a.i_val <= b.i_val);
				stack.push_back(res); break;
			}
			case CodeGen::CMD_CMPGE: {
				Value b = stack.back(); stack.pop_back(); Value a = stack.back(); stack.pop_back();
				Value res; res.type = IT::INT; res.var_idx = -1; 
                res.i_val = (a.i_val >= b.i_val);
				stack.push_back(res); break;
			}

			case CodeGen::CMD_PRINT: {
				if (stack.empty()) throw ERROR_THROW(701);
				Value v = stack.back(); stack.pop_back();
				if (v.type == IT::INT) std::cout << v.i_val << std::endl; // cout выведет unsigned число
				else std::cout << v.s_val << std::endl;
				break;
			}

			case CodeGen::CMD_JMP: { ip = instr.target; continue; }
			case CodeGen::CMD_JZ: {
				if (stack.empty()) throw ERROR_THROW(701);
				Value v = stack.back(); stack.pop_back();
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

				if (hasRet) {
					retVal = stack.back();
					stack.pop_back();
					retVal.var_idx = -1;     
				}

				if (callStack.empty()) {
					return;
				}

				StackFrame frame = callStack.top();
				callStack.pop();
				ip = frame.returnIP;

				while (stack.size() > frame.stackSize) {
					stack.pop_back();
				}

				if (hasRet) {
					stack.push_back(retVal);
				}
				continue;
			}

			case CodeGen::CMD_CALL_LIB: {
				IT::Entry libFunc = IT::GetEntry(idtable, (int)instr.target);
				std::string name = libFunc.id;
				Value res; res.var_idx = -1;

				if (name == "date") { char* d = date(); strcpy_s(res.s_val, d); res.type = IT::STR; }
				else if (name == "time") { char* t = get_time(); strcpy_s(res.s_val, t); res.type = IT::STR; }
				else if (name == "length") { Value arg = stack.back(); stack.pop_back(); res.i_val = str_len(arg.s_val); res.type = IT::INT; }
				else if (name == "tostr") { Value arg = stack.back(); stack.pop_back(); char* s = tostr(arg.i_val); strcpy_s(res.s_val, s); res.type = IT::STR; }
				else if (name == "touint") { Value arg = stack.back(); stack.pop_back(); res.i_val = touint(arg.s_val); res.type = IT::INT; }
				else if (name == "substr") { Value l = stack.back(); stack.pop_back(); Value s = stack.back(); stack.pop_back(); Value str = stack.back(); stack.pop_back(); char* resS = sub_str(str.s_val, s.i_val, l.i_val); strcpy_s(res.s_val, resS); res.type = IT::STR; }

				stack.push_back(res);
				break;
			}
			}
			ip++;
		}

		Log::WriteLine(log, "--- EXECUTOR FINISHED ---\n", nullptr);
	}
}