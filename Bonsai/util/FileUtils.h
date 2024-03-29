﻿#pragma once
#include <vector>


namespace bonsai
{
	typedef std::string String;


	static std::vector<String> SplitString(const String& string, const char delimiter)
	{
		size_t start = 0;
		size_t end = string.find_first_of(delimiter);

		std::vector<String> result;

		while (end <= String::npos)
		{
			result.emplace_back(string.substr(start, end - start));

			if (end == String::npos)
				break;

			start = end + 1;
			end = string.find_first_of(delimiter, start);
		}

		return result;
	}

	static String ReadFile(const String& filepath)
	{
		FILE* file = fopen(filepath.c_str(), "rt");
		if (file == nullptr)
			std::cout << "file could not be found: " << filepath.c_str() << std::endl;

		fseek(file, 0, SEEK_END);
		unsigned long length = ftell(file);
		char* data = new char[length + 1];
		memset(data, 0, length + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, length, file);
		fclose(file);

		String result(data);
		delete[] data;

		return result;
	}
}
