#include "Generator.h"
#include "Parm.h"
#include "LexAnalysis.h"
#include "IT.h"
#include "LT.h"
#include <sstream>
#include <cstring>
#include <iosfwd>
#include <stack>
#include <vector>
#include <map>

using namespace std;

namespace Gener
{
	static int conditionnum = 0;
	string numberOfPoints = "k";
	string itoS(int x) { stringstream r;  r << x;  return r.str(); }

	string genCallFuncCode(Lexer::LEX& tables, Log::LOG& log, int i)
	{
		string str;
		IT::Entry e = ITENTRY(i); // идентификатор функции
		stack <IT::Entry> temp;
		bool stnd = (e.idtype == IT::IDTYPE::S); // стандартная ли функция

		for (int j = i + 1; LEXEMA(j) != LEX_RIGHTTHESIS; j++)
		{
			if (LEXEMA(j) == LEX_ID || LEXEMA(j) == LEX_LITERAL)
				temp.push(ITENTRY(j)); // параметры в стек
		}
		str += "\n";

		// Заталкиваем параметры в стек
		while (!temp.empty())
		{
			if (temp.top().idtype == IT::IDTYPE::L && temp.top().iddatatype == IT::IDDATATYPE::STR || temp.top().iddatatype == IT::IDDATATYPE::CHAR)
				str = str + "push offset " + temp.top().id + "\n";
			else if (temp.top().iddatatype == IT::IDDATATYPE::SHORT)
			{
				// FIX: Расширяем word до dword перед push (Стек всегда работает с 4 байтами)
				str = str + "movsx eax, word ptr " + temp.top().id + "\n";
				str = str + "push eax\n";
			}
			else
				str = str + "push " + temp.top().id + "\n";
			temp.pop();
		}

		if (stnd)
			str += "push offset buffer\n";

		str = str + "call " + string(e.id) + IN_CODE_ENDL;

		// FIX: Если функция возвращает short (в AX), расширяем до EAX для дальнейших операций
		if (e.iddatatype == IT::IDDATATYPE::SHORT) {
			str += "cwde\n"; // Convert Word to Doubleword Extended (AX -> EAX)
		}

		return str;
	}

	string genEqualCode(Lexer::LEX& tables, Log::LOG& log, int i)
	{
		string str;
		IT::Entry e1 = ITENTRY(i - 1); // левая часть
		switch (e1.iddatatype)
		{
		case IT::IDDATATYPE::SHORT:
		{
			bool first = true;
			for (int j = i + 1; LEXEMA(j) != LEX_SEPARATOR; j++)
			{
				switch (LEXEMA(j))
				{
				case LEX_LITERAL:
				case LEX_LITERAL_HEX:
				case LEX_ID:
				{
					if (tables.lextable.table[j].idxTI != NULLIDX_TI && 
						(ITENTRY(j).idtype == IT::IDTYPE::F || ITENTRY(j).idtype == IT::IDTYPE::S)) // вызов функции
					{
						str = str + genCallFuncCode(tables, log, j); // вызов, результат в EAX
						str = str + "push eax\n";
						while (LEXEMA(j) != LEX_RIGHTTHESIS) j++;
						break;
					}
					else {
						if (tables.lextable.table[j].idxTI != NULLIDX_TI)
						{
							if (ITENTRY(j).iddatatype == IT::IDDATATYPE::SHORT)
							{
								// FIX: Расширяем word до dword
								str = str + "movsx eax, word ptr " + ITENTRY(j).id + "\n";
								str = str + "push eax\n";
							}
							else
								str = str + "push " + ITENTRY(j).id + "\n";
						}
					}
					break;
				}

				case LEX_PLUS:
					str = str + "pop ebx\npop eax\nadd eax, ebx\npush eax\n"; break;

				case LEX_MINUS:
					// FIX: Исправлено на стандартное вычитание.
					// Раньше тут была проверка флагов для модуля, что некорректно для арифметики.
					str = str + "pop ebx\npop eax\nsub eax, ebx\npush eax\n";
					break;

				case LEX_STAR:
					str = str + "pop ebx\npop eax\nimul eax, ebx\npush eax\n"; break;

				case LEX_DIRSLASH:
					// FIX: Убрано 'mov edx, 0'. 'cdq' корректно подготавливает EDX для знакового деления.
					str = str + "pop ebx\npop eax\ncdq\nidiv ebx\npush eax\n";
					break;

				case LEX_PERSENT:
					// FIX: Убрано 'mov edx, 0'.
					str = str + "pop ebx\npop eax\ncdq\nidiv ebx\npush edx\n";
					break;
				}
			}

			// Результат (в EAX или EBX после pop) сохраняем обратно в short
			str = str + "\npop ebx\nmov word ptr " + e1.id + ", bx\n";
			break;
		}
		case IT::IDDATATYPE::STR:
		{
			char lex = LEXEMA(i + 1);
			IT::Entry e2 = ITENTRY(i + 1);
			if (lex == LEX_ID && (e2.idtype == IT::IDTYPE::F || e2.idtype == IT::IDTYPE::S))
			{
				str += genCallFuncCode(tables, log, i + 1);
				str = str + "mov " + e1.id + ", eax";
			}
			else if (lex == LEX_LITERAL)
			{
				str = str + "mov " + e1.id + ", offset " + e2.id;
			}
			else
			{
				str = str + "mov ecx, " + e2.id + "\nmov " + e1.id + ", ecx";
			}
			break;
		}
		case IT::IDDATATYPE::CHAR:
		{
			char lex = LEXEMA(i + 1);
			IT::Entry e2 = ITENTRY(i + 1);
			if (lex == LEX_ID && (e2.idtype == IT::IDTYPE::F || e2.idtype == IT::IDTYPE::S))
			{
				str += genCallFuncCode(tables, log, i + 1);
				str = str + "mov " + e1.id + ", eax";
			}
			else if (lex == LEX_LITERAL)
			{
				str = str + "mov " + e1.id + ", offset " + e2.id;
			}
			else
			{
				str = str + "mov ecx, " + e2.id + "\nmov " + e1.id + ", ecx";
			}
			break;
		}
		}
		return str;
	}

	string genFunctionCode(Lexer::LEX& tables, int i, string funcname, int pcount)
	{
		string str;
		IT::Entry e = ITENTRY(i + 1);
		IT::IDDATATYPE type = e.iddatatype;
		str = SEPSTR(funcname) + string(e.id) + string(" PROC,\n\t");

		int j = i + 3;
		while (LEXEMA(j) != LEX_RIGHTTHESIS)
		{
			if (LEXEMA(j) == LEX_ID)
				// В 32-битном стеке параметры всегда занимают 4 байта (DWORD)
				str = str + string(ITENTRY(j).id) + (ITENTRY(j).iddatatype == IT::IDDATATYPE::SHORT ? " : DWORD, " : " : DWORD, ");
			j++;
		}
		int f = str.rfind(',');
		if (f > 0)
			str[f] = IN_CODE_SPACE;
		str += "\n; --- save registers ---\npush ebx\npush edx\n; ----------------------";
		return str;
	}

	string genExitCode(Lexer::LEX& tables, int i, string funcname, int pcount, IT::IDDATATYPE retType)
	{
		string str = "; --- restore registers ---\npop edx\npop ebx\n; -------------------------\n";
		if (LEXEMA(i + 1) != LEX_SEPARATOR)
		{
			IT::Entry retVal = ITENTRY(i + 1);
			if (retVal.iddatatype == IT::IDDATATYPE::SHORT)
				str = str + "mov ax, word ptr " + string(retVal.id) + "\n";
			else
				str = str + "mov eax, " + string(retVal.id) + "\n";
		}

		// FIX: Для STDCALL вызываемая функция должна очистить стек.
		// Размер очистки = количество параметров * 4 байта
		str += "ret " + itoS(pcount * 4) + "\n";

		str += funcname + " ENDP" + SEPSTREMP;
		return str;
	}

	string genCycleConditionCode(Lexer::LEX& tables, int i, int cycleNum)
	{
		string str;
		
		// Проверяем, есть ли оператор сравнения после is:
		// Если следующий элемент - оператор сравнения, это обычное сравнение
		// Если следующий элемент - true/false/literal/id, это простое условие
		
		if (i + 1 < tables.lextable.size)
		{
			char nextLex = LEXEMA(i + 1);
			
			// Простое условие: true, false, литерал или идентификатор без оператора
			if (nextLex == LEX_TRUE)
			{
				// Бесконечный цикл - всегда переходим обратно
				str = str + "jmp do" + itoS(cycleNum) + "\n";
				str = str + "donext" + itoS(cycleNum) + ":";
				return str;
			}
			else if (nextLex == LEX_FALSE)
			{
				// Цикл никогда не выполняется - сразу выходим
				str = str + "jmp donext" + itoS(cycleNum) + "\n";
				str = str + "donext" + itoS(cycleNum) + ":";
				return str;
			}
			else if (nextLex == LEX_LITERAL || nextLex == LEX_ID || nextLex == LEX_LITERAL_HEX)
			{
				// Литерал или идентификатор - проверяем значение
				if (tables.lextable.table[i + 1].idxTI != NULLIDX_TI)
				{
					IT::Entry entry = ITENTRY(i + 1);
					if (entry.iddatatype == IT::IDDATATYPE::SHORT)
					{
						short value = entry.value.vnum;
						
						// Если значение != 0, цикл бесконечный
						if (value != 0)
						{
							str = str + "jmp do" + itoS(cycleNum) + "\n";
						}
						else
						{
							str = str + "jmp donext" + itoS(cycleNum) + "\n";
						}
						str = str + "donext" + itoS(cycleNum) + ":";
						return str;
					}
				}
			}
		}
		
		// Обычное сравнение с оператором (проверяем, что есть оператор)
		if (i + 2 < tables.lextable.size && 
			(LEXEMA(i + 2) == LEX_MORE || LEXEMA(i + 2) == LEX_LESS || 
			 LEXEMA(i + 2) == LEX_EQUALS || LEXEMA(i + 2) == LEX_NOTEQUALS ||
			 LEXEMA(i + 2) == LEX_MOREEQUALS || LEXEMA(i + 2) == LEX_LESSEQUALS))
		{
			if (tables.lextable.table[i + 1].idxTI != NULLIDX_TI && 
				tables.lextable.table[i + 3].idxTI != NULLIDX_TI)
			{
				IT::Entry lft = ITENTRY(i + 1);
				IT::Entry rgt = ITENTRY(i + 3);

				string movInstr = (lft.iddatatype == IT::IDDATATYPE::SHORT) ? "mov dx, word ptr " : "mov edx, ";
				string cmpInstr = (rgt.iddatatype == IT::IDDATATYPE::SHORT) ? "cmp dx, word ptr " : "cmp edx, ";
				str = str + movInstr + lft.id + "\n" + cmpInstr + rgt.id + "\n";

				string jmpCmd;
				switch (LEXEMA(i + 2))
				{
				case LEX_MORE:  jmpCmd = "jg";  break;
				case LEX_LESS:   jmpCmd = "jl";  break;
				case LEX_EQUALS:    jmpCmd = "jz";  break;
				case LEX_NOTEQUALS:   jmpCmd = "jnz";  break;
				case LEX_MOREEQUALS:   jmpCmd = "jge";  break;
				case LEX_LESSEQUALS:   jmpCmd = "jle";  break;
				}

				str = str + "\n" + jmpCmd + " do" + itoS(cycleNum);
				str = str + "\ndonext" + itoS(cycleNum) + ":";
				return str;
			}
		}
		
		// Если ничего не подошло, возвращаем пустую строку (ошибка)
		return "";
	}

	string genConditionCode(Lexer::LEX& tables, int i, string& cyclecode)
	{
		string str;
		conditionnum++;
		cyclecode.clear();
		
		bool w = false, r = false, c = false;
		string wstr, rstr, rstr2;

		// Проверяем, есть ли оператор сравнения после is:
		if (i + 1 < tables.lextable.size)
		{
			char nextLex = LEXEMA(i + 1);
			
			// Простое условие: true, false, литерал или идентификатор без оператора
			if (nextLex == LEX_TRUE)
			{
				// Всегда истина - всегда переходим в istrue блок
				for (int j = i + 2; j < tables.lextable.size && LEXEMA(j) != LEX_DIEZ; j++)
				{
					if (LEXEMA(j) == LEX_ISTRUE) r = true;
					if (LEXEMA(j) == LEX_ISFALSE) w = true;
				}
				if (r) str = str + "jmp right" + itoS(conditionnum) + "\n";
				if (w) str = str + "wrong" + itoS(conditionnum) + ":\n";
				if (r) str = str + "right" + itoS(conditionnum) + ":";
				// Не создаем метку next здесь - она будет создана в case LEX_DIEZ
				return str;
			}
			else if (nextLex == LEX_FALSE)
			{
				// Всегда ложь - всегда переходим в isfalse блок или пропускаем
				for (int j = i + 2; j < tables.lextable.size && LEXEMA(j) != LEX_DIEZ; j++)
				{
					if (LEXEMA(j) == LEX_ISTRUE) r = true;
					if (LEXEMA(j) == LEX_ISFALSE) w = true;
				}
				if (w) str = str + "jmp wrong" + itoS(conditionnum) + "\n";
				if (r) str = str + "right" + itoS(conditionnum) + ":\n";
				if (w) str = str + "wrong" + itoS(conditionnum) + ":";
				// Не создаем метку next здесь - она будет создана в case LEX_DIEZ
				return str;
			}
			else if (nextLex == LEX_LITERAL || nextLex == LEX_ID || nextLex == LEX_LITERAL_HEX)
			{
				// Литерал или идентификатор - проверяем значение
				if (tables.lextable.table[i + 1].idxTI != NULLIDX_TI)
				{
					IT::Entry entry = ITENTRY(i + 1);
					if (entry.iddatatype == IT::IDDATATYPE::SHORT)
					{
						short value = entry.value.vnum;
						
						for (int j = i + 2; j < tables.lextable.size && LEXEMA(j) != LEX_DIEZ; j++)
						{
							if (LEXEMA(j) == LEX_ISTRUE) r = true;
							if (LEXEMA(j) == LEX_ISFALSE) w = true;
						}
						
						// Если значение != 0, условие истинно
						if (value != 0)
						{
							if (r) str = str + "jmp right" + itoS(conditionnum) + "\n";
							if (w) str = str + "wrong" + itoS(conditionnum) + ":\n";
							if (r) str = str + "right" + itoS(conditionnum) + ":";
							// Не создаем метку next здесь - она будет создана в case LEX_DIEZ
						}
						else
						{
							if (w) str = str + "jmp wrong" + itoS(conditionnum) + "\n";
							if (r) str = str + "right" + itoS(conditionnum) + ":\n";
							if (w) str = str + "wrong" + itoS(conditionnum) + ":";
							// Не создаем метку next здесь - она будет создана в case LEX_DIEZ
						}
						return str;
					}
				}
			}
		}
		
		// Обычное сравнение с оператором (проверяем, что есть оператор)
		if (i + 2 < tables.lextable.size && 
			(LEXEMA(i + 2) == LEX_MORE || LEXEMA(i + 2) == LEX_LESS || 
			 LEXEMA(i + 2) == LEX_EQUALS || LEXEMA(i + 2) == LEX_NOTEQUALS ||
			 LEXEMA(i + 2) == LEX_MOREEQUALS || LEXEMA(i + 2) == LEX_LESSEQUALS))
		{
			if (i + 3 < tables.lextable.size && 
				tables.lextable.table[i + 1].idxTI != NULLIDX_TI && 
				tables.lextable.table[i + 3].idxTI != NULLIDX_TI)
			{
				IT::Entry lft = ITENTRY(i + 1);
				IT::Entry rgt = ITENTRY(i + 3);

				for (int j = i + 5; j < tables.lextable.size && LEXEMA(j) != LEX_DIEZ; j++)
				{
					if (LEXEMA(j) == LEX_ISTRUE) r = true;
					if (LEXEMA(j) == LEX_ISFALSE) w = true;
					if (LEXEMA(j) == LEX_CYCLE) c = true;
				}

				string movInstr = (lft.iddatatype == IT::IDDATATYPE::SHORT) ? "mov dx, word ptr " : "mov edx, ";
				string cmpInstr = (rgt.iddatatype == IT::IDDATATYPE::SHORT) ? "cmp dx, word ptr " : "cmp edx, ";
				str = str + movInstr + lft.id + "\n" + cmpInstr + rgt.id + "\n";

				switch (LEXEMA(i + 2))
				{
				case LEX_MORE:  rstr = "jg";  wstr = "jl";  break;
				case LEX_LESS:   rstr = "jl";  wstr = "jg";  break;
				case LEX_EQUALS:    rstr = "jz";  wstr = "jnz";  break;
				case LEX_NOTEQUALS:   rstr = "jnz";  wstr = "jz";  break;
				case LEX_MOREEQUALS:   rstr = "jz"; rstr2 = "jg";  wstr = "jnz";  break;
				case LEX_LESSEQUALS:   rstr = "jz"; rstr2 = "jl";  wstr = "jnz";  break;
				}

				if (LEXEMA(i + 2) == LEX_MORE || LEXEMA(i + 2) == LEX_LESS || LEXEMA(i + 2) == LEX_EQUALS || LEXEMA(i + 2) == LEX_NOTEQUALS) {
					if (!c && r) str = str + "\n" + rstr + " right" + itoS(conditionnum);
					if (!c && w) str = str + "\n" + wstr + " wrong" + itoS(conditionnum);
				}
				if (LEXEMA(i + 2) == LEX_MOREEQUALS || LEXEMA(i + 2) == LEX_LESSEQUALS) {
					if (!c && r) str = str + "\n" + rstr + " right" + itoS(conditionnum) + "\n" + rstr2 + " right" + itoS(conditionnum);
					if (!c && w) str = str + "\n" + wstr + " wrong" + itoS(conditionnum);
				}
				if (c)
				{
					string jmpCmd;
					if (LEXEMA(i + 2) == LEX_MORE || LEXEMA(i + 2) == LEX_LESS || LEXEMA(i + 2) == LEX_EQUALS || LEXEMA(i + 2) == LEX_NOTEQUALS) {
						jmpCmd = rstr;
					}
					else if (LEXEMA(i + 2) == LEX_MOREEQUALS) {
						jmpCmd = "jge";
					}
					else if (LEXEMA(i + 2) == LEX_LESSEQUALS) {
						jmpCmd = "jle";
					}
					cyclecode = str + "\n" + jmpCmd + " do" + itoS(conditionnum);
					str = "";
				}
				else if (!r || !w)  str = str + "\njmp next" + itoS(conditionnum);
				return str;
			}
		}
		
		// Если ничего не подошло, возвращаем пустую строку (ошибка)
		return "";
	}

	vector <string> startFillVector(Lexer::LEX& tables)
	{
		vector <string> v;
		v.push_back(BEGIN);
		v.push_back(EXTERN);
		vector <string> vlt;  vlt.push_back(CONST);
		vector <string> vid;  vid.push_back(DATA);
		for (int i = 0; i < tables.idtable.size; i++)
		{
			IT::Entry e = tables.idtable.table[i];
			string str = "\t\t" + string(e.id);

			if (tables.idtable.table[i].idtype == IT::IDTYPE::L)
			{
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::SHORT:  str = str + " sword " + itoS(e.value.vnum);  break;
				case IT::IDDATATYPE::STR:  str = str + " byte '" + string(e.value.vstr.str) + "', 0";  break;
				case IT::IDDATATYPE::CHAR:  str = str + " byte '" + string(e.value.vstr.str) + "', 0";  break;
				}
				vlt.push_back(str);
			}
			else if (tables.idtable.table[i].idtype == IT::IDTYPE::V)
			{
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::SHORT: str = str + " sword 0";  break;
				case IT::IDDATATYPE::STR: str = str + " dword 0";  break;
				case IT::IDDATATYPE::CHAR: str = str + " dword 0";  break;
				}
				vid.push_back(str);
			}
		}
		v.insert(v.end(), vlt.begin(), vlt.end());
		v.insert(v.end(), vid.begin(), vid.end());
		v.push_back(CODE);
		return v;
	}

	void CodeGeneration(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log)
	{
		vector <string> v = startFillVector(tables);
		ofstream ofile(parm.out);
		string funcname;
		string cyclecode;
		int pcount;
		string str;
		IT::IDDATATYPE funcRetType = IT::IDDATATYPE::UNDEF;
		map<int, int> cycleNumMap;
		for (int i = 0; i < tables.lextable.size; i++)
		{
			switch (LEXEMA(i))
			{
			case LEX_MAIN:
			{
				conditionnum = 0; // Сбрасываем счетчик условий для main
				str = str + SEPSTR("MAIN") + "main PROC";
				break;
			}
			case LEX_FUNCTION:
			{
				conditionnum = 0; // Сбрасываем счетчик условий для новой функции
				funcname = ITENTRY(i + 1).id;
				pcount = ITENTRY(i + 1).value.params.count;
				funcRetType = ITENTRY(i + 1).iddatatype;
				str = genFunctionCode(tables, i, funcname, pcount);
				break;
			}
			case LEX_RETURN:
			{
				str = genExitCode(tables, i, funcname, pcount, funcRetType);
				break;
			}
			case LEX_ID:
			{
				if (LEXEMA(i + 1) == LEX_LEFTHESIS && LEXEMA(i - 1) != LEX_FUNCTION)
					str = genCallFuncCode(tables, log, i);
				break;
			}
			case LEX_CONDITION:
			{
				bool isCycleCondition = false;
				int cycleNum = -1;
				if (i > 0 && LEXEMA(i - 1) == LEX_DIEZ && i > 1 && LEXEMA(i - 2) == LEX_BRACELET)
				{
					if (cycleNumMap.find(i - 1) != cycleNumMap.end())
					{
						isCycleCondition = true;
						cycleNum = cycleNumMap[i - 1];
					}
				}

				if (isCycleCondition)
					str = genCycleConditionCode(tables, i, cycleNum);
				else
					str = genConditionCode(tables, i, cyclecode);
				break;
			}
			case LEX_BRACELET:
			{
				if (LEXEMA(i + 1) == LEX_ISFALSE || LEXEMA(i + 1) == LEX_ISTRUE)
					str = str + "jmp next" + itoS(conditionnum) + "\n";
				break;
			}
			case LEX_DIEZ:
			{
				if (LEXEMA(i - 1) == LEX_BRACELET)
				{
					bool c = false;
					bool isCycleEnd = false;
					bool isSimpleCondition = false; // Проверяем, было ли простое условие (true/false/literal)
					
					// Ищем условие перед этим # и проверяем, простое ли оно
					for (int j = i - 1; j >= 0 && j >= i - 10; j--)
					{
						if (LEXEMA(j) == LEX_CONDITION)
						{
							// Проверяем, что идет после is:
							if (j + 1 < tables.lextable.size)
							{
								char afterIs = LEXEMA(j + 1);
								// Если после is: сразу true/false/literal/id без оператора - это простое условие
								if (afterIs == LEX_TRUE || afterIs == LEX_FALSE || 
									afterIs == LEX_LITERAL || afterIs == LEX_ID || afterIs == LEX_LITERAL_HEX)
								{
									// Проверяем, есть ли оператор сравнения после
									if (j + 2 < tables.lextable.size)
									{
										char afterValue = LEXEMA(j + 2);
										if (afterValue != LEX_MORE && afterValue != LEX_LESS && 
											afterValue != LEX_EQUALS && afterValue != LEX_NOTEQUALS &&
											afterValue != LEX_MOREEQUALS && afterValue != LEX_LESSEQUALS)
										{
											isSimpleCondition = true;
										}
									}
									else
										isSimpleCondition = true;
								}
							}
							break;
						}
					}
					
					if (LEXEMA(i + 1) == LEX_CONDITION)
					{
						for (int j = i - 2; j >= 0 && j >= i - 20; j--)
						{
							if (LEXEMA(j) == LEX_CYCLE)
							{
								isCycleEnd = true;
								break;
							}
							if (LEXEMA(j) == LEX_CONDITION) break;
						}
					}

					for (int j = i; j > 0 && LEXEMA(j) != LEX_CONDITION; j--)
						if (LEXEMA(j) == LEX_CYCLE)
							c = true;

					if (isCycleEnd)
						str = "";
					else if (c)
						str = cyclecode + "\ndonext" + itoS(conditionnum) + ":";
					else if (!isSimpleCondition)
						str += "next" + itoS(conditionnum) + ":";
					// Для простых условий (true/false/literal без оператора) метка next не создается,
					// так как все метки уже созданы в genConditionCode
				}
				break;
			}
			case LEX_ISTRUE:
			{
				str = str + "right" + itoS(conditionnum) + ":";
				break;
			}
			case LEX_ISFALSE:
			{
				str = str + "wrong" + itoS(conditionnum) + ":";
				break;
			}
			case LEX_CYCLE:
			{
				bool hasConditionAfter = false;
				int diezPos = -1;
				for (int j = i + 1; j < tables.lextable.size && j < i + 50; j++)
				{
					if (LEXEMA(j) == LEX_BRACELET)
					{
						for (int k = j + 1; k < tables.lextable.size && k < j + 5; k++)
						{
							if (LEXEMA(k) == LEX_DIEZ)
							{
								diezPos = k;
								if (k + 1 < tables.lextable.size && LEXEMA(k + 1) == LEX_CONDITION)
									hasConditionAfter = true;
								break;
							}
						}
						break;
					}
				}

				if (hasConditionAfter)
				{
					conditionnum++;
					if (diezPos >= 0)
						cycleNumMap[diezPos] = conditionnum;
					str = str + "do" + itoS(conditionnum) + ":";
				}
				else
					str = str + "do" + itoS(conditionnum) + ":";
				break;
			}
			case LEX_EQUAL:
			{
				str = genEqualCode(tables, log, i);
				while (LEXEMA(++i) != LEX_SEPARATOR);
				break;
			}
			case LEX_NEWLINE:
			{
				str = str + "push offset newline\ncall outrad\n";
				break;
			}
			case LEX_WRITE:
			{
				if (LEXEMA(i + 1) == LEX_ID && LEXEMA(i + 2) == LEX_LEFTHESIS)
				{
					str = str + genCallFuncCode(tables, log, i + 1);
					str += "push eax\n";

					IT::Entry funcEntry = ITENTRY(i + 1);
					if (funcEntry.iddatatype == IT::IDDATATYPE::SHORT)
						str += "call outlich\n";
					else
						str += "call outrad\n";

					int j = i + 2;
					while (LEXEMA(j) != LEX_RIGHTTHESIS) {
						j++;
					}
					i = j;
				}
				else
				{
					IT::Entry e = ITENTRY(i + 1);
					switch (e.iddatatype)
					{
					case IT::IDDATATYPE::SHORT:
						str = str + "\nmovsx eax, word ptr " + e.id + "\npush eax\ncall outlich\n";
						break;
					case IT::IDDATATYPE::STR:
						if (e.idtype == IT::IDTYPE::L)  str = str + "\npush offset " + e.id + "\ncall outrad\n";
						else  str = str + "\npush " + e.id + "\ncall outrad\n";
						break;
					case IT::IDDATATYPE::CHAR:
						if (e.idtype == IT::IDTYPE::L)  str = str + "\npush offset " + e.id + "\ncall outrad\n";
						else  str = str + "\npush " + e.id + "\ncall outrad\n";
						break;
					}
				}
				break;
			}
			}
			if (!str.empty())
				v.push_back(str);
			str.clear();
		}
		v.push_back(END);

		for (auto x : v)
			ofile << x << endl;
		ofile.close();
	}
};