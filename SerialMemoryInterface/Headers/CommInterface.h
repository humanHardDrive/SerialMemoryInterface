#pragma once

#include <functional>
#include <chrono>

#include "boost/asio.hpp"
#include "boost/asio/serial_port.hpp"

#include "MemFile.h"

class CommInterface
{
public:
	CommInterface(const std::string& sDeviceName, boost::asio::io_context& ioContext);

	void run();
	void stop();

private:
	void stateProcess(size_t nBytes);

	static const uint8_t SERIAL_STX = 0xAA;
	static const uint8_t SERIAL_ETX = 0x55;

	struct MessageHeader
	{
		uint8_t cmd;
		uint32_t len;
	};

	boost::asio::serial_port m_SerialPort;
	std::string m_sDeviceName;
	uint8_t m_SerialBuf[128];

	std::chrono::system_clock::time_point m_LastCommTime;
	MessageHeader m_CurrentMessageHeader;
	uint8_t* m_pCurrentMessageHeader;
	uint8_t* m_pCurrentMessage;

	std::function<bool(uint8_t)> m_aProcessingFn[256];
};