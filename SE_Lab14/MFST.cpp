#pragma once

#include "stdafx.h"
#include <iostream>
#include <iomanip> 
using namespace GRB;

int FST_TRACE_n = -1;
char rbuf[205], sbuf[205], lbuf[1024];

namespace MFST {
	MfstState::MfstState() {
		tape_position = 0;
		nrule = -1;
		nrulechain = -1;
	}

	MfstState::MfstState(short pposition, MFSTSTACK pst, short pnrulechain)
	{
		tape_position = pposition;
		st = pst;
		nrulechain = pnrulechain;
	};

	MfstState::MfstState(short pposition, MFSTSTACK pst, short pnrule, short pnrulechain)
	{
		tape_position = pposition;
		st = pst;
		nrule = pnrule;
		nrulechain = pnrulechain;
	};

	Mfst::MfstDiagnosis::MfstDiagnosis() {
		tape_position = -1;
		rc_step = SURPRISE;
		nrule = -1;
		nrule_chain = -1;
	}

	Mfst::MfstDiagnosis::MfstDiagnosis(short position, RC_STEP prc_step, short pnrule, short pnrule_chain){
		tape_position = position;
		rc_step = prc_step;
		nrule = pnrule;
		nrule_chain = pnrule_chain;
	}

	Mfst::Mfst() {
		tape = 0;
		tape_size = tape_position = 0;
	}

	Mfst::Mfst(LT::LexTable plextable, GRB::Greibach pgreibach) {
		grebach = pgreibach;
		lextable = plextable;
		tape = new short[tape_size = lextable.size];

		for (int i = 0; i < lextable.size; i++) {
			tape[i] = GRB::Rule::Chain::T(lextable.table[i].lexema);
		}

		tape_position = 0;
		st.push(grebach.stbottomT);  
		st.push(grebach.startN);
		nrulechain = -1;
	}
	/*Mfst::RC_STEP Mfst::step() {
		RC_STEP rc = SURPRISE;
		if (tape_position < tape_size) {
			if (Rule::Chain::isN(st.top())) {
				GRB::Rule rule;
				if ((nrule = greibach.getRule(st.top(), rule)) >= 0) {
					GRB::Rule::Chain chain;
					if ((nrulechain = rule.getNextChain(tape[tape_position], chain, nrulechain + 1)) >= 0) {
						MFST_TRACE1;
						savestate();
						st.pop();
						push_chain(chain);
						rc = NS_OK;
						MFST_TRACE2;
					}
					else rc = NS_ERROR;
				}
				else if (st.top() == tape[tape_position]) {
					tape_position++;
					st.pop();
					nrulechain = -1;
					rc = TS_OK;
					MFST_TRACE3;
				}
				else {
					MFST_TRACE4("TS_NOK/NS_NORULECHAIN");
					rc = reststate() ? TS_OK : NS_NORULECHAIN;
				}
			}
			else {
				rc = TAPE_END;
				MFST_TRACE4("TAPE_END");
			}
			return rc;
		}
	}*/

	Mfst::RC_STEP Mfst::step()
	{
		RC_STEP rc = SURPRISE;

		//cout << "DEBUG: tape[" << tape_position << "] = "
		//    << GRB::Rule::Chain::alphabet_to_char(tape[tape_position])
		//    << ", stack top = "
		//    << GRB::Rule::Chain::alphabet_to_char(st.top())
		//    << endl;

		if (tape_position < tape_size)
		{

			if (ISNS(st.top()))
			{
				GRB::Rule rule;
				if ((nrule = grebach.getRule(st.top(), rule)) >= 0)
				{
					GRB::Rule::Chain chain;
					if ((nrulechain = rule.getNextChain(tape[tape_position], chain, nrulechain + 1)) >= 0)
					{
						MFST_TRACE1
							savestate(); st.pop(); push_chain(chain); rc = NS_OK;
						MFST_TRACE2
					}
					else
					{
						MFST_TRACE4("TNS_NORULECHAIN/NS_NORULE");
							savediagnosis(NS_NORULECHAIN); rc = reststate() ? NS_NORULECHAIN : NS_NORULE;
					}
				}
				else
				{
					rc = NS_ERROR;
				}
			}
			else if ((st.top() == tape[tape_position]))
			{
				tape_position++; st.pop(); nrulechain = -1; rc = TS_OK;
				MFST_TRACE3
			}
			else { MFST_TRACE4(TS_NOK / NS_NORULECHAIN) rc = reststate() ? TS_NOK : NS_NORULECHAIN; };
		}
		else {
			rc = TAPE_END;
			MFST_TRACE4("LENTA_END")
		}
		return rc;
	};


	bool Mfst::push_chain(GRB::Rule::Chain chain) {
		for (int i = chain.size - 1; i >= 0; i--) {
			st.push(chain.nt[i]);
		}

		return true;
	}

	bool Mfst::savestate() {
		stateStack.push(MfstState(tape_position, st, nrule, nrulechain));
		MFST_TRACE6("SAVESTATE:", stateStack.size());
		/*std::cout << "  [DBG] saved: pos=" << tape_position << " nrule=" << nrule << " nrch=" << nrulechain << " stk=" << st.size() << std::endl;*/
		return true;
	}
	bool Mfst::reststate() {
		bool rc = false;
		MfstState state;
		if (rc = (stateStack.size() > 0)) {
			state = stateStack.top();
			tape_position = state.tape_position;
			st = state.st;
			nrule = state.nrule;
			nrulechain = state.nrulechain;
			stateStack.pop();
			MFST_TRACE5("RESSTATE")
				MFST_TRACE2
			/*std::cout << "  [DBG] restored: pos=" << tape_position << " nrule=" << nrule << " nrch=" << nrulechain << " stk=" << st.size() << std::endl;*/
		}
		return rc;
	}

	bool Mfst::savediagnosis(RC_STEP prc_step) {
		bool rc = false;
		short k = 0;
		while (k < MFST_DIAGN_NUMBER && tape_position <= diagnosis[k].tape_position) k++;

		if (rc = (k < MFST_DIAGN_NUMBER)) {
			diagnosis[k] = MfstDiagnosis(tape_position, prc_step, nrule, nrulechain);
			for (short j = k + 1; j < MFST_DIAGN_NUMBER; j++) {
				diagnosis[j].tape_position = -1;
			}
		}
		return rc;
	}

	bool Mfst::start() {
		bool rc = false;
		RC_STEP rc_step = SURPRISE;
		char buf[MFST_DIAGN_MAXSIZE]{};
		rc_step = step();
		while (rc_step == NS_OK || rc_step == NS_NORULECHAIN || rc_step == TS_OK || rc_step == TS_NOK) {
			rc_step = step();
		}

		switch (rc_step)
		{
		case TAPE_END:
		{
			MFST_TRACE4("------>LENTA_END")
				std::cout << "------------------------------------------------------------------------------------------   ------" << std::endl;
			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d: всего строк %d, синтаксический анализ выполнен без ошибок", 0, lextable.table[lextable.size - 1].sn);
			std::cout << std::setw(4) << std::left << 0 << "всего строк " << lextable.table[lextable.size - 1].sn << ", синтаксический анализ выполнен без ошибок" << std::endl;
			rc = true;
			break;
		}

		case NS_NORULE:
		{
			MFST_TRACE4("------>NS_NORULE")
				std::cout << "------------------------------------------------------------------------------------------   ------" << std::endl;
			std::cout << getDiagnosis(0, buf) << std::endl;
			std::cout << getDiagnosis(1, buf) << std::endl;
			std::cout << getDiagnosis(2, buf) << std::endl;
			break;
		}

		case NS_NORULECHAIN:	MFST_TRACE4("------>NS_NORULECHAIN") break;
		case NS_ERROR:			MFST_TRACE4("------>NS_ERROR") break;
		case SURPRISE:			MFST_TRACE4("------>NS_SURPRISE") break;


		}
		return rc;
	}

	char* Mfst::getCSt(char* buf) {
		short p;
		for (int k = (signed)st.size() - 1; k >= 0; --k)
		{
			p = st.c[k];
			buf[st.size() - 1 - k] = GRB::Rule::Chain::alphabet_to_char(p);
		}
		buf[st.size()] = '\0';
		return buf;
	}

	char* Mfst::getCTape(char* buf, short pos, short n) {
		short i = 0, k = (pos + n < tape_size) ? pos + n : tape_size;

		for (int i = pos; i < k; i++)
			buf[i - pos] = GRB::Rule::Chain::alphabet_to_char(tape[i]);

		buf[i - pos] = '\0';
		return buf;
	}

	char* Mfst::getDiagnosis(short n, char* buf) {
		char* rc = new char[200] {};
		int errid = 0;
		int lpos = -1;
		if (n < MFST_DIAGN_NUMBER && (lpos = diagnosis[n].tape_position) >= 0)
		{
			errid = grebach.getRule(diagnosis[n].nrule).iderror;
			Error::ERROR err = Error::geterror(errid);
			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d: строка %d,%s", err.id, lextable.table[lpos].sn, err.message);
			rc = buf;
		}
		return rc;
	}

	void Mfst::printrules() {
		MfstState state;
		GRB::Rule rule;
		for (unsigned short i = 0; i < stateStack.size(); i++) {
			state = stateStack.c[i];
			rule = grebach.getRule(state.nrule);
			MFST_TRACE7;
		}
	}

	bool Mfst::savededucation() {
		MfstState state;
		GRB::Rule rule;
		deducation.nrules = new short[deducation.size = stateStack.size()];
		deducation.nrulechains = new short[deducation.size];

		for (unsigned short i = 0; i < stateStack.size(); i++)
		{
			state = stateStack.c[i];
			deducation.nrules[i] = state.nrule;
			deducation.nrulechains[i] = state.nrulechain;
		}
		return true;
	} 


}