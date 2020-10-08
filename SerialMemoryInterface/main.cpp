#include <iostream>
#include "boost/asio.hpp"
#include "boost/program_options.hpp"

#include "CommInterface.h"
#include "MemFile.h"

int main(int argc, char** argv)
{
	boost::program_options::options_description commandLineArguments("Arguments");
	commandLineArguments.add_options()
		("help", "")
		("file",		boost::program_options::value<std::string>(),	"Path to memory file")
		("memorySize",	boost::program_options::value<size_t>(),		"Size allowed for valid memory addresses")
		("port",		boost::program_options::value<std::string>(),	"Comm port")
		("baud",		boost::program_options::value<int>(),			"Baud rate")
		;

	boost::program_options::variables_map variableMap;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, commandLineArguments), variableMap);
	boost::program_options::notify(variableMap);

	if (variableMap.count("help"))
	{
		std::cout << commandLineArguments << std::endl;
		return 0;
	}

	std::string sFilePath;
	size_t nFileSize;
	std::string sPortName;
	int nBaudRate;

	if (variableMap.count("file"))
		sFilePath = variableMap["file"].as<std::string>();
	else
	{
		std::cout << "File path not specified" << std::endl;
		return 1;
	}

	if (variableMap.count("memorySize"))
		nFileSize = variableMap["memorySize"].as<size_t>();
	else
	{
		std::cout << "Memory size not specified" << std::endl;
		return 1;
	}

	if (variableMap.count("port"))
		sPortName = variableMap["port"].as<std::string>();
	else
	{
		std::cout << "COM port not specified" << std::endl;
		return 1;
	}

	if (variableMap.count("baud"))
		nBaudRate = variableMap["baud"].as<int>();
	else
	{
		std::cout << "Baud rate not specified" << std::endl;
		return 1;
	}

	MemFile file(nFileSize);
	boost::asio::io_context io;
	CommInterface serialInterface(sPortName, nBaudRate, io);

	if (argc > 1)
	{
		try
		{
			file.load(sFilePath);
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	serialInterface.run();
	io.run();

	return 0;
}