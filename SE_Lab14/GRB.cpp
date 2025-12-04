#include "GRB.h"

#define GRB_ERROR_SERIES 600

namespace GRB {
#define NS(n) Rule::Chain::N(n)
#define TS(n) Rule::Chain::T(n)

	Greibach greibach(NS('S'), TS('$'),
		{
		  Rule(NS('S'), GRB_ERROR_SERIES + 0,
			5,		//S →	m{NrE;};	|	tfi(F){NrE;};S	|	m{NrE;};S	|	tfi(F){NrE;}; | i{NrE;}
			Rule::Chain(8, TS('m'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';')),
			Rule::Chain(8, TS('i'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';')),
			Rule::Chain(14, TS('t'), TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';'), NS('S')),
			Rule::Chain(9, TS('m'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';'), NS('S')),
			Rule::Chain(13, TS('t'), TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';'))
		  ),Rule(NS('N'), GRB_ERROR_SERIES + 1,
		 10,		//N	→	dti; | rE; | i = E; | dtfi(F); | dti;N | rE;N | i=E;N | dtfi(F);N | pE; | pE;N
			Rule::Chain(4, TS('d'), TS('t'), TS('i'), TS(';')),
			Rule::Chain(4, TS('i'), TS('='), NS('E'), TS(';')),
			Rule::Chain(3, TS('r'), NS('E'), TS(';')),
			Rule::Chain(8, TS('d'), TS('t'), TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS(';')),
			Rule::Chain(5, TS('d'), TS('t'), TS('i'), TS(';'), NS('N')),
			Rule::Chain(4, TS('r'), NS('E'), TS(';'), NS('N')),
			Rule::Chain(5, TS('i'), TS('='), NS('E'), TS(';'), NS('N')),
			Rule::Chain(9, TS('d'), TS('t'), TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS(';'), NS('N')),
			Rule::Chain(3, TS('p'), NS('E'), TS(';')),
			Rule::Chain(4, TS('p'), NS('E'), TS(';'), NS('N'))
		),

		Rule(NS('E'), GRB_ERROR_SERIES + 2,
		9,		//E →	i	|	l	|	(E)	|	i(W)	|	iM	|	lM	|	(E)M	|	i(W)M|	i()
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l')),
			Rule::Chain(3, TS('('), NS('E'), TS(')')),
			Rule::Chain(4, TS('i'), TS('('), NS('W'), TS(')')),
			Rule::Chain(3, TS('i'), TS('('), TS(')')),
			Rule::Chain(2, TS('i'), NS('M')),
			Rule::Chain(2, TS('l'), NS('M')),
			Rule::Chain(4, TS('('), NS('E'), TS(')'), NS('M')),
			Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), NS('M'))
		),

		Rule(NS('F'), GRB_ERROR_SERIES + 3,
		 2,		//F	→	ti	|	ti,F
			Rule::Chain(2, TS('t'), TS('i')),
			Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('F'))
		),

		Rule(NS('W'), GRB_ERROR_SERIES + 4,
		 4,		//W	→	i	|	l	|	i,W	|	l,W
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l')),
			Rule::Chain(3, TS('i'), TS(','), NS('W')),
			Rule::Chain(3, TS('l'), TS(','), NS('W'))
		),

		Rule(NS('M'), GRB_ERROR_SERIES + 2,
			8, 
			Rule::Chain(2, TS('+'), NS('E')),
			Rule::Chain(3, TS('+'), NS('E'), NS('M')),
			Rule::Chain(2, TS('-'), NS('E')),
			Rule::Chain(3, TS('-'), NS('E'), NS('M')),
			Rule::Chain(2, TS('*'), NS('E')),
			Rule::Chain(3, TS('*'), NS('E'), NS('M')),
			Rule::Chain(2, TS('/'), NS('E')),
			Rule::Chain(3, TS('/'), NS('E'), NS('M'))
		)
		});

	Rule::Chain::Chain(short psize, GRBALPHABET s, ...) {
		this->size = psize;
		this->nt = new GRBALPHABET[psize];

		va_list args;
		va_start(args, s);
		this->nt[0] = s;
		for (int i = 1; i < psize; i++) {
			this->nt[i] = va_arg(args, GRBALPHABET);
		}
		va_end(args);
	}

	Rule::Rule(GRBALPHABET pnn, int iderror, short psize, Chain c, ...) {
		this->nn = pnn;
		this->iderror = iderror;
		this->size = psize;
		this->chains = new Chain[psize];

		va_list args;
		va_start(args, c);
		this->chains[0] = c;
		for (int i = 1; i < psize; i++) {
			this->chains[i] = va_arg(args, Chain);
		}
		va_end(args);
	}
	Greibach::Greibach(GRBALPHABET pstartN, GRBALPHABET pstbottom,std::initializer_list<Rule> rulesList)
	{
		startN = pstartN;
		stbottomT = pstbottom;
		size = static_cast<short>(rulesList.size());
		rules = new Rule[size];
		int i = 0;
		for (const Rule& r : rulesList) rules[i++] = r;
	};


	short Greibach::getRule(GRBALPHABET pnn, Rule& prule)
	{
		short rc = -1, k = 0;
		while (k < this->size && rules[k].nn != pnn)
			k++;
		if (k < this->size)
			prule = rules[rc = k];
		return rc;
	}
	Rule Greibach::getRule(short n)
	{
		Rule rc;
		if (n < this->size)
			rc = rules[n];
		return rc;
	}
	char* Rule::getCRule(char* b, short nchain)
	{
		char buf[200];
		b[0] = Chain::alphabet_to_char(this->nn);
		b[1] = '-';
		b[2] = '>';
		b[3] = 0x00;
		this->chains[nchain].getCChain(buf);
		strcat_s(b, sizeof(buf) + 5, buf);

		return b;
	}

	//short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j)
	//{
	//	short rc = -1;

	//	while (j < this->size && this->chains[j].nt[0] != t)
	//		j++;

	//	rc = (j < this->size ? j : -1);
	//	if (rc >= 0)
	//		pchain = chains[rc];
	//	return rc;
	//}

	short Rule::getNextChain(GRBALPHABET t, Chain& pchain, short j) {
		short rc = -1;
		for (short i = j; i < size; i++) {
			if (chains[i].nt[0] == t) {
				pchain = chains[i];
				return i;
			}
		}
		return -1;
	}
	char* Rule::Chain::getCChain(char* b)
	{
		for (int i = 0; i < this->size; i++)
			b[i] = Chain::alphabet_to_char(this->nt[i]);
		b[this->size] = 0;
		return b;
	}
	Greibach getGreibach()
	{
		return greibach;
	}
}
