#pragma once
#include "hasher.hpp"
#include "fileExtension.hpp"

#define NONSENSIFY_PUBLIC_KEY 213742069
#define MAX_BUFFER_SIZE_HASING (1024*1024)

struct NonsensifyConfig {
	bool tryToPreserveFileExtension;
	bool nonsensify_filename;
	EncriptionType encriptionType;

	NonsensifyConfig(bool tryToPreserveFileExtension, bool nonsensify_filename, EncriptionType encriptionType);
};


class NonsensifyAPI {
	Hasher hasher;

	CustomFileHeader createHeader(FileDescriptor &file, const NonsensifyConfig &config);
	CustomFileHeader readHeader(FileDescriptor &file);
	bool isValidFilenameToNonsensify(const string &filename);

public:
	NonsensifyAPI(shared_ptr<openglAPI::ComputeShaderProgram> computeShaderProgram) : hasher(MAX_BUFFER_SIZE_HASING, std::move(computeShaderProgram)) {}

	void nonensifyFile(FileDescriptor &file, const NonsensifyConfig &config);
	void denonsensifyFile(FileDescriptor &file);
};
