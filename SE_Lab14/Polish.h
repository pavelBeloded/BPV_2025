#pragma once
#include "LT.h"
#include "IT.h"

namespace Polish {
	bool PolishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable);
}