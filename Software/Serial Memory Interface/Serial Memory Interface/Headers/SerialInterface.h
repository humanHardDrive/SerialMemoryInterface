#pragma once

#include <string>

#include "ControlInterface.h"
#include <boost/asio.hpp>

/*
SerialInterface Class
This is the interface class between the hardware and software layer. It's responsible
for handling all communication over over serial and calling the appropriate handler
*/
class SerialInterface : public ControlInterface
{
public:
	SerialInterface(boost::asio::io_context& ioContext);
	SerialInterface(boost::asio::io_context& ioContext, const std::string& sPortName);
	~SerialInterface();
	
protected:
	void procesing();

private:
	std::string m_sPortName;
};