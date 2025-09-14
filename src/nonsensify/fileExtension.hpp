#pragma once
#include "../utils/file-management/filesUtils.hpp"

#define NONSENSIFY_MAGIC 0x2137420
#define NONSENSIFY_VERSION_MAJOR 0
#define NONSENSIFY_VERSION_MINOR 1
#define NONSENSIFY_VERSION ((NONSENSIFY_VERSION_MAJOR << 8) | NONSENSIFY_VERSION_MINOR)

enum EncriptionType {
	GLOBAL = 0,
	LOCAL = 1,
	PRIVATE = 2
};


char encriptionChar(EncriptionType type, bool filenameNonsensify);

EncriptionType encriptionTypeFromChar(char c);

bool isFilenameNonsensified(char c);




struct CustomFileHeader
{
	uint32_t magic;
	uint16_t version;
	uint64_t dataSize;
	char filename[255];
	char originalExtension[10];
	char encriptionType;

	CustomFileHeader(uint64_t dataSize, const string &filename, const string &originalExtension, EncriptionType encriptionType, bool filenameNonsensify);
	CustomFileHeader();
	~CustomFileHeader() = default;
	bool verify() const;
};
