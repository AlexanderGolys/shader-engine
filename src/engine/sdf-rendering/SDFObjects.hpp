#pragma once
#include "../../engine/specific.hpp"
#include "../../utils/file-management/filesUtils.hpp"
#include "../../engine/glslUtils.hpp"




/**
 @brief Template of GLSL function calculating signed distance function of SDFObject that is parameterised by default with center and rotation.
 @details The template has by default 4 additional parameters, __center__ and __rot0__, __rot1__, __rot2__. The latter 3 are combined to rotation matrix
 with automatically generated part of the body and argument x is precomposed with affine transformation of x by center and rotation before
 the body provided in constructor is executed.

 @remark In case of SDFObjects constructed from binary operations on primitives, templates from this class may be used only as helperMethods and do
 not represent SDFs of any objects being part of the scene. Affine transformations of such partial objects are still meaningfull as transformations of
 primitive components.
 */
class ShaderSDFMethodTemplate : public ShaderRealFunctionR3 {

public:
	ShaderSDFMethodTemplate(const string &name, const string &body, const string &returnCode, const vector<string> &keys = {}, int firstIndex = 0, const string &arrayName = "params");
	ShaderSDFMethodTemplate(const string &name, const string &returnCode);
	ShaderSDFMethodTemplate(const ShaderRealFunctionR3 &sh);

};




/**
 @brief Primitive represented by signed distance function as a functor and GLSL function template, parameterised with single SDFMaterial and parameters of
 each ShaderMethodTemplate loaded from uniform buffer arrays in shader.
 @details SDFObjects are not primitive objects geometrically, but are the smallest object type recognised as independent entity at the level of shader code by its
 index in SDFScene container.
 */
class SDFObject {
	RealFunctionR3 _d;
	vector<ShaderMethodTemplateFromUniform> helperMethods;
	ShaderSDFMethodTemplate mainFunction;

public:
	SDFObject(const RealFunctionR3 &d, const ShaderSDFMethodTemplate &mainFunction_, const vector<ShaderMethodTemplateFromUniform> &helperMethods_={}, const string &sdfName="sdf");
	virtual ~SDFObject();


	string generateSDFCode() const;
	string sdfName() const;
	float sdf(vec3 p) const;
	int numberOfHelpers() const;

	void setEvalSdf(const RealFunctionR3 &d);
	void setMain(const ShaderSDFMethodTemplate &main);
	void addAffineTransformToCppSDF(const vec3 &center, const mat3 &rotation);
	void addParameterToMain(const string &key);
	void addTranslationAndRotationParameters();

	void renameHelper(int i, const string &newName);
	string randomRenameHelper(int i);
	void randomRenameHelpers();
	void randomRenameAll();
	string randomRenameMain();

	vector<string> sortedTemplateFunctionNames() const;
	vector<string> sortedParameterNamesPerFunction(int i) const;
	ShaderMethodTemplateFromUniform getHelper(int i);
	ShaderMethodTemplateFromUniform getHelper(string name) const;

	bool nameUsed(const string &name) const;
	void resolveConflictsWithName(const string &name);
	string helperCall(int i, const string &args) const;
	string helperCall(int i, const string &arg1, const string &arg2) const;
	void renameMainAndResolve(const string &name);

	void increaseParameters(int n);
	int firstFreeParamIndex() const;
	int parameterIndex(int helperNumber, const string &key) const;


	void addHelperDefaultIncrease(const ShaderMethodTemplateFromUniform &helper);
	void addHelperWithIncrease(const ShaderMethodTemplateFromUniform &helper, int increase);
	void addHelperNoIncrease(const ShaderMethodTemplateFromUniform &helper);
	void moveMainToHelpers();

	string sdfCode() const;

	SDFObject unitaryPostcompose(const ShaderRealFunction &foo, const RealFunction &eval) const;
	friend SDFObject binaryPostcompose(const ShaderBinaryOperator &foo, BIHOM(float, float, float) eval, const SDFObject &obj1, const SDFObject &obj2);
	SDFObject unitaryPostcomposeWithKnownOperator(string operatorName, const RealFunction &eval) const;
	friend SDFObject binaryPostcomposeWithKnownOperator(string operatorName, BIHOM(float, float, float) eval, const SDFObject &obj1, const SDFObject &obj2);

	SDFObject translate(vec3 t) const;
	SDFObject rotate(const mat3 &r, vec3 center = vec3(0)) const;
	SDFObject scale(float s) const;
	SDFObject complement() const;
	SDFObject operator+(const SDFObject &other) const;
	SDFObject operator-(const SDFObject &other) const;
	SDFObject operator*(const SDFObject &other) const;
	SDFObject operator~() const;
	SDFObject offset(float d) const;
	SDFObject onion(float thickness) const;
	SDFObject smoothUnion(const SDFObject &other, float k) const;
	SDFObject smoothSubtract(const SDFObject &other, float k) const;
	SDFObject smoothIntersect(const SDFObject &other, float k) const;
};


SDFObject sphereSDF(float radius, const string &name = "sdf");
SDFObject boxSDF(vec3 size, string name = "sdf");
SDFObject planeSDF(const string &name = "sdf");
SDFObject roundBoxSDF(vec3 size, float r, const string &name = "sdf");
SDFObject torusSDF(float r, float R, string name = "sdf");
SDFObject smoothTubeSDF(float r1, float r2, float R, float k, string name = "sdf");
SDFObject triangleSDF(vec3 a, vec3 b, vec3 c, string name = "sdf");
SDFObject quadSDF(vec3 a, vec3 b, vec3 c, vec3 d, string name = "sdf");
