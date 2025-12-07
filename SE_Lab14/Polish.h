#pragma once
#include "LT.h"
#include "IT.h"
#include "Log.h" // Добавляем лог

namespace Polish {
	// Добавили аргумент log
	int PolishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable, Log::LOG log);
}