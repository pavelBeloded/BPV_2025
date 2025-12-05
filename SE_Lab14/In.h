#pragma once
#define IN_MAX_LEN_TEXT 1024*1024
#define IN_CODE_ENDL '\n'

#define IN_CODE_TABLE {\
/* 0–15   */ In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::I,In::IN::S ,In::IN::F,In::IN::F,In::IN::I,In::IN::F,In::IN::F, \
/* 16–31  */ In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F, \
/* 32–47  */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::F,In::IN::T, \
/* 48–63  */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::F, \
/* 64–79  */ In::IN::F,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/* 80–95  */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::F,In::IN::F,In::IN::F,In::IN::F,In::IN::F, \
/* 96–111 */ In::IN::F,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/*112–127 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::F,In::IN::T,In::IN::F,In::IN::F, \
/*128–143 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/*144–159 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/*160–175 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/*176–191 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/*192–207 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/*208–223 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/*224–239 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T, \
/*240–255 */ In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T,In::IN::T  \
}



namespace In {
	struct IN {
		enum Chars {
			T = 1024,
			F = 2048,
			I = 4096,
			S = 8192
		};
		int size;
		int lines;
		int ignore;
		unsigned char* text;
		int code[256];
	};
	IN getin(wchar_t infile[]);

}