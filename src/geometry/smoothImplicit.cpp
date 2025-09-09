#include "smoothImplicit.hpp"
#include "../utils/func.hpp"
#include "../engine/indexedRendering.hpp"

using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared, std::function;

using namespace glm;


vec3 SmoothImplicitSurface::newtonStepProject(vec3 p, float tolerance, int maxIter) const {
	vec3 last_q = p;
	for (int i = 0; i < maxIter; i++) {
		vec3 q = last_q - _F.df(last_q)*_F(last_q)/norm2(_F.df(last_q));
		if (norm2(q - last_q) < tolerance*tolerance)
			return q;
		last_q = q;
	}
	return last_q;
}

void SmoothImplicitSurface::projectPoints(IndexedMesh &mesh, float tolerance, int maxIter) {
	mesh.deformPerVertex([this, tolerance, maxIter](BufferedVertex &v) {
		v.setPosition(newtonStepProject(v.getPosition(), tolerance, maxIter));
		v.setNormal(normal(v.getPosition()));
	});
}

SmoothImplicitSurface SmoothImplicitSurface::operator&(const SpaceEndomorphism &g) const {
	return SmoothImplicitSurface([F=_F, g](vec3 x) { return F(g(x)); },
								 [F=_F, g](vec3 x) { return F.df(g(x))*g.df(x); },
								 min(_F.getEps(), g.getEps())	);
}

SmoothImplicitSurface SmoothImplicitSurface::isoSurface(float level) const {
	return SmoothImplicitSurface(_F - level);
}

void SmoothImplicitSurfacePencil::deformMesh(float t, IndexedMesh &mesh, float tolerance, int maxIter) {
	(*this)(t).projectPoints(mesh, tolerance, maxIter);
}

SmoothImplicitSurfacePencil interpolate(const SmoothImplicitSurface &s1, const SmoothImplicitSurface &s2) {
	return SmoothImplicitSurfacePencil([F=s1.getF(), G=s2.getF()](float t) {
		float s = smoothstep(0.f, 1.f, t);
		return (1-s)*F + s*G;
	});
}

SmoothImplicitSurfacePencil productMerge(const SmoothImplicitSurface &s1, const SmoothImplicitSurface &s2, float Rmax) {
	return SmoothImplicitSurfacePencil([F=s1.getF(), G=s2.getF(), Rmax](float t) {
	float s = smoothstep(0.f, 1.f, t);
	return F*G - s*Rmax;
	});
}


AffinePlane::AffinePlane(vec3 n, float d) :
SmoothImplicitSurface(RealFunctionR3([n, d](vec3 p) {return dot(p, n) - d; }, [n](vec3 p) {return n; })) {
    this->n = normalise(n);
    this->d = d;
    auto tangents = orthogonalComplementBasis(n);
    this->v1 = tangents.first;
    this->v2 = tangents.second;
    this->pivot = this->n * d;
}



AffinePlane::AffinePlane(vec3 pivot, vec3 v1, vec3 v2):
SmoothImplicitSurface(RealFunctionR3(
    [v1, v2, pivot](vec3 p) {return dot(p, normalise(cross(v1, v2))) - dot(pivot, normalise(cross(v1, v2))); },
    [v1, v2](vec3 p) {return normalise(cross(v1, v2)); })) {
    this->pivot = pivot;
    this->v1 = v1;
    this->v2 = v2;
    this->n = normalise(cross(v1, v2));
    this->d = dot(pivot, n);
}
    AffinePlane AffinePlane::spanOfPts(vec3 p0, vec3 p1, vec3 p2) {
    return AffinePlane(p0, p1 - p0, p2 - p0);
}

AffineLine AffinePlane::intersect(const AffinePlane &p) const {
	vec3 v = cross(n, p.normal());
	float denominator = norm2(n)*norm2(p.normal()) - dot(n, p.normal())*dot(n, p.normal());
	vec3 pivot = (d*norm2(p.normal()) - p.getD()*dot(n, p.normal()))/denominator +
				p.normal()*(p.getD()*norm2(n) - d*dot(n, p.normal()))/denominator;
	return AffineLine(pivot, v);
}


vec3 AffinePlane::intersect(const AffineLine &l) const {
	return l.pivot() + dot(pivot - l.pivot(), n) / dot(l.direction(), n) * l.direction();
}

mat3 AffinePlane::pivotAndBasis() const {
    return mat3(pivot, v1, v2);
}

vec2 AffinePlane::localCoordinates(vec3 p) const {
    return vec2(dot(p - pivot, v1), dot(p - pivot, v2));
}





SmoothParametricCurve::SmoothParametricCurve(Foo13 f,Foo13 df,  Foo13 ddf, PolyGroupID id, float t0, float t1, bool periodic, float epsilon)
{
    this->_f = f;
    this->_df = df;
    this->_ddf = ddf;
    this->eps = epsilon;
    this->t0 = t0;
    this->t1 = t1;
    this->periodic = periodic;
    this->id = id;
}

SmoothParametricCurve::SmoothParametricCurve(Foo13 f, Foo13 df, PolyGroupID id, float t0, float t1, bool periodic, float epsilon)
: SmoothParametricCurve(f, df, derivativeOperator(df, epsilon), id, t0, t1, periodic, epsilon) {}



SmoothParametricCurve::SmoothParametricCurve(const SmoothParametricCurve &other) :
    _f(other._f), _df(other._df), _ddf(other._ddf), _der_higher(other._der_higher), eps(other.eps), t0(other.t0), t1(other.t1),
    periodic(other.periodic), id(other.id){}

SmoothParametricCurve::SmoothParametricCurve(SmoothParametricCurve &&other) noexcept :
    _f(std::move(other._f)), _df(std::move(other._df)), _ddf(std::move(other._ddf)), _der_higher(std::move(other._der_higher)),
    eps(other.eps), t0(other.t0), t1(std::move(other.t1)), periodic(other.periodic), id(other.id) {}

SmoothParametricCurve &SmoothParametricCurve::operator=(const SmoothParametricCurve &other) {
    if (this == &other)
        return *this;
    _f = other._f;
    _df = other._df;
    _ddf = other._ddf;
    _der_higher = other._der_higher;
    eps = other.eps;
    t0 = other.t0;
    t1 = other.t1;
    periodic = other.periodic;
    id = other.id;
    return *this;
}
SmoothParametricCurve &SmoothParametricCurve::operator=(SmoothParametricCurve &&other) noexcept {
    if (this == &other)
        return *this;
    _f = std::move(other._f);
    _df = std::move(other._df);
    _ddf = std::move(other._ddf);
    _der_higher = std::move(other._der_higher);
    eps = other.eps;
    t0 = std::move(other.t0);
    t1 = std::move(other.t1);
    periodic = other.periodic;
    id = other.id;
    return *this;
}

SmoothParametricCurve::SmoothParametricCurve(Foo13 f, std::vector<Foo13> derivatives, PolyGroupID id, float t0, float t1, bool periodic, float epsilon) {
	this->_f = f;
	if (derivatives.size() > 0)
		this->_df = derivatives[0];
	else
		this->_df = derivativeOperator(f, epsilon);

	if (derivatives.size() > 1)
		this->_ddf = derivatives[1];
	else
		this->_ddf = derivativeOperator(_df, epsilon);

	this->_der_higher = [derivatives, f](int n) { return n==0 ? f : n <= derivatives.size() ? derivatives[n-1] : derivativeOperator(derivatives[n-1], 0.01f);} ;
	this->eps = epsilon;
    this->t0 = t0;
    this->t1 = t1;
    this->periodic = periodic;
    id = randomCurvaID();
}



SmoothParametricCurve SmoothParametricCurve::constCurve(vec3 v) {
	return SmoothParametricCurve([v](float t) {return v; },-10, 10, false, 0.01);
}

float SmoothParametricCurve::length(float t0, float t1, int n) const {
    float res = 0;
    float dt = (t1 - t0) / n;
    for (int i = 0; i < n; i++)
		res += norm(_f(t0 + dt*i) - _f(t0 + dt*(i+1)));
	return res;
}
SmoothParametricCurve SmoothParametricCurve::precompose(SpaceEndomorphism g_) const {
    return SmoothParametricCurve([f = this->_f, g=g_](float t) {return g(f(t)); },
                                  [f = this->_f, d = this->_df, g=g_](float t) {return g.df(f(t)) * d(t); },
                                  id, this->t0, this->t1, this->periodic, this->eps);
}
void SmoothParametricCurve::precomposeInPlace(SpaceEndomorphism g) {
    this->_f = [f = this->_f, gg=g](float t) {return gg(f(t)); };
    this->_df = [f = this->_f, d = this->_df, g](float t) {return g(f(t)) * d(t); };
    this->_ddf = [f = this->_f, d = this->_df, dd = this->_ddf, gg=g](float t) {return gg(f(t)) * dd(t) + gg.df(f(t)) * d(t); };
}

vec3 SmoothParametricCurve::operator()(float t) const { return this->_f(t); }

mat3 SmoothParametricCurve::FrenetFrame(float t) const {
	return mat3(tangent(t), normal(t), binormal(t));

}

float SmoothParametricCurve::curvature(float t) const {
	return norm(cross(_ddf(t), _df(t))) / pow(norm(_df(t)), 3);
}

float SmoothParametricCurve::torsion(float t) const {
	return dot(cross(_df(t), ddf(t)), higher_derivative(t, 3)) / norm2(cross(df(t), ddf(t)));
}

vec3 SmoothParametricCurve::curvature_vector(float t) const {
	return normal(t)/curvature(t);
}

AffineLine::AffineLine(vec3 p0, vec3 v) :
SmoothParametricCurve([p0, v](float t) {return p0 + t * normalise(v); },
					  [v](float t) {return normalise(v); },
					  [](float t) {return vec3(0, 0, 0); }) {
	this->p0 = p0;
	this->v = normalise(v);
}

AffineLine AffineLine::spanOfPts(vec3 p0, vec3 p1) {
	return AffineLine(p0, p1 - p0);
}


RealFunctionR3 AffineLine::distanceField() const {
	return RealFunctionR3([this](vec3 p) {return distance(p); });
}

vec3 AffineLine::orthogonalProjection(vec3 p) const {
	return p0 + dot(p - p0, v) * v / dot(v, v);
}

vec3 AffineLine::pivot() const {
	return p0;
}

vec3 AffineLine::direction() const {
	return v;
}

float AffineLine::distance(AffineLine &l) const {
	return dot((p0 - l.pivot()), cross(v, l.direction())) / norm(cross(v, l.direction()));
}

AffineLine AffineLine::operator+(vec3 v) const {
	return AffineLine(p0 + v, this->v);
}


float AffineLine::distance(vec3 p) const {
	return norm(cross(p - p0, v)) / norm(v);
}

bool AffineLine::contains(vec3 p, float eps) const {
	return distance(p) < eps;
}




vec3 ImplicitSurfacePoint::projectOnTangentPlane(vec3 q) const {
	vec3 ortho_coords = coords_in_frame(q);
	return getTangent1() * ortho_coords.x + getTangent2() * ortho_coords.y;
}
vec3 ImplicitSurfacePoint::rotateAroundNormal(vec3 q, float angle) const {
	vec3 projected = projectOnTangentPlane(q);
	return projected*cos(angle) + cross(projected, getNormal())*sin(angle);
}

vec3 ImplicitVolume::uniform_random_sample(int rec_limit, int it) const {
	if (it > rec_limit) 
		throw RecursionLimitExceeded(rec_limit, "uniform sampling from implicit volume", __FILE__, __LINE__);
	vec3 x = random_vec3(x_min, x_max);
	if (!contains(x))
		return uniform_random_sample(rec_limit, it+1);
	return x;
}

float TriangulatedImplicitSurface::angle(int i) { return points[i].getAngle(); }


ImplicitSurfacePoint TriangulatedImplicitSurface::projectOnSurface(vec3 p) {
	vec3 last_q = p;
	for (int i = 0; i < NewtonMaxSteps; i++) {
		vec3 q = last_q - F.df(last_q)*F(last_q)/norm2(F.df(last_q));
		if (norm(q - last_q) < NewtonEps) {
			last_q = q;
			break;
		}
		last_q = q;
	}
	return constructPoint(last_q);
}

ImplicitSurfacePoint TriangulatedImplicitSurface::constructPoint(vec3 p) {
	vec3 normal = normalise(F.df(p));
	vec3 t1;
	if (normal.x > .5 || normal.y>.5)
		t1 = normalise(vec3(normal.y, -normal.x, 0));
	else
		t1 = normalise(vec3(-normal.z, 0, normal.x));
	vec3 t2 = cross(normal, t1);
	return ImplicitSurfacePoint(p, mat3(normal, t1, t2), age, borderCheck(p));
}


void TriangulatedImplicitSurface::initialiseHexagon(vec3 p0, float len) {
	points = {projectOnSurface(p0)};
	for (int i = 0; i < 6; i++) {
		vec3 p = p0 + len*cos(i*TAU / 6)*vec3(1, 0, 0) + len*sin(i*TAU / 6)*vec3(0, 1, 0);
		points.push_back(projectOnSurface(p));
	}
	polygons = {{1, 2, 3, 4, 5, 6}};
}


void TriangulatedImplicitSurface::expandFrontPolygon() { throw std::logic_error("Not implemented"); }


void TriangulatedImplicitSurface::calculateMissingAngles() {
}


void TriangulatedImplicitSurface::calculateAngle(int i) {
	int ind_p = polygons[0][i];
	vec3 p = points[ind_p].getPosition();
	vec3 r = points[polygons[0][(i + 1) % 6]].getPosition();
	vec3 q = points[polygons[0][(i - 1 + polygons[0].size()) % 6]].getPosition();
	vec3 v1 = q-p;
	vec3 v2 = r-p;
	float angle = acos(dot(v1, v2) / (norm(v1)*norm(v2)));
	if (dot(cross(v1, v2), points[ind_p].getNormal()) < 0)
		angle = TAU - angle;
	points[ind_p].setAngle(angle);
}

int TriangulatedImplicitSurface::minAngleIndex() {
	int minInd = 0;
	float minAngle = TAU;
	for (int i = 0; i < polygons[0].size(); i++) {
			int ind = polygons[0][i];
			if (points[ind].getAngle() < minAngle && !points[ind].border) {
				minAngle = points[ind].getAngle();
				minInd = ind;
			}
		}
	return minInd;
}


std::array<CubeCorner, 3> CubeCorner::adjacentCorners() const {
	return {CubeCorner(!up, right, front),
			CubeCorner(up, !right, front),
			CubeCorner(up, right, !front)};
}

vector<CubeCorner> CubeCorner::adjacentCornersVec() const {
	return {CubeCorner(!up, right, front),
			CubeCorner(up, !right, front),
			CubeCorner(up, right, !front)};
}

bool nbhd(CubeCorner a, CubeCorner b) {
	int diff = 0;
	if (a.up != b.up) diff++;
	if (a.right != b.right) diff++;
	if (a.front != b.front) diff++;
	return diff == 1;
}

bool opposite(CubeCorner a, CubeCorner b) {
	return a.up == !b.up && a.right == !b.right && a.front == !b.front;
}

bool coplanar(CubeCorner a, CubeCorner b, CubeCorner c) {
	return a.front == b.front && a.front == c.front ||
			a.right == b.right && a.right == c.right ||
			a.up == b.up && a.up == c.up;
}

bool coplanar(CubeCorner a, CubeCorner b, CubeCorner c, CubeCorner d) {
	return a.front == b.front && a.front == c.front && a.front == d.front ||
			a.right == b.right && a.right == c.right && a.right == d.right ||
			a.up == b.up && a.up == c.up && a.up == d.up;
}

ivec3 TetrahedronMarching::edgeCenter(int i, int j) const {
	return (vertex(i) + vertex(j)) / 2;
}

vector<ivec3> TetrahedronMarching::edgeCentersFromVertex(int i) const {
	vector<ivec3> result = {};
	for (int j = 0; j < 4; j++)
		if (j != i)
			result.push_back(edgeCenter(i, j));
	return result;
}

vector<vector<ivec3>> TetrahedronMarching::trianglesBetween(int i, int j) const
{
	vector<vector<ivec3>> result = {};
	vector<int> comp = setMinus<int>({0, 1, 2, 3}, {i, j});
	ivec3 p11 = edgeCenter(i, comp[0]);
	ivec3 p12 = edgeCenter(i, comp[1]);
	ivec3 p21 = edgeCenter(j, comp[0]);
	ivec3 p22 = edgeCenter(j, comp[1]);
	result.push_back({p11, p12, p21});
	result.push_back({p12, p21, p22});
	return result;
}

vector<ivec3> TetrahedronMarching::edgeCentersFromVertex(ivec3 v) const {
	for (int i = 0; i < 4; i++)
		if (vertex(i) == v)
			return edgeCentersFromVertex(i);
	throw std::logic_error("vertex not found");
}

vector<vector<ivec3>> TetrahedronMarching::trianglesBetween(ivec3 v1, ivec3 v2) const {
	for (int i = 0; i < 4; i++)
		if (vertex(i) == v1)
			for (int j = 0; j < 4; j++)
				if (vertex(j) == v2)
					return trianglesBetween(i, j);
	throw std::logic_error("vertices incompatible");
}


ivec3 TetrahedronMarching::vertex(int i) const {
	vector<ivec3> vertices = {p1, p2, p3, p4};
	return vertices[i];
}

vector<ivec3> TetrahedronMarching::complement(const vector<ivec3> &arr) const {
return setMinus<ivec3>({p1, p2, p3, p4}, arr);
}

std::array<std::array<CubeCorner, 2>, 3> CubeCorner::adjacentEdges() const {
	std::array<CubeCorner, 3> corners = adjacentCorners();
	return {std::array<CubeCorner, 2>{*this, corners[0]},
			std::array<CubeCorner, 2>{*this, corners[1]},
			std::array<CubeCorner, 2>{*this, corners[2]}};
}

MarchingCubeChunk::MarchingCubeChunk(const vec3 &corner0, const vec3 &corner1, const ivec3 &res, const std::shared_ptr<SmoothImplicitSurface> &surface)
	: id(randomID()), cornerLow(corner0), cornerHigh(corner1), res(res), surface(surface) {
	vertexStep = (corner1 - corner0) / (vec3(res)*4.0f);
}

vec3 MarchingCubeChunk::vertexPosition(ivec3 i) const {
	return vec3(cornerLow.x + i.x*vertexStep.x,
				cornerLow.y + i.y*vertexStep.y,
				cornerLow.z + i.z*vertexStep.z);
}

vec3 MarchingCubeChunk::cornerPosition(ivec3 cube, CubeCorner corner) const {
	return vertexPosition(cornerVertex(cube, corner));
}

vec3 MarchingCubeChunk::edgeCenter(ivec3 cube, CubeCorner corner1, CubeCorner corner2) const {
	return (cornerPosition(cube, corner1) + cornerPosition(cube, corner2)) / 2.0f;
}

ivec3 MarchingCubeChunk::cornerVertex(ivec3 cube, CubeCorner corner) {
	return (cube + ivec3(corner.up, corner.right, corner.front))*4;
}

ivec3 MarchingCubeChunk::edgeCenterVertex(ivec3 cube, CubeCorner corner1, CubeCorner corner2) {
	if (corner1.up != corner2.up && corner1.right != corner2.right)
		throw std::logic_error("not neighbours");
	if (corner1.up != corner2.up && corner1.front != corner2.front)
		throw std::logic_error("not neighbours");
	if (corner1.right != corner2.right && corner1.front != corner2.front)
		throw std::logic_error("not neighbours");

	if (corner1.front != corner2.front)
		return cube*4 + ivec3(corner1.up, corner1.right, 0)*4 + ivec3(0, 0, 2);
	if (corner1.right != corner2.right)
		return cube*4 + ivec3(corner1.up, 0, corner1.front)*4 + ivec3(0, 2, 0);
	return cube*4 + ivec3(0, corner1.right, corner1.front)*4 + ivec3(2, 0, 0);
}

void MarchingCubeChunk::generate() {
	if (!triangles.empty())
		return;
	for (int i = 0; i < res.x; i++)
		for (int j = 0; j < res.y; j++)
			for (int k = 0; k < res.z; k++)
				generateCube(ivec3(i, j, k));
}


bool MarchingCubeChunk::vertexAlreadyAdded(ivec3 vertex) const {
	return addedVertices.contains(hash_ivec3(vertex));
}

int MarchingCubeChunk::addVertex(ivec3 vertex) {
	if (vertexAlreadyAdded(vertex))
		return addedVertices[hash_ivec3(vertex)];
	addedVertices[hash_ivec3(vertex)] = vertices.size();
	vertices.push_back(vertex);
	return vertices.size() - 1;
}

bool MarchingCubeChunk::cornerAlreadyAdded(ivec3 box, CubeCorner corner) const {
	return vertexAlreadyAdded(cornerVertex(box, corner));
}

int MarchingCubeChunk::addCorner(ivec3 box, CubeCorner corner) {
	return addVertex(cornerVertex(box, corner));
}

bool MarchingCubeChunk::edgeCenterAlreadyAdded(ivec3 box, CubeCorner corner1, CubeCorner corner2) const {
	return vertexAlreadyAdded(edgeCenterVertex(box, corner1, corner2));
}
int MarchingCubeChunk::addEdgeCenter(ivec3 box, CubeCorner corner1, CubeCorner corner2) {
	return addVertex(edgeCenterVertex(box, corner1, corner2));
}

void MarchingCubeChunk::addTriangle(ivec3 a, ivec3 b, ivec3 c) {
	int ia = addVertex(a);
	int ib = addVertex(b);
	int ic = addVertex(c);
	triangles.push_back(ivec3(ia, ib, ic));
}

void MarchingCubeChunk::generateTetra() {
	for (int i = 0; i < res.x; i++)
		for (int j = 0; j < res.y; j++)
			for (int k = 0; k < res.z; k++)
				generateCubeTetra(ivec3(i, j, k));
}

void MarchingCubeChunk::generateCubeTetra(ivec3 ind) {
	for (auto tetra: subdivideCube(ind))
		generateTetrahedron(tetra);
}

void MarchingCubeChunk::generateTetrahedron(const TetrahedronMarching &tetra) {
	vector<ivec3> positive = {};
	for (ivec3 vertex: tetra.vertices())
		if (surface->operator()(vertexPosition(vertex)) > 0)
			positive.push_back(vertex);
	if (positive.size() > 2)
		positive = tetra.complement(positive);

	if (positive.empty())
		return;

	if (positive.size() == 1) {
		vector<ivec3> trVertices = tetra.edgeCentersFromVertex(positive[0]);
		addTriangle(trVertices[0], trVertices[1], trVertices[2]);
		return;
	}

	vector<vector<ivec3>> triangles = tetra.trianglesBetween(positive[0], positive[1]);
	addTriangle(triangles[0][0], triangles[0][1], triangles[0][2]);
	addTriangle(triangles[1][0], triangles[1][1], triangles[1][2]);

}


vector<TetrahedronMarching> MarchingCubeChunk::subdivideCubeFace(ivec3 center, ivec3 corner11, ivec3 corner12, ivec3 corner21, ivec3 corner22) const {
	ivec3 e1 = (corner11 + corner12) / 2;
	ivec3 e2 = (corner12 + corner22) / 2;
	ivec3 e3 = (corner22 + corner21) / 2;
	ivec3 e4 = (corner21 + corner11) / 2;
	ivec3 m = (corner11 + corner22) / 2;
	return {TetrahedronMarching(center, corner11, e1, e4),
			TetrahedronMarching(center, m, e1, e4),
			TetrahedronMarching(center, corner12, e1, e2),
			TetrahedronMarching(center, m, e1, e2),
			TetrahedronMarching(center, corner22, e2, e3),
			TetrahedronMarching(center, m, e2, e3),
			TetrahedronMarching(center, corner21, e3, e4),
			TetrahedronMarching(center, m, e3, e4)};
}

vector<TetrahedronMarching> MarchingCubeChunk::subdivideCube(ivec3 cube) const {
	ivec3 center = cube*4 + ivec3(2, 2, 2);
	ivec3 p0 = cube*4;
	vector<TetrahedronMarching> s1 = subdivideCubeFace(center, p0, p0 + ivec3(4, 0, 0), p0 + ivec3(0, 4, 0), p0 + ivec3(4, 4, 0));
	vector<TetrahedronMarching> s2 = subdivideCubeFace(center, p0, p0 + ivec3(0, 4, 0), p0 + ivec3(0, 0, 4), p0 + ivec3(0, 4, 4));
	vector<TetrahedronMarching> s3 = subdivideCubeFace(center, p0, p0 + ivec3(0, 0, 4), p0 + ivec3(4, 0, 0), p0 + ivec3(4, 0, 4));
	vector<TetrahedronMarching> s4 = subdivideCubeFace(center, p0 + ivec3(4, 0, 0), p0 + ivec3(4, 0, 4), p0 + ivec3(4, 4, 0), p0 + ivec3(4, 4, 4));
	vector<TetrahedronMarching> s5 = subdivideCubeFace(center, p0 + ivec3(0, 4, 0), p0 + ivec3(0, 4, 4), p0 + ivec3(4, 4, 0), p0 + ivec3(4, 4, 4));
	vector<TetrahedronMarching> s6 = subdivideCubeFace(center, p0 + ivec3(0, 0, 4), p0 + ivec3(0, 4, 4), p0 + ivec3(4, 0, 4), p0 + ivec3(4, 4, 4));
	s1.insert(s1.end(), s2.begin(), s2.end());
	s1.insert(s1.end(), s3.begin(), s3.end());
	s1.insert(s1.end(), s4.begin(), s4.end());
	s1.insert(s1.end(), s5.begin(), s5.end());
	s1.insert(s1.end(), s6.begin(), s6.end());
	return s1;
}


vector<PrimitiveCubeConfiguration> MarchingCubeChunk::configurations(ivec3 cube) const {
	auto diff = differingCorners(cube);
	int noDiff = diff.size();

	if (noDiff == 0)
		return {};
	if (noDiff == 1)
		return {PrimitiveCubeConfiguration{CORNER, differingCorners(cube)}};
	if (noDiff == 2) {
		if (nbhd(diff[0], diff[1]))
			return {PrimitiveCubeConfiguration{EDGE, diff}};
		return {PrimitiveCubeConfiguration{CORNER, {diff[0]}},
					PrimitiveCubeConfiguration{CORNER, {diff[1]}}};
	}
	if (noDiff == 3) {
		auto c1 = diff[0];
		auto c2 = diff[1];
		auto c3 = diff[2];
		if (faceDiag(c1, c2) && faceDiag(c2, c3) && faceDiag(c3, c1))
			return {PrimitiveCubeConfiguration{CORNER, {diff[0]}},
					PrimitiveCubeConfiguration{CORNER, {diff[1]}},
					PrimitiveCubeConfiguration{CORNER, {diff[2]}}};
		if (coplanar(c1, c2, c3))
			return {PrimitiveCubeConfiguration{TRIPLE, diff}};
		if (nbhd(c1, c2))
			return {PrimitiveCubeConfiguration{EDGE, {c1, c2}},
					PrimitiveCubeConfiguration{CORNER, {c3}}};
		if (nbhd(c2, c3))
			return {PrimitiveCubeConfiguration{EDGE, {c2, c3}},
					PrimitiveCubeConfiguration{CORNER, {c1}}};
		return {PrimitiveCubeConfiguration{EDGE, {c1, c3}},
				PrimitiveCubeConfiguration{CORNER, {c2}}};
	}

	auto c1 = diff[0];
	auto c2 = diff[1];
	auto c3 = diff[2];
	auto c4 = diff[3];

	if (faceDiag(c1, c2) && faceDiag(c2, c3) && faceDiag(c3, c4) && faceDiag(c4, c1))
		return {PrimitiveCubeConfiguration{CORNER, {diff[0]}},
				PrimitiveCubeConfiguration{CORNER, {diff[1]}},
				PrimitiveCubeConfiguration{CORNER, {diff[2]}},
				PrimitiveCubeConfiguration{CORNER, {diff[3]}},};

	if (coplanar(c1, c2, c3, c4)) {
		if (c1.front == c2.front && c1.front == c3.front && c1.front == c4.front)
			return {PrimitiveCubeConfiguration(CUBEFACE, {
			CubeCorner(false, false, c1.front),
			CubeCorner(false, true, c1.front),
			CubeCorner(true, true, c1.front),
			CubeCorner(true, false, c1.front)})};
		if (c1.right == c2.right && c1.right == c3.right && c1.right == c4.right)
			return {PrimitiveCubeConfiguration(CUBEFACE, {
			CubeCorner( false,c1.right,  false),
			CubeCorner( false,c1.right,  true),
			CubeCorner( true, c1.right, true),
			CubeCorner( true, c1.right, false)})};
		return {PrimitiveCubeConfiguration(CUBEFACE, {
			CubeCorner( c1.up, false, false),
			CubeCorner( c1.up, false, true),
			CubeCorner(c1.up,  true, true),
			CubeCorner(c1.up,  true, false)})};
	}

	for (auto c: diff) {
		auto comp = setMinus(diff, c);
		if (nbhd(c, comp[0]) && nbhd(c, comp[1]) && nbhd(c, comp[2]))
			return {PrimitiveCubeConfiguration{TETRA, {c, comp[0], comp[1], comp[2]}}};
		for (auto d: comp) {
			auto compcomp = setMinus(comp, d);
			if (nbhd(c, d) && nbhd(compcomp[1], compcomp[0])
				&& !nbhd(c, compcomp[0]) && !nbhd(c, compcomp[1])
				&& !nbhd(d, compcomp[0]) && !nbhd(d, compcomp[1]))
				return {PrimitiveCubeConfiguration{EDGE, {c, d}},
						PrimitiveCubeConfiguration{EDGE, {compcomp[0], compcomp[1]}}};
		}
	}
	for (auto c: diff) {
		auto comp = setMinus(diff, c);
		if (!nbhd(c, comp[0]) && !nbhd(c, comp[1]) && !nbhd(c, comp[2]))
			return {PrimitiveCubeConfiguration{CORNER, {c}},
					PrimitiveCubeConfiguration{TRIPLE, comp}};
	}
	for (auto c: diff) {
		auto comp = setMinus(diff, c);
		for (auto d: comp) {
			auto compcomp = setMinus(comp, d);
			if (nbhd(c, d) && nbhd(d, compcomp[0]) && nbhd(compcomp[0], compcomp[1]))
				return {PrimitiveCubeConfiguration{LONG, {c, d, compcomp[0], compcomp[1]}}};
			if (nbhd(c, d) && nbhd(d, compcomp[1]) && nbhd(compcomp[1], compcomp[0]))
				return {PrimitiveCubeConfiguration{LONG, {c, d, compcomp[1], compcomp[0]}}};
		}
	}
	return {};
}

void MarchingCubeChunk::generatePrimitiveConfiguration(const PrimitiveCubeConfiguration &config, ivec3 cube) {
	if (config.type == CORNER) {
//		return;
		auto v = config.corners[0];
		auto adjacents = v.adjacentCornersVec();
		int a = addEdgeCenter(cube, v, adjacents[0]);
		int b = addEdgeCenter(cube, v, adjacents[1]);
		int c = addEdgeCenter(cube, v, adjacents[2]);
		triangles.push_back(ivec3(a, b, c));
	}
	if (config.type == EDGE) {
//		return;
		auto c1 = config.corners[0];
		auto c2 = config.corners[1];

		CubeCorner a1 = CubeCorner(0);
		CubeCorner a2 = CubeCorner(0);
		CubeCorner b1 = CubeCorner(0);
		CubeCorner b2 = CubeCorner(0);

		if (c1.up != c2.up) {
			a1 = CubeCorner(c1.up, c1.right, !c1.front);
			b1 = CubeCorner(c1.up, !c1.right, c1.front);
			a2 = CubeCorner(c2.up, c1.right, !c1.front);
			b2 = CubeCorner(c2.up, !c1.right, c1.front);
		}
		else if (c1.right != c2.right) {
			a1 = CubeCorner(c1.up, c1.right, !c1.front);
			b1 = CubeCorner(!c1.up, c1.right, c1.front);
			a2 = CubeCorner(c1.up, c2.right, !c1.front);
			b2 = CubeCorner(!c1.up, c2.right, c1.front);
		}
		else {
			a1 = CubeCorner(c1.up, !c1.right, c1.front);
			b1 = CubeCorner(!c1.up, c1.right, c1.front);
			a2 = CubeCorner(c1.up, !c1.right, c2.front);
			b2 = CubeCorner(!c1.up, c1.right, c2.front);
		}


		int a = addEdgeCenter(cube, c1, a1);
		int b = addEdgeCenter(cube, c1, b1);
		int c = addEdgeCenter(cube, c2, b2);
		int d = addEdgeCenter(cube, c2, a2);
		triangles.push_back(ivec3(a, b, c));
		triangles.push_back(ivec3(a, c, d));
	}
	if (config.type == TRIPLE) {
//		return;
		auto c1 = config.corners[0];
		auto c2 = config.corners[1];
		auto c3 = config.corners[2];

		if (nbhd(c2, c1) && nbhd(c2, c3)) {
			c1 = config.corners[1];
			c2 = config.corners[0];
			c3 = config.corners[2];
		}

		if (nbhd(c3, c1) && nbhd(c2, c3)) {
			c1 = config.corners[2];
			c2 = config.corners[0];
			c3 = config.corners[1];
		}

		CubeCorner d1 = CubeCorner(0);
		for (auto c: complementaryCorners({c1, c2, c3}))
			if (coplanar(c1, c2, c3, c))
				d1 = c;

		auto d2 = setMinus(c2.adjacentCornersVec(), {d1, c1})[0];
		auto d3 = setMinus(c3.adjacentCornersVec(), {d1, c1})[0];
		auto d4 = setMinus(c1.adjacentCornersVec(), {c2, c3})[0];

		int a2 = addEdgeCenter(cube, d1, c2);
		int a3 = addEdgeCenter(cube, d1, c3);
		int b2 = addEdgeCenter(cube, c2, d2);
		int b3 = addEdgeCenter(cube, c3, d3);
		int c = addEdgeCenter(cube, c1, d4);
		triangles.push_back(ivec3(a2, a3, b3));
		triangles.push_back(ivec3(a2, b3, b2));
		triangles.push_back(ivec3(b2, b3, c));
	}
	if (config.type == CUBEFACE) {
		auto c1 = config.corners[0];
		auto c2 = config.corners[1];
		auto c3 = config.corners[2];
		auto c4 = config.corners[3];

		auto d1 = setMinus(c1.adjacentCornersVec(), config.corners)[0];
		auto d2 = setMinus(c2.adjacentCornersVec(), config.corners)[0];
		auto d3 = setMinus(c3.adjacentCornersVec(), config.corners)[0];
		auto d4 = setMinus(c4.adjacentCornersVec(), config.corners)[0];

		int a = addEdgeCenter(cube, c1, d1);
		int b = addEdgeCenter(cube, c2, d2);
		int c = addEdgeCenter(cube, c3, d3);
		int d = addEdgeCenter(cube, c4, d4);

		triangles.push_back(ivec3(a, b, c));
		triangles.push_back(ivec3(a, c, d));
	}
	if (config.type == TETRA) {
//		return;
		auto c = config.corners[0];
		auto c1 = config.corners[1];
		auto c2 = config.corners[2];
		auto c3 = config.corners[3];

		CubeCorner d12 = CubeCorner(0);
		CubeCorner d13 = CubeCorner(0);
		CubeCorner d23 = CubeCorner(0);

		for (auto d: complementaryCorners(config.corners)) {
			if (coplanar(c, c1, c2, d))
				d12 = d;
			if (coplanar(c, c1, c3, d))
				d13 = d;
			if (coplanar(c, c2, c3, d))
				d23 = d;
		}

		int a1 = addEdgeCenter(cube, c1, d13);
		int a2 = addEdgeCenter(cube, c1, d12);
		int a3 = addEdgeCenter(cube, c2, d12);
		int a4 = addEdgeCenter(cube, c2, d23);
		int a5 = addEdgeCenter(cube, c3, d23);
		int a6 = addEdgeCenter(cube, c3, d13);

		triangles.push_back(ivec3(a1, a2, a3));
		triangles.push_back(ivec3(a1, a3, a4));
		triangles.push_back(ivec3(a1, a4, a6));
		triangles.push_back(ivec3(a4, a5, a6));
	}

	if (config.type == LONG) {
//		return;
		auto c1 = config.corners[0];
		auto c2 = config.corners[1];
		auto c3 = config.corners[2];
		auto c4 = config.corners[3];

		CubeCorner f1 = CubeCorner(0);
		CubeCorner f2 = CubeCorner(0);

		for (auto c: complementaryCorners(config.corners)) {
			if (coplanar(c1, c2, c3, c))
				f1 = c;
			if (coplanar(c2, c3, c4, c))
				f2 = c;
		}

		CubeCorner u1 = setMinus(c1.adjacentCornersVec(), {c2, f1})[0];
		CubeCorner u2 = setMinus(c4.adjacentCornersVec(), {c3, f2})[0];

		int a1 = addEdgeCenter(cube, c1, f1);
		int a2 = addEdgeCenter(cube, c1, u1);
		int a3 = addEdgeCenter(cube, c4, f2);
		int a4 = addEdgeCenter(cube, c4, u2);
		int a5 = addEdgeCenter(cube, c3, f1);

		triangles.push_back(ivec3(a1, a2, a5));
		triangles.push_back(ivec3(a1, a3, a5));
		triangles.push_back(ivec3(a3, a4, a5));
	}
}


void MarchingCubeChunk::generateCube(ivec3 ind) {
	for (PrimitiveCubeConfiguration config: configurations(ind))
		generatePrimitiveConfiguration(config, ind);
}

vector<CubeCorner> MarchingCubeChunk::differingCorners(ivec3 cube) const {
	vector<CubeCorner> positiveCorners = {};
	for (int i = 0; i < 8; i++) {
		CubeCorner c = CubeCorner(i);
		if (cornerSign(cube, c))
			positiveCorners.push_back(c);
	}
	if (positiveCorners.size() > 4)
		return complementaryCorners(positiveCorners);
	return positiveCorners;
}

bool MarchingCubeChunk::cornerSign(ivec3 cube, CubeCorner corner) const {
	return surface->operator()(cornerPosition(cube, corner)) > 0;
}

int MarchingCubeChunk::noDifferentCorners(ivec3 cube) const {
	return differingCorners(cube).size();
}
vector<CubeCorner> MarchingCubeChunk::allCorners() {
	return {CubeCorner(0), CubeCorner(1), CubeCorner(2), CubeCorner(3), CubeCorner(4), CubeCorner(5), CubeCorner(6), CubeCorner(7)};
}

vector<CubeCorner> MarchingCubeChunk::complementaryCorners(const vector<CubeCorner> &corners) {
	return setMinus(allCorners(), corners);
}


IndexedMesh MarchingCubeChunk::generateMesh(bool tetra) {
	vertices = {};
	triangles = {};
	addedVertices = {};

	IndexedMesh mesh = IndexedMesh();
	if (tetra)
		generateTetra();
	else
		generate();

	addToMesh(mesh);
	return mesh;
}

IndexedMesh MarchingCubeChunk::generateMesh(bool tetra, float tolerance, int maxIter) {
	vertices = {};
	triangles = {};
	addedVertices = {};

	IndexedMesh mesh = IndexedMesh();
	if (tetra)
		generateTetra();
	else
		generate();

	addToMesh(mesh, tolerance, maxIter);
	return mesh;
}


void MarchingCubeChunk::addToMesh(IndexedMesh &mesh) {
	vector<Vertex> vertices_hard = {};
	for (ivec3 v: this->vertices)
		vertices_hard.emplace_back(vertexPosition(v), vec2(1.f*v.x/res.x, 1.f*v.y/res.y), surface->normal(vertexPosition(v)));
	mesh.addNewPolygroup(vertices_hard, triangles, id);
}

void MarchingCubeChunk::addToMesh(IndexedMesh &mesh,  float tolerance, int maxIter){
	vector<Vertex> vertices_hard = {};
	for (ivec3 v: this->vertices) {
		vec3 p = surface->newtonStepProject(vertexPosition(v), tolerance, maxIter);
		vertices_hard.emplace_back(p, vec2(1.f*v.x/res.x, 1.f*v.y/res.y), surface->normal(p));
	}
		mesh.addNewPolygroup(vertices_hard, triangles, id);
}


AffinePlane SmoothParametricCurve::osculatingPlane(float t) const {
	return AffinePlane(binormal(t), dot(binormal(t), _f(t)));
}
