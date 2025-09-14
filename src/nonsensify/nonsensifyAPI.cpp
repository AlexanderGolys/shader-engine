#include "nonsensifyAPI.hpp"

#include "randomUtils.hpp"

NonsensifyConfig::NonsensifyConfig(bool tryToPreserveFileExtension, bool nonsensify_filename, EncriptionType encriptionType)
: tryToPreserveFileExtension(tryToPreserveFileExtension), nonsensify_filename(nonsensify_filename), encriptionType(encriptionType) {}

CustomFileHeader NonsensifyAPI::createHeader(FileDescriptor &file, const NonsensifyConfig &config) {
	return CustomFileHeader(file.getSize(), file.getFilename(), file.getExtension(), config.encriptionType, config.nonsensify_filename);
}

CustomFileHeader NonsensifyAPI::readHeader(FileDescriptor &file) {
	file.mapFile();
	CustomFileHeader header;
	file.readData(&header, sizeof(CustomFileHeader), 0);

	if (!header.verify()) {
		file.closeFile();
		throw ValueError("File " + file.getFilename() + " is not a valid nonsensified file.", __FILE__, __LINE__);
	}
	return header;
}

bool NonsensifyAPI::isValidFilenameToNonsensify(const string &filename) {
	return isValidFilename(filename) && filename.size() <= 255 - 9;
}

void NonsensifyAPI::nonensifyFile(FileDescriptor &file, const NonsensifyConfig &config) {
	CustomFileHeader header = createHeader(file, config);
	string filename;
	if (config.nonsensify_filename)
		filename = randomStringLetters(16) + ".nonsense";
	else {
		if (!isValidFilenameToNonsensify(file.getFilename()))
			throw ValueError("Filename " + file.getFilename() + " is not valid to nonsensify.", __FILE__, __LINE__);
		filename = file.getFilename() + ".nonsense";
	}
	if (!isValidFilename(filename))
		throw ValueError("Generated filename " + filename + " is not valid.", __FILE__, __LINE__);

	hasher.hashFile(file);
	file.addPaddingAtStart(sizeof(CustomFileHeader));
	file.writeData(&header, sizeof(CustomFileHeader), 0);
	file.rename(filename);
	LOG("Nonsensified to file " + filename);

}

void NonsensifyAPI::denonsensifyFile(FileDescriptor &file) {
	file.mapFile();

	if (file.getSize() < sizeof(CustomFileHeader)) {
		file.closeFile();
		throw ValueError("File " + file.getFilename() + " is too small to be a valid nonsensified file.", __FILE__, __LINE__);
	}

	CustomFileHeader header = readHeader(file);
	file.removeDataFromStart(sizeof(CustomFileHeader));
	hasher.hashFile(file);
	file.rename(header.filename);
	LOG("Denonsensified to file " + string(header.filename));
}
