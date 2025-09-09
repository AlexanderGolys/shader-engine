#pragma once
#include "macros.hpp"


inline string plural (string word) {
	if (word.size() == 0)
		return word;
	bool caps = word.back() >= 'A' && word.back() <= 'Z' && word.size() > 1;
	bool es = word.back() == 's'||
			  word.back() == 'x' ||
			  word.back() == 'z' ||
			  (word.size() > 1 && word[word.size()-2] == 'c' && word.back() == 'h') ||
			  (word.size() > 1 && word[word.size()-2] == 's' && word.back() == 'h') ||
			  word.back() == 'S'||
			  word.back() == 'X' ||
			  word.back() == 'Z' ||
			  (word.size() > 1 && word[word.size()-2] == 'C' && word.back() == 'H') ||
			  (word.size() > 1 && word[word.size()-2] == 'S' && word.back() == 'H');
	if (es && !caps)
		return word + "es";
	if (es)
		return word + "ES";
	if (caps)
		return word + "S";
	return word + "s";
}


class ErrorClassWrapper : public std::exception {
    string msg_;
    string file_;
    int line_;
    string full_msg_;
public:
    explicit ErrorClassWrapper(const string& msg, const char* file, int line)
        : msg_(msg), file_(file), line_(line) {
        full_msg_ = msg_ + " [at " + file_ + ":" + std::to_string(line_) + "]";
    }
    const char* what() const noexcept override {
        return full_msg_.c_str();
    }
    string message() const { return msg_; }
    string file() const { return file_; }
    int line() const { return line_; }
};

class NotImplementedError : public ErrorClassWrapper {
public:
    NotImplementedError(const string& notImplementedMethodName, const string& lackingType, const char* file, int line)
        : ErrorClassWrapper(lackingType + " " + notImplementedMethodName + " is not implemented yet.", file, line) {}
    NotImplementedError(const string& notImplementedMethodName, const char* file, int line)
    : ErrorClassWrapper(notImplementedMethodName + " is not implemented yet.", file, line) {}
};

class RecursionLimitExceeded : public ErrorClassWrapper {
public:
    RecursionLimitExceeded(int limit, const string &where, const char* file, int line)
        : ErrorClassWrapper("Recursion limit (" + std::to_string(limit) + "levels) exceeded during " + where, file, line) {}
};

class IndexOutOfBounds : public ErrorClassWrapper {
public:
    IndexOutOfBounds(int index, int size, const string &structure_name, const char* file, int line)
        : ErrorClassWrapper("Index " + to_string(index) + " out of bounds ("+ structure_name + " of size: "+to_string(size)+")", file, line) {}
    IndexOutOfBounds(int index, const string &size, const string &structure_name, const char* file, int line)
        : ErrorClassWrapper("Index " + to_string(index) + " out of bounds ("+ structure_name + " of size: "+size+")", file, line) {}
    IndexOutOfBounds(const string &index, const string &size, const string &structure_name, const char* file, int line)
    : ErrorClassWrapper("Index " + index + " out of bounds ("+ structure_name + " of size: "+size+")", file, line) {}
};

class NotImplementedMethodError : public NotImplementedError {
public:
    NotImplementedMethodError(const string& methodName, const char* file, int line)
        : NotImplementedError(methodName, "Method", file, line) {}
};

class NotImplementedFunctionError : public NotImplementedError {
public:
    NotImplementedFunctionError(const string& name, const char* file, int line)
        : NotImplementedError(name, "Function", file, line) {}
};

class NotImplementedVariantError : public NotImplementedError {
public:
    NotImplementedVariantError(const string& variant, const string& ofWhat, const char* file, int line)
        : NotImplementedError(variant + " of " + ofWhat, "Variant", file, line) {}
};

class UnknownVariantError : public ErrorClassWrapper {
public:
    UnknownVariantError(const string& variant, const string& ofWhat, const char* file, int line)
        : ErrorClassWrapper("Variant " + variant + " of  " + ofWhat + " is an unknown type in this context.", file, line) {}
    UnknownVariantError(const string& msg, const char* file, int line)
        : ErrorClassWrapper(msg, file, line) {}
};

class IllegalVariantError : public ErrorClassWrapper {
public:
    IllegalVariantError(const string& variant, const string& ofWhat, const string& rejectingMethod, const char* file, int line)
        : ErrorClassWrapper(plural(ofWhat) + " in variant " + variant + " are considered invalid by method " + rejectingMethod + ".", file, line) {}
    IllegalVariantError(const string& msg, const char* file, int line)
        : ErrorClassWrapper(msg, file, line) {}
};

class IllegalArgumentError : public ErrorClassWrapper {
public:
    IllegalArgumentError(const string& msg, const char* file, int line)
        : ErrorClassWrapper(msg, file, line) {}
};

class ValueError : public ErrorClassWrapper {
public:
    ValueError(const string& msg, const char* file, int line)
        : ErrorClassWrapper(msg, file, line) {}
};

class SystemError : public ErrorClassWrapper {
public:
    SystemError(const string& msg, const char* file, int line)
        : ErrorClassWrapper(msg, file, line) {}
};

class FileSystemError : public SystemError {
public:
    using SystemError::SystemError;
};

class FileNotFoundError : public FileSystemError {
public:
    FileNotFoundError(const string& filename, const char* file, int line)
        : FileSystemError("File " + filename + " not found.", file, line) {}
    FileNotFoundError(const string& filename, const string& dir, const char* file, int line)
        : FileSystemError("File " + filename + " not found in " + dir, file, line) {}
};

class InvalidFileError : public SystemError {
public:
    InvalidFileError(const string& filename, const string& reason, const char* file, int line)
        : SystemError("File " + filename + " is invalid (" + reason + ").", file, line) {}
};

class ZeroDivisionError : public ValueError {
public:
    ZeroDivisionError(const string& msg, const char* file, int line)
        : ValueError(msg, file, line) {}
};
