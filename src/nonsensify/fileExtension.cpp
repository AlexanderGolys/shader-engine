
#include "fileExtension.hpp"

#include <cstring>

char encriptionChar(EncriptionType type, bool filenameNonsensify) {
	char base = static_cast<char>(type);
	if (filenameNonsensify)
		base |= 0b100;
	return base;
}

EncriptionType encriptionTypeFromChar(char c) {
	return static_cast<EncriptionType>(c & 0b11);
}

bool isFilenameNonsensified(char c) {
	return (c & 0b100) != 0;
}

CustomFileHeader::CustomFileHeader(uint64_t dataSize, const string &filename, const string &originalExtension, EncriptionType encriptionType, bool filenameNonsensify)
: magic(NONSENSIFY_MAGIC), version(NONSENSIFY_VERSION), dataSize(dataSize), encriptionType(encriptionChar(encriptionType, filenameNonsensify)) {
	memset(this->filename, 0, sizeof(this->filename));
	memset(this->originalExtension, 0, sizeof(this->originalExtension));
	strncpy(this->filename, filename.c_str(), sizeof(this->filename) - 1);
	strncpy(this->originalExtension, originalExtension.c_str(), sizeof(this->originalExtension) - 1);
}

CustomFileHeader::CustomFileHeader(): magic(0), version(0), dataSize(0), encriptionType(0) {
	memset(filename, 0, sizeof(filename));
	memset(originalExtension, 0, sizeof(originalExtension));
}

bool CustomFileHeader::verify() const { return magic == NONSENSIFY_MAGIC;}
