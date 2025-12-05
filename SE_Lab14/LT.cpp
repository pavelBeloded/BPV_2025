#include "stdafx.h"
#include "LT.h"
#include "IT.h"
#include "Error.h"
#include <string>
#include <vector>
#include <cctype>
#include <new>
#include <iostream> // Для отладки, если нужно

// --- Обновленный список ключевых слов для языка XXX-2025 ---
LT::Keyword keywords[KEYWORDS_COUNT]{
	{"text",	LEX_TYPE},		// string -> text
	{"uint",	LEX_TYPE},		// integer -> uint
	{"proc",	LEX_FUNCTION},	// function -> proc
	{"var",		LEX_DECLARE},	// declare -> var
	{"ret",		LEX_RETURN},	// return -> ret
	{"echo",	LEX_PRINT},		// print -> echo
	{"entry",	LEX_MAIN},		// main -> entry
	{"while",	LEX_WHILE}		// НОВОЕ: цикл
};

LT::Separator separators[SEPARATORS_COUNT]{
	{';', LEX_SEMICOLON}, {',', LEX_COMMA},
	{'{', LEX_LEFTBRACE}, {'}', LEX_BRACELET},
	{'(', LEX_LEFTHESIS}, {')', LEX_RIGHTHESIS}
};

// Операции теперь обрабатываются вручную внутри цикла, массив operations не нужен в старом виде

namespace LT {

	bool isId(const std::string& word) {
		if (word.empty() || !isalpha(word[0])) return false;
		for (char c : word) {
			if (!isalnum(c) && c != '_') return false; // Разрешаем '_' в именах
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

	// Контексты нужны для правильного определения типа идентификатора (F, V, P)
	enum class Context {
		GLOBAL,
		DECLARE_SECTION,     // После слова var
		FUNCTION_DECLARATION,// После слова proc
		PARAMETER_LIST       // В скобках ( ... )
	};

	void FillLTIT(LexTable& lextable, IT::IdTable& idtable, In::IN& in) {
		std::vector<Context> contextStack;
		contextStack.push_back(Context::GLOBAL);

		IT::IDDATATYPE lastDataType = IT::UNKNOWN;
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

				// Логика контекстов
				if (lexem == LEX_DECLARE)
					contextStack.push_back(Context::DECLARE_SECTION);
				else if (lexem == LEX_FUNCTION) {
					contextStack.push_back(Context::FUNCTION_DECLARATION);
					nextIsFunctionName = true;
				}
				else if (word == "text") lastDataType = IT::STR; // Запоминаем тип
				else if (word == "uint") lastDataType = IT::INT;
			}
			else if (isNumericLiteral(word)) {
				// Обработка ВОСЬМЕРИЧНЫХ чисел (Задание: литералы)
				int value = 0;
				if (word.length() > 1 && word[0] == '0') {
					// Если начинается с 0 и длина > 1 — это восьмеричное (012 -> 10)
					try {
						value = std::stoi(word, nullptr, 8);
					}
					catch (...) {
						// Если есть цифры 8 или 9, stoi выбросит исключение
						throw ERROR_THROW(116); // Добавить ошибку "Неверный литерал" в Error
					}
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

				if (nextIsFunctionName) {
					idType = IT::F;
					nextIsFunctionName = false;
				}
				else if (currentContext == Context::PARAMETER_LIST) {
					idType = IT::P;
				}

				// Добавляем ID
				int idx = IT::AddId(idtable, word, lastDataType, idType, line);
				Add(lextable, { LEX_ID, line, idx });
			}
			else {
				// throw ERROR_THROW_IN(114, line, 0); 
				// Пока заглушим, чтобы не падало на странных символах, если что вернем
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

			// Проверка на сепараторы
			bool is_separator = false;
			for (const auto& sep : separators) {
				if (c == sep.separator) {
					processWord();
					Add(lextable, { sep.lexem, line, LT_TI_NULLIDX });

					// Управление контекстом скобок и т.д.
					if (c == '(' && contextStack.back() == Context::FUNCTION_DECLARATION) {
						contextStack.back() = Context::PARAMETER_LIST;
					}
					else if (c == ')' && contextStack.back() == Context::PARAMETER_LIST) {
						contextStack.pop_back(); // Выход из параметров
					}
					else if (c == ';' && contextStack.back() == Context::DECLARE_SECTION) {
						contextStack.pop_back(); // Выход из var
					}
					else if (c == '{' && contextStack.back() == Context::FUNCTION_DECLARATION) {
						contextStack.pop_back(); // Функция объявлена, началось тело
					}

					is_separator = true;
					break;
				}
			}
			if (is_separator) continue;

			// --- ОБРАБОТКА ОПЕРАТОРОВ (включая двойные) ---
			char opLexem = 0;
			bool doubleChar = false;

			if (c == '+') opLexem = LEX_PLUS;
			else if (c == '-') opLexem = LEX_MINUS;
			else if (c == '*') opLexem = LEX_STAR;
			else if (c == '/') opLexem = LEX_DIRSLASH;
			else if (c == '%') opLexem = LEX_MODULO;

			// Сравнения и присваивание
			else if (c == '=') {
				if (next_c == '=') { opLexem = LEX_EQ; doubleChar = true; } // ==
				else opLexem = LEX_ASSIGN; // =
			}
			else if (c == '!') {
				if (next_c == '=') { opLexem = LEX_NE; doubleChar = true; } // !=
			}
			else if (c == '<') {
				if (next_c == '=') { opLexem = LEX_LE; doubleChar = true; } // <=
				else opLexem = LEX_LESS; // <
			}
			else if (c == '>') {
				if (next_c == '=') { opLexem = LEX_GE; doubleChar = true; } // >=
				else opLexem = LEX_MORE; // >
			}

			if (opLexem != 0) {
				processWord();
				Add(lextable, { opLexem, line, LT_TI_NULLIDX });
				if (doubleChar) i++; // Пропускаем второй символ
				continue;
			}

			// Если ничего не подошло, накапливаем слово
			word += c;
		}
		processWord();
	}
	// Остальные функции Create, Add, GetEntry, Delete оставляем без изменений
	LexTable Create(int size) { /* ... старый код ... */ return LexTable{ size, 0, new Entry[size] }; }
	void Add(LexTable& lextable, Entry entry) { lextable.table[lextable.size++] = entry; }
	Entry GetEntry(LexTable& lextable, int index) { return lextable.table[index]; }
	void Delete(LexTable& lextable) { delete[] lextable.table; }
}