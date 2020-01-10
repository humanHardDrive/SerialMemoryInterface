#include "MemFile.h"
#include "ParserException.h"

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

		if (sExt == "s")
			loadSFile(file);
		else if (sExt == "hex")
			loadHexFile(file);
		else
			throw NoParserException();

		mergeBlocks();
	}
	else
		throw NoParserException();
}

void MemFile::clear()
{
	for (MemBlock block : m_Blocks)
		block.cleanup();
}

void MemFile::loadHexFile(std::ifstream& file)
{
	char buf[2 * 256];

	if (file.is_open())
	{
		HEX_PARSE_STATE state = HEX_PARSE_STATE::START_CODE;
		std::queue<HexRecord> records;

		while (file.read(buf, HEX_PARSER[state]))
		{
			switch (state)
			{
			case HEX_PARSE_STATE::START_CODE:
				if (buf[0] == ':')
				{
					records.push(HexRecord());
					state = HEX_PARSE_STATE::BYTE_COUNT;
				}
				/*Ignore any start of line characters*/
				else if (buf[0] != '\r' && buf[0] != '\n')
					throw InvalidHexStartTokenException();
				break;

			case HEX_PARSE_STATE::BYTE_COUNT:
				records.back().m_Record.m_Size = hexToDec(buf[0]) * 16 + hexToDec(buf[1]);
				/*Because each byte is stored as 2 hex characters, the size is doubled*/
				HEX_PARSER[HEX_PARSE_STATE::DATA] = records.back().m_Record.m_Size * 2;

				state = HEX_PARSE_STATE::ADDRESS;
				break;

			case HEX_PARSE_STATE::ADDRESS:
				/*Address is given in big endian format*/
				records.back().m_Record.m_Address = (hexToDec(buf[0]) * 4096) + (hexToDec(buf[1]) * 256) + (hexToDec(buf[2]) * 16) + (hexToDec(buf[3]) * 1);

				state = HEX_PARSE_STATE::RECORD_TYPE;
				break;

			case HEX_PARSE_STATE::RECORD_TYPE:
				records.back().m_Record.m_RecordType = (hexToDec(buf[0]) * 16 + hexToDec(buf[1]));

				state = HEX_PARSE_STATE::DATA;
				break;

			case HEX_PARSE_STATE::DATA:
				for (uint16_t i = 0; i < HEX_PARSER[HEX_PARSE_STATE::DATA] / 2; i++)
					records.back().m_Record.m_Data[i] = hexToDec(buf[2 * i]) * 16 + hexToDec(buf[2 * i + 1]);

				state = HEX_PARSE_STATE::CHECKSUM;
				break;

			case HEX_PARSE_STATE::CHECKSUM:
				records.back().m_Checksum = hexToDec(buf[0]) * 16 + hexToDec(buf[1]);

				state = HEX_PARSE_STATE::START_CODE;
				break;
			}
		}

		/*Not having reached the EOF would indicate something went wrong*/
/*
		if (!file.eof())
			throw FileNotFoundException();*/

		file.close();

		parseHexRecords(records);
	}
	else
		throw FileNotFoundException();
}

void MemFile::parseHexRecords(std::queue<HexRecord>& records)
{
	bool bEndFound = false;
	uint64_t addressOffset = 0;

	while (!bEndFound && !records.empty())
	{
		/*Check the checksum*/
		if (records.front().m_Checksum != records.front().calcChecksum(&records.front().m_Record))
			throw InvalidChecksumException();

		switch (records.front().m_Record.m_RecordType)
		{
		case HexRecordData::RECORD_TYPE_DATA:
			m_Blocks.push_back(MemBlock());
			m_Blocks.back().add(records.front().m_Record.m_Address + addressOffset, records.front().m_Record.m_Data, records.front().m_Record.m_Size);
			m_Size += m_Blocks.back().size();

			std::push_heap(m_Blocks.begin(), m_Blocks.end(),
				[](MemBlock a, MemBlock b)
			{
				if (a.startAddr() < b.startAddr())
					return true;

				return false;
			});
			break;

			/*Shift 16*/
		case HexRecordData::RECORD_TYPE_ELA:
			addressOffset = _byteswap_ushort(*(unsigned short*)records.front().m_Record.m_Data);
			addressOffset <<= 16;
			break;

		case HexRecordData::RECORD_TYPE_SLA:
			break;

			/*Times 16*/
		case HexRecordData::RECORD_TYPE_ESA:
			addressOffset = _byteswap_ushort(*(unsigned short*)records.front().m_Record.m_Data);
			addressOffset *= 16;
			break;

		case HexRecordData::RECORD_TYPE_SSA:
			break;

		case HexRecordData::RECORD_TYPE_EOF:
			/*All done*/
			/*TODO: Check if there are any records left. Throw?*/
			bEndFound = true;
			break;
		}

		records.pop();
	}

	if (!bEndFound)
		throw EndRecordNotFoundException();

	std::sort_heap(m_Blocks.begin(), m_Blocks.end(),
		[](MemBlock a, MemBlock b)
	{
		if (a.startAddr() < b.startAddr())
			return true;

		return false;
	});
}

void MemFile::loadSFile(std::ifstream& file)
{
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
