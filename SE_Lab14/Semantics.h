#pragma once
#include "LT.h"
#include "IT.h"

#pragma once
#include "LT.h"
#include "IT.h"
#include "Log.h"

namespace Semantics {
	void InjectStandardLibrary(IT::IdTable& idtable);
	bool Analyze(LT::LexTable& lextable, IT::IdTable& idtable, Log::LOG log);
	void CheckParamCount(LT::LexTable& lextable, IT::IdTable& idtable);
}