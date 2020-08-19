#pragma once
#include "ParserBase.h"

/*https://www.gsp.com/cgi-bin/man.cgi?section=5&topic=z80-file*/
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