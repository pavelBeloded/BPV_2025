#include "stdafx.h"
#include "CodeGen.h"
#include "Error.h"
#include <stack>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>

namespace CodeGen {

	struct LoopScope {
		size_t conditionStart;
		size_t jzInstruction;
	};

	ByteCode Generate(LT::LexTable& lextable, IT::IdTable& idtable) {
		ByteCode code;
		std::stack<LoopScope> loops;
		std::map<int, size_t> functionMap;

		size_t entryPoint = 0;
		size_t currentStatementStart = 0;

		code.push_back({ CMD_JMP, 0 }); // Заглушка для прыжка на entry

		for (int i = 0; i < lextable.size; i++) {
			LT::Entry entry = lextable.table[i];
			size_t currentAddr = code.size();

			switch (entry.lexema) {

			case LEX_FUNCTION: {
				if (i + 1 < lextable.size && lextable.table[i + 1].lexema == LEX_ID) {
					int funcIdIdx = lextable.table[i + 1].idxTI;
					functionMap[funcIdIdx] = currentAddr;
					i++;

					IT::Entry funcEntry = IT::GetEntry(idtable, funcIdIdx);
					std::string prefix = std::string(funcEntry.id) + "$";
					std::vector<int> paramIndices;

					for (int k = 0; k < idtable.size; k++) {
						if (idtable.table[k].idtype == IT::P) {
							std::string pname = idtable.table[k].id;
							if (pname.find(prefix) == 0) {
								paramIndices.push_back(k);
							}
						}
					}

					// POP параметров в обратном порядке
					for (int k = (int)paramIndices.size() - 1; k >= 0; k--) {
						code.push_back({ CMD_POP, (size_t)paramIndices[k] });
					}

					// Пропускаем параметры до тела функции
					while (i + 1 < lextable.size) {
						if (lextable.table[i + 1].lexema == LEX_LEFTBRACE) break;
						i++;
					}
				}
				break;
			}

			case LEX_MAIN: {
				entryPoint = currentAddr;
				break;
			}

			case LEX_DECLARE: {
				bool hasInit = true;
				if (i + 3 < lextable.size) {
					if (lextable.table[i + 3].lexema == LEX_SEMICOLON) hasInit = false;
				}
				if (hasInit) i += 1;
				else i += 2;
				break;
			}

			case LEX_ID: {
				IT::Entry itEntry = IT::GetEntry(idtable, entry.idxTI);
				if (itEntry.idtype == IT::F) {
					code.push_back({ CMD_CALL, (size_t)entry.idxTI });
				}
				else {
					code.push_back({ CMD_PUSH, (size_t)entry.idxTI });
				}
				break;
			}

			case LEX_LITERAL: {
				code.push_back({ CMD_PUSH, (size_t)entry.idxTI });
				break;
			}

			case LEX_PLUS:	code.push_back({ CMD_ADD, 0 }); break;
			case LEX_MINUS: code.push_back({ CMD_SUB, 0 }); break;
			case LEX_STAR:	code.push_back({ CMD_MUL, 0 }); break;
			case LEX_DIRSLASH: code.push_back({ CMD_DIV, 0 }); break;
			case LEX_MODULO: code.push_back({ CMD_MOD, 0 }); break;

			case LEX_EQ:	code.push_back({ CMD_CMPE, 0 }); break;
			case LEX_NE:	code.push_back({ CMD_CMPNE, 0 }); break;
			case LEX_LESS:	code.push_back({ CMD_CMPL, 0 }); break;
			case LEX_MORE:	code.push_back({ CMD_CMPG, 0 }); break;
			case LEX_LE:	code.push_back({ CMD_CMPLE, 0 }); break;
			case LEX_GE:	code.push_back({ CMD_CMPGE, 0 }); break;

			case LEX_ASSIGN: {
				code.push_back({ CMD_POP, (size_t)LT_TI_NULLIDX });
				break;
			}

			case LEX_PRINT: {
				code.push_back({ CMD_PRINT, 0 });
				break;
			}
			case LEX_RETURN: {
				code.push_back({ CMD_RET, 0 });
				break;
			}

			case LEX_WHILE: {
				code.push_back({ CMD_JZ, 0 });
				loops.push({ currentStatementStart, code.size() - 1 });
				break;
			}

			case LEX_SEMICOLON:
			case LEX_LEFTBRACE: {
				currentStatementStart = code.size();
				break;
			}

			case LEX_BRACELET: {
				if (!loops.empty()) {
					LoopScope loop = loops.top();
					loops.pop();
					code.push_back({ CMD_JMP, loop.conditionStart });
					code[loop.jzInstruction].target = code.size();
				}
				currentStatementStart = code.size();
				break;
			}

			case LEX_TYPE: break;
			case '#': break;
			case LEX_COMMA: break;
			case LEX_LEFTHESIS: break;
			case LEX_RIGHTHESIS: break;

			default: break;
			}
		}

		if (entryPoint > 0) {
			code[0].target = entryPoint;
		}

		for (auto& instr : code) {
			if (instr.op == CMD_CALL) {
				int funcIdx = (int)instr.target;
				if (functionMap.count(funcIdx)) {
					instr.target = functionMap[funcIdx];
				}
				else {
					instr.op = CMD_CALL_LIB;
				}
			}
		}

		return code;
	}

	void Debug(ByteCode& bytecode, IT::IdTable& idtable, Log::LOG log) {
		std::stringstream ss;
		ss << "\n--- BYTECODE DUMP ---\n";
		for (size_t i = 0; i < bytecode.size(); i++) {
			ss << std::setw(3) << i << ": ";
			switch (bytecode[i].op) {
			case CMD_PUSH: ss << "PUSH " << IT::GetEntry(idtable, (int)bytecode[i].target).id; break;
			case CMD_POP:
				if (bytecode[i].target == LT_TI_NULLIDX) ss << "ASSIGN";
				else ss << "POP " << IT::GetEntry(idtable, (int)bytecode[i].target).id;
				break;
			case CMD_ADD: ss << "ADD"; break;
			case CMD_SUB: ss << "SUB"; break;
			case CMD_MUL: ss << "MUL"; break;
			case CMD_DIV: ss << "DIV"; break;
			case CMD_CMPE: ss << "CMPE"; break;
			case CMD_CMPL: ss << "CMPL"; break;
			case CMD_CMPG: ss << "CMPG"; break;
			case CMD_JMP: ss << "JMP " << bytecode[i].target; break;
			case CMD_JZ: ss << "JZ " << bytecode[i].target; break;
			case CMD_CALL: ss << "CALL " << bytecode[i].target; break;
			case CMD_RET: ss << "RET"; break;
			case CMD_PRINT: ss << "PRINT"; break;
			case CMD_CALL_LIB: ss << "CALL LIB " << IT::GetEntry(idtable, (int)bytecode[i].target).id; break;
			default: ss << "OP " << bytecode[i].op;
			}
			ss << "\n";
		}
		ss << "---------------------\n";
		Log::WriteLine(log, ss.str().c_str(), nullptr);
	}
}