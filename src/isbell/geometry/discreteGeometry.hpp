#pragma once

#include "engine/indexedRendering.hpp"


class TriangulatedManifold : public IndexedMesh {
	vector<ivec2> edgesVert;
	vector<ivec3> facesEdg;
	PolyGroupID id;

public:
	vec3 vertexPosition(int i) const { return getVertices(id)[i].getPosition(); }
	mat2x3 edgeVertices (int i) const { return {
	mat2x3(vertexPosition(edgesVert[i].x), vertexPosition(edgesVert[i].y))}; }
	mat3 faceVertices(int i);
	vec3 edgeVector(int i) const { return vertexPosition(edgesVert[i].y) - vertexPosition(edgesVert[i].x); }
	mat3 edgesVectorsOfFace(int i) const { return mat3(edgeVector(facesEdg[i].x), edgeVector(facesEdg[i].y), edgeVector(facesEdg[i].z)); }
	mat3 tangentNormalFrameOfFace(int i) const {return GramSchmidtProcess(edgesVectorsOfFace(i));}

	ivec2 edgeVerticesIndices(int i) const { return edgesVert[i]; }
	ivec3 faceEdgesIndices(int i) const { return facesEdg[i]; }

	mat2x3 orthoFaceTangents(int i) const;
	mat2x3 stdFaceTangents(int i) const;
	vec2 stdTangentToOrtho(vec2 v, int i) const;
	vec2 orthoTangentToStd(vec2 v, int i) const;

	int numEdges() const { return edgesVert.size(); }
	int numFaces() const { return facesEdg.size(); }
	int numVertices() const { return getVertices(id).size(); }
};


class Discrete1Form;


class DiscreteRealFunctionManifold {
	FloatVector values;
	std::shared_ptr<TriangulatedManifold> domain;
public:
	explicit DiscreteRealFunctionManifold(const vector<float> &values, const std::shared_ptr<TriangulatedManifold> &domain) : values(values), domain(domain) {}
	explicit DiscreteRealFunctionManifold(const FloatVector &values, const std::shared_ptr<TriangulatedManifold> &domain) : values(values), domain(domain) {}

	float operator()(int i) { return values[i]; }
	DiscreteRealFunctionManifold operator+(const DiscreteRealFunctionManifold &f) const { return DiscreteRealFunctionManifold(values + f.values, domain); }
	DiscreteRealFunctionManifold operator-(const DiscreteRealFunctionManifold &f) const { return DiscreteRealFunctionManifold(values - f.values, domain); }
	DiscreteRealFunctionManifold operator*(float a) const { return DiscreteRealFunctionManifold(values*a, domain); }
	DiscreteRealFunctionManifold operator/(float a) const { return DiscreteRealFunctionManifold(values/a, domain); }
	DiscreteRealFunctionManifold operator-() const { return DiscreteRealFunctionManifold(-values, domain); }
	FloatVector toBumpBasis() const;
	static DiscreteRealFunctionManifold bump(int i);

	float edgeLength(int i) const { return length(domain->edgeVector(i)); }
	float faceArea(int i) const { return abs(det(domain->faceVertices(i))); }
	Discrete1Form exteriorDerivative() const;
};

class Discrete2Form;

class Discrete1Form {
	FloatVector edgeValues;
	std::shared_ptr<TriangulatedManifold> domain;

public:
	explicit Discrete1Form(const vector<float> &values, const std::shared_ptr<TriangulatedManifold> &domain) : edgeValues(values), domain(domain) {}
	explicit Discrete1Form(const FloatVector &values, const std::shared_ptr<TriangulatedManifold> &domain) :  edgeValues(values), domain(domain) {}

	float operator()(int i) { return  edgeValues[i]; }
	Discrete1Form operator+(const Discrete1Form &f) const { return Discrete1Form( edgeValues + f. edgeValues, domain); }
	Discrete1Form operator-(const Discrete1Form &f) const { return Discrete1Form( edgeValues - f. edgeValues, domain); }
	Discrete1Form operator*(float a) const { return  Discrete1Form( edgeValues*a, domain); }
	Discrete1Form operator/(float a) const { return Discrete1Form( edgeValues/a, domain); }
	Discrete1Form operator-() const { return Discrete1Form(- edgeValues, domain); }
	FloatVector toBumpOrthoBasis() const;
	static Discrete1Form bumpOrtho(int i, int j);
	static Discrete1Form bumpStd(int i, int j);
	float integrate(const vector<int> &edgePath) const;
	Discrete2Form exteriorDerivative() const;
//	Discrete2Form d() const { return exteriorDerivative(); }
};

class Discrete2Form {
	FloatVector faceValues;
	std::shared_ptr<TriangulatedManifold> domain;
public:
	explicit Discrete2Form(const vector<float> &values, const std::shared_ptr<TriangulatedManifold> &domain) : faceValues(values), domain(domain) {}
	explicit Discrete2Form(const FloatVector &values, const std::shared_ptr<TriangulatedManifold> &domain) : faceValues(values), domain(domain) {}
	Discrete2Form operator+(const Discrete2Form &f) const { return Discrete2Form( faceValues + f. faceValues, domain); }
	Discrete2Form operator-(const Discrete2Form &f) const { return Discrete2Form( faceValues - f. faceValues, domain); }
	Discrete2Form operator*(float a) const { return  Discrete2Form( faceValues*a, domain); }
	Discrete2Form operator/(float a) const { return Discrete2Form(  faceValues/a, domain); }
	Discrete2Form operator-() const { return Discrete2Form(- faceValues, domain); }
	float integrate();

	FloatVector toBumpOrthoBasis() const;
	static Discrete2Form bumpOrtho(int i, int j);
	static Discrete2Form bumpStd(int i, int j);
};
