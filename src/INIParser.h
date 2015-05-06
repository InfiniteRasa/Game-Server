#ifndef TR_INI_PARSER_H
#define TR_INI_PARSER_H

#include <string>
#include <minIni.h>

class INIParser
{
	public:
		INIParser(const char* Filename);
		~INIParser();

		void SetFile(const char* Filename);
		bool GetBool(const char* Section, const char* Key, bool Default = false);
		int GetInt(const char* Section, const char* Key, int Default = 0);
		long GetLong(const char* Section, const char* Key, long Default = 0);
		std::string GetString(const char* Section, const char* Key, const char* Default = "");

	private:
		std::string File;
};

#endif