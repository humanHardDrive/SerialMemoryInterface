#pragma once

#include "ParserBase.h"

class SParser : public ParserBase
{
public:
	void load(std::ifstream& file, std::vector<MemBlock>& aBlocks);

private:
	enum class S_PARSE_STATE
	{

	};

	void parseRecords();
};