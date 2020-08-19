#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <fstream>

#ifndef UNITIALIZED_DATA
#define UNITIALIZED_DATA	0x00
#endif

enum class ENDIANESS
{
	BIG_ENDIAN = 0,
	LITTLE_ENDIAN
};

class MemFile
{
public:
	MemFile(size_t nFileSize);
	~MemFile();

	void load(const std::string& sFilePath);

	uint64_t applicationStart() { return m_ApplicationStartAddress; }
	uint64_t size() { return m_Size; }

private:
	uint64_t m_ApplicationStartAddress;
	size_t m_Size;

	uint8_t* m_pFile;

	void clear();

	uint8_t hexToDec(char c);
};