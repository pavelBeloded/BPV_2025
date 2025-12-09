#include <iostream>
#include <cwchar>
#include "stdafx.h"


int wmain(int argc, wchar_t* argv[]) {
	setlocale(LC_ALL, "Ru");
	Log::LOG log;
	Out::OUT out;
	Parm::PARM parm{};
	try {
		parm = Parm::getparm(argc, argv);
		log = Log::getlog(parm.log);
		out = Out::getout(parm.out);
		Log::WriteLog(log);
		Log::WriteParm(log, parm);
		In::IN in = In::getin(parm.in);
		Log::WriteIn(log, in);
		In::RemoveExtraSpaces(in);

		LT::LexTable lextable = LT::Create(LT_MAXSIZE);
		IT::IdTable idtable = IT::Create(TI_MAXSIZE);

		Semantics::InjectStandardLibrary(idtable);

		LT::FillLTIT(lextable, idtable, in);

		Log::WriteLT(log, lextable);
		Log::WriteIT(log, idtable);

		MFST_TRACE_START;
		MFST::Mfst mfst(lextable, GRB::getGreibach(), log);
		if (!mfst.start()) {
			throw ERROR_THROW(600); 
		}

		mfst.savededucation();
		mfst.printrules();
		Semantics::CheckParamCount(lextable, idtable);
		Log::WriteLine(log, "\n--- Построение ПОЛИЗ ---", nullptr);
		bool polish_ok = true;
		for (int i = 0; i < lextable.size; i++) {
			if (lextable.table[i].lexema == LEX_ASSIGN) {
				int next = Polish::PolishNotation(i - 1, lextable, idtable, log);
				if (next == -1) polish_ok = false; else i = next;
			}
			else if (lextable.table[i].lexema == LEX_RETURN ||
				lextable.table[i].lexema == LEX_PRINT ||
				lextable.table[i].lexema == LEX_WHILE) {
				int next = Polish::PolishNotation(i, lextable, idtable, log);
				if (next == -1) polish_ok = false; else i = next;
			}
		}
		if (!polish_ok) throw ERROR_THROW(700);

		Log::WriteLT(log, lextable);


		Log::WriteLine(log, "\n--- Семантический анализ ---", nullptr);
		if (Semantics::Analyze(lextable, idtable, log)) {
			Log::WriteLine(log, "Семантический анализ прошел успешно.", nullptr);
		}

		Log::WriteLine(log, "\n--- Генерация байт-кода ---", nullptr);
		CodeGen::ByteCode bytecode = CodeGen::Generate(lextable, idtable);
		CodeGen::Debug(bytecode, idtable, log);

		Log::WriteLine(log, "\n--- Выполнение (Интерпретатор) ---", nullptr);
		Executor::Execute(bytecode, idtable, log);


		Log::WriteLine(log, "\n--- Генерация ASM ---", nullptr);
		Gener::CodeGeneration(bytecode, idtable, parm.out);

		//Out::WriteIn(out, in);
	}
	catch (Error::ERROR error) {
		if (log.stream == nullptr) {
			if (parm.in[0] == '\0') {
				wcscpy_s(parm.log, L"logs.txt");
			}
			log = Log::getlog(parm.log);
			Log::WriteLog(log);
		}
		Log::WriteError(log, error);
		if (error.id == 111) {
			Out::WriteError(out, error);
		}
	}
	Log::Close(log);
	system("pause");
	return 0;
}