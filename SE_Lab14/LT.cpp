#include "stdafx.h"
#include "LT.h"
#include "IT.h"
#include "Error.h"
#include <string>
#include <vector>
#include <cctype>
#include <new>
#include <iostream>

// Ключевые слова
LT::Keyword keywords[KEYWORDS_COUNT]{
	{"text",	LEX_TYPE},
	{"uint",	LEX_TYPE},
	{"proc",	LEX_FUNCTION},
	{"var",		LEX_DECLARE},
	{"ret",		LEX_RETURN},
	{"echo",	LEX_PRINT},
	{"entry",	LEX_MAIN},
	{"while",	LEX_WHILE}
};

LT::Separator separators[SEPARATORS_COUNT]{
	{';', LEX_SEMICOLON}, {',', LEX_COMMA},
	{'{', LEX_LEFTBRACE}, {'}', LEX_BRACELET},
	{'(', LEX_LEFTHESIS}, {')', LEX_RIGHTHESIS}
};

namespace LT {

	bool isId(const std::string& word) {
		if (word.empty() || !isalpha(word[0])) return false;
		for (char c : word) {
			if (!isalnum(c) && c != '_') return false;
		}
		return true;
	}

	bool isNumericLiteral(const std::string& word) {
		if (word.empty()) return false;
		for (char c : word) if (!isdigit(c)) return false;
		return true;
	}

	bool isStringLiteral(const std::string& word) {
		return word.length() >= 2 && word.front() == '\'' && word.back() == '\'';
	}

	char getKeywordLexem(const std::string& word) {
		for (const auto& kw : keywords) {
			if (kw.keyword == word) return kw.lexem;
		}
		return 0;
	}

	enum class Context {
		GLOBAL,
		DECLARE_SECTION,
		FUNCTION_DECLARATION,
		PARAMETER_LIST
	};

	void FillLTIT(LexTable& lextable, IT::IdTable& idtable, In::IN& in) {
		std::vector<Context> contextStack;
		contextStack.push_back(Context::GLOBAL);

		IT::IDDATATYPE lastDataType = IT::UNKNOWN;

		// --- НОВОЕ: Текущая область видимости ---
		std::string currentScope = "global"; // По умолчанию

		bool nextIsFunctionName = false;
		bool isInsideString = false;

		std::string word = "";
		int line = 1;

		auto processWord = [&]() {
			if (word.empty()) return;

			char lexem = getKeywordLexem(word);
			if (lexem) {
				// Ключевое слово
				Add(lextable, { lexem, line, LT_TI_NULLIDX });

				if (lexem == LEX_DECLARE)
					contextStack.push_back(Context::DECLARE_SECTION);

				else if (lexem == LEX_FUNCTION) { // proc
					contextStack.push_back(Context::FUNCTION_DECLARATION);
					nextIsFunctionName = true;
				}

				else if (lexem == LEX_MAIN) { // entry
					currentScope = "entry"; // Входим в scope entry
				}

				else if (word == "text") lastDataType = IT::STR;
				else if (word == "uint") lastDataType = IT::INT;
			}
			else if (isNumericLiteral(word)) {
				int value = 0;
				if (word.length() > 1 && word[0] == '0') {
					try { value = std::stoi(word, nullptr, 8); } // Восьмеричное
					catch (...) { throw ERROR_THROW(116); }
				}
				else {
					value = std::stoi(word);
				}
				int idx = IT::AddIntLiteral(idtable, value, line);
				Add(lextable, { LEX_LITERAL, line, idx });
			}
			else if (isStringLiteral(word)) {
				std::string literalContent = word.substr(1, word.length() - 2);
				int idx = IT::AddStringLiteral(idtable, literalContent, line);
				Add(lextable, { LEX_LITERAL, line, idx });
			}
			else if (isId(word)) {
				IT::IDTYPE idType = IT::V;
				Context currentContext = contextStack.back();

				// Логика определения типа и скоупа
				if (nextIsFunctionName) {
					idType = IT::F;
					nextIsFunctionName = false;
					currentScope = word; // Имя функции становится текущим скоупом
					// Функции добавляем БЕЗ префикса (они глобальны)
					int idx = IT::AddId(idtable, word, lastDataType, idType, line);
					Add(lextable, { LEX_ID, line, idx });
				}
				else {
					// Это параметр или переменная
					if (currentContext == Context::PARAMETER_LIST) idType = IT::P;

					// Формируем имя с префиксом: "scope$id"
					// Например: summa$x или entry$y
					std::string scopedName = currentScope + "$" + word;

					// --- ЛОГИКА ПОИСКА/ДОБАВЛЕНИЯ ---

					// 1. Если мы в секции объявления (var или параметры) -> Создаем новую
					if (currentContext == Context::DECLARE_SECTION || currentContext == Context::PARAMETER_LIST) {
						// Проверяем, нет ли дубликата в текущем скоупе
						if (IT::IsId(idtable, scopedName) != TI_NULLIDX) {
							// Ошибка: повторное объявление (можно кинуть error, но пока просто найдем)
						}
						int idx = IT::AddId(idtable, scopedName, lastDataType, idType, line);
						Add(lextable, { LEX_ID, line, idx });
					}
					// 2. Если мы используем переменную (x = ...)
					else {
						// Сначала ищем локальную (summa$x)
						int idx = IT::IsId(idtable, scopedName);

						if (idx == TI_NULLIDX) {
							// Если локальной нет, ищем глобальную (имя функции для рекурсии или вызова)
							idx = IT::IsId(idtable, word);
						}

						if (idx == TI_NULLIDX) {
							// Если переменная не объявлена - это семантическая ошибка.
							// В рамках лабы лексера можно либо кинуть ошибку, либо добавить как новую (но лучше ошибку)
							throw ERROR_THROW_IN(301, line, 0); // Ошибка: Необъявленный идентификатор
						}

						Add(lextable, { LEX_ID, line, idx });
					}
				}
			}
			word.clear();
			};

		for (int i = 0; i < in.size; ++i) {
			unsigned char c = in.text[i];
			unsigned char next_c = (i + 1 < in.size) ? in.text[i + 1] : 0;

			if (isInsideString) {
				word += c;
				if (c == '\'') {
					isInsideString = false;
					processWord();
				}
				continue;
			}
			if (c == '\'') {
				processWord();
				isInsideString = true;
				word += c;
				continue;
			}
			if (isspace(c)) {
				processWord();
				if (c == '\n') line++;
				continue;
			}

			bool is_separator = false;
			for (const auto& sep : separators) {
				if (c == sep.separator) {
					processWord();
					Add(lextable, { sep.lexem, line, LT_TI_NULLIDX });

					if (c == '(' && contextStack.back() == Context::FUNCTION_DECLARATION)
						contextStack.back() = Context::PARAMETER_LIST;
					else if (c == ')' && contextStack.back() == Context::PARAMETER_LIST)
						contextStack.pop_back();
					else if (c == ';' && contextStack.back() == Context::DECLARE_SECTION)
						contextStack.pop_back();
					else if (c == '{' && contextStack.back() == Context::FUNCTION_DECLARATION)
						contextStack.pop_back();

					is_separator = true;
					break;
				}
			}
			if (is_separator) continue;

			char opLexem = 0;
			bool doubleChar = false;
			if (c == '+') opLexem = LEX_PLUS;
			else if (c == '-') opLexem = LEX_MINUS;
			else if (c == '*') opLexem = LEX_STAR;
			else if (c == '/') opLexem = LEX_DIRSLASH;
			else if (c == '%') opLexem = LEX_MODULO;
			else if (c == '=') { if (next_c == '=') { opLexem = LEX_EQ; doubleChar = true; } else opLexem = LEX_ASSIGN; }
			else if (c == '!') { if (next_c == '=') { opLexem = LEX_NE; doubleChar = true; } }
			else if (c == '<') { if (next_c == '=') { opLexem = LEX_LE; doubleChar = true; } else opLexem = LEX_LESS; }
			else if (c == '>') { if (next_c == '=') { opLexem = LEX_GE; doubleChar = true; } else opLexem = LEX_MORE; }

			if (opLexem != 0) {
				processWord();
				Add(lextable, { opLexem, line, LT_TI_NULLIDX });
				if (doubleChar) i++;
				continue;
			}
			word += c;
		}
		processWord();
	}

	// Остальные функции без изменений...
	LexTable Create(int size) {
		if (size <= 0 || size > LT_MAXSIZE) throw ERROR_THROW(211);
		return LexTable{ size, 0, new Entry[size] };
	}
	void Add(LexTable& lextable, Entry entry) {
		if (lextable.size >= lextable.maxsize) throw ERROR_THROW(210);
		lextable.table[lextable.size++] = entry;
	}
	Entry GetEntry(LexTable& lextable, int index) {
		if (index < 0 || index >= lextable.size) throw ERROR_THROW(212);
		return lextable.table[index];
	}
	void Delete(LexTable& lextable) {
		delete[] lextable.table;
		lextable.table = nullptr;
	}
}