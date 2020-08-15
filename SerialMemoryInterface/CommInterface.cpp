#include "CommInterface.h"

#include <exception>

CommInterface::CommInterface(const std::string& sDeviceName, boost::asio::io_context& ioContext) :
	m_SerialPort(ioContext),
	m_sDeviceName(sDeviceName)
{
}

void CommInterface::run()
{
	m_SerialPort.open(m_sDeviceName);

	if (m_SerialPort.is_open())
	{
		/*TODO:
		Get these as parameters to the interface
		*/
		m_SerialPort.set_option(boost::asio::serial_port::baud_rate(115200));
		m_SerialPort.set_option(boost::asio::serial_port::character_size(8));
		m_SerialPort.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));

		/*Setup the ansynchronous reads*/
		boost::asio::async_read(m_SerialPort, boost::asio::buffer(m_SerialBuf), boost::asio::transfer_at_least(1), 
			[this](const boost::system::error_code& errorCode, size_t nBytesCount)
		{
			/*Unreferenced*/
			errorCode;
			nBytesCount;
		});
	}
	else
	{
		/*Couldn't open the port*/
		throw std::exception();
	}
}

void CommInterface::stop()
{
}
