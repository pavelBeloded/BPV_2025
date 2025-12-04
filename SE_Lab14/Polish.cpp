#include "stdafx.h"
#include "Polish.h"
#include <stack>
#include <vector>
#include <iostream>

namespace Polish {

	int GetPriority(char lexema) {
		switch (lexema) {
		case LEX_LEFTHESIS:
		case LEX_RIGHTHESIS:
			return 0;
		case LEX_COMMA:
			return 1;
		case LEX_PLUS:
		case LEX_MINUS:
			return 2;
		case LEX_STAR:
		case LEX_DIRSLASH:
			return 3;
		default:
			return 0;
		}
	}

	bool PolishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable) {
		std::stack<LT::Entry> stack;
		std::vector<LT::Entry> out_buf;

		// 1. Поиск конца выражения (ищем точку с запятой)
		int end_pos = lextable_pos;
		while (end_pos < lextable.size && lextable.table[end_pos].lexema != LEX_SEMICOLON) {
			end_pos++;
		}
		if (end_pos == lextable.size) return false; // Не нашли ;

		// 2. Алгоритм Дейкстры
		for (int i = lextable_pos; i < end_pos; i++) {
			LT::Entry entry = lextable.table[i];

			switch (entry.lexema) {
			case LEX_ID: {
				IT::Entry idEntry = IT::GetEntry(idtable, entry.idxTI);
				if (idEntry.idtype == IT::F) {
					stack.push(entry); // Функции в стек
				}
				else {
					out_buf.push_back(entry); // Переменные на выход
				}
				break;
			}
			case LEX_LITERAL: {
				out_buf.push_back(entry); // Литералы на выход
				break;
			}
			case LEX_LEFTHESIS: {
				stack.push(entry);
				break;
			}
			case LEX_RIGHTHESIS: {
				while (!stack.empty() && stack.top().lexema != LEX_LEFTHESIS) {
					out_buf.push_back(stack.top());
					stack.pop();
				}
				if (!stack.empty()) stack.pop(); // Удаляем '('

				// Если после скобок была функция (например fi(x,y)), выталкиваем её
				if (!stack.empty()) {
					if (stack.top().lexema == LEX_ID) {
						IT::Entry checkFunc = IT::GetEntry(idtable, stack.top().idxTI);
						if (checkFunc.idtype == IT::F) {
							out_buf.push_back(stack.top());
							stack.pop();
						}
					}
				}
				break;
			}
			case LEX_PLUS:
			case LEX_MINUS:
			case LEX_STAR:
			case LEX_DIRSLASH: {
				while (!stack.empty() && GetPriority(stack.top().lexema) >= GetPriority(entry.lexema)) {
					out_buf.push_back(stack.top());
					stack.pop();
				}
				stack.push(entry);
				break;
			}
			case LEX_COMMA: {
				while (!stack.empty() && stack.top().lexema != LEX_LEFTHESIS) {
					out_buf.push_back(stack.top());
					stack.pop();
				}
				break;
			}
			}
		}

		// Выталкиваем остатки
		while (!stack.empty()) {
			if (stack.top().lexema == LEX_LEFTHESIS || stack.top().lexema == LEX_RIGHTHESIS) return false;
			out_buf.push_back(stack.top());
			stack.pop();
		}

		// 3. Модификация таблицы лексем (запись результата)
		int out_index = 0;
		for (auto& item : out_buf) {
			lextable.table[lextable_pos + out_index] = item;
			out_index++;
		}

		// Заполняем "хвост" пустыми значениями (как null на скриншоте)
		// Используем заглушку '#', чтобы в логе было видно, что ячейка свободна
		for (int k = lextable_pos + out_index; k < end_pos; k++) {
			lextable.table[k].lexema = '#';
			lextable.table[k].idxTI = LT_TI_NULLIDX;
		}

		// 4. КОРРЕКТНЫЙ Вывод в консоль
		std::cout << "Polish: ";
		for (int k = 0; k < out_index; k++) {
			LT::Entry e = lextable.table[lextable_pos + k];

			if (e.lexema == LEX_ID) {
				// Выводим имя переменной или функции
				std::cout << IT::GetEntry(idtable, e.idxTI).id;
			}
			else if (e.lexema == LEX_LITERAL) {
				// Выводим значение литерала
				IT::Entry lit = IT::GetEntry(idtable, e.idxTI);
				if (lit.iddatatype == IT::INT)
					std::cout << lit.value.vint;
				else
					std::cout << "'" << lit.value.vstr.str << "'";
			}
			else {
				// Выводим символ операции (+, -, *, /)
				std::cout << e.lexema;
			}
			std::cout << " ";
		}
		std::cout << std::endl;

		return true;
	}
}