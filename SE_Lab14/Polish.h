#pragma once
#include "LT.h"
#include "IT.h"

namespace Polish {
	int PolishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable);
}