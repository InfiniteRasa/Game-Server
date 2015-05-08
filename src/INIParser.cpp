#include "INIParser.h"

INIParser::INIParser(const char* Filename)
{
	this->File = std::string(Filename);
}

INIParser::~INIParser()
{
}

void INIParser::SetFile(const char* Filename)
{
	this->File = std::string(Filename);
}

bool INIParser::GetBool(const char* Section, const char* Key, bool Default)
{
	int def = Default == 1 ? true : false;
	int ret = ini_getbool(Section, Key, def, this->File.c_str());
	bool ret2= ret == 1 ? true : false;
	return ret2;
}

int INIParser::GetInt(const char* Section, const char* Key, int Default)
{
	long ret = ini_getl(Section, Key, (long) Default, this->File.c_str());
	return (int)ret;
}

long INIParser::GetLong(const char* Section, const char* Key, long Default)
{
	long ret = ini_getl(Section, Key, Default, this->File.c_str());
	return ret;
}

std::string INIParser::GetString(const char* Section, const char* Key, const char* Default)
{
	char buffer[255];
	char* buff = buffer;
	int ret = ini_gets(Section, Key, Default, buff, 255, this->File.c_str());
	std::string rets = std::string(buff);
	return rets;
}
