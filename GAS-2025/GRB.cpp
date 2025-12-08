#include "Header.h"
#define GRB_ERROR_SERIES 600

typedef short GRBALPHABET;

namespace GRB
{
	// Количество правил (16) осталось прежним, изменилось количество цепочек внутри правил K и X
	Greibach greibach(NS('S'), TS('$'), 16,

		Rule(NS('S'), GRB_ERROR_SERIES, 3,						// Начальный символ
			Rule::Chain(6, TS('t'), TS('f'), TS('i'), NS('P'), NS('T'), NS('S')),
			Rule::Chain(6, TS('p'), TS('f'), TS('i'), NS('P'), NS('G'), NS('S')),
			Rule::Chain(4, TS('m'), TS('['), NS('K'), TS(']'))
		),

		Rule(NS('T'), GRB_ERROR_SERIES + 2, 2,					// Данные в теле функции
			Rule::Chain(5, TS('['), TS('e'), NS('V'), TS(';'), TS(']')),
			Rule::Chain(6, TS('['), NS('K'), TS('e'), NS('V'), TS(';'), TS(']'))
		),

		Rule(NS('G'), GRB_ERROR_SERIES + 3, 2,					// Данные в теле программы
			Rule::Chain(4, TS('['), TS('e'), TS(';'), TS(']')),
			Rule::Chain(5, TS('['), NS('K'), TS('e'), TS(';'), TS(']'))
		),

		Rule(NS('P'), GRB_ERROR_SERIES + 1, 2,					// Параметры функции
			Rule::Chain(3, TS('('), NS('E'), TS(')')),
			Rule::Chain(2, TS('('), TS(')'))
		),

		Rule(NS('E'), GRB_ERROR_SERIES + 4, 2,					// Список параметров функции						
			Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('E')),
			Rule::Chain(2, TS('t'), TS('i'))
		),

		Rule(NS('F'), GRB_ERROR_SERIES + 5, 2,					// Вызов функции (аргументы)					
			Rule::Chain(3, TS('('), NS('N'), TS(')')),
			Rule::Chain(2, TS('('), TS(')'))
		),

		Rule(NS('N'), GRB_ERROR_SERIES + 6, 4,					// Перечисление аргументов вызова		
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l')),
			Rule::Chain(3, TS('i'), TS(','), NS('N')),
			Rule::Chain(3, TS('l'), TS(','), NS('N'))
		),

		Rule(NS('R'), GRB_ERROR_SERIES + 7, 5,					// Операторы сравнения	
			Rule::Chain(3, TS('r'), NS('Y'), TS('#')),
			Rule::Chain(3, TS('w'), NS('Y'), TS('#')),
			Rule::Chain(3, TS('c'), NS('Y'), TS('#')),
			Rule::Chain(5, TS('r'), NS('Y'), TS('w'), NS('Y'), TS('#')),
			Rule::Chain(5, TS('w'), NS('Y'), TS('r'), NS('Y'), TS('#'))
		),

		Rule(NS('Y'), GRB_ERROR_SERIES + 8, 1,					// Операнд сравнения			
			Rule::Chain(3, TS('['), NS('X'), TS(']'))
		),

		Rule(NS('Z'), GRB_ERROR_SERIES + 9, 3,					// Логические операции	
			Rule::Chain(3, TS('i'), NS('L'), TS('i')),
			Rule::Chain(3, TS('i'), NS('L'), TS('l')),
			Rule::Chain(3, TS('l'), NS('L'), TS('i'))
		),

		Rule(NS('L'), GRB_ERROR_SERIES + 10, 6,					// Символы сравнения		
			Rule::Chain(1, TS('<')),
			Rule::Chain(1, TS('>')),
			Rule::Chain(1, TS('&')),
			Rule::Chain(1, TS('!')),
			Rule::Chain(1, TS('~')),
			Rule::Chain(1, TS('@'))
		),

		Rule(NS('A'), GRB_ERROR_SERIES + 11, 5,					// Арифметические знаки
			Rule::Chain(1, TS('+')),
			Rule::Chain(1, TS('-')),
			Rule::Chain(1, TS('*')),
			Rule::Chain(1, TS('/')),
			Rule::Chain(1, TS('%'))
		),

		Rule(NS('V'), GRB_ERROR_SERIES + 12, 3,					// Возврат значения (Literal, ID, Hex)
			Rule::Chain(1, TS('l')),
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('h'))
		),

		Rule(NS('W'), GRB_ERROR_SERIES + 13, 8,					// Выражения (Expression)
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l')),
			Rule::Chain(3, TS('('), NS('W'), TS(')')),
			Rule::Chain(5, TS('('), NS('W'), TS(')'), NS('A'), NS('W')),
			Rule::Chain(2, TS('i'), NS('F')),
			Rule::Chain(3, TS('i'), NS('A'), NS('W')),
			Rule::Chain(3, TS('l'), NS('A'), NS('W')),
			Rule::Chain(4, TS('i'), NS('F'), NS('A'), NS('W'))
		),

		// -------------------------------------------------------------------------------------
		// ИЗМЕНЕНИЯ ЗДЕСЬ: Увеличено число цепочек с 16 до 18
		// Добавлена поддержка вызова функции внутри output
		// Добавлена поддержка нового синтаксиса цикла: cycle [...] # is: условие
		// -------------------------------------------------------------------------------------
		Rule(NS('K'), GRB_ERROR_SERIES + 14, 18,
			Rule::Chain(7, TS('n'), TS('t'), TS('i'), TS('='), NS('V'), TS(';'), NS('K')),	// Объявление + инициализация
			Rule::Chain(5, TS('n'), TS('t'), TS('i'), TS(';'), NS('K')),	// Объявление
			Rule::Chain(5, TS('i'), TS('='), NS('W'), TS(';'), NS('K')),	// Присваивание

			Rule::Chain(4, TS('o'), NS('V'), TS(';'), NS('K')),				// Вывод переменной/литерала
			Rule::Chain(5, TS('o'), TS('i'), NS('F'), TS(';'), NS('K')),    // [NEW] Вывод результата функции (recursive)

			Rule::Chain(3, TS('^'), TS(';'), NS('K')),						// Return
			Rule::Chain(5, TS('?'), NS('Z'), TS('#'), NS('R'), NS('K')),	// Condition
			Rule::Chain(4, TS('i'), NS('F'), TS(';'), NS('K')),				// Вызов функции
			Rule::Chain(9, TS('c'), TS('['), NS('X'), TS(']'), TS('#'), TS('?'), NS('Z'), TS(';'), NS('K')),	// cycle [тело] # is: условие #;

			Rule::Chain(6, TS('n'), TS('t'), TS('i'), TS('='), NS('V'), TS(';')),	// Терминал: Объявление + инициализация
			Rule::Chain(4, TS('i'), TS('='), NS('W'), TS(';')),			// Терминал: Присваивание
			Rule::Chain(4, TS('n'), TS('t'), TS('i'), TS(';')),			// Терминал: Объявление

			Rule::Chain(3, TS('o'), NS('V'), TS(';')),					// Терминал: Вывод переменной
			Rule::Chain(4, TS('o'), TS('i'), NS('F'), TS(';')),         // [NEW] Терминал: Вывод результата функции

			Rule::Chain(2, TS('^'), TS(';')),							// Терминал: Return
			Rule::Chain(4, TS('?'), NS('Z'), TS('#'), NS('R')),			// Терминал: Condition
			Rule::Chain(3, TS('i'), NS('F'), TS(';')),					// Терминал: Вызов функции
			Rule::Chain(7, TS('c'), TS('['), NS('X'), TS(']'), TS('#'), TS('?'), NS('Z'), TS('#'))	// Терминал: cycle [тело] # is: условие #
		),

		// -------------------------------------------------------------------------------------
		// ИЗМЕНЕНИЯ ЗДЕСЬ: Увеличено число цепочек с 8 до 10
		// Добавлена поддержка вызова функции внутри output (внутри блоков условий)
		// -------------------------------------------------------------------------------------
		Rule(NS('X'), GRB_ERROR_SERIES + 15, 10,
			Rule::Chain(5, TS('i'), TS('='), NS('W'), TS(';'), NS('X')),	// Присваивание

			Rule::Chain(4, TS('o'), NS('V'), TS(';'), NS('X')),				// Вывод
			Rule::Chain(5, TS('o'), TS('i'), NS('F'), TS(';'), NS('X')),    // [NEW] Вывод функции

			Rule::Chain(3, TS('^'), TS(';'), NS('X')),						// Return
			Rule::Chain(4, TS('i'), NS('F'), TS(';'), NS('X')),				// Вызов

			Rule::Chain(4, TS('i'), TS('='), NS('W'), TS(';')),			// Терминал: Присваивание

			Rule::Chain(3, TS('o'), NS('V'), TS(';')),					// Терминал: Вывод
			Rule::Chain(4, TS('o'), TS('i'), NS('F'), TS(';')),         // [NEW] Терминал: Вывод функции

			Rule::Chain(2, TS('^'), TS(';')),							// Терминал: Return
			Rule::Chain(3, TS('i'), NS('F'), TS(';'))					// Терминал: Вызов
		)
	);


	Rule::Chain::Chain(short psize, GRBALPHABET s, ...)
	{
		nt = new GRBALPHABET[size = psize];
		int* p = (int*)&s;
		for (short i = 0; i < psize; ++i)
			nt[i] = (GRBALPHABET)p[i];
	};

	Rule::Rule(GRBALPHABET pnn, int piderror, short psize, Chain c, ...)
	{
		nn = pnn;
		iderror = piderror;
		chains = new Chain[size = psize];
		Chain* p = &c;
		for (int i = 0; i < size; ++i)
			chains[i] = p[i];
	};


	Greibach::Greibach(GRBALPHABET pstartN, GRBALPHABET pstbottom, short psize, Rule r, ...)
	{
		startN = pstartN;
		stbottomT = pstbottom;
		rules = new Rule[size = psize];
		Rule* p = &r;
		for (int i = 0; i < size; ++i)
			rules[i] = p[i];
	}

	Greibach getGreibach()
	{
		return greibach;
	}

	short Greibach::getRule(GRBALPHABET pnn, Rule& prule)
	{
		short rc = -1;
		short k = 0;
		while (k < size && rules[k].nn != pnn)
			++k;
		if (k < size)
			prule = rules[rc = k];
		return rc;
	}

	Rule Greibach::getRule(short n)
	{
		Rule rc;
		if (n < size)
			rc = rules[n];
		return rc;
	};

	char* Rule::getCRule(char* b, short nchain)
	{
		char bchain[200];
		b[0] = Chain::alphabet_to_char(nn);
		b[1] = '-';
		b[2] = '>';
		b[3] = 0x00;
		chains[nchain].getCChain(bchain);
		strcat_s(b, sizeof(bchain) + 5, bchain);
		return b;
	};

	short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j)
	{
		short rc = -1;
		while (j < size && chains[j].nt[0] != t)
			++j;
		rc = (j < size ? j : -1);
		if (rc >= 0)
			pchain = chains[rc];
		return rc;
	}

	char* Rule::Chain::getCChain(char* b)
	{
		for (int i = 0; i < size; ++i)
			b[i] = alphabet_to_char(nt[i]);
		b[size] = 0x00;
		return b;
	}

}