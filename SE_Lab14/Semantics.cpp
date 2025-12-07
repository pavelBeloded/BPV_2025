#include "stdafx.h"
#include "Semantics.h"
#include "Error.h"
#include <stack>
#include <string>
#include <vector>
#include <iostream>

namespace Semantics {

	struct LibFunc {
		std::string name;
		IT::IDDATATYPE returnType;
		std::vector<IT::IDDATATYPE> paramTypes;
	};

	std::vector<LibFunc> stdLib = {
		{ "date",   IT::STR, {} },
		{ "time",   IT::STR, {} },
		{ "length", IT::INT, { IT::STR } },
		{ "tostr",  IT::STR, { IT::INT } },
		{ "touint", IT::INT, { IT::STR } },
		{ "substr", IT::STR, { IT::STR, IT::INT, IT::INT } }
	};

	void InjectStandardLibrary(IT::IdTable& idtable) {
		for (const auto& func : stdLib) {
			IT::AddId(idtable, func.name, func.returnType, IT::F, 0);
		}
	}

	std::vector<IT::IDDATATYPE> GetFunctionParams(IT::IdTable& idtable, int funcIdx) {
		if (funcIdx < 0 || funcIdx >= idtable.size) return {};

		IT::Entry funcEntry = IT::GetEntry(idtable, funcIdx);
		char cleanName[ID_MAXSIZE + 1];
		memset(cleanName, 0, ID_MAXSIZE + 1);
		strncpy_s(cleanName, funcEntry.id, ID_MAXSIZE);
		std::string funcName = cleanName;

		for (const auto& lib : stdLib) {
			if (lib.name == funcName) return lib.paramTypes;
		}

		std::vector<IT::IDDATATYPE> params;
		std::string prefix = funcName + "$";

		for (int i = 0; i < idtable.size; i++) {
			IT::Entry e = idtable.table[i];
			if (e.idtype == IT::P) {
				char varNameBuf[ID_MAXSIZE + 1];
				memset(varNameBuf, 0, ID_MAXSIZE + 1);
				strncpy_s(varNameBuf, e.id, ID_MAXSIZE);
				std::string varName = varNameBuf;

				if (varName.find(prefix) == 0) {
					params.push_back(e.iddatatype);
				}
			}
		}
		return params;
	}

	bool Analyze(LT::LexTable& lextable, IT::IdTable& idtable) {
		std::stack<IT::IDDATATYPE> typeStack;
		bool entryPointFound = false;

		for (int i = 0; i < lextable.size; i++) {
			if (lextable.table[i].lexema == LEX_MAIN) {
				entryPointFound = true;
				break;
			}
		}
		if (!entryPointFound) throw ERROR_THROW(305);

		for (int i = 0; i < lextable.size; i++) {
			LT::Entry entry = lextable.table[i];

			switch (entry.lexema) {

			case LEX_LITERAL: {
				IT::Entry itEntry = IT::GetEntry(idtable, entry.idxTI);
				typeStack.push(itEntry.iddatatype);
				break;
			}

			case LEX_ID: {
				if (i > 0 && lextable.table[i - 1].lexema == LEX_FUNCTION) break;

				IT::Entry itEntry = IT::GetEntry(idtable, entry.idxTI);

				if (itEntry.idtype == IT::F) {
					std::vector<IT::IDDATATYPE> expectedParams = GetFunctionParams(idtable, entry.idxTI);

					// --- DEBUG LOG START ---
				/*	std::cout << "[DEBUG] Call Function: " << itEntry.id
						<< " | Expected Params: " << expectedParams.size()
						<< " | Stack Size: " << typeStack.size() << std::endl;*/
					// -----------------------

					if (typeStack.size() < expectedParams.size()) {
						throw ERROR_THROW_IN(306, entry.sn, -1);
					}

					for (int p = (int)expectedParams.size() - 1; p >= 0; p--) {
						IT::IDDATATYPE argType = typeStack.top();
						typeStack.pop();
						if (argType != expectedParams[p]) throw ERROR_THROW_IN(604, entry.sn, -1);
					}
					typeStack.push(itEntry.iddatatype);
				}
				else {
					typeStack.push(itEntry.iddatatype);
				}
				break;
			}

			case LEX_PLUS: case LEX_MINUS: case LEX_STAR: case LEX_DIRSLASH: case LEX_MODULO: {
				if (typeStack.size() < 2) throw ERROR_THROW_IN(602, entry.sn, -1);
				IT::IDDATATYPE right = typeStack.top(); typeStack.pop();
				IT::IDDATATYPE left = typeStack.top(); typeStack.pop();

				if (left == IT::INT && right == IT::INT) typeStack.push(IT::INT);
				else if (entry.lexema == LEX_PLUS && left == IT::STR && right == IT::STR) typeStack.push(IT::STR);
				else throw ERROR_THROW_IN(303, entry.sn, -1);
				break;
			}

			case LEX_EQ: case LEX_NE: case LEX_LESS: case LEX_MORE: case LEX_LE: case LEX_GE: {
				if (typeStack.size() < 2) throw ERROR_THROW_IN(602, entry.sn, -1);
				IT::IDDATATYPE right = typeStack.top(); typeStack.pop();
				IT::IDDATATYPE left = typeStack.top(); typeStack.pop();
				if (left != right) throw ERROR_THROW_IN(303, entry.sn, -1);
				typeStack.push(IT::INT);
				break;
			}

			case LEX_ASSIGN: {
				if (typeStack.size() < 2) {
					// --- DEBUG LOG ---
					std::cout << "[DEBUG] Assign Error. Stack Size: " << typeStack.size() << std::endl;
					throw ERROR_THROW_IN(602, entry.sn, -1);
				}
				IT::IDDATATYPE right = typeStack.top(); typeStack.pop();
				IT::IDDATATYPE left = typeStack.top(); typeStack.pop();
				if (left != right) throw ERROR_THROW_IN(303, entry.sn, -1);
				break;
			}

			case LEX_PRINT:
			case LEX_RETURN: {
				if (typeStack.empty()) throw ERROR_THROW_IN(602, entry.sn, -1);
				typeStack.pop();
				break;
			}

			case LEX_WHILE: {
				if (typeStack.empty()) throw ERROR_THROW_IN(602, entry.sn, -1);
				typeStack.pop();
				// После while(...) мы переходим в блок, стек нужно чистить?
				// Нет, while сам по себе statement.
				break;
			}

			case '#':
			case LEX_SEMICOLON:
			case LEX_LEFTBRACE:
			case LEX_BRACELET:
				while (!typeStack.empty()) typeStack.pop();
				break;
			}
		}
		return true;
	}
}