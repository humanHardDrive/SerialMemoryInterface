#include <iostream>

#include "MemFile.h"

int main(int argc, char** argv)
{
	MemFile file;

	std::cout << "Args: " << argc << std::endl;
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << std::endl;
	std::cout << std::endl;

	if (argc > 1)
	{
		try
		{
			file.load(argv[1]);
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	return 0;
}