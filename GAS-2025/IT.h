#pragma once
#include <iostream>

#define MAXSIZE_ID	16						//   
#define SCOPED_ID_MAXSIZE   MAXSIZE_ID*2	//    + 
#define MAXSIZE_TI		4096				//    
#define SHORT_DEFAULT	0x0000				//   digit
#define STR_DEFAULT	0x00					//   sting
#define NULLIDX_TI		0xffffffff			//  
#define STR_MAXSIZE	255						//   
#define CHAR_MAXSIZE 1						//   
#define SHORT_MAXSIZE   32767			//   
#define SHORT_MINSIZE   -32768			//   
#define MAX_PARAMS_COUNT 3					//  
#define CONCAT_PARAMS_CNT 2					//-  concat
#define RANDOM_PARAMS_CNT 2					//-  random
#define LENGHT_PARAMS_CNT 1					//-  lenght
#define ATON_PARAMS_CNT 1					//-  atoii
#define COMPARE_PARAMS_CNT 2				//-  compare
#define CONCAT_TYPE IT::IDDATATYPE::STR
#define COMPARE_TYPE IT::IDDATATYPE::SHORT
#define LENGHT_TYPE IT::IDDATATYPE::SHORT
#define RANDOM_TYPE IT::IDDATATYPE::SHORT
#define ATON_TYPE IT::IDDATATYPE::SHORT


namespace IT
{
	enum IDDATATYPE { SHORT = 1, STR = 2, PROC = 3, CHAR = 4, UNDEF };//  : , , ( ), 
	enum IDTYPE { V = 1, F = 2, P = 3, L = 4, S = 5 };	// : ,, ,, 
	enum STDFNC { F_COMPARE, F_RANDOM, F_CONCAT, F_LENGHT, F_ATON, F_NOT_STD };	//
	static const IDDATATYPE CONCAT_PARAMS[] = { IT::IDDATATYPE::STR, IT::IDDATATYPE::STR };//  concatstr
	static const IDDATATYPE LENGHT_PARAMS[] = { IT::IDDATATYPE::STR };// strlen
	static const IDDATATYPE ATON_PARAMS[] = { IT::IDDATATYPE::STR };//- atoi
	static const IDDATATYPE RANDOM_PARAMS[] = { IT::IDDATATYPE::SHORT, IT::IDDATATYPE::SHORT };//- atoi
	static const IDDATATYPE COMPARE_PARAMS[] = { IT::IDDATATYPE::STR, IT::IDDATATYPE::STR };//- atoi
	struct Entry
	{
		union
		{
			short	vnum;            			// integer
			struct
			{
				int len;					// 
				char str[STR_MAXSIZE - 1];//
			} vstr;							// 
			struct
			{
				int count;					//  
				IDDATATYPE* types;			// 
			} params;
		} value;						// 
		int			idxfirstLE;				// 		
		char		id[SCOPED_ID_MAXSIZE];	//
		IDDATATYPE	iddatatype;				// 
		IDTYPE		idtype;					// 

		Entry()							//  
		{
			this->value.vnum = SHORT_DEFAULT;
			this->value.vstr.len = NULL;
			this->value.params.count = NULL;
		};
		Entry(char* id, int idxLT, IDDATATYPE datatype, IDTYPE idtype) // 
		{
			strncpy_s(this->id, id, SCOPED_ID_MAXSIZE - 1);
			this->idxfirstLE = idxLT;
			this->iddatatype = datatype;
			this->idtype = idtype;
		};
	};
	struct IdTable		// 
	{
		int maxsize;	//  < TI_MAXSIZE
		int size;		//   < maxsize
		Entry* table;	//  
	};
	IdTable Create(int size = NULL);	//  < TI_MAXSIZE
	void Add(					//  
		IdTable& idtable,		// 
		Entry entry);			//  
	int isId(					//:  (), TI_NULLIDX()
		IdTable& idtable,		// 
		char id[SCOPED_ID_MAXSIZE]);	// 
	bool SetValue(IT::Entry* entry, char* value);	//  
	bool SetValue(IT::IdTable& idtable, int index, char* value);
	void writeIdTable(std::ostream* stream, IT::IdTable& idtable); // 
};
