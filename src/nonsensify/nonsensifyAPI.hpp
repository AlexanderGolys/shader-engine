#pragma once
#include "hasher.hpp"
#include "fileExtension.hpp"

#define NONSENSIFY_PUBLIC_KEY 213742069

struct NonsensifyConfig {
	bool tryToPreserveFileExtension;
	bool nonsensify_filename;

	NonsensifyConfig(bool tryToPreserveFileExtension, bool nonsensify_filename) : tryToPreserveFileExtension(tryToPreserveFileExtension), nonsensify_filename(nonsensify_filename) {}
};
