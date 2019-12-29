#include "SerialInterface.h"

SerialInterface::SerialInterface(boost::asio::io_context& ioContext) :
	ControlInterface(ioContext)
{

}

SerialInterface::SerialInterface(boost::asio::io_context& ioContext, const std::string& sPortName) :
	m_sPortName(sPortName),
	ControlInterface(ioContext)
{
}

SerialInterface::~SerialInterface()
{
}
