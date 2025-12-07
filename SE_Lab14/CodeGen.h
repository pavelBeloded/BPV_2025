#pragma once
#include "LT.h"
#include "IT.h"
#include "Log.h" // Добавляем для вывода в файл
#include <vector>
#include <string>

namespace CodeGen {
	enum OpCode {
		CMD_NOP = 0,
		CMD_PUSH,
		CMD_POP,

		CMD_ADD,
		CMD_SUB,
		CMD_MUL,
		CMD_DIV,
		CMD_MOD,

		CMD_CMPE,
		CMD_CMPNE,
		CMD_CMPG,
		CMD_CMPL,
		CMD_CMPGE,
		CMD_CMPLE,

		CMD_JMP,
		CMD_JZ,

		CMD_CALL,
		CMD_CALL_LIB,
		CMD_RET,
		CMD_PRINT
	};

	struct Instruction {
		OpCode op;			// Команда
		size_t target;		// Аргумент (индекс ID или адрес перехода)
	};

	typedef std::vector<Instruction> ByteCode;

	ByteCode Generate(LT::LexTable& lextable, IT::IdTable& idtable);

	void Debug(ByteCode& bytecode, IT::IdTable& idtable, Log::LOG log);
}