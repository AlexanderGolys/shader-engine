#include "macroParsing.hpp"

#include "configFiles.hpp"
#include "metaUtils.hpp"
#include "randomUtils.hpp"



CodeMacro::CodeMacro(const string &key, const string &c)
: replacementKey(key), replacementCode(c) {}

CodeMacro::CodeMacro(const string &key, const CodeFileDescriptor &file)
: replacementKey(key) {
	CodeFileDescriptor f = file;
	replacementCode = f.getCode();
}

CodeMacro::CodeMacro(const CodeMacro &other)
: replacementKey(other.replacementKey), replacementCode(other.replacementCode) {}

CodeMacro::CodeMacro(CodeMacro &&other) noexcept
: replacementKey(std::move(other.replacementKey)), replacementCode(std::move(other.replacementCode)) {}

CodeMacro &CodeMacro::operator=(const CodeMacro &other) {
	if (this == &other) return *this;
	replacementKey = other.replacementKey;
	replacementCode = other.replacementCode;
	return *this;
}

CodeMacro &CodeMacro::operator=(CodeMacro &&other) noexcept {
	if (this == &other) return *this;
	replacementKey = std::move(other.replacementKey);
	replacementCode = std::move(other.replacementCode);
	return *this;
}

string CodeMacro::apply(const string &codeScheme) const {
	string result = codeScheme;
	size_t pos = 0;
	while ((pos = result.find(replacementKey, pos)) != std::string::npos) {
		result.replace(pos, replacementKey.length(), replacementCode);
		pos += replacementCode.length();
	}
	return result;
}

string CodeMacro::getKey() const {
	return replacementKey;
}

string JSONParser::operator[](const string &key) const {
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

ConfigFile::ConfigFile(const string &configPath) : jsonParser(configPath) {}

string ConfigFile::operator[](const string &key) const {
	return jsonParser[key];
}

string ConfigFile::at(const string &key) const {return jsonParser[key];}

Path ConfigFile::getRoot() const {
	return Path(at("root_rel_build"));
}

Path ConfigFile::getMainShaderDirectory() const {
	return Path(at("shadersDir"));
}

Path ConfigFile::getSDFMacroShader() const {
	return Path(at("templateShaders"));
}

Path ConfigFile::getMathToolsShaderMacro() const {
	return Path(at("mathToolsShaderMacro"));
}

Path ConfigFile::getLightToolsShaderMacro() const {
	return Path(at("lightToolsShaderMacro"));
}

Path ConfigFile::getStructsShaderMacro() const {
	return Path(at("structsShaderMacro"));
}

Path ConfigFile::getShadersDir() const {
	return Path(at("shadersDir"));
}

Path ConfigFile::getScreenshotsDir() const {
	return Path(at("screenshotsDir"));
}

TemplateCodeFile::TemplateCodeFile(const CodeFileDescriptor &templateCode, const CodeFileDescriptor &target, const vector<CodeMacro> &macros)
: templateCode(templateCode), target(target), macros(macros) {}

string TemplateCodeFile::generateCodeText() const {
	string code = templateCode.readCode();
	for (const auto &macro: macros)
		code = macro.apply(code);
	return code;
}

void TemplateCodeFile::render() const {
	target.writeCode(generateCodeText());
}

CodeFileDescriptor TemplateCodeFile::generatedCodeFile() const {
	render();
	return CodeFileDescriptor(target);
}

void TemplateCodeFile::addMacro(const CodeMacro &macro) {
	for (auto &m: macros)
		if (m.getKey() == macro.getKey()) {
			m = macro;
			return;
		}
	macros.push_back(macro);
}

TemplateCodeFile TemplateCodeFile::operator+(const CodeMacro &other) const {
	TemplateCodeFile res = *this;
	res.addMacro(other);
	return res;
}

void TemplateCodeFile::operator+=(const CodeMacro &other) {
	addMacro(other);
}

void TemplateCodeFile::operator+=(const TemplateCodeFile &other) {
	for (const auto &macro: other.macros) addMacro(macro);
	target = other.target;
}

TemplateCodeFile TemplateCodeFile::operator+(const TemplateCodeFile &other) const {
	TemplateCodeFile res = *this;
	res += other;
	return res;
}

ShaderMethodTemplate::ShaderMethodTemplate(const string &returnType, const string &name, const string &arguments, const string &body, const string &returnCode, const std::map<string, string> &templateParameters)
: returnType(returnType), name(name), arguments(arguments), body(body), returnCode(returnCode), templateParameters(templateParameters) {}

ShaderMethodTemplate::ShaderMethodTemplate(const string &returnType, const string &name, const string &arguments, const string &returnCode, const std::map<string, string> &templateParameters)
: ShaderMethodTemplate(returnType, name, arguments, "", returnCode, templateParameters) {}

string ShaderMethodTemplate::generateBody() const {
	string result = body;
	for (const auto &[fst, snd]: templateParameters)
		result = replaceAll(result, fst, snd);
	return result;
}

string ShaderMethodTemplate::generateCall(const string &args) const {
	return name + "(" + args + ")";
}

string ShaderMethodTemplate::generateCall(const string &arg1, const string &arg2) const {
	return name + "(" + arg1 + ", " + arg2 + ")";
}

string ShaderMethodTemplate::generateCall(const string &arg1, const string &arg2, const string &arg3) const {
	return name + "(" + arg1 + ", " + arg2 + ", " + arg3 + ")";
}

string ShaderMethodTemplate::randomRename() {
	name = name + "_" + randomStringNumeric();
	return name;
}

void ShaderMethodTemplate::changeName(const string &newName) {
	name = newName;
}

void ShaderMethodTemplate::replaceFunctionCalls(const string &oldName, const string &newName) {
	body = replaceAll(body, oldName + "(", newName + "(");
	returnCode = replaceAll(returnCode, oldName + "(", newName + "(");
}

ShaderMethodTemplate ShaderMethodTemplate::composeReturnWith(const ShaderMethodTemplate &f2) const {
	ShaderMethodTemplate res = *this;
	res.setReturnCode(f2.generateCall(returnCode));
	res.setReturnType(f2.returnType);
	return res;
}

std::map<string, string> ShaderMethodTemplateFromUniform::generateTemplateDict() {
	std::map<string, string> result = {};
	for (int i = 0; i < keys.size(); i++)
		result[keys[i]] = parameterInCodeStr(i);
	return result;
}

ShaderMethodTemplateFromUniform::ShaderMethodTemplateFromUniform(const string &returnType, const string &name, const string &arguments, const string &body, const string &returnCode, const vector<string> &keys, int firstIndex, const string &arrayName)
: ShaderMethodTemplate(returnType, name, arguments, body, returnCode), arrayName(arrayName), _firstIndex(firstIndex), keys(keys) {
	templateParameters = generateTemplateDict();
}

ShaderMethodTemplateFromUniform::ShaderMethodTemplateFromUniform(const string &returnType, const string &name, const string &arguments, const string &returnCode)
: ShaderMethodTemplateFromUniform(returnType, name, arguments, "", returnCode) {}

void ShaderMethodTemplateFromUniform::setFirstIndex(int newIndex) {
	_firstIndex = newIndex;
	templateParameters = generateTemplateDict();
}

void ShaderMethodTemplateFromUniform::increseFirstIndex(int n) {
	setFirstIndex(_firstIndex + n);
}

int ShaderMethodTemplateFromUniform::firstFreeIndex() const {
	return _firstIndex + keys.size();
}

ShaderMethodTemplateFromUniform ShaderMethodTemplateFromUniform::composeReturnWith(const ShaderMethodTemplate &f2) const {
	return ShaderMethodTemplateFromUniform(f2.getReturnType(), getName(), getArguments(), getBody(), f2.generateCall(getReturnCode()), keys, _firstIndex, arrayName);
}

ShaderRealFunction::ShaderRealFunction(const string &name, const string &body, const string &returnCode)
: ShaderMethodTemplateFromUniform("float", name, "float x", body, returnCode) {}

ShaderRealFunction::ShaderRealFunction(const string &name, const string &returnCode)
: ShaderRealFunction(name, "", returnCode) {}

ShaderMethodTemplate operator&(const string &f1Name, const ShaderMethodTemplate &f2) {
	return ShaderMethodTemplate(f2.getReturnType(), f2.getName(), f2.getArguments(), f2.getBody(), f1Name + "(" + f2.getReturnCode() + ")", f2.templateParameters);
}

string ShaderMethodTemplate::generateCode() const {
	return returnType + " " + name + "(" + arguments + ") {\n" + generateBody() + "\nreturn " + returnCode + ";\n}\n";
}

void ShaderMethodTemplate::applyRenaming(const string &oldName, const string &newName) {
	replaceFunctionCalls(oldName, newName);
	if (name == oldName) changeName(newName);
}

ShaderMethodTemplateFromUniform operator&(const string &f1Name, const ShaderMethodTemplateFromUniform &f2) {
	return ShaderMethodTemplateFromUniform(f2.getReturnType(), f2.getName(), f2.getArguments(), f2.getBody(), f1Name + "(" + f2.getReturnCode() + ")", f2.keys, f2._firstIndex, f2.arrayName);
}

int ShaderMethodTemplateFromUniform::parameterIndex(const string &key) const {
	return _firstIndex + indexOf(key, keys);
}

string ShaderMethodTemplateFromUniform::parameterInCodeStr(int i) const {
	return arrayName + "[" + str(i+_firstIndex) + "]";
}

int ShaderMethodTemplateFromUniform::firstIndex() const {
	return _firstIndex;
}

vector<string> ShaderMethodTemplateFromUniform::getSortedKeys() const {
	return keys;
}

string ShaderMethodTemplateFromUniform::getArrayName() const {
	return arrayName;
}

void ShaderMethodTemplateFromUniform::addParameter(const string &key) {
	keys.push_back(key);
	templateParameters = generateTemplateDict();
}

ShaderRealFunction operator&(const string &f1Name, const ShaderRealFunction &f2) {
	return ShaderRealFunction(f2.getName(), f2.getBody(), f1Name + "(" + f2.getReturnCode() + ")");
}

ShaderRealFunction ShaderRealFunction::composeReturnWith(const ShaderMethodTemplate &f2) const {
	return ShaderRealFunction(getName(), getBody(), f2.generateCall(getReturnCode()));
}

ShaderBinaryOperator::ShaderBinaryOperator(const string &name, const string &body, const string &returnCode)
: ShaderMethodTemplateFromUniform("float", name, "float x, float y", body, returnCode) {}

ShaderBinaryOperator::ShaderBinaryOperator(const string &name, const string &returnCode)
: ShaderBinaryOperator(name, "", returnCode) {}

ShaderRealFunctionR3::ShaderRealFunctionR3(const string &name, const string &body, const string &returnCode, const vector<string> &keys, int firstIndex, const string &arrayName)
: ShaderMethodTemplateFromUniform("float", name, "vec3 x", body, returnCode, keys, firstIndex, arrayName) {}

ShaderRealFunctionR3 operator&(const string &f1Name, const ShaderRealFunctionR3 &f2) {
	return ShaderRealFunctionR3(f2.getName(), f2.getBody(), f1Name + "(" + f2.getReturnCode() + ")");
}

ShaderRealFunctionR3 ShaderRealFunctionR3::composeReturnWith(const ShaderMethodTemplate &f2) const {
	return ShaderRealFunctionR3(getName(), getBody(), f2.generateCall(getReturnCode()));
}
