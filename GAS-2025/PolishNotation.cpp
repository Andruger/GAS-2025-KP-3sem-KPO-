#include "Header.h"
#include <stack>
#include <cstring>
#include <iostream> // ��� ������

using namespace std;
namespace Polish
{
    int getPriority(LT::Entry& e)
    {
        switch (e.lexema)
        {
        case LEX_LEFTHESIS: case LEX_RIGHTTHESIS: return 0;
        case LEX_PLUS: case LEX_MINUS: return 1;
        case LEX_STAR: case LEX_DIRSLASH: case LEX_PERSENT: return 2;
        default: return -1;
        }
    }

    bool PolishNotation(Lexer::LEX& tbls, Log::LOG& log)
    {
        unsigned curExprBegin = 0;
        ltvec v; // ������ ��������� ������� ������
        LT::LexTable new_table = LT::Create(tbls.lextable.maxsize);
        intvec vpositions = getExprPositions(tbls); // ������� ������ ���������

        for (int i = 0; i < tbls.lextable.size; i++)
        {
            if (curExprBegin < vpositions.size() && i == vpositions[curExprBegin]) // ����� �� ���������� ������ ���������
            {
                int lexcount = fillVector(vpositions[curExprBegin], tbls.lextable, v); // ��������� ������ �������� �� � ������ �������
                if (lexcount > 1)
                {
                    bool rc = setPolishNotation(tbls.idtable, log, vpositions[curExprBegin], v); // �������� ������ � �������� �������
                    if (!rc)
                        return false;

                    // ����� ���������������� ���������
                    for (int i = 0; i < v.size(); i++)
                    {
                        cout << v[i].lexema;
                    }
                    cout << endl;
                }

                addToTable(new_table, tbls.idtable, v); // ��������� ����p������ ������ � �� + ������������� ��
                i += lexcount - 1;
                curExprBegin++;
                continue;
            }
            if (tbls.lextable.table[i].lexema == LEX_ID || tbls.lextable.table[i].lexema == LEX_LITERAL)
            {
                int firstind = Lexer::getIndexInLT(new_table, tbls.lextable.table[i].idxTI);
                if (firstind == -1)
                    firstind = new_table.size;
                tbls.idtable.table[tbls.lextable.table[i].idxTI].idxfirstLE = firstind;
            }
            LT::Add(new_table, tbls.lextable.table[i]);
        }

        tbls.lextable = new_table;
        return true;
    }

    int __cdecl fillVector(int posExprBegin, LT::LexTable& lextable, ltvec& v)
    {
        v.clear();
        for (int i = posExprBegin; i < lextable.size; i++)
        {
            if (lextable.table[i].lexema == LEX_SEPARATOR)
                break;
            else v.push_back(LT::Entry(lextable.table[i]));
        }
        return v.size();
    }

    void addToTable(LT::LexTable& new_table, IT::IdTable& idtable, ltvec& v)
    {
        for (unsigned i = 0; i < v.size(); i++)
        {
            LT::Add(new_table, v[i]);
            // ���������� �������� ����� ����� �� � ��
            if (v[i].lexema == LEX_ID || v[i].lexema == LEX_LITERAL)
            {
                int firstind = Lexer::getIndexInLT(new_table, v[i].idxTI);
                idtable.table[v[i].idxTI].idxfirstLE = firstind;
            }
        }
    }

    intvec getExprPositions(Lexer::LEX& tbls)
    {
        intvec v;
        bool f_begin = false; // ������� ���������� ����� ���������
        bool f_end = false;  // ������� ���������� ������ ���������
        int begin = 0;  int end = 0;

        for (int i = 0; i < tbls.lextable.size; i++)
        {
            if (tbls.lextable.table[i].lexema == LEX_EQUAL) // ������ ���������
            {
                begin = i + 1;
                f_begin = true;
                continue;
            }
            if (f_begin && tbls.lextable.table[i].lexema == LEX_SEPARATOR) // ����� ���������
            {
                end = i;
                f_end = true;
                continue;
            }
            if (f_begin && f_end) // �������� ������ � ����� ��������� � ������
            {
                v.push_back(begin);
                f_begin = f_end = false;
            }
        }
        return v;
    }

    bool __cdecl setPolishNotation(IT::IdTable& idtable, Log::LOG& log, int lextable_pos, ltvec& v)
    {
        // �������������� ������
        vector<LT::Entry> result;
        // ���� ��� ���������� ����������
        stack<LT::Entry> s;
        // ���� ������ �������
        bool ignore = false;

        for (unsigned i = 0; i < v.size(); i++)
        {
            if (ignore) // ����� ������� ������� ������������� ��������� � ������� � ���������
            {
                result.push_back(v[i]);
                if (v[i].lexema == LEX_RIGHTTHESIS)
                    ignore = false;
                continue;
            }
            int priority = getPriority(v[i]); // ��� ���������

            if (v[i].lexema == LEX_LEFTHESIS || v[i].lexema == LEX_RIGHTTHESIS || v[i].lexema == LEX_PLUS || v[i].lexema == LEX_MINUS || v[i].lexema == LEX_STAR || v[i].lexema == LEX_DIRSLASH || v[i].lexema == LEX_PERSENT)
            {
                if (s.empty() || v[i].lexema == LEX_LEFTHESIS)
                {
                    s.push(v[i]);
                    continue;
                }

                if (v[i].lexema == LEX_RIGHTTHESIS)
                {
                    // ������������ ��������� �� ������
                    while (!s.empty() && s.top().lexema != LEX_LEFTHESIS)
                    {
                        result.push_back(s.top());
                        s.pop();
                    }
                    if (!s.empty() && s.top().lexema == LEX_LEFTHESIS)
                        s.pop();
                    continue;
                }
                // ������������ ��������� � �������/������ ����������� � ���������
                while (!s.empty() && getPriority(s.top()) >= priority)
                {
                    result.push_back(s.top());
                    s.pop();
                }
                s.push(v[i]);
            }

            if (v[i].lexema == LEX_LITERAL || v[i].lexema == LEX_ID) // �������������, ������������� ������� ��� �������
            {
                if (idtable.table[v[i].idxTI].idtype == IT::IDTYPE::F || idtable.table[v[i].idxTI].idtype == IT::IDTYPE::S)
                    ignore = true;
                result.push_back(v[i]); // ������� ������� � �������������� ������
            }
            if (v[i].lexema != LEX_LEFTHESIS & v[i].lexema != LEX_RIGHTTHESIS & v[i].lexema != LEX_PLUS & v[i].lexema != LEX_MINUS & v[i].lexema != LEX_STAR & v[i].lexema != LEX_DIRSLASH & v[i].lexema != LEX_PERSENT & v[i].lexema != LEX_ID & v[i].lexema != LEX_LITERAL)
            {
                Log::writeError(log.stream, Error::GetError(1));
                return false;
            }
        }

        while (!s.empty()) { result.push_back(s.top()); s.pop(); }
        v = result;
        return true;
    }
}
