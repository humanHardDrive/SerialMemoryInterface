#include "CommInterface.h"

#include <exception>
#include <iostream>

CommInterface::CommInterface(const std::string& sDeviceName, boost::asio::io_context& ioContext) :
	m_SerialPort(ioContext),
	m_sDeviceName(sDeviceName),
	m_pCurrentMessageHeader(nullptr),
	m_pCurrentMessage(nullptr)
{
	m_ProcessFnMap[CmdType::READ] = [this](uint8_t c) { return processRead(c); };
	m_ProcessFnMap[CmdType::WRITE] = [this](uint8_t c) { return processWrite(c); };
	//This function needs to always exist;
	m_ProcessFnMap[CmdType::ALL_CMD] = [this](uint8_t c) { return invalidProcess(c); };
}

CommInterface::~CommInterface()
{
	if (m_pCurrentMessage)
		free(m_pCurrentMessage);
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
			[this](const boost::system::error_code& errorCode, size_t nBytesCount)
		{
			asyncReadCallback(errorCode, nBytesCount);
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

void CommInterface::sendMessage(uint8_t /*cmdType*/, size_t /*nBytes*/, void * /*pBuf*/)
{
}

//For messages like write that have a header and a separate stream of data, this function
//can be used to stitch all the buffers together to avoid more malloc
void CommInterface::sendMessage(uint8_t cmdType, size_t nCount, std::initializer_list<size_t> nBytes, std::initializer_list<void*> pBuf)
{
	uint8_t* pSendBuf;
	size_t nSendSize, nTotalSize;

	//Figure out the total size of all the parts
	nTotalSize = 0;
	for (size_t i = 0; i < nCount; i++)
		nTotalSize += nBytes.begin()[i];

	//Stitch the message together
	for (size_t i = 0; i < nCount; i++)
	{
		nSendSize = nBytes.begin()[i];

		if (i == 0) //First message prepend the header and STX
			nSendSize += 1 + sizeof(MessageHeader);
		else if (i == (nCount - 1)) //Last message append the ETX
			nSendSize += 1;

		//Allocate buffer to send
		pSendBuf = (uint8_t*)malloc(nSendSize);

		if (i == 0) //Copy in the STX, message header, and message
		{
			MessageHeader header;
			header.cmd = cmdType;
			header.len = (uint32_t)nTotalSize;

			pSendBuf[0] = SERIAL_STX;
			memcpy(&pSendBuf[1], &header, sizeof(MessageHeader));
			memcpy(&pSendBuf[1 + sizeof(MessageHeader)], pBuf.begin()[i], nBytes.begin()[i]);
		}
		else if (i == (nCount - 1)) //Copy in the message and ETX
		{
			memcpy(pSendBuf, pBuf.begin()[i], nBytes.begin()[i]);
			pSendBuf[nBytes.begin()[i]] = SERIAL_ETX;
		}
		else //Just copy the message
			memcpy(pSendBuf, pBuf.begin()[i], nBytes.begin()[i]);

		boost::asio::write(m_SerialPort, boost::asio::buffer(pSendBuf, nTotalSize + 2 + sizeof(MessageHeader)));
		free(pSendBuf);
	}
}

void CommInterface::asyncReadCallback(const boost::system::error_code & /*errorCode*/, size_t nBytesCount)
{
#ifdef _DEBUG
	std::cout << "Got " << nBytesCount << " bytes" << std::endl;
#endif
	//Reset the message processing state if there's a large gap between messages
	if (m_pCurrentMessageHeader &&
		std::chrono::system_clock::now() - m_LastCommTime > std::chrono::milliseconds(5))
	{
		m_pCurrentMessageHeader = nullptr;
#ifdef _DEBUG
		std::cout << "Message timeout. Resetting" << std::endl;
#endif
	}

	//Process received data
	stateProcess(nBytesCount);
	//Update the last communication time
	m_LastCommTime = std::chrono::system_clock::now();

	//Treat a 0 byte read as the port closing
	if (nBytesCount)
	{
		//Start another async read
		boost::asio::async_read(m_SerialPort, boost::asio::buffer(m_SerialBuf), boost::asio::transfer_at_least(1),
			[this](const boost::system::error_code& errorCode, size_t nBytesCount)
		{
			asyncReadCallback(errorCode, nBytesCount);
		});
	}
}

void CommInterface::asyncWriteCallback(const boost::system::error_code & /*errorCode*/, size_t nBytesCount)
{
#ifdef _DEBUG
	std::cout << "Wrote " << nBytesCount << " bytes" << std::endl;
#endif
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

			if (m_pCurrentMessage)
			{
				free(m_pCurrentMessage);
				m_pCurrentMessage = nullptr; //Reset the message data pointer
			}

			//Reset processing function flags
			m_CurrentProcessFn = std::function<bool(uint8_t)>();
			m_bDoneProcessing = false;
		}
		//All of the data for the header has been collected
		else if (m_pCurrentMessageHeader && ((uint8_t*)&m_CurrentMessageHeader + sizeof(MessageHeader)) == m_pCurrentMessageHeader)
		{
#ifdef _DEBUG
			if (!m_pCurrentMessage)
				std::cout << "Processing message cmd=" << (int)m_CurrentMessageHeader.cmd << " length=" << m_CurrentMessageHeader.len << std::endl;
#endif
			//Grab the right processing function
			if (!m_CurrentProcessFn)
			{
				if (m_ProcessFnMap.find(static_cast<CmdType>(m_CurrentMessageHeader.cmd)) != m_ProcessFnMap.end())
					m_CurrentProcessFn = m_ProcessFnMap[static_cast<CmdType>(m_CurrentMessageHeader.cmd)];
				else
					m_CurrentProcessFn = m_ProcessFnMap[CmdType::ALL_CMD];
			}

			//Hand data over to the processing function
			if(!m_bDoneProcessing)
				m_bDoneProcessing = m_CurrentProcessFn(m_SerialBuf[i]);

			//Use an ETX to call the processing function at least once (for 0 length messages)
			if (!m_CurrentMessageHeader.len && m_SerialBuf[i] == SERIAL_ETX)
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
		else if(m_pCurrentMessageHeader)
		{
			*m_pCurrentMessageHeader = m_SerialBuf[i];
			m_pCurrentMessageHeader++;
		}
	}
}

bool CommInterface::processRead(uint8_t c)
{
	if (!m_pCurrentMessage)
	{
		//Allocate space for the message information
		m_pCurrentMessage = (uint8_t*)malloc(sizeof(ReadWriteMsg));
		//Set the write pointer at the start of the message
		m_pCurrentMessageIndex = m_pCurrentMessage;
	}
	
	if(m_pCurrentMessage)
	{
		//Stuff data into the message
		if (m_pCurrentMessageIndex < (m_pCurrentMessage + sizeof(ReadWriteMsg)))
		{
			*m_pCurrentMessageIndex = c;
			m_pCurrentMessageIndex++;
		}
		else
		//Send data back to device
		{
			ReadWriteMsg* pMsg = (ReadWriteMsg*)m_pCurrentMessage;

			sendMessage(static_cast<uint8_t>(CmdType::WRITE), 2, {sizeof(ReadWriteMsg), pMsg->len}, {pMsg, m_SerialBuf});
			//Clean up current message
			free(pMsg);

			//Return that processing has completed
			return true;
		}
	}

	return false;
}

bool CommInterface::processWrite(uint8_t c)
{
	if (!m_pCurrentMessage)
	{
		//Allocate space for the message information
		m_pCurrentMessage = (uint8_t*)malloc(sizeof(ReadWriteMsg));
		//Set the write pointer at the start of the message
		m_pCurrentMessageIndex = m_pCurrentMessage;
	}

	if (m_pCurrentMessage)
	{
		//Stuff data into message
		if ((m_pCurrentMessage + sizeof(ReadWriteMsg)) < m_pCurrentMessageIndex)
		{
			*m_pCurrentMessageIndex = c;
			m_pCurrentMessageIndex++;
		}
		//All of the data has been written back into memory file
		else if(!m_CurrentMessageHeader.len)
		{
			return true;
		}
		//Write data back to memfile
	}

	return false;
}

bool CommInterface::invalidProcess(uint8_t)
{
	//Does nothing
	return true;
}
