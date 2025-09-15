#pragma once
#include <array>

#include <iosfwd>
#include <utility>
#include <vector>

//#include "src/engine/indexedRendering.hpp"

#include "smoothParametric.hpp"

class AffineLine;
class TriangulatedImplicitSurface;
class IndexedMesh;
// R3 -> R
class SmoothImplicitSurface {
    RealFunctionR3 _F;
public:
    explicit SmoothImplicitSurface(const RealFunctionR3 &F) : _F(F) {}
	explicit SmoothImplicitSurface(Foo31 F, float epsilon=0.01) : _F(RealFunctionR3(std::move(F), epsilon)) {}
	explicit SmoothImplicitSurface(Foo1111 F, float epsilon=0.01) : _F(RealFunctionR3(std::move(F), epsilon)) {}
	SmoothImplicitSurface(Foo31 F, Foo33 dF, float epsilon=0.01) : _F(RealFunctionR3(std::move(F), std::move(dF), epsilon)) {}
	SmoothImplicitSurface(Foo1111 F, Foo1113 dF, float epsilon=0.01) : _F(RealFunctionR3(std::move(F), std::move(dF), epsilon)) {}

    float operator()(vec3 p) const { return _F(p); }
	vec3 normal(vec3 p) const { return normalise(_F.df(p)); }
	vec3 newtonStepProject(vec3 p, float tolerance, int maxIter) const;
	void projectPoints(IndexedMesh &mesh, float tolerance, int maxIter);
	RealFunctionR3 getF() const {return _F;}

	SmoothImplicitSurface operator& (const SpaceEndomorphism &g) const;
	SmoothImplicitSurface isoSurface(float level) const;
	SmoothImplicitSurface shift(vec3 v) const {return *this & SpaceEndomorphism::translation(-v);}
	SmoothImplicitSurface scale(float a) const {return *this & SpaceAutomorphism::scaling(1/a);}
	SmoothImplicitSurface rotate(vec3 axis, float angle) const {return *this & SpaceAutomorphism::rotation(axis, -angle);}
};

class SmoothImplicitSurfacePencil {
	HOM(float, RealFunctionR3) _F_t;
public:
	explicit SmoothImplicitSurfacePencil(HOM(float, RealFunctionR3) F_t) : _F_t(std::move(F_t)) {}
	explicit SmoothImplicitSurfacePencil(HOM(float, SmoothImplicitSurface) F_t) : _F_t([F_t](float t) { return F_t(t).getF(); }) {}
	SmoothImplicitSurface operator()(float t) const { return SmoothImplicitSurface(_F_t(t)); }
	void deformMesh(float t, IndexedMesh &mesh, float tolerance, int maxIter);\
};

SmoothImplicitSurfacePencil interpolate(const SmoothImplicitSurface &s1, const SmoothImplicitSurface &s2);
SmoothImplicitSurfacePencil productMerge(const SmoothImplicitSurface &s1, const SmoothImplicitSurface &s2, float Rmax);

class AffinePlane : public SmoothImplicitSurface {
    vec3 n;
    float d; // (n, x) - d = 0
    vec3 pivot, v1, v2;

public:
    AffinePlane(vec3 n, float d);
    AffinePlane(vec3 pivot, vec3 v1, vec3 v2);
    static AffinePlane spanOfPts(vec3 p0, vec3 p1, vec3 p2);
    AffineLine intersect(const AffinePlane &p) const;
    float distance (vec3 p) const { return dot(p, n) - d; }
    vec3 orthogonalProjection(vec3 p) const { return p - distance(p) * n; }
    bool contains(vec3 p, float eps=1e-6) const { return abs(distance(p)) < eps; }

    std::pair<vec3, float> equationCoefs() const { return {n, d}; }
    vec3 intersect(const AffineLine &l) const;
    RealFunctionR3 distanceField() const {return RealFunctionR3([this](vec3 p) {return distance(p);} ); }
    mat3 pivotAndBasis() const;
    vec2 localCoordinates(vec3 p) const;

    vec3 normal() const { return n; }
    float getD() const { return d; }
};



class ImplicitSurfacePoint {
	vec3 p;
	mat3 orthoFrame;
	int birth_time;
	float angle = 0;
	bool requiresRecalculation = true;


public:
	bool border = false;

	ImplicitSurfacePoint(vec3 p, const mat3 &frame, int birth_time, bool border) : p(p), orthoFrame(frame), birth_time(birth_time), border(border) {}


	vec3 projectOnTangentPlane(vec3 q) const;
	vec3 rotateAroundNormal(vec3 q, float angle) const;

	vec3 coords_in_frame(vec3 v) const { return inverse(orthoFrame) * v; }
	vec3 getPosition() const { return p; }
	vec3 getNormal() const { return orthoFrame[2]; }
	std::pair<vec3, vec3> getTangents() const { return {orthoFrame[0], orthoFrame[1]}; }
	vec3 getTangent1() const { return  orthoFrame[0]; }
	vec3 getTangent2() const { return  orthoFrame[1]; }

	void markForRecalculation() { requiresRecalculation = true; }
	void markAsCalculated() { requiresRecalculation = false; }
	bool needsRecalculation() const { return requiresRecalculation; }
	float getAngle() const { return angle; }
	void setAngle(float a) { angle = a; requiresRecalculation = false; }
};

class ImplicitVolume {
	RealFunctionR3 _F; // F(x) >= 0 inside, F(x) < 0 outside
	vec3 x_min, x_max; // bounding box for the volume
public:
	ImplicitVolume(const RealFunctionR3 &F, vec3 bound_min, vec3 bound_max) : _F(F), x_min(bound_min), x_max(bound_max) {}
	bool contains(vec3 p) const { return _F(p) >= 0; }
	float separating_function(vec3 p) const { return _F(p); }
	vec3 inside_normal(vec3 p) const { return normalise(_F.df(p)); }
	SmoothImplicitSurface isoSurface(float level) const { return SmoothImplicitSurface(_F - level); }
	SmoothImplicitSurface boundary_surface() const { return isoSurface(0); }

	vec3 uniform_random_sample(int rec_limit=10000, int it=0) const;
	pair<vec3, vec3> bounding_box() const { return {x_min, x_max}; }

	ImplicitVolume set_union(const ImplicitVolume &other) const {
		return ImplicitVolume(max(_F, other._F), vec3(std::min(x_min.x, other.x_min.x), std::min(x_min.y, other.x_min.y), std::min(x_min.z, other.x_min.z)),
							  vec3(std::max(x_max.x, other.x_max.x), std::max(x_max.y, other.x_max.y), std::max(x_max.z, other.x_max.z)));
	}
	ImplicitVolume intersect(const ImplicitVolume &other) const {
		return ImplicitVolume(min(_F, other._F), vec3(std::max(x_min.x, other.x_min.x), std::max(x_min.y, other.x_min.y), std::max(x_min.z, other.x_min.z)),
							  vec3(std::min(x_max.x, other.x_max.x), std::min(x_max.y, other.x_max.y), std::min(x_max.z, other.x_max.z)));
	}


};


class TriangulatedImplicitSurface {
	RealFunctionR3 F;
	HOM(vec3, bool) borderCheck;
	int max_iter;
	int NewtonMaxSteps;
	float NewtonEps;
	int age = 0;
	std::vector<ImplicitSurfacePoint> points = {};
	vector<ivec3> triangles = {};
	vector<vector<int>> polygons = {};

	float angle(int i);

public:
	TriangulatedImplicitSurface(const RealFunctionR3 &F, int max_iter, HOM(vec3, bool) borderCheck, int NewtonMaxSteps, float NewtonEps);

	ImplicitSurfacePoint projectOnSurface(vec3 p);
	ImplicitSurfacePoint constructPoint(vec3 p);

	void initialiseHexagon(vec3 p0, float len);
	void expandFrontPolygon();
	void calculateMissingAngles();
	void calculateAngle(int i);
	int minAngleIndex();

	void checkFrontPolygonSelfIntersections();
	void checkFrontPolygonIntersectionWithPolygon(int i);
	void expandFrontPolygon(int i);
	void step();

	void generate();
};

class CubeCorner {
public:
	bool up, right, front;
	CubeCorner(bool up, bool right, bool front) : up(up), right(right), front(front) {}
	explicit CubeCorner(int i) : up(i & 1), right(i & 2), front(i & 4) {}
	int index() const { return up + 2*right + 4*front; }
	std::array<std::array<CubeCorner, 2>, 3> adjacentEdges() const;
	std::array<CubeCorner, 3> adjacentCorners() const;
	vector<CubeCorner> adjacentCornersVec() const;

	bool operator==(const CubeCorner &other) const { return up == other.up && right == other.right && front == other.front; }
	bool operator!=(const CubeCorner &other) const { return !(*this == other); }
};

bool nbhd(CubeCorner a, CubeCorner b);
bool opposite(CubeCorner a, CubeCorner b);
inline bool faceDiag(CubeCorner a, CubeCorner b) {return !opposite(a, b) && !nbhd(a, b); }
bool coplanar(CubeCorner a, CubeCorner b, CubeCorner c);
bool coplanar(CubeCorner a, CubeCorner b, CubeCorner c, CubeCorner d);




enum PrimitiveCubeConfigurationType {
	CORNER, EDGE, TRIPLE, CUBEFACE, LONG, TETRA
};

struct PrimitiveCubeConfiguration {
	PrimitiveCubeConfigurationType type;
	vector<CubeCorner> corners;
};


class TetrahedronMarching {
	ivec3 p1, p2, p3, p4;
public:
	TetrahedronMarching(ivec3 p1, ivec3 p2, ivec3 p3, ivec3 p4) : p1(p1), p2(p2), p3(p3), p4(p4) {}
	ivec3 edgeCenter(int i, int j) const;
	vector<ivec3> edgeCentersFromVertex(int i) const;
	vector<vector<ivec3>> trianglesBetween(int i, int j) const;
	vector<ivec3> edgeCentersFromVertex(ivec3 v) const;
	vector<vector<ivec3>> trianglesBetween(ivec3 v1, ivec3 v2) const;
	ivec3 vertex(int i) const;
	ivec3 operator[](int i) const { return vertex(i); }
	vector<ivec3> complement(const vector<ivec3> &arr) const;
	vector<ivec3> vertices() const { return {p1, p2, p3, p4}; }
};


class MarchingCubeChunk {
	PolyGroupID id;
	vec3 cornerLow, cornerHigh;
	ivec3 res;
	vector<ivec3> vertices = {};
	std::map<std::string, int> addedVertices = {};
	vector<ivec3> triangles = {};
	std::shared_ptr<SmoothImplicitSurface> surface;
	vec3 vertexStep;

	vector<PrimitiveCubeConfiguration> configurations(ivec3 cube) const;
	vector<CubeCorner> differingCorners(ivec3 cube) const;
	bool cornerSign(ivec3 cube, CubeCorner corner) const;
	int noDifferentCorners(ivec3 cube) const;

	static vector<CubeCorner> complementaryCorners(const vector<CubeCorner> &corners);
	static vector<CubeCorner> allCorners();


public:
	MarchingCubeChunk(const vec3 &corner0, const vec3 &corner1, const ivec3 &res, const std::shared_ptr<SmoothImplicitSurface> &surface);
	MarchingCubeChunk(const vec3 &cornerHighSym, const ivec3 &res, const std::shared_ptr<SmoothImplicitSurface> &surface) : MarchingCubeChunk(-cornerHighSym, cornerHighSym, res, surface) {}

	vec3 vertexPosition(ivec3 i) const;
	vec3 cornerPosition(ivec3 cube, CubeCorner corner) const;
	vec3 edgeCenter(ivec3 cube, CubeCorner corner1, CubeCorner corner2) const;
	static ivec3 cornerVertex(ivec3 cube, CubeCorner corner) ;
	static ivec3 edgeCenterVertex(ivec3 cube, CubeCorner corner1, CubeCorner corner2);
	PolyGroupID getID() const { return id; }

	void generate();
	void generateCube(ivec3 ind);
	bool vertexAlreadyAdded(ivec3 vertex) const;
	int addVertex(ivec3 vertex);
	bool cornerAlreadyAdded(ivec3 box, CubeCorner corner) const;
	int addCorner(ivec3 box, CubeCorner corner);
	bool edgeCenterAlreadyAdded(ivec3 box, CubeCorner corner1, CubeCorner corner2) const;
	int addEdgeCenter(ivec3 box, CubeCorner corner1, CubeCorner corner2);
	void addTriangle(ivec3 a, ivec3 b, ivec3 c);
	void generatePrimitiveConfiguration(const PrimitiveCubeConfiguration &config, ivec3 cube);


	void generateTetra();
	void generateCubeTetra(ivec3 ind);
	void generateTetrahedron(const TetrahedronMarching &tetra);
	vector<TetrahedronMarching> subdivideCubeFace(ivec3 center, ivec3 corner11, ivec3 corner12, ivec3 corner21, ivec3 corner22) const;
	vector<TetrahedronMarching> subdivideCube(ivec3 cube) const;



	void addToMesh(IndexedMesh &mesh);
	void addToMesh(IndexedMesh &mesh,  float tolerance, int maxIter);
	IndexedMesh generateMesh(bool tetra);
	IndexedMesh generateMesh(bool tetra, float tolerance, int maxIter);
};
