#include "pyMarshalString.h"

pyMarshalString::pyMarshalString()
{
	this->Init();
	// todo: dynamic buffer allocation ?
	//		 global buffer pointer instead of local buffer?
}

pyMarshalString::~pyMarshalString()
{
}

void pyMarshalString::Init()
{
	this->stackIndex = 0;
	this->index = 0;
}

void pyMarshalString::TupleBegin()
{
	if (this->index + 5 > BUFFER_SIZE)
	{ printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__); }
	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }
	
	this->containerStack[this->stackIndex].type = 't';
	this->containerStack[this->stackIndex].startOffset = this->index;
	this->containerStack[this->stackIndex].subelements = 0;

	this->buffer[this->index] = 0x28; 
	this->index++;
	this->index += 4;
	this->stackIndex++;
}

void pyMarshalString::TupleEnd()
{
	this->stackIndex--;
	if (this->containerStack[this->stackIndex].type != 't' )
	{ __debugbreak(); }
	*(unsigned int*)(this->buffer + this->containerStack[this->stackIndex].startOffset + 1) = this->containerStack[this->stackIndex].subelements;
}

void pyMarshalString::ListBegin()
{
	if (this->index + 5 > BUFFER_SIZE)
	{ printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__); }
	if (this->stackIndex )
	{ this->containerStack[this->stackIndex-1].subelements++; }
	this->containerStack[this->stackIndex].type = 'l';
	this->containerStack[this->stackIndex].startOffset = this->index;
	this->containerStack[this->stackIndex].subelements = 0;
	
	this->buffer[this->index] = 0x5B; 
	this->index++;
	this->index += 4;
	this->stackIndex++;
}

void pyMarshalString::ListEnd()
{
	this->stackIndex--;
	if (this->containerStack[this->stackIndex].type != 'l')
	{ __debugbreak(); }
	*(unsigned int*)(this->buffer + this->containerStack[this->stackIndex].startOffset + 1) = this->containerStack[this->stackIndex].subelements;
}
	
void pyMarshalString::DictBegin()
{
	if (this->index + 1 > BUFFER_SIZE)
	{ printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__); }
	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }
	this->containerStack[this->stackIndex].type = 'd';
	this->containerStack[this->stackIndex].startOffset = this->index;
	this->containerStack[this->stackIndex].subelements = 0;

	this->buffer[this->index] = 0x7B; 
	this->index++;
	this->stackIndex++;
}

void pyMarshalString::DictEnd()
{
	if (this->index + 1 > BUFFER_SIZE)
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	this->stackIndex--;
	if (this->containerStack[this->stackIndex].type != 'd')
	{ __debugbreak(); }
	this->buffer[this->index] = 0x30; 
	this->index++;
}

void pyMarshalString::DictAddKey(char *s)
{
	unsigned int slen = strlen(s);
	if (this->index + slen + 4 > BUFFER_SIZE)
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	this->buffer[this->index] = 0x74; // 't'
	this->index++;
	*(int*)(this->buffer + this->index) = slen; 
	this->index += 4;

	for(unsigned int i = 0; i < slen; i++)
	{
		*(char*)(this->buffer + this->index) = s[i]; 
		this->index++;
	}

	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }

}
	
void pyMarshalString::AddInt(int value)
{
	if (this->index + 5 > BUFFER_SIZE)
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	this->buffer[this->index] = 0x69; // 'i'
	this->index++;
	*(int*)(this->buffer + this->index) = value;
	this->index += 4;
	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }
}

void pyMarshalString::AddString(char *s)
{	
	unsigned int slen = strlen(s);
	if (this->index + slen + 4 > BUFFER_SIZE)
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	this->buffer[this->index] = 's'; // 0x73
	this->index++;
	*(int*)(this->buffer + this->index) = slen;
	this->index += 4;

	for(unsigned int i = 0; i < slen; i++)
	{
		*(char*)(this->buffer + this->index) = s[i];
		this->index++;
	}

	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }
}

void pyMarshalString::AddUnicode(char *s)
{
	unsigned int slen = strlen(s);
	if (this->index + slen + 4 > BUFFER_SIZE)
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	this->buffer[this->index] = 'u';
	this->index++;
	*(int*)(this->buffer + this->index) = slen;
	this->index += 4;

	for(unsigned int i = 0; i < slen; i++)
	{
		*(char*)(this->buffer + this->index) = s[i];
		this->index++;
	}

	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }
}

void pyMarshalString::AddNoneStruct()
{
	if (this->index + 1 > BUFFER_SIZE)
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	this->buffer[this->index] = 0x4E;
	this->index++;

	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }
}

void pyMarshalString::AddBool(bool b)
{
	if (this->index + 1 > BUFFER_SIZE)
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	this->buffer[this->index] = b ? 'T' : 'F';
	this->index++;

	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }
}

void pyMarshalString::AddFloat(float value)
{
	char floatData[128];
	sprintf(floatData, "%f", value);
	int len = strlen(floatData);
	if (this->index + 2 + len > BUFFER_SIZE)
	{ printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__); }
	this->buffer[this->index] = 0x66;
	this->index++;
	this->buffer[this->index] = len;
	this->index++;

	for(int i = 0; i < len; i++)
	{
		*(char*)(this->buffer + this->index) = floatData[i]; 
		this->index++;
	}

	if (this->stackIndex)
	{ this->containerStack[this->stackIndex-1].subelements++; }
}

unsigned char* pyMarshalString::Data()
{
	return this->buffer;
}
unsigned int pyMarshalString::Length()
{
	return this->index;
}

////////////////////////////////////////////////////

pyMarshalString& pyMarshalString::operator<<(uint64 value)
{ this->AddInt(value); return *this; } // need to cast?

pyMarshalString& pyMarshalString::operator<<(sint64 value)
{ this->AddInt(value); return *this; }

pyMarshalString& pyMarshalString::operator<<(uint32 value)
{ this->AddInt(value); return *this; }

pyMarshalString& pyMarshalString::operator<<(sint32 value)
{ this->AddInt(value); return *this; }

pyMarshalString& pyMarshalString::operator<<(uint16 value)
{ this->AddInt(value); return *this; }

pyMarshalString& pyMarshalString::operator<<(sint16 value)
{ this->AddInt(value); return *this; }

pyMarshalString& pyMarshalString::operator<<(char* value)
{ this->AddString(value); return *this; }

pyMarshalString& pyMarshalString::operator<<(bool value)
{ this->AddBool(value); return *this; }

pyMarshalString& pyMarshalString::operator<<(float value)
{ this->AddFloat(value); return *this; }

pyMarshalString& pyMarshalString::operator<<(char value)
{
	if (value == '(') { this->TupleBegin();		}
	if (value == ')') { this->TupleEnd();		}
	if (value == '[') { this->ListBegin();		}
	if (value == ']') { this->ListEnd();		}
	if (value == 'N') { this->AddNoneStruct();	}

	return *this;
}
