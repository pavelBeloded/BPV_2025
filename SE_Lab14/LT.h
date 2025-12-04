#pragma once

#include "IT.h"

#define LEXEMA_FIXSIZE 1
#define LT_MAXSIZE 4096 //4096
#define LT_TI_NULLIDX 0xfffffff
#define LEX_STRING 't'
#define LEX_INTEGER 't'
#define LEX_ID 'i'
#define LEX_LITERAL 'l'
#define LEX_FUNCTION 'f'
#define LEX_DECLARE 'd'
#define LEX_RETURN 'r'
#define LEX_PRINT 'p'
#define LEX_MAIN 'm'
#define LEX_SEMICOLON ';'
#define LEX_COMMA ','
#define LEX_LEFTBRACE '{'
#define LEX_BRACELET '}'
#define LEX_LEFTHESIS '('
#define LEX_RIGHTHESIS ')'
#define LEX_PLUS '+'
#define LEX_MINUS '-'
#define LEX_STAR '*'
#define LEX_DIRSLASH '/'
#define LEX_EQUAL '='
#define KEYWORD_MAX_LEN 10

#define KEYWORDS_COUNT 7
#define OPERATIONS_COUNT 5
#define SEPARATORS_COUNT 6

namespace LT {

	struct Keyword {
		char keyword[KEYWORD_MAX_LEN];
		char lexem;
	};

	struct Separator {
		char separator;
		char lexem;
	};

	struct Operation {
		char operation;
		char lexem;
	};

	struct Entry {
		char lexema;
		int sn; //номер строки в исходном тексте
		int idxTI; //индекс в таблице идентификаторов 

		/*Entry(char lexema, int sn) {
			this->lexema = lexema;
			this->sn = sn;
		}*/
	};
	struct LexTable
	{
		int maxsize; //емкость таблицы < LT_MAXSIZE
		int size; //current size < maxsize
		Entry* table; // array of lines
	};

	LexTable Create(int size); // create LT
	void Add(LexTable& lextable, Entry entry);
	Entry GetEntry(LexTable& lextable, int index);
	void Delete(LexTable& lextable);
	void FillLTIT(LexTable& lextable, IT::IdTable& idtable, In::IN& in);
	bool isId(const std::string& word);
}