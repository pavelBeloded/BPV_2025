#pragma once
#include "LT.h"
#include "IT.h"

namespace Semantics {
	void InjectStandardLibrary(IT::IdTable& idtable);

	bool Analyze(LT::LexTable& lextable, IT::IdTable& idtable);
}