#include "MemFile.h"
#include "HexParser.h"
#include "SParser.h"
#include "Z80Parser.h"

#include <algorithm>
/*This is used for endianness conversion*/
#include <intrin.h>

MemFile::MemFile(size_t nFileSize) :
	m_Size(nFileSize),
	m_pFile(nullptr)
{
	m_pFile = (uint8_t*)malloc(nFileSize);
}

MemFile::~MemFile()
{
	if (m_pFile)
		free(m_pFile);
}

/*MemFile Definitions*/
void MemFile::load(const std::string & sFilePath)
{
	int dotIndex = sFilePath.find_last_of('.');
	if (dotIndex >= 0)
	{
		std::string sExt = sFilePath.substr(dotIndex + 1);
		std::ifstream file(sFilePath, std::ios_base::binary);
		std::shared_ptr<ParserBase> pParser;

		if (sExt == "s")
			pParser = std::make_shared<SParser>();
		else if (sExt == "hex")
			pParser = std::make_shared<HexParser>();
		else if (sExt == "z80")
			pParser = std::make_shared<Z80Parser>();
		else
			throw NoParserException(sExt);

		if (pParser)
			pParser->load(file, m_pFile);
	}
	else
		throw NoParserException("");
}

uint8_t MemFile::hexToDec(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);

	return 0;
}
