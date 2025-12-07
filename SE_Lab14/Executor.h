#pragma once
#include "CodeGen.h"
#include "IT.h"
#include "Log.h"

namespace Executor {
	void Execute(CodeGen::ByteCode& code, IT::IdTable& idtable, Log::LOG log);
}