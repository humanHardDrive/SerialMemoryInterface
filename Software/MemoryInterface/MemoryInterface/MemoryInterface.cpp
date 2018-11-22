#include <iostream>

#include "Msgs.h"
#include <chrono>
#include <Windows.h>
#include <atlstr.h>

bool SetCommProperties(HANDLE port, unsigned int baudrate, unsigned char bytesize, bool parity, unsigned char stopbits)
{
	DCB dcb;

	if (!GetCommState(port, &dcb))
		return false;

	dcb.BaudRate = baudrate;
	dcb.ByteSize = bytesize;
	dcb.Parity = (parity ? ODDPARITY : NOPARITY);
	dcb.StopBits = stopbits;

	if (!SetCommState(port, &dcb))
		return false;

	return true;
}

bool SetCommTimeout(HANDLE port, unsigned int t)
{
	COMMTIMEOUTS timeout;

	if (!GetCommTimeouts(port, &timeout))
		return false;

	timeout.ReadIntervalTimeout = 0;
	timeout.ReadTotalTimeoutMultiplier = 0;
	timeout.ReadTotalTimeoutConstant = t;
	timeout.WriteTotalTimeoutMultiplier = 0;
	timeout.WriteTotalTimeoutConstant = t;

	if (!SetCommTimeouts(port, &timeout))
		return false;

	return true;
}

int ReadCommPort(HANDLE port, unsigned char* buffer, unsigned int len)
{
	DWORD retval = 0;

	ReadFile(port, buffer, len, &retval, 0);

	return (int)retval;
}

int WriteCommPort(HANDLE port, unsigned char* buffer, unsigned int len)
{
	DWORD retval = -1;

	if (len > 0)
		WriteFile(port, buffer, len, &retval, NULL);

	return (int)retval;
}

HANDLE OpenComPort(CString portname)
{
	DCB dcb;
	HANDLE port = CreateFile(
		portname,
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (!GetCommState(port, &dcb))
		return INVALID_HANDLE_VALUE;
	else
		return port;
}

CString ScanCommPorts(unsigned int min, unsigned int max)
{
	unsigned char versionrqt[] = {PACKET_STX, VERSION_REQUEST, 0, 0, PACKET_ETX};
	unsigned char versionrsp[8];

	for (unsigned int i = min; i <= max; i++)
	{
		CString portname;
		portname.Format(_T("COM%d"), i);

		HANDLE comport = OpenComPort(portname);
		if (comport != INVALID_HANDLE_VALUE)
		{
			if (SetCommProperties(comport, 115200, 8, false, 1) && SetCommTimeout(comport, 500))
			{
				WriteCommPort(comport, versionrqt, 5);
				if (ReadCommPort(comport, versionrsp, 7))
				{
					if (versionrsp[0] == PACKET_STX && versionrsp[1] == VERSION_REQUEST &&
						versionrsp[2] == 2 && versionrsp[3] == 0 && versionrsp[6] == PACKET_ETX)
					{

						CloseHandle(comport);
						return portname;
					}
				}
			}
			CloseHandle(comport);
		}
	}

	return CString("");
}

int main(int argc, char** argv)
{
    std::cout << "Hello World!\n"; 

	ScanCommPorts(1, 25);

	return 0;
}
