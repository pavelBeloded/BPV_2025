#include "stdafx.h"
#include "LT.h"
#include "IT.h"
#include "Error.h"
#include <string>
#include <vector>
#include <cctype>
#include <new>
#include <iostream>

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

		std::string currentScope = "global";   

		bool nextIsFunctionName = false;
		bool isInsideString = false;

		std::string word = "";
		int line = 1;

		auto processWord = [&]() {
			if (word.empty()) return;

			char lexem = getKeywordLexem(word);
			if (lexem) {
				Add(lextable, { lexem, line, LT_TI_NULLIDX });

				if (lexem == LEX_DECLARE)
					contextStack.push_back(Context::DECLARE_SECTION);

				else if (lexem == LEX_FUNCTION) {  
					contextStack.push_back(Context::FUNCTION_DECLARATION);
					nextIsFunctionName = true;
				}

				else if (lexem == LEX_MAIN) {  
					currentScope = "entry";     
				}

				else if (word == "text") lastDataType = IT::STR;
				else if (word == "uint") lastDataType = IT::INT;
			}
			else if (isNumericLiteral(word)) {
				int value = 0;
				if (word.length() > 1 && word[0] == '0') {
					try { value = std::stoi(word, nullptr, 8); }  
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

				if (nextIsFunctionName) {
					idType = IT::F;
					nextIsFunctionName = false;
					currentScope = word;      
					int idx = IT::AddId(idtable, word, lastDataType, idType, line);
					Add(lextable, { LEX_ID, line, idx });
				}
				else {
					if (currentContext == Context::PARAMETER_LIST) idType = IT::P;

					std::string scopedName = currentScope + "$" + word;

					if (currentContext == Context::DECLARE_SECTION || currentContext == Context::PARAMETER_LIST) {
						if (IT::IsId(idtable, scopedName) != TI_NULLIDX) {
						}
						int idx = IT::AddId(idtable, scopedName, lastDataType, idType, line);
						Add(lextable, { LEX_ID, line, idx });
					}
					else {
						int idx = IT::IsId(idtable, scopedName);

						if (idx == TI_NULLIDX) {
							idx = IT::IsId(idtable, word);
						}

						if (idx == TI_NULLIDX) {
							throw ERROR_THROW_IN(301, line, 0);    
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
			if (c == '/' && next_c == '/') {
				while (i < in.size && in.text[i] != '\n') {
					i++;
				}
				processWord();
				line++;
				continue;
			}
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