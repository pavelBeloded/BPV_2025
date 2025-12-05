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
        MFST::Mfst mfst(lextable, GRB::getGreibach());
        mfst.start(); 

        mfst.savededucation();
        mfst.printrules();

        std::cout << "\n--- Построение ПОЛИЗ ---" << std::endl;
        bool polish_ok = true;
        for (int i = 0; i < lextable.size; i++) {

            // Если нашли =
            if (lextable.table[i].lexema == LEX_ASSIGN) {
                // Вызываем для (i-1) и получаем позицию конца
                int next_pos = Polish::PolishNotation(i - 1, lextable, idtable);

                if (next_pos == -1) {
                    polish_ok = false;
                }
                else {
                    // ПЕРЕПРЫГИВАЕМ обработанный кусок!
                    // next_pos указывает на ';', цикл сделает i++, и мы окажемся на следующей инструкции
                    i = next_pos;
                }
            }
            // Если нашли return, echo, while
            else if (lextable.table[i].lexema == LEX_RETURN ||
                lextable.table[i].lexema == LEX_PRINT ||
                lextable.table[i].lexema == LEX_WHILE) {

                int next_pos = Polish::PolishNotation(i, lextable, idtable);

                if (next_pos == -1) {
                    polish_ok = false;
                }
                else {
                    i = next_pos; // ПЕРЕПРЫГИВАЕМ
                }
            }
        }

        if (!polish_ok) {
            throw ERROR_THROW(700); 
        }

        Log::WriteLine(log, "\n--- Таблица лексем после ПОЛИЗ ---", nullptr);
        Log::WriteLT(log, lextable);

        std::cout << "\n--- Семантический анализ ---" << std::endl;
        if (Semantics::Analyze(lextable, idtable)) {
            std::cout << "Семантический анализ прошел успешно." << std::endl;
            Log::WriteLine(log, "Семантический анализ прошел успешно.", nullptr);
        }

        // 7. Генерация кода (Следующий этап)
        // ...

        Out::WriteIn(out, in);
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