#pragma once
#include "filesUtils.hpp"





class CodeMacro {
	string replacementKey;
	string replacementCode;
public:
	CodeMacro(const string &key, const string &c);
	CodeMacro(const string &key, const CodeFileDescriptor &file);
	CodeMacro(const CodeMacro &other);
	CodeMacro(CodeMacro &&other) noexcept;
	CodeMacro &operator=(const CodeMacro &other);
	CodeMacro &operator=(CodeMacro &&other) noexcept;
	string apply(const string &codeScheme) const;
	string getKey() const;
};


class TemplateCodeFile {
	vector<CodeMacro> macros;
	CodeFileDescriptor templateCode;
	CodeFileDescriptor target;


public:
	TemplateCodeFile(const CodeFileDescriptor &templateCode, const CodeFileDescriptor &target, const vector<CodeMacro> &macros);
	string generateCodeText() const;
	void render() const;
	CodeFileDescriptor generatedCodeFile() const;
	void addMacro(const CodeMacro &macro);
	TemplateCodeFile operator+(const TemplateCodeFile &other) const;
	TemplateCodeFile operator+(const CodeMacro &other) const;
	void operator+=(const CodeMacro &other);
	void operator+=(const TemplateCodeFile &other);
};



/**
 * @brief Class representing structure of GLSL function and generating methods for objects dynamically created on the C++ side.
 * @details Body block of a code may contained parameter macros. This postponed evaluation can be used to recognised parts of the code
 * parameterised by some additional data on GLSL side, for example from uniform array. Code generation of this part is postponed for reusability of the code
 * with different parameters in the same shader, convenient functional operators such as composition and independence of the template on concrete shader code.

 */
class ShaderMethodTemplate {


	string returnType; //! Codomain type
	string name; //! Function name
	string arguments; //! Full string representing arguments, as a block inside the parentheses foo(...)
	string body; //! Body of the function, allowing for creating templates with parameters replaced by blocks of GLSL code at later stage.
	string returnCode; //! Code following return statement. Parameter macros are not allowed here and must be evaluated inside the body block.

protected:
	//! Macros of additional parameters replaced by blocks of GLSL code during code generation.
	std::map<string, string> templateParameters;

public:
	ShaderMethodTemplate(const string &returnType, const string &name, const string &arguments, const string &body, const string &returnCode, const std::map<string, string> &templateParameters = {});
	ShaderMethodTemplate(const string &returnType, const string &name, const string &arguments, const string &returnCode, const std::map<string, string> &templateParameters = {});

	string generateBody() const;
	string generateCode() const;
	string generateCall(const string &args) const;
	string generateCall(const string &arg1, const string &arg2) const;
	string generateCall(const string &arg1, const string &arg2, const string &arg3) const;
	int numberOfParameters() const { return templateParameters.size(); }

	string randomRename();
	string getName() const { return name; }
	void changeName(const string &newName);
	void replaceFunctionCalls(const string &oldName, const string &newName);
	void applyRenaming(const string &oldName, const string &newName);

	void setReturnType(const string &newType) { returnType = newType; }
	void setArguments(const string &newArgs) { arguments = newArgs; }
	void setBody(const string &newBody) { body = newBody; }
	void setReturnCode(const string &newReturnCode) { returnCode = newReturnCode; }

	string getReturnType() const { return returnType; }
	string getArguments() const { return arguments; }
	string getBody() const { return body; }
	string getReturnCode() const { return returnCode; }


	friend ShaderMethodTemplate operator&(const string &f1Name, const ShaderMethodTemplate &f2);
	ShaderMethodTemplate composeReturnWith(const ShaderMethodTemplate &f2) const;
};



/**
 * @brief Template using parameters taken as consecutive values from GLSL uniform array starting from given index.
 * @details The type of array elements is arbitrary, as the macros are only associating parameter names with indexed elements
 * of a form "param[i]". This construction is useful when several objects from the same family are loaded into the shader
 * described by the same template and indexed by parameters loaded from the CPU side in uniform array, making possible
 * changing their states dynamically during animation without recompiling shader source code.
 */
class ShaderMethodTemplateFromUniform : public ShaderMethodTemplate {
	string arrayName;  //!< Name of the uniform array in GLSL code
	int _firstIndex; //! index of first parameter in the array
	vector<string> keys; //! Keys describing the parameters in a way meaningful from CPU side.
	std::map<string, string> generateTemplateDict(); //! generating mapping of parameters to array elements.
	string parameterInCodeStr(int i) const; //! GLSL code accessing i-th element of the array
public:
	ShaderMethodTemplateFromUniform(const string &returnType, const string &name, const string &arguments, const string &body, const string &returnCode, const vector<string> &keys={}, int firstIndex=0, const string &arrayName="params");
	ShaderMethodTemplateFromUniform(const string &returnType, const string &name, const string &arguments, const string &returnCode);

	void setFirstIndex(int newIndex); //! reindexing the first parameter
	void increseFirstIndex(int n); //! increasing the first parameter index by n
	int firstFreeIndex() const; //! index of last parameter
	int firstIndex() const; //! getter of first parameter
	int parameterIndex(const string &key) const; //! index of parameter with given key
	vector<string> getSortedKeys() const; //! getter of keys
	string getArrayName() const; //! getter of array name
	void addParameter(const string &key); //! adding new parameter at the end of the array

	friend ShaderMethodTemplateFromUniform operator&(const string &f1Name, const ShaderMethodTemplateFromUniform &f2); //! composition operator realised in-place in return statement with function of given name
	ShaderMethodTemplateFromUniform composeReturnWith(const ShaderMethodTemplate &f2) const;
};




class ShaderRealFunction : public ShaderMethodTemplateFromUniform {
	HOM(float, float) cpp_function;
public:
	ShaderRealFunction(const string &name, const string &body, const string &returnCode);
	ShaderRealFunction(const string &name, const string &returnCode);

	ShaderRealFunction composeReturnWith(const ShaderMethodTemplate &f2) const;
};

class ShaderBinaryOperator : public ShaderMethodTemplateFromUniform {
public:
	ShaderBinaryOperator(const string &name, const string &body, const string &returnCode);
	ShaderBinaryOperator(const string &name, const string &returnCode);
};

class ShaderRealFunctionR3 : public ShaderMethodTemplateFromUniform {
public:
	ShaderRealFunctionR3(const string &name, const string &body, const string &returnCode, const vector<string> &keys={}, int firstIndex=0, const string &arrayName="params");
	ShaderRealFunctionR3(const string &name, const string &returnCode) : ShaderRealFunctionR3(name, "", returnCode) {}

	friend ShaderRealFunctionR3 operator&(const string &f1Name, const ShaderRealFunctionR3 &f2);
	ShaderRealFunctionR3 composeReturnWith(const ShaderMethodTemplate &f2) const;
};
