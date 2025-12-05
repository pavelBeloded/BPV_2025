#include "GRB.h"

#define GRB_ERROR_SERIES 600

namespace GRB {
#define NS(n) Rule::Chain::N(n)
#define TS(n) Rule::Chain::T(n)

	Greibach greibach(NS('S'), TS('$'),
		{
			// S - Стартовое правило
			// Теперь здесь 6 цепочек, покрывающих все случаи
			Rule(NS('S'), GRB_ERROR_SERIES + 0,
			6,
			// 1. entry { N ret E ; } ;  (Точка входа с телом)
			Rule::Chain(8, TS('m'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';')),

			// 2. entry { ret E ; } ;    (Точка входа БЕЗ тела, сразу возврат)
			Rule::Chain(7, TS('m'), TS('{'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';')),

			// 3. proc func(F) { N ret E; }; S  (Функция с ПАРАМЕТРАМИ и ТЕЛОМ)
			Rule::Chain(14, TS('t'), TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';'), NS('S')),

			// 4. proc func(F) { ret E; }; S    (Функция с ПАРАМЕТРАМИ, но БЕЗ ТЕЛА)
			Rule::Chain(13, TS('t'), TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';'), NS('S')),

			// 5. proc func() { N ret E; }; S   (Функция БЕЗ ПАРАМЕТРОВ, с ТЕЛОМ)
			Rule::Chain(13, TS('t'), TS('f'), TS('i'), TS('('), TS(')'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';'), NS('S')),

			// 6. proc func() { ret E; }; S     (Функция БЕЗ ПАРАМЕТРОВ и БЕЗ ТЕЛА)
			Rule::Chain(12, TS('t'), TS('f'), TS('i'), TS('('), TS(')'), TS('{'), TS('r'), NS('E'), TS(';'), TS('}'), TS(';'), NS('S'))
		),

				// N - Операторы
				// 14 цепочек: присваивание, объявление, вызов, while, echo
				Rule(NS('N'), GRB_ERROR_SERIES + 1,
				14,
					// --- Одиночные операторы ---
					Rule::Chain(4, TS('d'), TS('t'), TS('i'), TS(';')),				// var type id;
					Rule::Chain(4, TS('i'), TS('='), NS('E'), TS(';')),				// id = expr;
					Rule::Chain(3, TS('r'), NS('E'), TS(';')),						// ret expr;
					Rule::Chain(3, TS('p'), NS('E'), TS(';')),						// echo expr;
					Rule::Chain(6, TS('d'), TS('t'), TS('i'), TS('='), NS('E'), TS(';')), // var type id = expr;

					// while ( E ) { N };
					Rule::Chain(8, TS('w'), TS('('), NS('E'), TS(')'), TS('{'), NS('N'), TS('}'), TS(';')),

					// declare func (прототипы внутри N, если нужно)
					Rule::Chain(8, TS('d'), TS('t'), TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS(';')),

					// --- Операторы с продолжением (N) ---
					Rule::Chain(5, TS('d'), TS('t'), TS('i'), TS(';'), NS('N')),	// var... N
					Rule::Chain(5, TS('i'), TS('='), NS('E'), TS(';'), NS('N')),	// id=... N
					Rule::Chain(4, TS('r'), NS('E'), TS(';'), NS('N')),				// ret... N
					Rule::Chain(4, TS('p'), NS('E'), TS(';'), NS('N')),				// echo... N
					Rule::Chain(7, TS('d'), TS('t'), TS('i'), TS('='), NS('E'), TS(';'), NS('N')), // init... N

					// while (...) { N }; N
					Rule::Chain(9, TS('w'), TS('('), NS('E'), TS(')'), TS('{'), NS('N'), TS('}'), TS(';'), NS('N')),

					// declare func... N
					Rule::Chain(9, TS('d'), TS('t'), TS('f'), TS('i'), TS('('), NS('F'), TS(')'), TS(';'), NS('N'))
				),

				// E - Выражения (9 цепочек)
				Rule(NS('E'), GRB_ERROR_SERIES + 2,
				9,
					Rule::Chain(1, TS('i')),									// id
					Rule::Chain(1, TS('l')),									// literal
					Rule::Chain(3, TS('('), NS('E'), TS(')')),					// (E)
					Rule::Chain(4, TS('i'), TS('('), NS('W'), TS(')')),			// id(args)
					Rule::Chain(3, TS('i'), TS('('), TS(')')),					// id() -- ВЫЗОВ БЕЗ АРГУМЕНТОВ
					Rule::Chain(2, TS('i'), NS('M')),							// id + ...
					Rule::Chain(2, TS('l'), NS('M')),							// literal + ...
					Rule::Chain(4, TS('('), NS('E'), TS(')'), NS('M')),			// (E) + ...
					Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), NS('M'))	// id(args) + ...
				),

				// F - Параметры объявления (2 цепочки)
				Rule(NS('F'), GRB_ERROR_SERIES + 3,
				 2,
					Rule::Chain(2, TS('t'), TS('i')),				// type id
					Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('F')) // type id, ...
				),

				// W - Аргументы вызова (4 цепочки)
				Rule(NS('W'), GRB_ERROR_SERIES + 4,
				 4,
					Rule::Chain(1, TS('i')),					// id
					Rule::Chain(1, TS('l')),					// literal
					Rule::Chain(3, TS('i'), TS(','), NS('W')),	// id, ...
					Rule::Chain(3, TS('l'), TS(','), NS('W'))	// literal, ...
				),

				// M - Хвост выражения (22 цепочки)
				Rule(NS('M'), GRB_ERROR_SERIES + 2,
				22,
					// Арифметика
					Rule::Chain(2, TS('+'), NS('E')), Rule::Chain(3, TS('+'), NS('E'), NS('M')),
					Rule::Chain(2, TS('-'), NS('E')), Rule::Chain(3, TS('-'), NS('E'), NS('M')),
					Rule::Chain(2, TS('*'), NS('E')), Rule::Chain(3, TS('*'), NS('E'), NS('M')),
					Rule::Chain(2, TS('/'), NS('E')), Rule::Chain(3, TS('/'), NS('E'), NS('M')),
					Rule::Chain(2, TS('%'), NS('E')), Rule::Chain(3, TS('%'), NS('E'), NS('M')),

					// Сравнения
					Rule::Chain(2, TS('e'), NS('E')), Rule::Chain(3, TS('e'), NS('E'), NS('M')), // ==
					Rule::Chain(2, TS('n'), NS('E')), Rule::Chain(3, TS('n'), NS('E'), NS('M')), // !=
					Rule::Chain(2, TS('<'), NS('E')), Rule::Chain(3, TS('<'), NS('E'), NS('M')), // <
					Rule::Chain(2, TS('>'), NS('E')), Rule::Chain(3, TS('>'), NS('E'), NS('M')), // >
					Rule::Chain(2, TS('1'), NS('E')), Rule::Chain(3, TS('1'), NS('E'), NS('M')), // <=
					Rule::Chain(2, TS('2'), NS('E')), Rule::Chain(3, TS('2'), NS('E'), NS('M'))  // >=
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
