#include "stdafx.h"
#include "Polish.h"
#include "Error.h"
#include <stack>
#include <vector>
#include <iostream>

namespace Polish {

	const char* standardLibs[] = { "date", "time", "length", "tostr", "touint", "substr" };

	bool IsFunction(IT::IdTable& idtable, IT::Entry& entry) {
		if (entry.idtype == IT::F) return true;
		for (const char* funcName : standardLibs) {
			if (strcmp(entry.id, funcName) == 0) return true;
		}
		for (int i = 0; i < idtable.size; i++) {
			if (idtable.table[i].idtype == IT::F && strcmp(idtable.table[i].id, entry.id) == 0) return true;
		}
		return false;
	}

	int GetPriority(char lexema) {
		switch (lexema) {
		case LEX_RETURN: case LEX_PRINT: case LEX_WHILE: return 1;
		case LEX_ASSIGN: return 2;
		case LEX_LEFTHESIS: case LEX_RIGHTHESIS: return 0;
		case LEX_COMMA: return 1;
		case LEX_EQ: case LEX_NE: case LEX_LESS: case LEX_MORE: case LEX_LE: case LEX_GE: return 3;
		case LEX_PLUS: case LEX_MINUS: return 4;
		case LEX_STAR: case LEX_DIRSLASH: case LEX_MODULO: return 5;
		default: return 0;
		}
	}

	// Возвращаем int (индекс конца) вместо bool
	int PolishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable) {
		std::stack<LT::Entry> stack;
		std::vector<LT::Entry> out_buf;

		int end_pos = lextable_pos;
		while (end_pos < lextable.size) {
			char l = lextable.table[end_pos].lexema;
			if (l == LEX_SEMICOLON || l == LEX_LEFTBRACE) break;
			end_pos++;
		}

		// Если вышли за границы - ошибка
		if (end_pos > lextable.size) return -1;

		for (int i = lextable_pos; i < end_pos; i++) {
			LT::Entry entry = lextable.table[i];
			char lex = entry.lexema;

			switch (lex) {
			case LEX_ID: {
				IT::Entry idEntry = IT::GetEntry(idtable, entry.idxTI);
				if (IsFunction(idtable, idEntry)) stack.push(entry);
				else out_buf.push_back(entry);
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
				while (!stack.empty() && stack.top().lexema != LEX_LEFTHESIS) {
					out_buf.push_back(stack.top());
					stack.pop();
				}
				if (!stack.empty()) stack.pop();
				if (!stack.empty() && stack.top().lexema == LEX_ID) {
					IT::Entry checkFunc = IT::GetEntry(idtable, stack.top().idxTI);
					if (IsFunction(idtable, checkFunc)) {
						out_buf.push_back(stack.top());
						stack.pop();
					}
				}
				break;
			}
			case LEX_COMMA: {
				while (!stack.empty() && stack.top().lexema != LEX_LEFTHESIS) {
					out_buf.push_back(stack.top());
					stack.pop();
				}
				break;
			}
			default: {
				while (!stack.empty() && GetPriority(stack.top().lexema) >= GetPriority(lex)) {
					out_buf.push_back(stack.top());
					stack.pop();
				}
				stack.push(entry);
				break;
			}
			}
		}

		while (!stack.empty()) {
			if (stack.top().lexema == LEX_LEFTHESIS || stack.top().lexema == LEX_RIGHTHESIS) return -1;
			out_buf.push_back(stack.top());
			stack.pop();
		}

		int out_index = 0;
		for (auto& item : out_buf) {
			lextable.table[lextable_pos + out_index] = item;
			out_index++;
		}

		for (int k = lextable_pos + out_index; k < end_pos; k++) {
			lextable.table[k].lexema = '#';
			lextable.table[k].idxTI = LT_TI_NULLIDX;
		}

		/*std::cout << "Polish: ";
		for (int k = 0; k < out_index; k++) {
			LT::Entry e = lextable.table[lextable_pos + k];
			if (e.lexema == LEX_ID) std::cout << IT::GetEntry(idtable, e.idxTI).id << " ";
			else if (e.lexema == LEX_LITERAL) {
				IT::Entry lit = IT::GetEntry(idtable, e.idxTI);
				if (lit.iddatatype == IT::INT) std::cout << lit.value.vint << " ";
				else std::cout << "'" << lit.value.vstr.str << "' ";
			}
			else {
				switch (e.lexema) {
				case LEX_ASSIGN: std::cout << "= "; break;
				case LEX_PLUS: std::cout << "+ "; break;
				case LEX_MINUS: std::cout << "- "; break;
				case LEX_STAR: std::cout << "* "; break;
				case LEX_DIRSLASH: std::cout << "/ "; break;
				case LEX_EQ: std::cout << "== "; break;
				case LEX_RETURN: std::cout << "ret "; break;
				case LEX_PRINT: std::cout << "echo "; break;
				case LEX_WHILE: std::cout << "while "; break;
				default: std::cout << e.lexema << " ";
				}
			}
		}
		std::cout << std::endl;*/

		// Возвращаем индекс, на котором остановились (точка с запятой или скобка)
		return end_pos;
	}
}