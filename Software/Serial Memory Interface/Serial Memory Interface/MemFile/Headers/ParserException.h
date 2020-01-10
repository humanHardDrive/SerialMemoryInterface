#pragma once

#include <exception>

struct NoParserException : public std::exception
{
	const char* what() const throw()
	{
		return "No parser defined for file type";
	}
};

struct FileNotFoundException : public std::exception
{
	const char* what() const throw()
	{
		return "File not found";
	}
};

struct InvalidHexStartTokenException : public std::exception
{
	const char* what() const throw()
	{
		return "Invalid start token";
	}
};

struct InvalidChecksumException : public std::exception
{
	const char* what() const throw()
	{
		return "Invalid checksum";
	}
};

struct EndRecordNotFoundException : public std::exception
{
	const char* what() const throw()
	{
		return "No end record found";
	}
};