#pragma once
#include "CodeGen.h"
#include "IT.h"

namespace Executor {
	void Execute(CodeGen::ByteCode& code, IT::IdTable& idtable);
}