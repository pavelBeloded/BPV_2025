#include "stdafx.h"
#include "Polish.h"
#include "Error.h" // Обязательно подключаем для ERROR_THROW
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

			// Операции сравнения (приоритет 2)
		case LEX_EQ: case LEX_NE:
		case LEX_LESS: case LEX_MORE:
		case LEX_LE: case LEX_GE:
			return 2;

			// Сложение и вычитание (приоритет 3)
		case LEX_PLUS:
		case LEX_MINUS:
			return 3;

			// Умножение, деление и остаток (приоритет 4)
		case LEX_STAR:
		case LEX_DIRSLASH:
		case LEX_MODULO:
			return 4;

		default:
			return 0;
		}
	}

	bool PolishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable) {
		std::stack<LT::Entry> stack;
		std::vector<LT::Entry> out_buf;

		// 1. Поиск конца выражения
		int end_pos = lextable_pos;
		while (end_pos < lextable.size) {
			char l = lextable.table[end_pos].lexema;
			if (l == LEX_SEMICOLON || l == LEX_LEFTBRACE) break;
			end_pos++;
		}

		// Защита от выхода за границы, если ; не найдена
		if (end_pos == lextable.size && lextable.table[end_pos - 1].lexema != LEX_SEMICOLON) {
			// Можно кинуть ошибку 700 (Общая ошибка ПОЛИЗ)
			throw ERROR_THROW_IN(700, lextable.table[lextable_pos].sn, -1);
		}

		// 2. Алгоритм Дейкстры (Сортировочная станция)
		for (int i = lextable_pos; i < end_pos; i++) {
			LT::Entry entry = lextable.table[i];

			switch (entry.lexema) {
			case LEX_ID: {
				IT::Entry idEntry = IT::GetEntry(idtable, entry.idxTI);
				if (idEntry.idtype == IT::F) {
					stack.push(entry);
				}
				else {
					out_buf.push_back(entry);
				}
				break;
			}
			case LEX_LITERAL: {
				out_buf.push_back(entry);
				break;
			}
			case LEX_LEFTHESIS: {
				stack.push(entry);
				break;
			}
			case LEX_RIGHTHESIS: {
				// Выталкиваем до открывающей скобки
				while (!stack.empty() && stack.top().lexema != LEX_LEFTHESIS) {
					out_buf.push_back(stack.top());
					stack.pop();
				}

				// ОШИБКА 702: Если стек пуст, значит не нашли пару '('
				if (stack.empty()) {
					throw ERROR_THROW_IN(702, entry.sn, -1);
				}

				stack.pop(); // Удаляем '('

				// Если это вызов функции func(a,b), выталкиваем имя функции
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
							   // Все операторы
			case LEX_PLUS: case LEX_MINUS: case LEX_STAR: case LEX_DIRSLASH: case LEX_MODULO:
			case LEX_EQ: case LEX_NE: case LEX_LESS: case LEX_MORE: case LEX_LE: case LEX_GE:
			{
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
				// ОШИБКА 702: Если при поиске запятой не нашли открывающую скобку
				if (stack.empty()) {
					throw ERROR_THROW_IN(702, entry.sn, -1);
				}
				break;
			}
			}
		}

		// Выталкиваем оставшиеся операторы
		while (!stack.empty()) {
			// ОШИБКА 702: Если в стеке остались скобки - баланс нарушен
			if (stack.top().lexema == LEX_LEFTHESIS || stack.top().lexema == LEX_RIGHTHESIS) {
				throw ERROR_THROW_IN(702, lextable.table[lextable_pos].sn, -1);
			}
			out_buf.push_back(stack.top());
			stack.pop();
		}

		// 3. Запись результата обратно в таблицу лексем
		int out_index = 0;
		for (auto& item : out_buf) {
			lextable.table[lextable_pos + out_index] = item;
			out_index++;
		}

		// Забиваем хвост заглушками
		for (int k = lextable_pos + out_index; k < end_pos; k++) {
			lextable.table[k].lexema = '#';
			lextable.table[k].idxTI = LT_TI_NULLIDX;
		}

		// 4. Вывод в консоль (с расшифровкой операторов)
		std::cout << "Polish: ";
		for (int k = 0; k < out_index; k++) {
			LT::Entry e = lextable.table[lextable_pos + k];

			if (e.lexema == LEX_ID) {
				std::cout << IT::GetEntry(idtable, e.idxTI).id;
			}
			else if (e.lexema == LEX_LITERAL) {
				IT::Entry lit = IT::GetEntry(idtable, e.idxTI);
				if (lit.iddatatype == IT::INT) std::cout << lit.value.vint;
				else std::cout << "'" << lit.value.vstr.str << "'";
			}
			else {
				// Красивый вывод операторов
				switch (e.lexema) {
				case LEX_PLUS: std::cout << "+"; break;
				case LEX_MINUS: std::cout << "-"; break;
				case LEX_STAR: std::cout << "*"; break;
				case LEX_DIRSLASH: std::cout << "/"; break;
				case LEX_MODULO: std::cout << "%"; break;
				case LEX_EQ: std::cout << "=="; break;
				case LEX_NE: std::cout << "!="; break;
				case LEX_LE: std::cout << "<="; break;
				case LEX_GE: std::cout << ">="; break;
				case LEX_LESS: std::cout << "<"; break;
				case LEX_MORE: std::cout << ">"; break;
				case LEX_ASSIGN: std::cout << "="; break;
				default: std::cout << e.lexema;
				}
			}
			std::cout << " ";
		}
		std::cout << std::endl;

		return true;
	}
}