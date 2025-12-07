#pragma once
#include "stdafx.h"
#include <stack>
#include <iostream>
#include <iomanip> 
#include <sstream> 
#include "Log.h"   

#define MFST_DIAGN_NUMBER 3
#define MFST_DIAGN_MAXSIZE 2*ERROR_MAXSIZE_MESSAGE

#define MFST_TRACE_START log.stream != nullptr ? Log::WriteLine(log, "--- MFST TRACE START ---", nullptr) : void()

#define NS(n)       GRB::Rule::Chain::N(n)
#define TS(n)       GRB::Rule::Chain::T(n)
#define ISNS(n)     GRB::Rule::Chain::isN(n)

#define MFST_TRACE1     printTrace(1);
#define MFST_TRACE2     printTrace(2);
#define MFST_TRACE3     printTrace(3);
#define MFST_TRACE4(c)  printTrace(4, c);
#define MFST_TRACE5(c)  printTrace(5, c);
#define MFST_TRACE6(c,k) printTrace(6, c, k);
#define MFST_TRACE7     printTrace(7);


class MFSTSTACK : public std::stack<short> {
public:
	using std::stack<short>::c;
};

namespace MFST {

	struct MfstState {
		short tape_position;
		short nrule;
		short nrulechain;
		MFSTSTACK st;
		MfstState();
		MfstState(short position, MFSTSTACK stack, short pnrulechain);
		MfstState(short position, MFSTSTACK stack, short pnrule, short pnrulechain);
	};

	class MFSTSTATE : public std::stack<MfstState> {
	public:
		using std::stack<MfstState>::c;
	};

	struct Mfst {
		enum RC_STEP {
			NS_OK,
			NS_NORULE,
			NS_NORULECHAIN,
			NS_ERROR,
			TS_OK,
			TS_NOK,
			TAPE_END,
			SURPRISE
		};

		struct MfstDiagnosis {
			short tape_position;
			RC_STEP rc_step;
			short nrule;
			short nrule_chain;
			MfstDiagnosis();
			MfstDiagnosis(short position, RC_STEP prc_step, short pnrule, short pnrule_chain);
		} diagnosis[MFST_DIAGN_NUMBER];

		GRBALPHABET* tape;
		short tape_position;
		short nrule;
		short nrulechain;
		short tape_size;
		GRB::Greibach grebach;
		LT::LexTable lextable;
		MFSTSTACK st;
		MFSTSTATE stateStack;

		Log::LOG log;

		Mfst();
		Mfst(LT::LexTable plextable, GRB::Greibach pgreibach, Log::LOG plog);

		char* getCSt(char* buf);
		char* getCTape(char* buf, short pos, short n = 25);
		char* getDiagnosis(short n, char* buf);
		bool savestate();
		bool reststate();
		bool push_chain(GRB::Rule::Chain chain);
		RC_STEP step();
		bool start();
		bool savediagnosis(RC_STEP prc_step);
		void printrules();

		void printTrace(int mode, const char* msg = "", int k = 0);

		struct Deducation {
			short size;
			short* nrules;
			short* nrulechains;
			Deducation() {
				size = 0;
				nrules = 0;
				nrulechains = 0;
			}
		} deducation;
		bool savededucation();
	};
}