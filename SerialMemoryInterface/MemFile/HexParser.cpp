#include "HexParser.h"

#include <algorithm>

void HexParser::load(std::ifstream& file, uint8_t* pFile)
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
					throw InvalidHexStartTokenException(buf[0]);
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

		parseRecords(records, pFile);
	}
	else
		throw FileNotFoundException();
}

void HexParser::parseRecords(std::queue<HexRecord>& records, uint8_t* pFile)
{
	bool bEndFound = false;
	uint64_t addressOffset = 0;

	while (!bEndFound && !records.empty())
	{
		/*Check the checksum*/
		if (records.front().m_Checksum != records.front().calcChecksum(&records.front().m_Record))
			throw InvalidHexChecksumException(records.front().m_Checksum, 0);

		switch (records.front().m_Record.m_RecordType)
		{
		case HexRecordData::RECORD_TYPE_DATA:
			memcpy(pFile + records.front().m_Record.m_Address + addressOffset, records.front().m_Record.m_Data, records.front().m_Record.m_Size);
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
		throw HexEndRecordNotFoundException();
}