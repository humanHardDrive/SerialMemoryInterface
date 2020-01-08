#pragma once

#include <string>

#include "ControlInterface.h"
#include <boost/asio.hpp>

enum class SERIAL_MODE
{
	MODE_8N1 = 0,
	MODE_8N2,
	MODE_8E1,
	MODE_8E2,
	MODE_8O1,
	MODE_8O2
};

/*
SerialInterface Class
This is the interface class between the hardware and software layer. It's responsible
for handling all communication over over serial and calling the appropriate handler
*/
class SerialInterface : public ControlInterface
{
public:
	SerialInterface(boost::asio::io_context& ioContext);
	SerialInterface(boost::asio::io_context& ioContext, const std::string& sPortName, uint32_t nBaudRate, SERIAL_MODE mode);
	~SerialInterface();
	
	/*Only allow these values to change when not connected*/
	void setPort(const std::string& sPortName)	{ if (!m_bConnected) m_sPortName = sPortName; }
	void setBaud(uint32_t nBaud)				{ if (!m_bConnected) m_BaudRate = nBaud; }
	void setMode(SERIAL_MODE mode)				{ if (!m_bConnected) m_SerialMode = mode; }

	bool connect();
	void disconnect();

protected:
	void procesing();

private:
	const uint32_t DEFAULT_BAUD_RATE = 9200;
	const SERIAL_MODE DEFAULT_SERIAL_MODE = SERIAL_MODE::MODE_8N1;

	std::string m_sPortName;
	uint32_t m_BaudRate;
	SERIAL_MODE m_SerialMode;
};