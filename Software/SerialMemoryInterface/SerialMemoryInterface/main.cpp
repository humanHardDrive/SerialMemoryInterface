#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

#include "Msgs.h"
#include "SerialPort.h"

void HandleVersionResponse(char* buffer)
{
	VERSION_REQUEST_RSP rsp;
	memcpy(&rsp, buffer, sizeof(VERSION_REQUEST_RSP));

	std::cout << __FUNCTION__ << "\tv" << (int)rsp.VersionMajor << "." << (int)rsp.VersionMinor << std::endl;
}

void HandleChachSizeResponse(char* buffer)
{
	CACHE_REQUEST_RSP rsp;
	memcpy(&rsp, buffer, sizeof(CACHE_REQUEST_RSP));

	std::cout << __FUNCTION__ << "\tWord Size: " << rsp.WordSize << "\tCache Slots: " << rsp.CacheSize << std::endl;
}

void HandleMemoryReadRequest(char* buffer)
{
	uint16_t address = *((uint16_t*)buffer);

	std::cout << __FUNCTION__ << "\tAddress: " << address << std::endl;
}

void HandleMemoryWriteRequest(char* buffer)
{
	uint16_t address = *((uint16_t*)buffer);

	std::cout << __FUNCTION__ << "\tAddress: " << address << std::endl;
}

void HandleCommand(char* buffer, unsigned int len)
{
	if (!len)
		return;

	switch (buffer[0])
	{
		case NO_COMMAND:
		std::cout << __FUNCTION__ << "\t" << "NO_COMMAND" << std::endl;
		break;

		case VERSION_REQUEST:
		std::cout << __FUNCTION__ << "\t" << "VERSION_REQUEST" << std::endl;
		HandleVersionResponse(buffer + 1);
		break;

		case CACHE_SIZE_REQUEST:
		std::cout << __FUNCTION__ << "\t" << "CACHE_SIZE_REQUEST" << std::endl;
		HandleChachSizeResponse(buffer + 1);
		break;

		case MEMORY_REQUEST:
		std::cout << __FUNCTION__ << "\t" << "MEMORY_REQUEST" << std::endl;
		HandleMemoryReadRequest(buffer + 1);
		break;
		
		case MEMORY_WRITE:
		std::cout << __FUNCTION__ << "\t" << "MEMORY_WRITE" << std::endl;
		break;

		default:
		std::cout << __FUNCTION__ << "\t" << "UNKNOWN_COMMAND" << std::endl;
		break;
	}
}

void ParseSerialPacket(char c)
{
	static unsigned int parsestate = WAITING_FOR_STX;
	static unsigned int datalength = 0, datacount = 0;
	static char databuffer[MAX_PACKET_LENGTH];

	switch (parsestate)
	{
		case WAITING_FOR_STX:
		if (c == PACKET_STX)
		{
			parsestate = WAITING_FOR_LEN_LSB;
			datalength = datacount = 0;
		}
		break;

		case WAITING_FOR_LEN_LSB:
		datalength |= (unsigned int)c;
		parsestate = WAITING_FOR_LEN_MSB;
		break;

		case WAITING_FOR_LEN_MSB:
		datalength |= ((unsigned int)c) << 8;

		if (datalength != 0)
			parsestate = WAITING_FOR_DATA;
		else
			parsestate = WAITING_FOR_ETX;
		break;

		case WAITING_FOR_DATA:
		databuffer[datacount++] = c;

		if (datacount == datalength)
			parsestate = WAITING_FOR_ETX;
		break;

		case WAITING_FOR_ETX:
		if (c == PACKET_ETX)
		{
			parsestate = WAITING_FOR_STX;

			HandleCommand(databuffer, datalength);
		}
		break;
	}
}

int main(int argc, char** argv)
{
	char* portname = "\\\\.\\COM3";
	char buffer[MAX_PACKET_LENGTH];
	
	SerialPort meminterface(portname);

	if (!meminterface.isConnected())
	{
		std::cout << "WHAT?" << std::endl;
		return -1;
	}

	while (meminterface.isConnected())
	{
		unsigned int bytesread = meminterface.readSerialPort(buffer, MAX_PACKET_LENGTH);

		for (unsigned int i = 0; i < bytesread; i++)
			ParseSerialPacket(buffer[i]);
	}

	return 0;
}