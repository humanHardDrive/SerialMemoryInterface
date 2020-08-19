#pragma once
#include "ParserBase.h"

class Z80Parser : public ParserBase
{
public:
	void load(std::ifstream& file, uint8_t* pFile);

private:
	enum class Z80_PARSE_STATE
	{
		HEADER = 0,
		ADDRESS,
		DATA
	};
};