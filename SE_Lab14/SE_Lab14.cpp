//#include <iostream>
//#include <cwchar>
//#include <vector>
//#include <functional>
//#include "stdafx.h"
//#include "Parm.h"
//#include "Log.h"
//#include "Error.h"
//#include "IT.h"
//#include "LT.h"
//
//
//void test_IT_200_Overflow() {
//    IT::IdTable idtable = IT::Create(5);
//    for (int i = 0; i < 6; ++i) {
//        IT::AddId(idtable, "var" + std::to_string(i), IT::INT, IT::V, i);
//    }
//}
//
//void test_IT_202_NotFound() {
//    IT::IdTable idtable = IT::Create(5);
//    IT::AddId(idtable, "var", IT::INT, IT::V, 1);
//    IT::GetEntry(idtable, 99);
//}
//
//void test_IT_203_InvalidEntry() {
//    IT::IdTable idtable = IT::Create(5);
//    std::string long_literal(TI_STR_MAXSIZE + 5, 'a');
//    IT::AddStringLiteral(idtable, long_literal, 1);
//}
//
//void test_IT_204_SizeExceeded() {
//    IT::Create(TI_MAXSIZE + 1);
//}
//
//void test_IT_205_InvalidSize() {
//    IT::Create(-1);
//}
//
//void test_IT_206_CreationFailed() {
//    IT::Create(2000000000);
//}
//
//void test_IT_207_EmptyId() {
//    IT::IdTable idtable = IT::Create(5);
//    IT::AddId(idtable, "", IT::INT, IT::V, 1);
//}
//
//void test_LT_210_Overflow() {
//    LT::LexTable lextable = LT::Create(5);
//    for (int i = 0; i < 6; ++i) {
//        LT::Add(lextable, { LEX_ID, i, i });
//    }
//}
//
//void test_LT_211_InvalidSize() {
//    LT::Create(0);
//}
//
//void test_LT_212_IndexOutOfBounds() {
//    LT::LexTable lextable = LT::Create(5);
//    LT::Add(lextable, { LEX_ID, 1, 1 });
//    LT::GetEntry(lextable, -5);
//}
//
//void test_LT_214_CreationFailed() {
//    LT::Create(2000000000);
//}
//
//int wmain(int argc, wchar_t* argv[]) {
//    setlocale(LC_ALL, "Ru");
//        Log::LOG log;
//        Out::OUT out;
//        Parm::PARM parm{};
//            parm = Parm::getparm(argc, argv);
//            log = Log::getlog(parm.log);
//            out = Out::getout(parm.out);
//            Log::WriteLog(log);
//            Log::WriteParm(log, parm);
//            In::IN in = In::getin(parm.in);
//            Log::WriteIn(log, in);
//            In::RemoveExtraSpaces(in);
//            //std::cout << in.text;
//            try {
//                std::cout << "Запущен тест обработки ошибок. Результаты будут записаны в файл " <<  std::endl;
//            }
//            catch (Error::ERROR e) {
//                std::cout << "Критическая ошибка: не удалось создать файл лога. " << e.message << std::endl;
//                return 1;
//            }
//
//            std::vector<std::pair<std::function<void()>, const char*>> tests = {
//                { test_IT_200_Overflow,       "IT.200: Переполнение таблицы идентификаторов" },
//                { test_IT_202_NotFound,       "IT.202: Запрос идентификатора по неверному индексу" },
//                { test_IT_203_InvalidEntry,   "IT.203: Добавление слишком длинного строкового литерала" },
//                { test_IT_204_SizeExceeded,   "IT.204: Создание таблицы идентификаторов больше максимальной" },
//                { test_IT_205_InvalidSize,    "IT.205: Создание таблицы идентификаторов с некорректным размером" },
//                { test_IT_206_CreationFailed, "IT.206: Ошибка выделения памяти для таблицы идентификаторов" },
//                { test_IT_207_EmptyId,        "IT.207: Попытка добавить пустой идентификатор" },
//
//                { test_LT_210_Overflow,         "LT.210: Переполнение лексической таблицы" },
//                { test_LT_211_InvalidSize,      "LT.211: Создание лексической таблицы с некорректным размером" },
//                { test_LT_212_IndexOutOfBounds, "LT.212: Запрос лексемы по неверному индексу" },
//                { test_LT_214_CreationFailed,   "LT.214: Ошибка выделения памяти для лексической таблицы" }
//            };
//
//            for (const auto& test : tests) {
//                Log::WriteLine(log, "--- Запуск теста: ", test.second, " ---", nullptr);
//                try {
//                    test.first();
//                    Log::WriteLine(log, "!!! ПРОВАЛ ТЕСТА: Ожидаемое исключение не было вызвано.", nullptr);
//                }
//                catch (Error::ERROR error) {
//                    Log::WriteError(log, error);
//                }
//                catch (...) {
//                    Log::WriteLine(log, "!!! ПРОВАЛ ТЕСТА: Было вызвано непредвиденное исключение.", nullptr);
//                }
//                Log::WriteLine(log, "--- Тест завершен ---\n", nullptr);
//            }
//    Log::WriteLine(log, "====== ВСЕ ТЕСТЫ ОБРАБОТКИ ОШИБОК ЗАВЕРШЕНЫ ======", nullptr);
//    Log::Close(log);
//    system("pause");
//    return 0;
//}








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
        LT::FillLTIT(lextable, idtable, in);

        Log::WriteLT(log, lextable);
        Log::WriteIT(log, idtable);
        
        /*int sn = lextable.table[lextable.size - 1].sn;
        LT::Add(lextable, {'$', ++sn, LT_TI_NULLIDX });*/
        MFST_TRACE_START;
        MFST::Mfst mfst(lextable, GRB::getGreibach());
        mfst.start();

        mfst.savededucation();
        mfst.printrules();

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


