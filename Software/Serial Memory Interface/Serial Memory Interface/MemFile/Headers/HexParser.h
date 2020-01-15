#pragma once
#include "ParserBase.h"

class HexParser : public ParserBase
{
public:
	void load(std::ifstream& file, std::vector<MemBlock>& aBlocks);

protected:
	void mergeBlocks(std::vector<MemBlock>& aBlocks);

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

	struct HexRecordData
	{
		static const size_t MAX_DATA_SIZE = 256;

		static const uint8_t RECORD_TYPE_DATA = 0;
		static const uint8_t RECORD_TYPE_EOF = 1;
		static const uint8_t RECORD_TYPE_ESA = 2;
		static const uint8_t RECORD_TYPE_SSA = 3;
		static const uint8_t RECORD_TYPE_ELA = 4;
		static const uint8_t RECORD_TYPE_SLA = 5;

		uint16_t m_Address;
		size_t m_Size;
		uint8_t m_RecordType;
		uint8_t m_Data[MAX_DATA_SIZE];

		HexRecordData()
		{
			memset(m_Data, 0, MAX_DATA_SIZE);
		}
	};

	struct HexRecord
	{
		HexRecordData m_Record;
		uint8_t m_Checksum;

		uint8_t calcChecksum(HexRecordData* record)
		{
			uint8_t checksum = 0;

			for (size_t i = 0; i < sizeof(HexRecordData); i++)
				checksum += ((uint8_t*)record)[i];

			checksum = ~checksum;
			checksum++;

			return checksum;
		}
	};

	void parseRecords(std::queue<HexRecord>& records, std::vector<MemBlock>& aBlocks);
};

class InvalidHexStartTokenException : public std::exception
{
public:
	InvalidHexStartTokenException(char cToken)
	{
		m_sWhat = "Invalid token " + cToken;
		m_sWhat += " vs ':'";
	}

	const char* what()	{ return m_sWhat.c_str(); }

private:
	std::string m_sWhat;
};

class InvalidHexChecksumException : public std::exception
{
public:
	InvalidHexChecksumException(uint8_t fileChecksum, uint8_t calcChecksum)
	{
		m_sWhat = "Invalid checksum " + std::to_string(fileChecksum) + " vs " + std::to_string(calcChecksum);
	}

	const char* what() { return m_sWhat.c_str(); }

private:
	std::string m_sWhat;
};

class HexEndRecordNotFoundException : public std::exception
{

};