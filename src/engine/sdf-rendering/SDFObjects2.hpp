#pragma once
#include "func.hpp"
#include "exceptions.hpp"

struct GLSLPrimitiveType {
	enum BuiltInType {
		GLSL_VOID,
		GLSL_FLOAT,
		GLSL_INT,
		GLSL_BOOL,
		GLSL_VEC2,
		GLSL_VEC3,
		GLSL_VEC4,
		GLSL_MAT2,
		GLSL_MAT3,
		GLSL_MAT4,
		GLSL_IVEC2,
		GLSL_IVEC3,
		GLSL_IVEC4,
		SE3_STRUCT
	} type;

	string typeName() const;
	GLSLPrimitiveType(BuiltInType t) : type(t) {}
	bool operator==(const GLSLPrimitiveType &other) const;
};

struct GLSLParameterType {
	GLSLPrimitiveType type;
	int arraySize;

	GLSLParameterType(GLSLPrimitiveType type, int arraySize = 1);
	GLSLParameterType(GLSLPrimitiveType::BuiltInType type, int arraySize = 1);

	string typeName() const;
	bool operator==(const GLSLParameterType &other) const;
};


string glslPrimitiveTypeName(GLSLPrimitiveType t);
string glslParameterTypeName(GLSLParameterType t);

// Forward declaration
struct GLSLStruct;

struct GLSLValidType
{
    variant<shared_ptr<GLSLStruct>, GLSLParameterType> type;

	GLSLValidType(shared_ptr<GLSLStruct> t) : type(t) {}
	GLSLValidType(const GLSLParameterType &t) : type(t) {}
	GLSLValidType(const GLSLPrimitiveType &t) : type(GLSLParameterType(t)) {}

	string typeName() const;
	bool isStruct() const;
	bool operator==(const GLSLValidType &other) const;
	bool operator==(const GLSLStruct &other) const;
	bool operator==(const GLSLParameterType &other) const;
	bool operator==(const GLSLPrimitiveType &other) const;
	bool operator==(const GLSLPrimitiveType::BuiltInType &other) const;

};





struct GLSLStruct
{
    string name;
    dict<string, GLSLValidType> members;
	GLSLStruct(const string &name, const dict<string, GLSLValidType> &members);

    string declarationCode() const;
    string typeName() const;
	bool operator==(const GLSLStruct &other) const;
};


class SDFStateStruct : public GLSLStruct {
public:
	SDFStateStruct(const GLSLStruct &base);
};



class GLSLFunction {
public:
	string name;
	GLSLValidType returnType;
	vector<pair<GLSLValidType, string>> arguments;
	string body;
	string returnLine;

	GLSLFunction(const string &name, const GLSLValidType &returnType, const vector<pair<GLSLValidType, string>> &arguments, const string &body, const string &returnLine);

	string generateCode() const;
	string getName() const;

};


struct SDFPrimitiveCentered {
	GLSLStruct sdfParametersStatic;
	GLSLFunction sdfFunction;
	SDFPrimitiveCentered(const GLSLStruct &sdfParametersStatic, const GLSLFunction &sdfFunction);
};



/*
 * This transforms SDF function using action of isometry group of R3 SE3 on itself.
 * Recall that every group action indiuces opposite side action after applying any contravariant functor, in particular on function space C(R3,R).
 * Similarly covariant functors induce same side actions, so paramedterised function spaces follow. As SDF is well behaved wrt isometries, this extends
 * for free the parameter space of the object we need to specify ourselves, e.g. now torus depends only on its 2 radii, not the center and rotation.
 *
 * The action of SE3 on R3 is implemented in quat.glsl module that need to be included in the shader code for this to work.
 */
struct SDFPrimitive {
	SDFStateStruct sdfState;
	GLSLFunction sdfFunction;
	SDFPrimitive(const SDFPrimitiveCentered &SDFDefaultPositioned);
};



class SDFObj {
	GLSLFunction sdf;
	SDFStateStruct state;
};
