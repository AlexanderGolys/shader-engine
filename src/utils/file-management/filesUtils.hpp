#pragma once

#include "../exceptions.hpp"





class Path {

public:
	filesystem::path path;

	Path();
	explicit Path(const string &path);
	explicit Path(const char* path);
	explicit Path(const filesystem::path &path);

	Path(const Path &other);
	Path(Path &&other) noexcept;
	Path &operator=(const Path &other);
	Path &operator=(Path &&other) noexcept;

	void makePrefferedStyle();

	Path operator+(const string &other) const;
	Path operator+(const Path &other) const;


	bool endsWithFile(const string &filename) const;
	bool endsWithFile() const;
	bool isAbsolute() const {
		return path.is_absolute();
	}
	bool isPrimitive() const {
		return is_empty(path.parent_path());
	}

	Path pureDirectory() const;
	string filename() const;
	string fileExtension() const;

	Path makeRelative(const Path &other) const;
	Path makeRelative(const string &other) const;
	Path makeAbsolute() const;
	Path goUp() const;
	string to_str() const;
};

class FilePath {
	Path path;
public:
	FilePath(const string &path);
	FilePath(const Path &path);
	FilePath(const Path &directory, const string &filename);
	FilePath(const string &directory, const string &filename);
	Path getPath() const;
	string getFilename() const;
	string getExtension() const;
	bool exists() const;
	void copyTo(const Path &destination) const;
	void moveTo(const Path &destination) const;
	void rename(const string &newName) const;
	void remove() const;
	void resize(size_t newSize) const;
	size_t size() const;
	string to_str() const;

};


class CodeFileDescriptor {
	FilePath path;

public:
	virtual ~CodeFileDescriptor() = default;
	CodeFileDescriptor(const string &filename, const string &directory, bool rootRelative = true);
	CodeFileDescriptor(const string &filename, const Path &directory, bool rootRelative = true);
	explicit CodeFileDescriptor(const Path &path, bool rootRelative = true);
	explicit CodeFileDescriptor(const string &path, bool rootRelative = true);
	Path getPath() const;
	string getFilename() const;
	Path getDirectory() const;
	string extension() const;
	virtual string getCode();
	string readCode() const;
	bool exists() const;
	void writeCode(const string &code) const;
	void modifyCode(const string &code) const;
	void saveNewCode(const string &code) const;
	bool recogniseDirectoryNamingStyle();
	void changeDirectoryNamingStyle(bool unix = true);
	void changeLine(const string &line, int lineNumber);
	string readLine(int lineNumber) const;
};


class FileDescriptor {
	void *address = nullptr;
	void *fileHandle = nullptr;
	void *mappingHandle = nullptr;
	size_t bytesize;
	FilePath path;


public:
	FileDescriptor(const FilePath &filePath);
	FileDescriptor(const string &path) : FileDescriptor(FilePath(path)) {}
	FileDescriptor(const char* path) : FileDescriptor(string(path)) {}
	~FileDescriptor();

	void mapFile();
	void closeFile();
	size_t getSize();
	void* getAddress();
	void flush() const;
	string getPath() const;
	string extension() const;
	string getFilename() const;
	void resize(size_t newSize);
};
