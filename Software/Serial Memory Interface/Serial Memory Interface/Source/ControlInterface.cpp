#include "ControlInterface.h"

ControlInterface::ControlInterface(boost::asio::io_context & ioContext) :
	m_bConnected(false),
	m_IOContext(ioContext)
{
}
