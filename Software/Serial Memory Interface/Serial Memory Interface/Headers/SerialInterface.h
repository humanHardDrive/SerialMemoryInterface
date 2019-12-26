#pragma once

#include <string>

#include "ControlInterface.h"

/*
SerialInterface Class
This is the interface class between the hardware and software layer. It's responsible
for handling all communication over over serial and calling the approprate handler
*/
class SerialInterface : public ControlInterface
{
public:
	SerialInterface(const std::string& sPortName);
	~SerialInterface();
	
protected:
	void procesing();

private:
};