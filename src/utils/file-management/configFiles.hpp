#pragma once
#include "filesUtils.hpp"
#include "metaUtils.hpp"

#define DEFAULT_CONFIG_PATH "config/config.txt"


class JSONParser : public CodeFileDescriptor {
	using CodeFileDescriptor::CodeFileDescriptor;

	string getStringValue(const string &key) const {
		string code = readCode();
		code = removeWhitespace(code);
		size_t keyPos = code.find("\"" + key + "\":\"");
		if (keyPos == string::npos)
			throw ValueError("Key " + key + " not found in JSON file", __FILE__, __LINE__);
		size_t colonPos = code.find(':', keyPos);
		if (colonPos == string::npos)
			throw ValueError("Colon not found after key " + key + " in JSON file", __FILE__, __LINE__);
		size_t valueStart = code.find_first_not_of(" \t\n\r", colonPos + 1);
		if (valueStart == string::npos || code[valueStart] != '"')
			throw ValueError("String value not found for key " + key + " in JSON file", __FILE__, __LINE__);
		size_t valueEnd = code.find('"', valueStart + 1);
		if (valueEnd == string::npos)
			throw ValueError("Closing quote not found for string value of key " + key + " in JSON file", __FILE__, __LINE__);
		return code.substr(valueStart + 1, valueEnd - valueStart - 1);
	}
};

class ConfigFile{
	std::unordered_map<string, string> config;
public:
	ConfigFile(const string &configPath = DEFAULT_CONFIG_PATH);
	string operator[](const string &key) const;
	string check(const string &key) const;

	Path getRoot() const;
	Path getMainShaderDirectory() const;
	Path getSDFMacroShader() const;
	Path getMathToolsShaderMacro() const;
	Path getLightToolsShaderMacro() const;
	Path getStructsShaderMacro() const;
	Path getShadersDir() const;
	Path getScreenshotsDir() const;
};
