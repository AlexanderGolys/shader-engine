
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

CustomFileHeader::CustomFileHeader(uint64_t dataSize, const string &filename, const string &originalExtension, EncriptionType encriptionType, bool filenameNonsensify): magic(NONSENSIFY_MAGIC), version(NONSENSIFY_VERSION), dataSize(dataSize), encriptionType(encriptionChar(encriptionType, filenameNonsensify)) {
	memset(this->filename, 0, sizeof(this->filename));
	memset(this->originalExtension, 0, sizeof(this->originalExtension));
	strncpy(this->filename, filename.c_str(), sizeof(this->filename) - 1);
	strncpy(this->originalExtension, originalExtension.c_str(), sizeof(this->originalExtension) - 1);
}

void CustomFileHeader::serialize(char *buffer) const {
	memcpy(buffer, this, sizeof(CustomFileHeader));
}

void CustomFileHeader::deserialize(const char *buffer) {
	memcpy(this, buffer, sizeof(CustomFileHeader));
}
