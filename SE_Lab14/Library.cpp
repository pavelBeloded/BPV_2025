#include "stdafx.h"
#include <iostream>
#include <ctime>
#include <string>

// extern "C" нужен, чтобы функции имели простые имена для Ассемблера
// __stdcall нужен, чтобы функции сами чистили стек (стандарт WINAPI)

extern "C" {

	// length(text) -> uint
	int __stdcall length(char* s) {
		if (s == nullptr) return 0;
		return std::strlen(s);
	}

	// tostr(uint) -> text
	// ВНИМАНИЕ: Возвращаем указатель на статический буфер. 
	// В многопоточной среде это плохо, но для курсового - стандарт.
	char* __stdcall tostr(int n) {
		static char buf[255];
		sprintf_s(buf, "%d", n);
		return buf;
	}

	// touint(text) -> uint
	int __stdcall touint(char* s) {
		if (s == nullptr) return 0;
		try {
			return std::stoi(s);
		}
		catch (...) {
			return 0;
		}
	}

	// date() -> text
	char* __stdcall date() {
		static char buf[20];
		time_t now = time(0);
		struct tm tstruct;
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%d.%m.%Y", &tstruct);
		return buf;
	}

	// time() -> text
	char* __stdcall get_time() { // time - зарезервировано, назовем get_time (в IT переименуем)
		static char buf[20];
		time_t now = time(0);
		struct tm tstruct;
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
		return buf;
	}

	// substr(text, start, len) -> text
	char* __stdcall substr(char* s, int start, int len) {
		static char buf[255];
		if (s == nullptr || start < 0 || len <= 0) {
			buf[0] = '\0';
			return buf;
		}

		int strLen = std::strlen(s);
		if (start >= strLen) {
			buf[0] = '\0';
			return buf;
		}

		// Защита от переполнения
		if (start + len > strLen) {
			len = strLen - start;
		}

		// Копируем
		strncpy_s(buf, s + start, len);
		buf[len] = '\0'; // Терминатор
		return buf;
	}

	// Функция паузы (полезно для консоли)
	void __stdcall pause_prog() {
		system("pause");
	}
}