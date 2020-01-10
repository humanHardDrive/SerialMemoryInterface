#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>

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