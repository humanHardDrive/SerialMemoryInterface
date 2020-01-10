#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <fstream>

#include "FileRecords.h"

#ifndef UNITIALIZED_DATA
#define UNITIALIZED_DATA	0x00
#endif

enum class ENDIANESS
{
	BIG_ENDIAN = 0,
	LITTLE_ENDIAN
};

class MemBlock
{
public:
	MemBlock();

	void cleanup();

	void add(uint64_t addr, uint8_t* pData, size_t size);

	const uint64_t startAddr() { return m_StartAddress; }
	const size_t size() { return m_Size; }
	uint8_t* data() { return m_pData; }

	template<ENDIANESS e>
	void get(uint64_t addr, void* pBuf, size_t count);

	template<ENDIANESS e>
	void set(uint64_t addr, void* pBuf, size_t count);

private:
	void append(uint64_t addr, uint8_t* pData, size_t size);
	void prepend(uint64_t addr, uint8_t* pData, size_t size);

	uint8_t* m_pData;

	uint64_t m_StartAddress;
	size_t m_Size;
};

template<>
inline void MemBlock::get<ENDIANESS::BIG_ENDIAN>(uint64_t addr, void* pBuf, size_t count)
{
	uint64_t readAddr = (addr - m_StartAddress);

	for (size_t i = 0; i < count; i++, readAddr++)
	{
		if (readAddr < m_Size)
			((uint8_t*)pBuf)[count - i - 1] = m_pData[readAddr];
		else
			((uint8_t*)pBuf)[count - i - 1] = (uint8_t)(UNITIALIZED_DATA & 0xFF);
	}
}

template<>
inline void MemBlock::get<ENDIANESS::LITTLE_ENDIAN>(uint64_t addr, void* pBuf, size_t count)
{
	uint64_t readAddr = (addr - m_StartAddress);

	for (size_t i = 0; i < count; i++, readAddr++)
	{
		if (readAddr < m_Size)
			((uint8_t*)pBuf)[i] = m_pData[readAddr];
		else
			((uint8_t*)pBuf)[i] = (uint8_t)(UNITIALIZED_DATA & 0xFF);
	}
}

template<>
inline void MemBlock::set<ENDIANESS::BIG_ENDIAN>(uint64_t addr, void* pBuf, size_t count)
{
	uint64_t readAddr = (addr - m_StartAddress);

	for (size_t i = 0; i < count; i++)
	{
		if (readAddr < m_Size)
			m_pData[readAddr] = ((uint8_t*)pBuf)[count - i - 1];
	}
}

template<>
inline void MemBlock::set<ENDIANESS::LITTLE_ENDIAN>(uint64_t addr, void* pBuf, size_t count)
{
	uint64_t readAddr = (addr - m_StartAddress);

	for (size_t i = 0; i < count; i++, readAddr++)
	{
		if (readAddr < m_Size)
			m_pData[readAddr] = ((uint8_t*)pBuf)[i];
	}
}

class MemFile
{
public:
	~MemFile();

	void load(const std::string& sFilePath);

	template<ENDIANESS e>
	void get(uint64_t addr, void* pBuf, size_t count);

	template<ENDIANESS e>
	void set(uint64_t addr, void* pBuf, size_t count);

	uint64_t applicationStart() { return m_ApplicationStartAddress; }
	uint64_t size() { return m_Size; }

private:
	enum class HEX_PARSE_STATE
	{
		START_CODE = 0,
		BYTE_COUNT,
		ADDRESS,
		RECORD_TYPE,
		DATA,
		CHECKSUM
	};

	static const uint8_t HEX_RECORD_TYPE_DATA = 0;
	static const uint8_t HEX_RECORD_TYPE_EOF = 1; /*End of file*/
	static const uint8_t HEX_RECORD_TYPE_ESA = 2; /*Extended segment address*/
	static const uint8_t HEX_RECORD_TYPE_SSA = 3; /*Start segment address*/
	static const uint8_t HEX_RECORD_TYPE_ELA = 4; /*Extended linear address*/
	static const uint8_t HEX_RECORD_TYPE_SLA = 5; /*Start linear address*/

	std::map<HEX_PARSE_STATE, size_t> HEX_PARSER = {
		{HEX_PARSE_STATE::START_CODE, 1},
		{HEX_PARSE_STATE::BYTE_COUNT, 2},
		{HEX_PARSE_STATE::ADDRESS, 4},
		{HEX_PARSE_STATE::RECORD_TYPE, 2},
		{HEX_PARSE_STATE::DATA, 0}, /*This is set to the correct length later*/
		{HEX_PARSE_STATE::CHECKSUM, 2}
	};

	enum class S_PARSE_STATE
	{
		START_CODE = 0
	};

	std::map<S_PARSE_STATE, size_t> S_PARSER;

	uint64_t m_ApplicationStartAddress;
	uint64_t m_Size;

	void clear();

	void loadHexFile(std::ifstream& file);
	void parseHexRecords(std::queue<HexRecord>& records);

	void loadSFile(std::ifstream& file);

	void mergeBlocks();
	bool areBlocksAdj(MemBlock& a, MemBlock& b);

	uint8_t hexToDec(char c);

	std::vector<MemBlock> m_Blocks;
};

template<ENDIANESS e>
inline void MemFile::get(uint64_t addr, void * pBuf, size_t count)
{
	for (MemBlock block : m_Blocks)
	{
		if (addr >= block.startAddr() && addr < (block.startAddr() + block.size()))
		{
			block.get<e>(addr, pBuf, count);
			return;
		}
	}
}

template<ENDIANESS e>
inline void MemFile::set(uint64_t addr, void * pBuf, size_t count)
{
	for (MemBlock block : m_Blocks)
	{
		if (addr >= block.startAddr() && addr < (block.startAddr() + block.size()))
		{
			block.set<e>(addr, pBuf, count);
			return;
		}
	}
}