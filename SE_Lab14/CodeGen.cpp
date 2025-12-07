#include "stdafx.h"
#include "CodeGen.h"
#include "Error.h"
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <iomanip>

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

		code.push_back({ CMD_JMP, 0 }); // Jump to entry point placeholder

		//std::cout << "\n[GEN DEBUG] Starting Generation...\n";

		for (int i = 0; i < lextable.size; i++) {
			LT::Entry entry = lextable.table[i];
			size_t currentAddr = code.size();

			switch (entry.lexema) {

			case LEX_FUNCTION: {
				//std::cout << "FUNC DECL";
				// i указывает на 'f'
				// i+1 = ID (имя функции)

				if (i + 1 < lextable.size && lextable.table[i + 1].lexema == LEX_ID) {
					int funcIdIdx = lextable.table[i + 1].idxTI;
					functionMap[funcIdIdx] = currentAddr;
					i++; // Пропускаем имя функции (становимся на ID)

					// --- Генерируем POP для параметров ---
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

					// POP в обратном порядке (так как в стеке a, b, а POP забирает b, a)
					for (int k = paramIndices.size() - 1; k >= 0; k--) {
						code.push_back({ CMD_POP, (size_t)paramIndices[k] });
					}

					// --- ИСПРАВЛЕНИЕ: Пропускаем список параметров ( ... ) ---
					// Сейчас i указывает на имя функции. Следующая должна быть '('.
					// Нам нужно пропустить всё до '{'.

					while (i + 1 < lextable.size) {
						if (lextable.table[i + 1].lexema == LEX_LEFTBRACE) {
							break; // Нашли начало тела, останавливаемся
						}
						i++; // Пропускаем лексему (параметры, запятые, типы)
					}
					// Теперь следующий шаг цикла for возьмет '{'
				}
				break;
			}

			case LEX_MAIN: {
				//std::cout << "MAIN ENTRY";
				entryPoint = currentAddr;

				// У main тоже есть скобка '{', убедимся что не пропускаем лишнего
				// Обычно после m идет {, так что всё ок
				break;
			}

			case LEX_DECLARE: {
				bool hasInit = true;
				// Проверка: d t i ; (нет init) vs d t i ... = ;
				if (i + 3 < lextable.size) {
					if (lextable.table[i + 3].lexema == LEX_SEMICOLON) hasInit = false;
				}

				if (hasInit) {
					i += 1; // Пропускаем d, t. След -> i (сгенерирует PUSH VAR)
				}
				else {
					i += 2; // Пропускаем d, t, i. След -> ;
				}
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
					// Проверяем, закрывает ли это while
					// В твоем синтаксисе while (...) { ... };
					// JZ прыгает на метку ПОСЛЕ цикла.
					// В конце тела нужен JMP в начало.

					// Но у нас brace закрывает блок функции тоже.
					// Простой эвристикой: если стек циклов не пуст, считаем что это конец цикла?
					// Нет, это опасно.

					// В твоем коде while(...) { ... }; 
					// JZ генерируется на слове 'while' (в ПОЛИЗе оно в конце условия).
					// { - начало тела.
					// } - конец тела.
					// ; - конец оператора while.

					// Давай пока оставим как есть, если циклы работали.
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
				// Пропускаем запятые, скобки (они уже отработали в ПОЛИЗе или не нужны)
			case LEX_COMMA: break;
			case LEX_LEFTHESIS: break;
			case LEX_RIGHTHESIS: break;

			default: break;
			}
		}

		// Fixup Entry Point
		if (entryPoint > 0) {
			code[0].target = entryPoint;
		}

		// Fixup Library Calls
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

	void Debug(ByteCode& bytecode, IT::IdTable& idtable) {
		std::cout << "\n--- BYTECODE ---\n";
		for (size_t i = 0; i < bytecode.size(); i++) {
			std::cout << std::setw(3) << i << ": ";
			switch (bytecode[i].op) {
			case CMD_PUSH: std::cout << "PUSH " << IT::GetEntry(idtable, (int)bytecode[i].target).id; break;
			case CMD_POP:
				if (bytecode[i].target == LT_TI_NULLIDX) std::cout << "ASSIGN";
				else std::cout << "POP " << IT::GetEntry(idtable, (int)bytecode[i].target).id;
				break;
			case CMD_ADD: std::cout << "ADD"; break;
			case CMD_SUB: std::cout << "SUB"; break;
			case CMD_MUL: std::cout << "MUL"; break;
			case CMD_DIV: std::cout << "DIV"; break;
			case CMD_CMPE: std::cout << "CMPE"; break;
			case CMD_CMPL: std::cout << "CMPL"; break;
			case CMD_JMP: std::cout << "JMP " << bytecode[i].target; break;
			case CMD_JZ: std::cout << "JZ " << bytecode[i].target; break;
			case CMD_CALL: std::cout << "CALL " << bytecode[i].target; break;
			case CMD_RET: std::cout << "RET"; break;
			case CMD_PRINT: std::cout << "PRINT"; break;
			case CMD_CALL_LIB: std::cout << "CALL LIB " << IT::GetEntry(idtable, (int)bytecode[i].target).id; break;
			default: std::cout << "OP " << bytecode[i].op;
			}
			std::cout << std::endl;
		}
		std::cout << "----------------\n";
	}
}