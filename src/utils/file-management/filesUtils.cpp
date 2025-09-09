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
		throw FileSystemError("Current directory not found", __FILE__, __LINE__);
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
	return path.has_filename() && path.filename().string() == filename;
}

bool Path::endsWithFile() const {
	return path.has_filename();
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
	if (path.has_filename())
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

// Path::operator string() const { return path; }
string FileDescriptor::getPath() const {
	return path.to_str();
}

string FileDescriptor::extension() const {
	return path.getExtension();
}

string FileDescriptor::getFilename() const {
	return path.getFilename();
}

void FileDescriptor::resize(size_t newSize) {
	path.resize(newSize);
	bytesize = newSize;
	if (address)
		mapFile();
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
	if (exists()) std::cout << "Overwriting code in file " << getFilename() << std::endl;
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

void CodeFileDescriptor::saveNewCode(const string &code) const {
	if (exists())
		throw SystemError("File " + getFilename() + " already exists in " + getDirectory().to_str() + ".", __FILE__, __LINE__);
	writeCode(code);
}

bool CodeFileDescriptor::recogniseDirectoryNamingStyle() {
	return getDirectory().to_str().find('/') != string::npos;
}



size_t FileDescriptor::getSize() {
	return path.size();
}

void* FileDescriptor::getAddress() {
	if (address == nullptr)
		mapFile();
	return address;
}
