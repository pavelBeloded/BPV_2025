#include "stdafx.h"
#include "Semantics.h"
#include "Error.h"
#include <stack>
#include <string>
#include <vector>
#include <sstream>

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
	int GetExpectedParamCount(IT::IdTable& idtable, int funcIdx) {
		IT::Entry funcEntry = IT::GetEntry(idtable, funcIdx);
		std::string funcName(funcEntry.id);

		for (const auto& lib : stdLib) {
			if (lib.name == funcName) return (int)lib.paramTypes.size();
		}

		int count = 0;
		std::string prefix = funcName + "$";
		for (int i = 0; i < idtable.size; i++) {
			if (idtable.table[i].idtype == IT::P) {
				std::string varName(idtable.table[i].id);
				if (varName.find(prefix) == 0) {
					count++;
				}
			}
		}
		return count;
	}

	void CheckParamCount(LT::LexTable& lextable, IT::IdTable& idtable) {
		for (int i = 0; i < lextable.size; i++) {

			if (lextable.table[i].lexema == LEX_ID) {

				if (i > 0 && lextable.table[i - 1].lexema == LEX_FUNCTION) continue;

				int idxTI = lextable.table[i].idxTI;
				IT::Entry itEntry = IT::GetEntry(idtable, idxTI);

				if (itEntry.idtype == IT::F) {

					if (i + 1 >= lextable.size || lextable.table[i + 1].lexema != LEX_LEFTHESIS) {
						continue;
					}

					int actualCount = 0;
					int nextLexIdx = i + 2;    

					if (lextable.table[nextLexIdx].lexema == LEX_RIGHTHESIS) {
						actualCount = 0;
					}
					else {
						actualCount = 1;
						int balance = 0;         

						for (int j = nextLexIdx; j < lextable.size; j++) {
							char lex = lextable.table[j].lexema;

							if (lex == LEX_LEFTHESIS) {
								balance++;
							}
							else if (lex == LEX_RIGHTHESIS) {
								if (balance == 0) {
									break;
								}
								balance--;
							}
							else if (lex == LEX_COMMA) {
								if (balance == 0) {
									actualCount++;
								}
							}
							else if (lex == LEX_SEMICOLON) {
								break;
							}
						}
					}

					int expectedCount = GetExpectedParamCount(idtable, idxTI);

					if (actualCount != expectedCount) {
						throw ERROR_THROW_IN(306, lextable.table[i].sn, -1);
					}
				}
			}
		}
	}
	bool Analyze(LT::LexTable& lextable, IT::IdTable& idtable, Log::LOG log) {
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

				if (i > 0 && lextable.table[i - 1].lexema == LEX_TYPE &&
					i + 1 < lextable.size && lextable.table[i + 1].lexema == LEX_SEMICOLON) {
					break;
				}

				IT::Entry itEntry = IT::GetEntry(idtable, entry.idxTI);

				if (itEntry.idtype == IT::F) {
					std::vector<IT::IDDATATYPE> expectedParams = GetFunctionParams(idtable, entry.idxTI);

					if (typeStack.size() < expectedParams.size()) {
						throw ERROR_THROW_IN(306, entry.sn, -1);
					}

					for (int p = (int)expectedParams.size() - 1; p >= 0; p--) {
						IT::IDDATATYPE argType = typeStack.top();
						typeStack.pop();

						if (argType != expectedParams[p]) {
							throw ERROR_THROW_IN(604, entry.sn, -1);
						}
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
				if (typeStack.size() < 2) throw ERROR_THROW_IN(602, entry.sn, -1);
				IT::IDDATATYPE right = typeStack.top(); typeStack.pop();
				IT::IDDATATYPE left = typeStack.top(); typeStack.pop();

				if (left != right) throw ERROR_THROW_IN(303, entry.sn, -1);
				break;
			}

			case LEX_PRINT:
			case LEX_RETURN:
			case LEX_WHILE: {
				if (typeStack.empty()) throw ERROR_THROW_IN(602, entry.sn, -1);
				typeStack.pop();
				break;
			}

			case LEX_SEMICOLON:
			case LEX_BRACELET:
			{
				if (typeStack.size() > 1) {
					while (!typeStack.empty()) typeStack.pop();

					throw ERROR_THROW_IN(603, entry.sn, -1);
				}

				while (!typeStack.empty()) typeStack.pop();
				break;
			}

			case LEX_LEFTBRACE:
			case '#':
				while (!typeStack.empty()) typeStack.pop();
				break;
			}
		}
		return true;
	}
}
