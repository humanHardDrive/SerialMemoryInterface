#include "MemFile.h"
#include "HexParser.h"
#include "SParser.h"

#include <algorithm>
/*This is used for endianness conversion*/
#include <intrin.h>

/*MemBlock Definitions*/
MemBlock::MemBlock() :
	m_StartAddress(0),
	m_Size(0),
	m_pData(nullptr)
{
}

void MemBlock::cleanup()
{
	if (m_pData)
	{
		delete m_pData;
		m_pData = nullptr;
	}
}

void MemBlock::add(uint64_t addr, uint8_t* pData, size_t size)
{
	if (!m_pData)
		m_StartAddress = addr;

	if (addr >= m_StartAddress)
		append(addr, pData, size);
	else
		prepend(addr, pData, size);
}

/*Easier case*/
void MemBlock::append(uint64_t addr, uint8_t * pData, size_t size)
{
	size_t spaceNeeded = (size_t)((addr + size) - m_StartAddress);

	m_pData = (uint8_t*)realloc(m_pData, spaceNeeded);

	if (m_pData)
	{
		memcpy(m_pData + (addr - m_StartAddress), pData, size);
		m_Size = spaceNeeded;
	}
}

/*Harder case*/
void MemBlock::prepend(uint64_t addr, uint8_t * pData, size_t size)
{
}

MemFile::~MemFile()
{
	clear();
}

/*MemFile Definitions*/
void MemFile::load(const std::string & sFilePath)
{
	clear();

	int dotIndex = sFilePath.find_last_of('.');
	if (dotIndex >= 0)
	{
		std::string sExt = sFilePath.substr(dotIndex + 1);
		std::ifstream file(sFilePath);
		std::shared_ptr<ParserBase> pParser;

		if (sExt == "s")
			pParser = std::make_shared<SParser>();
		else if (sExt == "hex")
			pParser = std::make_shared<HexParser>();
		else
			throw NoParserException(sExt);

		if (pParser)
			pParser->load(file, m_Blocks);

		mergeBlocks();
	}
	else
		throw NoParserException("");
}

void MemFile::clear()
{
	for (MemBlock block : m_Blocks)
		block.cleanup();
}

/*For optimal performance, the blocks are sorted in ascending order according
to their starting address*/
void MemFile::mergeBlocks()
{
	size_t sampleIndex = 0;

	for (size_t i = 0; i < m_Blocks.size() - 1;)
	{
		/*Check if the two blocks touch*/
		if (areBlocksAdj(m_Blocks[i], m_Blocks[i + 1]))
		{
			/*Concatenate the two blocks together*/
			m_Blocks[i].add(m_Blocks[i + 1].startAddr(), m_Blocks[i + 1].data(), m_Blocks[i + 1].size());
			/*Remove the old block's data*/
			m_Blocks[i + 1].cleanup();
			/*Remove the block from the array*/
			m_Blocks.erase(m_Blocks.begin() + i + 1);
		}
		/*Move on to the next block*/
		else
			i++;
	}
}

bool MemFile::areBlocksAdj(MemBlock & a, MemBlock & b)
{
	/*Need to check if the blocks over lap. Need to also handle in the mem blocks*/
	if ((a.startAddr() + a.size()) == b.startAddr())
		return true;
	else if ((b.startAddr() + b.size()) == a.startAddr())
		return true;

	return false;
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
