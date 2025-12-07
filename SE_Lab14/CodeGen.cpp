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
		std::map<int, size_t> functionMap; // ID Index -> Address in ByteCode

		size_t entryPoint = 0;
		size_t currentStatementStart = 0;

		// 1. Добавляем заглушку для прыжка в начало программы (на entry)
		// Индекс 0 всегда будет JMP. Мы запатчим его в конце.
		code.push_back({ CMD_JMP, 0 });

		for (int i = 0; i < lextable.size; i++) {
			LT::Entry entry = lextable.table[i];
			size_t currentAddr = code.size();

			switch (entry.lexema) {

				// --- ОБЪЯВЛЕНИЕ ФУНКЦИИ ---
			case LEX_FUNCTION: {
				// Следующий токен - это имя функции (ID)
				if (i + 1 < lextable.size && lextable.table[i + 1].lexema == LEX_ID) {
					int funcIdIdx = lextable.table[i + 1].idxTI;
					// Запоминаем адрес начала функции
					functionMap[funcIdIdx] = currentAddr;
					// Пропускаем ID, чтобы не сгенерировать лишний CALL или PUSH
					i++;
				}
				break;
			}

							 // --- ТОЧКА ВХОДА ---
			case LEX_MAIN: {
				entryPoint = currentAddr;
				break;
			}

						 // --- ОБЪЯВЛЕНИЕ ПЕРЕМЕННОЙ ---
			case LEX_DECLARE: {
				// Формат: var type id ...
				// Нам нужно пропустить type и id, чтобы не генерировать для них PUSH
				// (память и так есть в IT, а инициализация "=" обработается отдельно)
				if (i + 2 < lextable.size) {
					// Пропускаем 'type' и 'id'
					i += 2;
				}
				break;
			}

							// --- ИДЕНТИФИКАТОРЫ ---
			case LEX_ID: {
				IT::Entry itEntry = IT::GetEntry(idtable, entry.idxTI);
				if (itEntry.idtype == IT::F) {
					// Это вызов функции. Пока пишем ID из таблицы, потом заменим на Адрес
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

							// --- ОПЕРАТОРЫ ---
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
				// Специальный флаг для интерпретатора: взять адрес из стека
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

							 // Пропускаем типы (uint, text) если они встретились отдельно (например в параметрах)
			case LEX_TYPE: break;

				// Заглушки
			case '#': break;
			}
		}

		// --- ФИНАЛИЗАЦИЯ (PATCHING) ---

		// 1. Патчим самый первый JMP на точку входа (entry)
		if (entryPoint > 0) {
			code[0].target = entryPoint;
		}

		// 2. Патчим вызовы функций (заменяем ID на Адрес)
		for (auto& instr : code) {
			if (instr.op == CMD_CALL) {
				int funcIdx = (int)instr.target;

				// Если это пользовательская функция, у нее есть адрес
				if (functionMap.count(funcIdx)) {
					instr.target = functionMap[funcIdx];
				}
				// Если это библиотечная функция (date, length), адреса в байт-коде нет.
				// Оставляем как есть (индекс в IT), интерпретатор обработает это отдельно.
			}
		}

		return code;
	}

	void Debug(ByteCode& bytecode, IT::IdTable& idtable) {
		std::cout << "\n--- BYTECODE GENERATION ---\n";
		int i = 0;
		for (const auto& instr : bytecode) {
			std::cout << std::setw(3) << i << ": ";
			switch (instr.op) {
			case CMD_PUSH:	std::cout << "PUSH\t" << IT::GetEntry(idtable, (int)instr.target).id; break;

			case CMD_POP:
				if (instr.target == LT_TI_NULLIDX) std::cout << "ASSIGN";
				else std::cout << "POP \t" << IT::GetEntry(idtable, (int)instr.target).id;
				break;

			case CMD_ADD:	std::cout << "ADD"; break;
			case CMD_SUB:	std::cout << "SUB"; break;
			case CMD_MUL:	std::cout << "MUL"; break;
			case CMD_DIV:	std::cout << "DIV"; break;
			case CMD_MOD:	std::cout << "MOD"; break;
			case CMD_CMPE:	std::cout << "CMPE (==)"; break;
			case CMD_CMPNE:	std::cout << "CMPNE (!=)"; break;
			case CMD_CMPL:	std::cout << "CMPL (<)"; break;
			case CMD_CMPG:	std::cout << "CMPG (>)"; break;
			case CMD_CMPLE:	std::cout << "CMPLE (<=)"; break;
			case CMD_CMPGE:	std::cout << "CMPGE (>=)"; break;
			case CMD_JMP:	std::cout << "JMP \t" << instr.target; break;
			case CMD_JZ:	std::cout << "JZ  \t" << instr.target; break;
			case CMD_RET:	std::cout << "RET"; break;
			case CMD_PRINT:	std::cout << "PRINT"; break;

			case CMD_CALL: {
				// Если target слишком большой (адрес), значит это пользовательская функция
				// Если маленький (индекс IT), значит библиотечная
				// Просто выводим число, либо пытаемся угадать имя для красоты
				if (instr.target < idtable.size)
					std::cout << "CALL LIB\t" << IT::GetEntry(idtable, (int)instr.target).id;
				else
					std::cout << "CALL \t" << instr.target;
				break;
			}
			}
			std::cout << std::endl;
			i++;
		}
		std::cout << "---------------------------\n";
	}
}