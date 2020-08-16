#include "CommInterface.h"

#include <exception>
#include <iostream>

CommInterface::CommInterface(const std::string& sDeviceName, boost::asio::io_context& ioContext) :
	m_SerialPort(ioContext),
	m_sDeviceName(sDeviceName),
	m_pCurrentMessageHeader(nullptr),
	m_pCurrentMessage(nullptr)
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

		/*Setup the asynchronous reads*/
		boost::asio::async_read(m_SerialPort, boost::asio::buffer(m_SerialBuf), boost::asio::transfer_at_least(1), 
			[this](const boost::system::error_code& /*errorCode*/, size_t nBytesCount)
		{
#ifdef _DEBUG
			std::cout << "Got " << nBytesCount << " bytes" << std::endl;
#endif
			//Reset the message processing state if there's a large gap between messages
			if (std::chrono::system_clock::now() - m_LastCommTime > std::chrono::milliseconds(5))
				m_pCurrentMessageHeader = nullptr;

			//Process received data
			stateProcess(nBytesCount);
			//Update the last communication time
			m_LastCommTime = std::chrono::system_clock::now();
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

void CommInterface::stateProcess(size_t nBytes)
{
	for(size_t i = 0; i < nBytes; i++)
	{
		//Start of a new message
		if (!m_pCurrentMessageHeader && m_SerialBuf[i] == SERIAL_STX)
		{
#ifdef _DEBUG
			std::cout << "New message" << std::endl;
#endif
			memset(&m_CurrentMessageHeader, 0, sizeof(MessageHeader));
			m_pCurrentMessageHeader = (uint8_t*)&m_CurrentMessageHeader;
			m_pCurrentMessage = nullptr; //Reset the message data pointer
		}
		//All of the data for the header has been collected
		else if (m_pCurrentMessageHeader && ((uint8_t*)&m_CurrentMessageHeader + sizeof(MessageHeader)) == m_pCurrentMessageHeader)
		{
#ifdef _DEBUG
			if (!m_pCurrentMessage)
				std::cout << "Processing message cmd=" << m_CurrentMessageHeader.cmd << " length=" << m_CurrentMessageHeader.len << std::endl;
#endif
			if (m_aProcessingFn[m_CurrentMessageHeader.cmd])
				m_aProcessingFn[m_CurrentMessageHeader.cmd](m_SerialBuf[i]);

			//Use an ETX to call the processing function at least once (for 0 length messages)
			if (!m_pCurrentMessageHeader && m_SerialBuf[i] == SERIAL_ETX)
			{
				m_pCurrentMessageHeader = nullptr; //Reset the message header pointer
#ifdef _DEBUG
				std::cout << "Message end" << std::endl;
#endif
			}
			else
				m_CurrentMessageHeader.len--; //Decrement the data length
		}
		//Stuff data into the message header
		else
		{
			*m_pCurrentMessageHeader = m_SerialBuf[i];
			m_pCurrentMessageHeader++;
		}
	}
}