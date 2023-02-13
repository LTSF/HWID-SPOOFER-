#pragma once

#include <windows.h>
#include <string>

#include <fstream>
#include <ctime>

#include <vector>

namespace integrity
{
	namespace utilities
	{
		void get_clipboard_data();

		std::vector< std::string > split_string(const std::string& str, const std::string& by);

		char* get_data(std::string url);
	}

	bool check();
}

struct global_integrity
{
	std::string token;

	struct
	{
		std::string type;
	} gpu;
};

extern global_integrity integrity_intellisense;