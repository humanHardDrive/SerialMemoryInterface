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

	static constexpr char Z80_HEADER[8] = {'Z', '8', '0', 'A', 'S', 'M', 0x1a, 0x0a};

	std::map<Z80_PARSE_STATE, size_t> Z80_PARSER = 
	{
		{Z80_PARSE_STATE::HEADER, 8},
		{Z80_PARSE_STATE::ADDRESS, 2},
		{Z80_PARSE_STATE::DATA, 0}
	};
};