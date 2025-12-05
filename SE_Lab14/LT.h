#pragma once
#include "IT.h"

#define LEXEMA_FIXSIZE 1
#define LT_MAXSIZE 4096
#define LT_TI_NULLIDX 0xfffffff

// --- Новые ключевые слова (семантически совпадают со старыми) ---
#define LEX_TYPE		't'  // uint, text (было integer, string)
#define LEX_ID			'i'
#define LEX_LITERAL		'l'
#define LEX_FUNCTION	'f'  // proc (было function)
#define LEX_DECLARE		'd'  // var (было declare)
#define LEX_RETURN		'r'  // ret (было return)
#define LEX_PRINT		'p'  // echo (было print)
#define LEX_MAIN		'm'  // entry (было main)

// --- Новые конструкции для курсового ---
#define LEX_WHILE       'w'  // while

// --- Разделители ---
#define LEX_SEMICOLON	';'
#define LEX_COMMA		','
#define LEX_LEFTBRACE	'{'
#define LEX_BRACELET	'}'
#define LEX_LEFTHESIS	'('
#define LEX_RIGHTHESIS	')'

// --- Арифметические операции (уникальные коды!) ---
#define LEX_PLUS		'+'
#define LEX_MINUS		'-'
#define LEX_STAR		'*'
#define LEX_DIRSLASH	'/'
#define LEX_MODULO      '%'  // Остаток от деления (стандарт для uint)

// --- Операции присваивания и сравнения ---
#define LEX_ASSIGN		'='  // =
#define LEX_EQ			'e'  // == (equal)
#define LEX_NE			'n'  // != (not equal)
#define LEX_LESS		'<'  // <
#define LEX_MORE		'>'  // >
#define LEX_LE			'1'  // <= (less equal)
#define LEX_GE			'2'  // >= (greater equal)

#define KEYWORD_MAX_LEN 10
#define KEYWORDS_COUNT 8      // Было 7, добавился while
#define SEPARATORS_COUNT 6
// Operations count убираем из define, так как проверка будет сложнее

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