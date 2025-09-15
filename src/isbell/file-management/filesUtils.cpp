#include "filesUtils.hpp"

#include <fstream>
#include <io.h>
#include <iostream>
#include <sstream>

#include "configFiles.hpp"
#include "exceptions.hpp"
#include "metaUtils.hpp"
#include "randomUtils.hpp"

using namespace glm;



CodeFileDescriptor::CodeFileDescriptor(const string &filename, const Path &directory, bool rootRelative)
: path(directory, filename) {
	if (rootRelative)
		path = FilePath(path.getPath().makeAbsolute());
}

CodeFileDescriptor::CodeFileDescriptor(const Path &path, bool rootRelative)
: CodeFileDescriptor(path.filename(), path.pureDirectory(), rootRelative) {}

CodeFileDescriptor::CodeFileDescriptor(const string &filename, const string &directory, bool rootRelative)
: CodeFileDescriptor(filename, Path(directory), rootRelative) {}

CodeFileDescriptor::CodeFileDescriptor(const string &path, bool rootRelative) : CodeFileDescriptor(Path(path), rootRelative) {}


Path CodeFileDescriptor::getPath() const {
	return path.getPath();
}
string CodeFileDescriptor::getFilename() const {
	return path.getFilename();
}
Path CodeFileDescriptor::getDirectory() const {
	return path.getPath().pureDirectory();
}



Path::Path() {
	path = filesystem::current_path();
	if (path==filesystem::path())
		THROW(FileSystemError, "Current directory not found");
	makePrefferedStyle();
}

Path::Path(const filesystem::path &path) : path(path) {
	if (is_empty(path))
		throw ValueError("Empty paths are not supported", __FILE__, __LINE__);
	makePrefferedStyle();
}


Path::Path(const string &path) : Path(filesystem::path(path)) {}

Path::Path(const Path &other) : path(other.path) {}

Path::Path(const char* path) : Path(string(path)) {}


Path::Path(Path &&other) noexcept : path(std::move(other.path)) {}

Path &Path::operator=(const Path &other) {
	if (this == &other) return *this;
	path = other.path;
	return *this;
}

Path &Path::operator=(Path &&other) noexcept {
	if (this == &other) return *this;
	path = std::move(other.path);
	return *this;
}

void Path::makePrefferedStyle() {
	path = path.make_preferred();
}


Path Path::operator+(const string &other) const {
	return Path(path / other);
}
Path Path::operator+(const Path &other) const {
	return Path(path / other.path);
}



bool Path::endsWithFile(const string &filename) const {
	return path.has_extension() && path.filename().string() == filename;
}

bool Path::endsWithFile() const {
	return path.has_extension();
}

bool Path::isAbsolute() const {
	return path.is_absolute();
}




bool Path::isPrimitive() const {
	return is_empty(path.parent_path());
}

Path Path::pureDirectory() const {
	if (path.has_filename())
		return Path(path.parent_path());
	return *this;
}




string Path::filename() const {
	if (path.has_filename())
		return path.filename().string();
	throw ValueError("Path does not end with a file", __FILE__, __LINE__);
}




string Path::fileExtension() const {
	if (path.has_extension())
		return path.extension().string();
	throw ValueError("Path does not end with a file", __FILE__, __LINE__);
}

Path Path::makeRelative(const Path &other) const {
	return Path(filesystem::relative(path, other.path));
}

Path Path::makeAbsolute() const {
	return Path(filesystem::absolute(path));
}
Path Path::makeRelative(const string &other) const {
	return makeRelative(Path(other));
}


Path Path::goUp() const {
	if (isPrimitive())
		throw ValueError("Parent path is empty, as the path is already primitive (" + to_str() + ")", __FILE__, __LINE__);
	return Path(path.parent_path());
}

string Path::to_str() const {
	return path.string();
}


FilePath::FilePath(const string &path): path(path) {
	if (!this->path.endsWithFile())
		throw ValueError("Path does not end with filename: " + path, __FILE__, __LINE__);
}

FilePath::FilePath(const Path &path): FilePath(path.to_str()) {}

FilePath::FilePath(const Path &directory, const string &filename): FilePath(directory + filename) {}

FilePath::FilePath(const string &directory, const string &filename): FilePath(Path(directory) + filename) {}

FilePath::FilePath(const filesystem::path &path): FilePath(path.string()) {}

Path FilePath::getPath() const { return path; }

string FilePath::getFilename() const { return path.filename(); }

string FilePath::getExtension() const { return path.fileExtension(); }

bool FilePath::exists() const { return filesystem::exists(path.path); }

void FilePath::copyTo(const Path &destination) const {
	filesystem::copy(path.path, destination.path, filesystem::copy_options::overwrite_existing);
}

void FilePath::moveTo(const Path &destination) const {
	filesystem::rename(path.path, destination.path);
}

void FilePath::rename(const string &newName) const {
	filesystem::rename(path.path, path.pureDirectory().path / newName);
}

void FilePath::remove() const {
	filesystem::remove(path.path);
}

void FilePath::resize(size_t newSize) const {
	filesystem::resize_file(path.path, newSize);
}

size_t FilePath::size() const {
	return filesystem::file_size(path.path);
}

string FilePath::to_str() const {
	return path.to_str();
}

bool FilePath::isPrimitive() const { return path.isPrimitive(); }

// Path::operator string() const { return path; }
string FileDescriptor::getPath() const {
	return path.to_str();
}

string FileDescriptor::getExtension() const {
	return path.getExtension();
}

string FileDescriptor::getFilename() const {
	return path.getFilename();
}

void FileDescriptor::resize(size_t newSize) {
	bool wasMapped = address != nullptr;
	if (wasMapped)
		closeFile();
	path.resize(newSize);
	bytesize = newSize;
	if (wasMapped)
		mapFile();
}

void FileDescriptor::addPaddingAtStart(size_t padding) {
	bool wasMapped = address != nullptr;
	size_t old_size = getSize();
	size_t new_size = old_size + padding;
	resize(new_size);
	if (!wasMapped)
		mapFile();
	memmove((char*)address + padding, address, old_size);
	memset(address, 0, padding);
	flush();
	if (!wasMapped)
		closeFile();
}

void FileDescriptor::writeData(const void *data, size_t size, size_t offset) {
	bool wasMapped = address != nullptr;
	if (!wasMapped)
		mapFile();
	if (offset + size > bytesize)
		throw ValueError("Writing data out of bounds: " + std::to_string(offset + size) + " > " + std::to_string(bytesize), __FILE__, __LINE__);
	memcpy((char*)address + offset, data, size);
	flush();
	if (!wasMapped)
		closeFile();
}

void FileDescriptor::rename(const string &newName) {
	bool wasMapped = address != nullptr;
	if (wasMapped)
		closeFile();
	path.rename(newName);
	if (wasMapped)
		mapFile();
}

void FileDescriptor::removeDataFromStart(size_t deletedChunkSize) {
	bool wasMapped = address != nullptr;
	size_t old_size = getSize();
	size_t new_size = old_size - deletedChunkSize;
	if (!wasMapped)
		mapFile();
	memmove(address, (char*)address + deletedChunkSize, new_size);
	flush();
	if (!wasMapped)
		closeFile();
	resize(new_size);
}

void FileDescriptor::readData(void *destination, size_t size, size_t offset) {
	bool wasMapped = address != nullptr;
	if (!wasMapped)
		mapFile();
	if (offset + size > bytesize)
		throw ValueError("Reading data out of bounds: " + std::to_string(offset + size) + " > " + std::to_string(bytesize), __FILE__, __LINE__);
	memcpy(destination, (char*)address + offset, size);
	if (!wasMapped)
		closeFile();
}

bool isValidFilenameCharacter(char c) {
	const string valid_nonalphanumeric = "._- ";
	if (isalnum(c)) return true;
	for (char valid : valid_nonalphanumeric)
		if (c == valid) return true;
	return false;
}

DirectoryDescriptor::DirectoryDescriptor(const Path &p): path(p) {
	if (path.endsWithFile())
		THROW(FileSystemError, "Path " + path.to_str() + " points to a file, not a directory");
}

DirectoryDescriptor::DirectoryDescriptor(const string &p): DirectoryDescriptor(Path(p)) {}

DirectoryDescriptor::DirectoryDescriptor(const filesystem::path &p): DirectoryDescriptor(Path(p)) {}



Path DirectoryDescriptor::getPath() const { return path;}

bool DirectoryDescriptor::exists() const {
	return filesystem::exists(path.path) && filesystem::is_directory(path.path);
}

vector<FilePath> DirectoryDescriptor::listFiles() const {
	vector<FilePath> files;
	if (!exists())
		throw FileSystemError("Directory " + path.to_str() + " not found at this device.", __FILE__, __LINE__);

	for (const auto & entry : filesystem::directory_iterator(path.path))
		if (filesystem::is_regular_file(entry.status()) && !filesystem::is_empty(entry.path()))
			files.emplace_back(entry.path());
	return files;

}

vector<DirectoryDescriptor> DirectoryDescriptor::listDirectories() const {
	vector<DirectoryDescriptor> dirs;
	if (path.isPrimitive())
		return dirs;
	if (!exists())
		THROW(FileSystemError, "Directory " + path.to_str() + " not found at this device.");
	for (const auto & entry : filesystem::directory_iterator(path.path))
		if (filesystem::is_directory(entry.status()) && !filesystem::is_empty(entry.path()))
			dirs.emplace_back(DirectoryDescriptor(Path(entry.path())));
	return dirs;

}

vector<FilePath> DirectoryDescriptor::listAllFilesRecursively() const {
	vector<FilePath> files = listFiles();
	vector<DirectoryDescriptor> dirs = listDirectories();
	for (const DirectoryDescriptor &dir : dirs)
		addAll(files, dir.listAllFilesRecursively());
	return files;
}

size_t DirectoryDescriptor::directorySize() const {
	vector<FilePath> files = listAllFilesRecursively();
	return sum<size_t, vector<size_t>>(map<FilePath, size_t>(files, [](const FilePath &f) { return f.size(); }));
}


bool isValidFilename(const string &filename) {
	if (filename.empty()) return false;
	if (filename.size() > 255) return false;

	bool contains_dot = false;
	bool contains_sth_after_dot = false;

	for (char c : filename) {
		if (!isValidFilenameCharacter(c))
			return false;

		if (contains_dot)
			contains_sth_after_dot = true;

		if (c == '.')
			contains_dot = true;
	}

	return contains_sth_after_dot;
}




void CodeFileDescriptor::changeLine(const string &line, int lineNumber) {
	std::ifstream file(getPath().to_str());
	if (!exists())
		throw FileNotFoundError(getFilename(), __FILE__, __LINE__);
	std::string code;
	int i = 0;
	while (std::getline(file, code)) {
		if (i == lineNumber) {
			code = line;
			break;
		}
		i++;
	}
	file.close();
	writeCode(code);
}

string CodeFileDescriptor::readLine(int lineNumber) const {
	std::ifstream file(getPath().to_str());
	if (!exists())
		throw FileNotFoundError(getFilename(), __FILE__, __LINE__);
	std::string code;
	int i = 0;
	while (std::getline(file, code)) {
		if (i == lineNumber) break;
		i++;
	}
	file.close();
	return code;
}

FileDescriptor::FileDescriptor(const FilePath &filePath)
: bytesize(0), path(filePath) {
	if (!path.exists())
		throw FileNotFoundError(filePath.to_str(), __FILE__, __LINE__);
	bytesize = path.size();
}




string CodeFileDescriptor::extension() const {
	return path.getExtension();
}

string CodeFileDescriptor::getCode() {
	std::ifstream shaderStream(getPath().to_str(), std::ios::in);
	if (shaderStream.is_open()) {
		string code;
		std::stringstream sstr;
		sstr << shaderStream.rdbuf();
		code = sstr.str();
		shaderStream.close();
		return code;
	}
	throw FileNotFoundError(getPath().to_str(), __FILE__, __LINE__);
}

string CodeFileDescriptor::readCode() const {
	std::ifstream shaderStream(getPath().to_str(), std::ios::in);
	if (shaderStream.is_open()) {
		string code;
		std::stringstream sstr;
		sstr << shaderStream.rdbuf();
		code = sstr.str();
		shaderStream.close();
		return code;
	}
	throw FileNotFoundError(getPath().to_str(), __FILE__, __LINE__);
}

bool CodeFileDescriptor::exists() const {
	std::ifstream shaderStream(getPath().to_str(), std::ios::in);
	return shaderStream.is_open();
}

void CodeFileDescriptor::writeCode(const string &code) const {
	std::ofstream shaderStream(getPath().to_str(), std::ios::out);
	if (shaderStream.is_open()) {
		shaderStream << code;
		shaderStream.close();
		return;
	}
	throw FileNotFoundError(getFilename(), __FILE__, __LINE__);
}

void CodeFileDescriptor::modifyCode(const string &code) const {
	if (!exists())
		throw FileNotFoundError(getFilename(), __FILE__, __LINE__);
	writeCode(code);
}

void CodeFileDescriptor::saveCopyToNewFile(const string &code) const {
	if (exists())
		throw SystemError("File " + getFilename() + " already exists in " + getDirectory().to_str() + ".", __FILE__, __LINE__);
	writeCode(code);
}

bool CodeFileDescriptor::recogniseDirectoryNamingStyle() {
	return getDirectory().to_str().find('/') != string::npos;
}



size_t FileDescriptor::getSize() {
	return bytesize;
}

void* FileDescriptor::getAddress() {
	if (address == nullptr)
		mapFile();
	return address;
}
