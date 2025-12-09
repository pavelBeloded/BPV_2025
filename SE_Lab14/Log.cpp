#include "stdafx.h"
#include <fstream>
#include <cwctype>
#include <cstdarg>
#include <ctime>
#include <iostream>
#include <iomanip>
#include "Log.h"
#include "Parm.h"
#include "Error.h"

#define outfile *log.stream

namespace Log {
	LOG getlog(wchar_t logfile[]) {
		LOG log = INITLOG;
		log.stream = new std::ofstream(logfile, std::ios::out | std::ios::trunc);
		if (!log.stream->is_open()) {
			throw ERROR_THROW(112);
		}
		wcscpy_s(log.logfile, logfile);
		return log;
	}

	void WriteLine(LOG log, const char* c, ...) {
		if (!log.stream || !log.stream->is_open()) return;

		va_list args;
		va_start(args, c);

		const char* current_str = c;
		while (current_str != nullptr) {
			*log.stream << current_str;
			current_str = va_arg(args, const char*);
		}

		va_end(args);
		*log.stream << std::endl;
	}

	void WriteLine(LOG log, wchar_t* c, ...) {
		if (!log.stream || !log.stream->is_open()) return;

		va_list args;
		va_start(args, c);

		wchar_t* wstr = c;
		while (wstr != nullptr && wstr[0] != L'\0') {
			size_t requiredSize = wcslen(wstr) * 4 + 1;
			char* buffer = new char[requiredSize];
			size_t convertedLength = 0;
			errno_t err = wcstombs_s(&convertedLength, buffer, requiredSize, wstr, _TRUNCATE);
			if (err == 0) (*log.stream) << buffer;
			delete[] buffer;
			wstr = va_arg(args, wchar_t*);
		}
		va_end(args);
		(*log.stream) << std::endl;
	}

	void WriteLog(LOG log) {
		if (!log.stream || !log.stream->is_open()) return;
		time_t now = time(nullptr);
		struct tm localTime;
		localtime_s(&localTime, &now);
		char timeBuffer[100];
		strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", &localTime);
		*log.stream << "-----  Протокол ----- " << timeBuffer << "------\n";
	}

	void WriteParm(LOG log, Parm::PARM parm) {
		if (!log.stream || !log.stream->is_open()) return;
		*log.stream << "----- Параметры -----" << std::endl;
		char buffer[PARM_MAX_SIZE * 4];
		auto convert = [&](const wchar_t* src) -> const char* {
			std::memset(buffer, 0, sizeof(buffer));
			size_t converted = 0;
			errno_t err = wcstombs_s(&converted, buffer, sizeof(buffer), src, _TRUNCATE);
			return (err == 0) ? buffer : "[ошибка]";
			};
		*log.stream << "-log: " << convert(parm.log) << std::endl;
		*log.stream << "-in : " << convert(parm.in) << std::endl;
		*log.stream << "-out: " << convert(parm.out) << std::endl;
	}

	void WriteIn(LOG log, In::IN in) {
		if (!log.stream || !log.stream->is_open()) return;
		*log.stream << "---- Статистика -----" << std::endl;
		*log.stream << "Количество символов: " << in.size << std::endl;
		*log.stream << "Проигнорировано: " << in.ignore << std::endl;
		*log.stream << "Количество строк: " << in.lines << std::endl;
	}

	void WriteError(LOG log, Error::ERROR error) {
		if (!log.stream || !log.stream->is_open()) {
			std::cout << "Ошибка " << error.id << ": " << error.message;
			if (error.inext.line != -1) {
				std::cout << " (строка " << error.inext.line << ", позиция " << error.inext.col << ")";
			}
			std::cout << std::endl;
		}
		else {
			*log.stream << "Ошибка " << error.id << ": " << error.message;
			if (error.inext.line != -1) {
				*log.stream << " (строка " << error.inext.line << ", позиция " << error.inext.col << ")";
			}
			*log.stream << std::endl;
		}
	}

	void WriteIT(LOG log, IT::IdTable& idtable) {
		if (!log.stream || !log.stream->is_open()) return;   
		outfile << "---- Таблица идентификаторов ----" << std::endl;
		outfile << std::left << std::setw(4) << "No" << "|" << std::setw(15) << "ID" << "|" << std::setw(7) << "Type" << "|" << std::setw(10) << "DataType" << "|" << std::setw(11) << "Idx" << "|" << "Value" << std::endl;
		outfile << std::setfill('-') << std::setw(60) << "-" << std::setfill(' ') << std::endl;
		for (int i = 0; i < idtable.size; i++) {
			IT::Entry entry = idtable.table[i];
			std::string typeStr;
			switch (entry.idtype) { case IT::V: typeStr = "Var"; break; case IT::F: typeStr = "Func"; break; case IT::P: typeStr = "Param"; break; case IT::L: typeStr = "Lit"; break; default: typeStr = "?"; break; }
											  std::string dataStr;
			switch (entry.iddatatype) { case IT::INT: dataStr = "uint"; break; case IT::STR: dataStr = "text"; break; default: dataStr = "?"; break; }

			outfile << std::setw(3) << i << " |" << std::setw(15) << entry.id << "|" << std::setw(7) << typeStr << "|" << std::setw(10) << dataStr << "|" << std::setw(11) << entry.idxfirstLE << "|";
			if (entry.idtype == IT::L) {
			if (entry.iddatatype == IT::INT) outfile << entry.value.vint;
			else if (entry.iddatatype == IT::STR) outfile << "'" << entry.value.vstr.str << "'";
			}
			outfile << std::endl;
		}
		outfile << std::endl;
	}

	void WriteLT(LOG log, LT::LexTable& lextable) {
		if (!log.stream || !log.stream->is_open()) return;   
		outfile << "---- Таблица лексем ----" << std::endl;
		int size = lextable.size;
		int lastLine = -1;
		for (int i = 0; i < size; i++) {
			int currentLine = lextable.table[i].sn;
			if (currentLine != lastLine) {
				if (i > 0) outfile << std::endl;
				outfile << std::setfill('0') << std::setw(3) << currentLine << " ";
				lastLine = currentLine;
			}
			outfile << lextable.table[i].lexema;
		}
		outfile << std::endl;
	}

	void Close(LOG log) {
		if (log.stream && log.stream->is_open()) {
			log.stream->close();
			delete log.stream;
		}
	}
}