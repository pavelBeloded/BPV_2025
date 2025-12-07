#pragma once
#include "LT.h"
#include "IT.h"
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
		size_t target;		// Аргумент:
		// 1. Для PUSH/POP/CALL - индекс в IdTable
		// 2. Для JMP/JZ - индекс инструкции в векторе ByteCode
		// 3. Для остальных - игнорируется (можно 0)
	};

	// Тип данных для всего байт-кода программы
	typedef std::vector<Instruction> ByteCode;

	// Основная функция генерации
	// На вход принимает таблицу лексем (ПОЛИЗ) и таблицу идентификаторов
	// Возвращает массив инструкций
	ByteCode Generate(LT::LexTable& lextable, IT::IdTable& idtable);
	void Debug(ByteCode& bytecode, IT::IdTable& idtable);
}