#include "Z80Parser.h"

void Z80Parser::load(std::ifstream & file, uint8_t * pFile)
{
	char buf[128];
	uint16_t nStartAddress = 0;

	if (file.is_open())
	{
		Z80_PARSE_STATE state = Z80_PARSE_STATE::HEADER;

		memset(buf, 0, sizeof(buf));
		while (file.read(buf, Z80_PARSER[state]))
		{
			switch (state)
			{
			case Z80_PARSE_STATE::HEADER:
				/*Verify that the header is valid*/
				if (memcmp(buf, Z80_HEADER, Z80_PARSER[state]) == 0)
					state = Z80_PARSE_STATE::ADDRESS;
				else
					throw std::exception();
				break;

			case Z80_PARSE_STATE::ADDRESS:
				/*Copy the starting address*/
				memcpy(&nStartAddress, buf, Z80_PARSER[state]);
				state = Z80_PARSE_STATE::DATA;
				Z80_PARSER[state] = sizeof(buf); //Setup the data read size
				//This will continue to read until there is no data left
				break;

			case Z80_PARSE_STATE::DATA:
				memcpy(pFile + nStartAddress, buf, Z80_PARSER[state]);
				pFile += nStartAddress;
				break;
			}

			memset(buf, 0, sizeof(buf));
		}

		file.close();
	}
	else
		throw FileNotFoundException();
}
