#define WIN32_LEAN_AND_MEAN

#include "SerialInterface.h"

SerialInterface::SerialInterface(boost::asio::io_context& ioContext) :
	ControlInterface(ioContext)
{

}

SerialInterface::SerialInterface(boost::asio::io_context & ioContext, const std::string & sPortName, uint32_t nBaudRate, SERIAL_MODE mode) :
	m_sPortName(sPortName),
	m_BaudRate(nBaudRate),
	m_SerialMode(mode),
	ControlInterface(ioContext)
{
}


SerialInterface::~SerialInterface()
{
}

bool SerialInterface::connect()
{
	return false;
}

void SerialInterface::disconnect()
{
}
