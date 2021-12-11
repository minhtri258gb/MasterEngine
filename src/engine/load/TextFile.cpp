#define __TEXT_FILE_CPP__

#include "common.h"
#include "TextFile.h"

using namespace mt::engine;

TextFile::TextFile(std::string filename)
{
	this->file.open(filename);
	if (!this->file)
		throw Exception("Unable to open file " + filename, __FILE__, __LINE__);
}

TextFile::~TextFile()
{
	this->file.close();
}

long TextFile::getLength()
{
	this->file.seekg(0, std::ios::end);
	long length = this->file.tellg();
	this->file.seekg(0, std::ios::beg);
	return length;
}

void TextFile::read(char* buffer, long length)
{
	this->file.read(buffer, length);
}
