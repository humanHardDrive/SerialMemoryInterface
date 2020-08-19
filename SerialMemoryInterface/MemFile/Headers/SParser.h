#pragma once

#include "ParserBase.h"

class SParser : public ParserBase
{
public:
	void load(std::ifstream& file, uint8_t* pFile);

private:
	enum class S_PARSE_STATE
	{

	};

	void parseRecords();
};