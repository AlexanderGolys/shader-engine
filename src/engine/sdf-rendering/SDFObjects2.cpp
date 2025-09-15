#include "SDFObjects2.hpp"


string GLSLPrimitiveType::typeName() const {
	switch (type) {
		case GLSL_FLOAT:	return "float";
		case GLSL_INT:  	return "int";
		case GLSL_BOOL: 	return "bool";
		case GLSL_VEC2: 	return "vec2";
		case GLSL_VEC3:		return "vec3";
		case GLSL_VEC4:		return "vec4";
		case GLSL_MAT2:		return "mat2";
		case GLSL_MAT3:		return "mat3";
		case GLSL_MAT4:		return "mat4";
		case GLSL_IVEC2:	return "ivec2";
		case GLSL_IVEC3:	return "ivec3";
		case GLSL_IVEC4:	return "ivec4";
		case GLSL_VOID:		return "void";
		case SE3_STRUCT:	return "SE3";
		default:
			THROW(IllegalVariantError, "Unrecognised GLSLPrimitiveType");
	}
}

bool GLSLPrimitiveType::operator==(const GLSLPrimitiveType &other) const {
	return type == other.type;
}


GLSLParameterType::GLSLParameterType(GLSLPrimitiveType type, int arraySize): type(type), arraySize(arraySize) {
	if (arraySize < 1)
		throw IllegalArgumentError("Array size must be at least 1.", __FILE__, __LINE__);
}

GLSLParameterType::GLSLParameterType(GLSLPrimitiveType::BuiltInType type, int arraySize): type(type), arraySize(arraySize) {}

string GLSLParameterType::typeName() const { return glslParameterTypeName(*this); }

bool GLSLParameterType::operator==(const GLSLParameterType &other) const {
	return type.typeName() == other.type.typeName();
}

string glslParameterTypeName(GLSLParameterType t) {
	if (t.arraySize == 1)
		return glslPrimitiveTypeName(t.type);
	return glslPrimitiveTypeName(t.type) + "[" + std::to_string(t.arraySize) + "]";
}

bool GLSLValidType::operator==(const GLSLValidType &other) const {
	if (isStruct() != other.isStruct())
		return false;
	if (isStruct())
		return *get<shared_ptr<GLSLStruct>>(type) == *get<shared_ptr<GLSLStruct>>(other.type);
	return get<GLSLParameterType>(type) == get<GLSLParameterType>(other.type);
}

bool GLSLValidType::operator==(const GLSLStruct &other) const {
	return isStruct() && other == *get<shared_ptr<GLSLStruct>>(type);
}

bool GLSLValidType::operator==(const GLSLParameterType &other) const {
	return !isStruct() && other == get<GLSLParameterType>(type);
}

bool GLSLValidType::operator==(const GLSLPrimitiveType &other) const {
	return !isStruct() && other == get<GLSLParameterType>(type).type;
}

bool GLSLValidType::operator==(const GLSLPrimitiveType::BuiltInType &other) const {
	return !isStruct() && other == get<GLSLParameterType>(type).type.type;
}


GLSLStruct::GLSLStruct(const string &name, const dict<string, GLSLValidType> &members): name(name), members(members) {}

string GLSLStruct::declarationCode() const {
	string s = "struct " + name + " {\n";
	for (const auto& [memberName, memberType] : members)
		s += "    " + memberType.typeName() + " " + memberName + ";\n";
	s += "};\n\n";
	return s;
}

string GLSLStruct::typeName() const {
	return name;
}

bool GLSLStruct::operator==(const GLSLStruct &other) const {
	return declarationCode() == other.declarationCode();
}

string GLSLValidType::typeName() const {
	if (holds_alternative<GLSLParameterType>(type))
		return get<GLSLParameterType>(type).typeName();
	return get<shared_ptr<GLSLStruct>>(type)->typeName();
}

bool GLSLValidType::isStruct() const {
	return not holds_alternative<GLSLParameterType>(type);
}

GLSLFunction::GLSLFunction(const string &name, const GLSLValidType &returnType, const vector<pair<GLSLValidType, string>> &arguments, const string &body, const string &returnLine)
: name(name), returnType(returnType), arguments(arguments), body(body), returnLine(returnLine) {}

string GLSLFunction::getName() const {
	return name;
}




SDFPrimitiveCentered::SDFPrimitiveCentered(const GLSLStruct &sdfParametersStatic, const GLSLFunction &sdfFunction)
: sdfParametersStatic(sdfParametersStatic), sdfFunction(sdfFunction) {
	if (sdfFunction.returnType != GLSLPrimitiveType::GLSL_FLOAT)
		THROW(ValueError, "SDF function must return float.");
	if (sdfFunction.arguments.size() != 2)
		THROW(ValueError, "SDF function must take exactly two arguments: a vec3 point and a struct of parameters.");
	if (sdfFunction.arguments.at(0).second != "param")
		THROW(ValueError, "Fi.at(0) argument of SDF function must be a parameter struct named 'param'.");
	if (sdfFunction.arguments.at(1).first != GLSLPrimitiveType::GLSL_VEC3 || sdfFunction.arguments[1].second != "x")
		THROW(ValueError, "Second argument of SDF function must be the position vector (vec3) named x");
}

SDFStateStruct::SDFStateStruct(const GLSLStruct &base):
GLSLStruct("State_" + base.name,
			dict<string, GLSLValidType>{
				{"transform", GLSLValidType(GLSLParameterType(GLSLPrimitiveType::SE3_STRUCT))},
				{"params", GLSLValidType(make_shared<GLSLStruct>(base))}
			}) {}


SDFPrimitive::SDFPrimitive(const SDFPrimitiveCentered &SDFDefaultPositioned)
: sdfState(SDFDefaultPositioned.sdfParametersStatic), sdfFunction(SDFDefaultPositioned.sdfFunction)
{
	sdfFunction.arguments[0].second += "__";
	sdfFunction.arguments[1].second += "__";
	auto param_struct_name = SDFDefaultPositioned.sdfParametersStatic.name;

	sdfFunction.body = "SE3 g = param__.transform;\n " + param_struct_name + " param = param__.params;\n" + "vec3 x = act_SE3(inv_SE3(g), x__);\n" + sdfFunction.body;
}
