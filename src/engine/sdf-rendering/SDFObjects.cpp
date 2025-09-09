//
// Created by PC on 11.12.2024.
//

#include "SDFObjects.hpp"





SDFObject::SDFObject(const RealFunctionR3 &d, const ShaderSDFMethodTemplate &mainFunction_, const vector<ShaderMethodTemplateFromUniform> &helperMethods_, const string &sdfName)
: _d(d),
helperMethods(helperMethods_),
mainFunction(mainFunction_) {
    renameMainAndResolve(sdfName);
}

SDFObject::~SDFObject() {}

string SDFObject::generateSDFCode() const {
	string result = "";
	for (const auto &helper : helperMethods)
		result += helper.generateCode();
	result += mainFunction.generateCode();
	return result;
}

float SDFObject::sdf(vec3 p) const {
	return _d(p);
}


int SDFObject::numberOfHelpers() const {
	return helperMethods.size();
}

void SDFObject::setEvalSdf(const RealFunctionR3 &d) { _d = d; }

void SDFObject::setMain(const ShaderSDFMethodTemplate &main) {
	resolveConflictsWithName(main.getName());
	mainFunction = main;
}


string SDFObject::randomRenameHelper(int i) {
	string oldName = helperMethods[i].getName();
	string newName = helperMethods[i].randomRename();
	for (auto &helper : helperMethods)
		helper.replaceFunctionCalls(oldName, newName);
	mainFunction.replaceFunctionCalls(oldName, newName);
	return newName;
}

void SDFObject::randomRenameHelpers() {
	for (int i = 0; i < helperMethods.size(); i++)
		randomRenameHelper(i);
}

void SDFObject::randomRenameAll() {
	randomRenameHelpers();
	randomRenameMain();
}

string SDFObject::randomRenameMain() {
	string oldName = mainFunction.getName();
	string newName = mainFunction.randomRename();
	return newName;
}

bool SDFObject::nameUsed(const string &name) const {
	for (const auto &helper : helperMethods)
		if (helper.getName() == name) return true;
	return mainFunction.getName() == name;
}

void SDFObject::resolveConflictsWithName(const string &name) {
	string newNanme = name+"_"+randomStringNumeric();
	for (auto& helper : helperMethods)
		helper.applyRenaming(name, newNanme);
	mainFunction.applyRenaming(name, newNanme);
}

string SDFObject::helperCall(int i, const string &args) const {
	return helperMethods[i].generateCall(args);
}

string SDFObject::helperCall(int i, const string &arg1, const string &arg2) const {return helperMethods[i].generateCall(arg1, arg2);}

void SDFObject::renameMainAndResolve(const string &name) {
	resolveConflictsWithName(name);
	mainFunction.changeName(name);
}

void SDFObject::increaseParameters(int n) {
	for (auto& helper : helperMethods)
		helper.increseFirstIndex(n);
	mainFunction.increseFirstIndex(n);
}




int SDFObject::firstFreeParamIndex() const {
	int m = mainFunction.firstFreeIndex();
	for (const auto &helper : helperMethods)
		m = std::max(m, helper.firstFreeIndex());
	return m;
}

void SDFObject::addHelperWithIncrease(const ShaderMethodTemplateFromUniform &helper, int increase) {
	resolveConflictsWithName(helper.getName());
	helperMethods.push_back(helper);
	helperMethods.back().increseFirstIndex(increase);
 }

SDFObject SDFObject::unitaryPostcomposeWithKnownOperator(string operatorName, const RealFunction &eval) const {
	RealFunctionR3 combinedSdfEval = eval & _d;
	ShaderSDFMethodTemplate main = operatorName & mainFunction;
	SDFObject result = *this;
	result.setEvalSdf(combinedSdfEval);
	result.setMain(main);
	return result;
}


SDFObject SDFObject::unitaryPostcompose(const ShaderRealFunction &foo, const RealFunction &eval) const {
	SDFObject result = unitaryPostcomposeWithKnownOperator(foo.getName(), eval);
	result.addHelperDefaultIncrease(foo);
	return result;
}




SDFObject binaryPostcompose(const ShaderBinaryOperator &foo, BIHOM(float, float, float) eval, const SDFObject &obj1, const SDFObject &obj2) {
	SDFObject result = binaryPostcomposeWithKnownOperator(foo.getName(), eval, obj1, obj2);
	result.addHelperDefaultIncrease(foo);
	return result;
}

SDFObject binaryPostcomposeWithKnownOperator(string operatorName, std::function<float(float, float)> eval, const SDFObject &obj1, const SDFObject &obj2) {
	auto combinedSdfEval = RealFunctionR3([eval, d1=obj1._d, d2=obj2._d](vec3 p) { return eval(d1(p), d2(p)); });
	auto result = obj1;
	result.setEvalSdf(combinedSdfEval);
	int firstSdfInd = result.numberOfHelpers();
	int increase = result.firstFreeParamIndex();

	result.addHelperNoIncrease(obj1.mainFunction);
	for (const auto& helper : obj2.helperMethods)
		result.addHelperWithIncrease(helper, increase);
	int secondSdfInd = result.numberOfHelpers();
	result.addHelperWithIncrease(obj2.mainFunction, increase);

	ShaderRealFunctionR3 main = ShaderRealFunctionR3("sdf_sum",
		operatorName + "(" + result.helperCall(firstSdfInd, "x") + ", " + result.helperCall(secondSdfInd, "x") + ")");
	result.setMain(main);
	return result;
}

SDFObject SDFObject::translate(vec3 t) const {
	throw NotImplementedError("Not implemented", __FILE__, __LINE__);
}

SDFObject SDFObject::rotate(const mat3 &r, vec3 center) const {
	throw NotImplementedError("SDFObject::rotate Not implemented", __FILE__, __LINE__);
}

SDFObject SDFObject::scale(float s) const {
	throw NotImplementedError("SDFObject::scale Not implemented", __FILE__, __LINE__);
}

SDFObject SDFObject::operator+(const SDFObject &other) const {
	return binaryPostcomposeWithKnownOperator("min",
		[](float x, float y) { return std::min(x, y); },
		*this,
		other);
}
SDFObject SDFObject::operator-(const SDFObject &other) const {
	//	ShaderMethodTemplate subOp = ShaderMethodTemplate("float", "subOp", "float x, float y",  "max(-x, y)");
	return binaryPostcomposeWithKnownOperator("max",
		[](float x, float y) { return std::max(x, y); },
		~*this,
		other);
}


SDFObject SDFObject::operator*(const SDFObject &other) const {
	return binaryPostcomposeWithKnownOperator("max",
		[](float x, float y) { return std::max(x, y); },
		*this,
		other);
}

SDFObject SDFObject::operator~() const {
	return unitaryPostcomposeWithKnownOperator("(-1)*", -RealFunction::x());
}


SDFObject SDFObject::offset(float d) const {
	ShaderRealFunction addition = ShaderRealFunction("sub_d", "x - " + str(d));
	return unitaryPostcompose(addition, RealFunction([d](float x){ return x - d; }));
}

SDFObject SDFObject::onion(float thickness) const {
	ShaderRealFunction modifier = ShaderRealFunction("onion", "abs(x) - " + str(thickness));
	return unitaryPostcompose(modifier, RealFunction([thickness](float x){ return abs(x) - thickness; }));
}

SDFObject SDFObject::smoothUnion(const SDFObject &other, float k) const {
	string _k = std::to_string(k);
	ShaderBinaryOperator minOp = ShaderBinaryOperator("SmoothMin",
		"float h = clamp( 0.5 + 0.5*(y-x)/"+_k+", 0.0, 1.0 );",
		"mix( y, x, h ) - "+_k+"*h*(1.0-h)");
	return binaryPostcompose(minOp, [k](float x, float y) {
		float h = clamp( 0.5 + 0.5*(y-x)/k, 0.0, 1.0 );
		return glm::mix( y, x, h ) - k*h*(1.0-h);
	}, *this, other);
}



SDFObject SDFObject::smoothSubtract(const SDFObject &other, float k) const {
	string _k = str(k);
	ShaderBinaryOperator minOp = ShaderBinaryOperator("SmoothSub",
		 "float h = clamp( 0.5 - 0.5*(y+x)/"+_k+", 0.0, 1.0 );",
		"mix( y, -x, h ) + "+_k+"*h*(1.0-h)");
	return binaryPostcompose(minOp, [k](float x, float y) {
		float h = clamp( 0.5 - 0.5*(y+x)/k, 0.0, 1.0 );
		return glm::mix( y, -x, h ) + k*h*(1.0-h);
	}, other, *this); }

SDFObject SDFObject::smoothIntersect(const SDFObject &other, float k) const {
	string _k = str(k);
	ShaderBinaryOperator minOp = ShaderBinaryOperator("SmoothIntersect",
		"float h = clamp( 0.5 - 0.5*(y-x)/"+_k+", 0.0, 1.0 );",
		"mix( y, x, h ) + "+_k+"*h*(1.0-h)" );
	return binaryPostcompose(minOp, [k](float x, float y) {
		float h = clamp( 0.5 - 0.5*(y-x)/k, 0.0, 1.0 );
		return glm::mix( y, x, h ) + k*h*(1.0-h);
	}, other, *this); }

string SDFObject::sdfName() const {
    return mainFunction.getName();
}
SDFObject SDFObject::complement() const {return ~*this;}

void SDFObject::renameHelper(int i, const string &newName)
{
	resolveConflictsWithName(newName);
	helperMethods[i].changeName(newName);
}

void SDFObject::addHelperDefaultIncrease(const ShaderMethodTemplateFromUniform &helper) {
	addHelperWithIncrease(helper, firstFreeParamIndex());}

void SDFObject::addHelperNoIncrease(const ShaderMethodTemplateFromUniform &helper) {
	resolveConflictsWithName(helper.getName());
	helperMethods.push_back(helper);
}

void SDFObject::moveMainToHelpers() {
	addHelperNoIncrease(mainFunction);
}






int SDFObject::parameterIndex(int templateIndex, const string &key) const {
	if (templateIndex == helperMethods.size())
		return mainFunction.parameterIndex(key);
	return helperMethods[templateIndex].parameterIndex(key);
}

vector<string> SDFObject::sortedTemplateFunctionNames() const {
	vector<string> result;
	for (const auto &helper : helperMethods)
		result.push_back(helper.getName());
	result.push_back(mainFunction.getName());
	return result;
}

vector<string> SDFObject::sortedParameterNamesPerFunction(int i) const {
	if (i == helperMethods.size())
		return mainFunction.getSortedKeys();
	return helperMethods[i].getSortedKeys();
}

ShaderMethodTemplateFromUniform SDFObject::getHelper(int i) { return i==helperMethods.size() ? mainFunction : helperMethods[i]; }

ShaderMethodTemplateFromUniform SDFObject::getHelper(string name) const {
	for (const auto &helper: helperMethods)
		if (helper.getName() == name)
			return helper;
	if (mainFunction.getName() == name)
		return mainFunction;
	throw ValueError("No helper with name " + name, __FILE__, __LINE__);
}

string SDFObject::sdfCode() const {
	string code = "";
	for (const auto &helper : helperMethods)
		code += helper.generateCode();
	code += mainFunction.generateCode();
	return code;
}

void SDFObject::addAffineTransformToCppSDF(const vec3 &center, const mat3 &rotation) {
	_d = RealFunctionR3([d = _d, center, rotation](vec3 x) { return d(transpose(rotation) * (x - center)); }, _d.getEps());
}

void SDFObject::addTranslationAndRotationParameters() {
	addParameterToMain("__center__");
	addParameterToMain("__rot0__");
	addParameterToMain("__rot1__");
	addParameterToMain("__rot2__");
	mainFunction.setBody("\tmat3 M = mat3(__rot0__, __rot1__, __rot2__);\n\tx = transpose(M)*(x-__center__);\n" + mainFunction.getBody());
}

void SDFObject::addParameterToMain(const string &key) { mainFunction.addParameter(key); }

SDFObject sphereSDF(float radius,const string &name) {
	auto d = RealFunctionR3([radius](vec3 p) { return length(p) - radius; });
	ShaderRealFunctionR3 main = ShaderRealFunctionR3(name, "float l = length(x) - __r__.x;", "l", {"__r__"});
	return SDFObject(d, main);
}
SDFObject boxSDF(vec3 size, string name) {
	auto d_ = RealFunctionR3([size](vec3 p) {
				vec3 q = abs(p) - size;
		return norm(max(q,0.0f)) + min(max(q.x,max(q.y,q.z)),0.0f);
	});
	ShaderRealFunctionR3 main = ShaderRealFunctionR3(name,
													 "\tvec3 q = abs(x) - __size__;\n",
													 "length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0)",
													 {"__size__"});
	return SDFObject(d_, main);
}

SDFObject planeSDF(const string &name) {
	auto d_ = RealFunctionR3([](vec3 p) { return p.z; });
	ShaderRealFunctionR3 main = ShaderRealFunctionR3(name, "", "x.z", {});
	return SDFObject(d_, main);
}
SDFObject roundBoxSDF(vec3 size, float r,  const string &name) {
	 auto d_ = RealFunctionR3([size, r](vec3 p) {
				vec3 q = abs(p) - size+ vec3(r);
		return norm(max(q,0.0f)) + min(max(q.x,max(q.y,q.z)),0.0f) - r;
	});
	ShaderRealFunctionR3 main = ShaderRealFunctionR3(name,
													 "\tfloat r=__r__.x;\n\tvec3 q = abs(x) - __size__ + vec3(r);",
													 "length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0)-r",
													 {"__size__", "__r__"});
	return SDFObject(d_, main);
}

SDFObject torusSDF(float r, float R, string name) {
	auto d_ = RealFunctionR3([r, R](vec3 p) {return norm(vec2(norm(vec2(p.x, p.y))-R,p.z)) - r;});
	ShaderRealFunctionR3 main = ShaderRealFunctionR3(name, "\tfloat l = length(vec2(length(x.xy)-__r__.y,x.z)) - __r__.x;", "l", {"__r__"});
	return SDFObject(d_, main);
}



//
//SDFObject smoothTubeSDF(float r1, float r2, float R, float k, vec3 center, const mat3 &rotation) {
//	return torusSDF(max(r1, r2), R, center, rotation).smoothSubtract(torusSDF(min(r1, r2), R, center, rotation), k);
//}
//
//
//SDFObject triangleSDF(vec3 a, vec3 b, vec3 c, string name) {
//	auto d = RealFunctionR3([a, b, c](vec3 x) {
//		vec3 v21 = b - a;
//		vec3 p1 = x - a;
//		vec3 v32 = c - b;
//		vec3 p2 = x - b;
//		vec3 v13 = a - c;
//		vec3 p3 = x - c;
//		vec3 nor = cross( v21, v13 );
//
//	return SQRT( (sign(dot(cross(v21,nor),p1)) +
//				  sign(dot(cross(v32,nor),p2)) +
//				  sign(dot(cross(v13,nor),p3))<2.0)
//				  ?
//				  min( min(
//				  norm2(v21*clamp(dot(v21,p1)/norm2(v21),0.0f,1.0)-p1),
//				  norm2(v32*clamp(dot(v32,p2)/norm2(v32),0.0f,1.0)-p2) ),
//				  norm2(v13*clamp(dot(v13,p3)/norm2(v13),0.0f,1.0)-p3) )
//				  :
//				  dot(nor,p1)*dot(nor,p1)/norm2(nor) );
//	});
//	auto main = SDFFunctionTemplate(name, [](vector<string> parameters) {
//		return "vec3 v21 = " + parameters[1] + " - " + parameters[0] + ";\n" +
//			  "vec3 p1 = x - " + parameters[0] + ";\n" +
//			  "vec3 v32 = " + parameters[2] + " - " + parameters[1] + ";\n" +
//			  "vec3 p2 = x - " + parameters[1] + ";\n" +
//			  "vec3 v13 = " + parameters[0] + " - " + parameters[2] + ";\n" +
//			  "vec3 p3 = x - " + parameters[2] + ";\n" +
//			  "vec3 nor = cross( v21, v13 );";
//	},
//			  string("SQRT( (sign(dot(cross(v21,nor),p1)) +\n") +
//				  "sign(dot(cross(v32,nor),p2)) +\n" +
//				  "sign(dot(cross(v13,nor),p3))<2.0)\n" +
//				  "?\n" +
//				  "min( min(\n" +
//				  "norm2(v21*clamp(dot(v21,p1)/norm2(v21),0.0f,1.0)-p1),\n" +
//				  "norm2(v32*clamp(dot(v32,p2)/norm2(v32),0.0f,1.0)-p2) ),\n" +
//				  "norm2(v13*clamp(dot(v13,p3)/norm2(v13),0.0f,1.0)-p3) )\n" +
//				  ":\n" +
//				  "dot(nor,p1)*dot(nor,p1)/norm2(nor);",
//		   { "params[0]", "params[1]", "params[2]",  });
//
//	return SDFObject(d, { a, b, c}, main, { 0, 1, 2});
//}
//
//
//SDFObject quadSDF(vec3 a, vec3 b, vec3 c, vec3 d, string name) {
//	auto d_ = RealFunctionR3([a, b, c, d](vec3 x) {
//			vec3 ba = b - a;
//			vec3 xa = x - a;
//			vec3 cb = c - b;
//			vec3 xb = x - b;
//			vec3 dc = d - c;
//			vec3 xc = x - c;
//			vec3 ad = a - d;
//			vec3 xd = x - d;
//			vec3 nor = cross(ba, ad);
//		return SQRT(
//	(sign(dot(cross(ba,nor),xa)) +
//	 sign(dot(cross(cb,nor),xb)) +
//	 sign(dot(cross(dc,nor),xc)) +
//	 sign(dot(cross(ad,nor),xd))<3.0)
//	 ?
//	 min( min( min(
//	 norm2(ba*clamp(dot(ba,xa)/norm2(ba),0.0f,1.0)-xa),
//	 norm2(cb*clamp(dot(cb,xb)/norm2(cb),0.0f,1.0)-xb) ),
//	 norm2(dc*clamp(dot(dc,xc)/norm2(dc),0.0f,1.0)-xc) ),
//	 norm2(ad*clamp(dot(ad,xd)/norm2(ad),0.0f,1.0)-xd) )
//	 :
//	 dot(nor,xa)*dot(nor,xa)/norm2(nor) );
//		});
//	auto main = SDFFunctionTemplate(name, [](vector<string> parameters) {
//		return "vec3 ba = " + parameters[1] + " - " + parameters[0] + ";\n" +
//			  "vec3 xa = x - " + parameters[0] + ";\n" +
//			  "vec3 cb = " + parameters[2] + " - " + parameters[1] + ";\n" +
//			  "vec3 xb = x - " + parameters[1] + ";\n" +
//			  "vec3 dc = " + parameters[3] + " - " + parameters[2] + ";\n" +
//			  "vec3 xc = x - " + parameters[2] + ";\n" +
//			  "vec3 ad = " + parameters[0] + " - " + parameters[3] + ";\n" +
//			  "vec3 xd = x - " + parameters[3] + ";\n" +
//			  "vec3 nor = cross(ba, ad);";
//	}, string("SQRT(\n") + "(\n" +  "sign(dot(cross(ba,nor),xa)) +\n" +
//	 "sign(dot(cross(cb,nor),xb)) +\n" +
//	 "sign(dot(cross(dc,nor),xc)) +\n" +
//	 "sign(dot(cross(ad,nor),xd))<3.0)\n" +
//	 "?\n" +
//	 "min( min( min(\n" +
//	 "norm2(ba*clamp(dot(ba,xa)/norm2(ba),0.0f,1.0)-xa),\n" +
//	 "norm2(cb*clamp(dot(cb,xb)/norm2(cb),0.0f,1.0)-xb) ),\n" +
//	 "norm2(dc*clamp(dot(dc,xc)/norm2(dc),0.0f,1.0)-xc) ),\n" +
//	 "norm2(ad*clamp(dot(ad,xd)/norm2(ad),0.0f,1.0)-xd) )\n" +
//	 ":\n" +
//	 "dot(nor,xa)*dot(nor,xa)/norm2(nor);", { "params[0]", "params[1]", "params[2]", "params[3]" });
//
//	return SDFObject(d_, { a, b, c, d}, main, { 0, 1, 2, 4});
//}
ShaderSDFMethodTemplate::ShaderSDFMethodTemplate(const string &name, const string &body, const string &returnCode, const vector<string> &keys, int firstIndex, const string &arrayName)
: ShaderRealFunctionR3(name, body, returnCode, keys, firstIndex, arrayName){}

ShaderSDFMethodTemplate::ShaderSDFMethodTemplate(const string &name, const string &returnCode)
: ShaderSDFMethodTemplate(name, "", returnCode) {}

ShaderSDFMethodTemplate::ShaderSDFMethodTemplate(const ShaderRealFunctionR3 &sh)
: ShaderSDFMethodTemplate(sh.getName(), sh.getBody(), sh.getReturnCode(), sh.getSortedKeys(), sh.firstIndex(), sh.getArrayName()){}
