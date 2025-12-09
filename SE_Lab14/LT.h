#pragma once
#include "IT.h"

#define LEXEMA_FIXSIZE 1
#define LT_MAXSIZE 4096
#define LT_TI_NULLIDX 0xfffffff

#define LEX_TYPE		't'       
#define LEX_ID			'i'
#define LEX_LITERAL		'l'
#define LEX_FUNCTION	'f'     
#define LEX_DECLARE		'd'     
#define LEX_RETURN		'r'     
#define LEX_PRINT		'p'     
#define LEX_MAIN		'm'     

#define LEX_WHILE       'w'   

#define LEX_SEMICOLON	';'
#define LEX_COMMA		','
#define LEX_LEFTBRACE	'{'
#define LEX_BRACELET	'}'
#define LEX_LEFTHESIS	'('
#define LEX_RIGHTHESIS	')'

#define LEX_PLUS		'+'
#define LEX_MINUS		'-'
#define LEX_STAR		'*'
#define LEX_DIRSLASH	'/'
#define LEX_MODULO      '%'        

#define LEX_ASSIGN		'='   
#define LEX_EQ			'e'    
#define LEX_NE			'n'     
#define LEX_LESS		'<'   
#define LEX_MORE		'>'   
#define LEX_LE			'1'     
#define LEX_GE			'2'     

#define KEYWORD_MAX_LEN 10
#define KEYWORDS_COUNT 8          
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

	struct Entry {
		char lexema;
		int sn;
		int idxTI;
	};

	struct LexTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	LexTable Create(int size);
	void Add(LexTable& lextable, Entry entry);
	Entry GetEntry(LexTable& lextable, int index);
	void Delete(LexTable& lextable);

	void FillLTIT(LexTable& lextable, IT::IdTable& idtable, In::IN& in);
}