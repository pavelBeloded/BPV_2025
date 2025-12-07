#include "stdafx.h"
#include <iostream>
#include <ctime>
#include <string>

extern "C" {

	int __stdcall length(char* s) {
		if (s == nullptr) return 0;
		return std::strlen(s);
	}

	char* __stdcall tostr(int n) {
		static char buf[255];
		sprintf_s(buf, "%d", n);
		return buf;
	}

	int __stdcall touint(char* s) {
		if (s == nullptr) return 0;
		try { return std::stoi(s); }
		catch (...) { return 0; }
	}

	char* __stdcall date() {
		static char buf[20];
		time_t now = time(0);
		struct tm tstruct;
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%d.%m.%Y", &tstruct);
		return buf;
	}

	char* __stdcall get_time() {
		static char buf[20];
		time_t now = time(0);
		struct tm tstruct;
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
		return buf;
	}

	char* __stdcall substr(char* s, int start, int len) {
		static char buf[255];
		if (s == nullptr || start < 0 || len <= 0) { buf[0] = '\0'; return buf; }
		int strLen = std::strlen(s);
		if (start >= strLen) { buf[0] = '\0'; return buf; }
		if (start + len > strLen) len = strLen - start;
		strncpy_s(buf, s + start, len);
		buf[len] = '\0';
		return buf;
	}

	void __stdcall pause_prog() {
		system("pause");
	}
}