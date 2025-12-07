#pragma once
#include "CodeGen.h"
#include "IT.h"

namespace Gener
{
	void CodeGeneration(CodeGen::ByteCode& bytecode, IT::IdTable& idtable, const wchar_t* out_file);
}