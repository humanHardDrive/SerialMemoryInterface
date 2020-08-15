#pragma once

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
	boost::asio::serial_port m_SerialPort;
	std::string m_sDeviceName;
	uint8_t m_SerialBuf[128];
};