#pragma once

#include "IT.h"
#include "LexAnalysis.h"
#include "LT.h"

#define SEPSTREMP "\n;------------------------------\n"
#define SEPSTR(x) "\n;----------- " + string(x) + " ------------\n"

#define BEGIN ".586\n"\
".model flat, stdcall\n"\
"includelib libucrt.lib\n"\
"includelib kernel32.lib\n"\
"includelib \../Debug/StaticLibrary.lib\n"\
"ExitProcess PROTO:DWORD \n"\
".stack 4096\n"

#define END "push 0\ncall ExitProcess\nmain ENDP\nend main"

#define EXTERN "\n outlich PROTO : DWORD\n"\
"\n outrad PROTO : DWORD\n"\
"\n concat PROTO : DWORD, : DWORD, : DWORD\n"\
"\n compare PROTO : DWORD, : DWORD, : DWORD\n"\
"\n rnd PROTO : DWORD, : DWORD, : DWORD\n"\
"\n slength PROTO : DWORD, : DWORD\n"\
"\n aton  PROTO : DWORD,  : DWORD\n"

#define ITENTRY(x) tables.idtable.table[tables.lextable.table[x].idxTI]
#define LEXEMA(x) tables.lextable.table[x].lexema

#define CONST ".const\n\t\tnewline byte 13, 10, 0"
#define DATA ".data\n\t\ttemp sword ?\n\t\tbuffer byte 256 dup(0)"
#define CODE ".code"

namespace Gener {
    void CodeGeneration(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log);
};
