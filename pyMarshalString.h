#include <string>
#pragma once

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

		pyMarshalString& operator<<(int   value);
		pyMarshalString& operator<<(char* value);
		pyMarshalString& operator<<(bool  value);
		pyMarshalString& operator<<(float value);

		pyMarshalString& operator<<(char  value);
};