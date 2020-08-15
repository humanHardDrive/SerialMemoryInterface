#pragma once

#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <vector>

#include "MemFile.h"

class ParserBase
{
public:
	virtual void load(std::ifstream& file, std::vector<MemBlock>& aBlocks) = 0;

protected:
	virtual void mergeBlocks(std::vector<MemBlock>& aBlocks);

	uint8_t hexToDec(char c);
};

class NoParserException : public std::exception
{
public:
	NoParserException(std::string sInfo)
	{
		m_sWhat = "No parser found for " + sInfo;
	}

	const char* what()
	{
		return m_sWhat.c_str();
	}

private:
	std::string m_sWhat;
};

class FileNotFoundException : public std::exception
{

};