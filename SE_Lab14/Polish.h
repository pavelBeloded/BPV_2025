#pragma once
#include "LT.h"
#include "IT.h"
#include "Log.h"

namespace Polish {
	int PolishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable, Log::LOG log);
}