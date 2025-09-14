#pragma once
#include "filesUtils.hpp"




enum CppExtension {
	CPP,
	HPP,
	H,
	TPP,
	C
};

CppExtension extensionFromString(const string &extStr);
bool isHeaderExtension(CppExtension ext);
bool isCompatibleWithC(CppExtension ext2);
bool isCppExtension(const string &extStr);




class CppCodeFile : public CodeFileDescriptor {
	CppExtension ext;
	bool isHeader;
	bool isCCompatible;

public:
	CppCodeFile(const Path &path);
	string getExtension() const { return extension(); }
	CppExtension getCppExtension() const { return ext; }
	bool header() const { return isHeader; }
	bool cCompatible() const { return isCCompatible; }
	string filename() const { return getFilename(); }
};

// struct CppNamespace {
// 	std::stack<string> nestedNamespaces;
//
// 	CppNamespace() = default;
// 	CppNamespace(const string &name);
// 	void addNamespace(const string &name);
// 	void removeNamespace();
// 	bool isEmpty() const;
// 	string fullName();
// };

struct CppMacro {
	string identifier;
	vector<string> parameters;
	bool dots;
	string replacement;

	CppMacro(const string &id, const vector<string> &params, const string &rep, bool dots = false);
};



class CppProject {
	DirectoryDescriptor root;

public:
	CppProject(const Path &rootPath);
	vector<CppCodeFile> listAllCppFiles(const vector<DirectoryDescriptor> &excludeDirs, const DirectoryDescriptor &subdir) const;
	vector<CppCodeFile> listAllCppFiles(const vector<DirectoryDescriptor> &excludeDirs) const;
};
