
#pragma once
#include "Global.h"

#define BUFFER_SIZE 2048
typedef struct
{
	char type; // 't', 'd'
	int startOffset;
	int subelements;
} containerStackEntry_tt;

class pyMarshalString
{
	private:
		unsigned char buffer[BUFFER_SIZE];
		int index;
		containerStackEntry_tt containerStack[16];
		int stackIndex;

	public:
		pyMarshalString();
		~pyMarshalString();

		void Init();

		void TupleBegin();
		void TupleEnd();

		void ListBegin();
		void ListEnd();
	
		void DictBegin();
		void DictEnd();
		void DictAddKey(char* value);
	
		void AddInt(int value);
		void AddString(char* value);
		void AddUnicode(char* value);
		void AddNoneStruct();
		void AddBool(bool value);
		void AddFloat(float value);

		unsigned char* Data();
		unsigned int Length();

		// custom types
		pyMarshalString& operator<<(uint64 value);
		pyMarshalString& operator<<(sint64 value);
		pyMarshalString& operator<<(uint32 value);
		pyMarshalString& operator<<(sint32 value);
		pyMarshalString& operator<<(uint16 value);
		pyMarshalString& operator<<(sint16 value);
		// default types
		pyMarshalString& operator<<(char* value);
		pyMarshalString& operator<<(bool  value);
		pyMarshalString& operator<<(float value);
		// char shortcuts
		pyMarshalString& operator<<(char  value);

		
};