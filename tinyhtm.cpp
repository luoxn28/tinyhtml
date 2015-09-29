#include "tinyhtm.h"

const char *TIHTMVERSION = "intyhtml-0.1";

const char *TiHtmVersion()
{
	return TIHTMVERSION;
}
static FILE *TiHtmOpen(const char *filename, const char *mode)
{
	return fopen(filename, mode);
}

// the scope of class TiHtmBase

// the scope of class TiHtmDocument

bool TiHtmDocument::loadFile()
{
	return loadFile(value.c_str());
}

bool TiHtmDocument::loadFile(std::string filename)
{
	return loadFile(filename.c_str());
}

bool TiHtmDocument::loadFile(const char *filename)
{
	if (!filename && value.empty())
	{
		TiHtmCout << "filename is null && value is empty" << TiHtmEndl;
		exit(-1);
	}

	FILE *file = TiHtmOpen(filename, "rb");

	if (file)
	{
		bool result = loadFile(file);
		fclose(file);
		return result;
	}
	else
	{
		return false;
	}
}

bool TiHtmDocument::loadFile(FILE *file)
{
	if (!file)
		exit(-1);
	
	long length = 0;
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buf = new char[length + 1];

	if (fread(buf, length, 1, file) != 1)
	{
		exit (-1);
	}
	buf[length] = '\0';

	TiHtmCout << buf << TiHtmEndl;

	delete []buf;
}

