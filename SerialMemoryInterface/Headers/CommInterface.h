#pragma once

#include <functional>
#include <map>
#include <chrono>

#include "boost/asio.hpp"
#include "boost/asio/serial_port.hpp"

#include "MemFile.h"

#pragma pack(1)
enum CmdType
{
	READ = 0,
	WRITE,
	ALL_CMD
};

struct ReadWriteMsg
{
	uint32_t startAddr;
	uint32_t len;
};

class CommInterface
{
public:
	CommInterface(const std::string& sDeviceName, const unsigned int nBaudRate, boost::asio::io_context& ioContext);
	~CommInterface();

	void run();
	void stop();

	void sendMessage(uint8_t cmdType, size_t nBytes, void* pBuf);
	void sendMessage(uint8_t cmdType, size_t nCount, std::initializer_list<size_t> nBytes, std::initializer_list<void*> pBuf);

private:
	void asyncReadCallback(const boost::system::error_code& errorCode, size_t nBytesCount);
	void asyncWriteCallback(const boost::system::error_code& errorCode, size_t nBytesCount);

	void stateProcess(size_t nBytes);

	bool processRead(uint8_t c);
	bool processWrite(uint8_t c);
	bool invalidProcess(uint8_t c);

	static const uint8_t SERIAL_STX = 0xAA;
	static const uint8_t SERIAL_ETX = 0x55;

	struct MessageHeader
	{
		uint8_t cmd;
		uint32_t len;
	};

	boost::asio::serial_port m_SerialPort;
	std::string m_sDeviceName;
	unsigned int m_nBaudRate;
	uint8_t m_SerialBuf[128];

	std::chrono::system_clock::time_point m_LastCommTime;
	MessageHeader m_CurrentMessageHeader;
	uint8_t *m_pCurrentMessageHeader;
	uint8_t *m_pCurrentMessage, *m_pCurrentMessageIndex;

	std::map<CmdType, std::function<bool(uint8_t)>> m_ProcessFnMap;
	std::function<bool(uint8_t)> m_CurrentProcessFn;
	bool m_bDoneProcessing;
};