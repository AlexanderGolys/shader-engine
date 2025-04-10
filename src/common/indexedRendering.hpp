#pragma once

#include "renderingUtils.hpp"
// #include "../geometry/pde.hpp"

#include <set>




struct buff4x4 {
    vector<vec4> a, b, c, d;
};

inline mat4 getMat(const buff4x4 &buff, int index) {
  return mat4(buff.a[index], buff.b[index], buff.c[index], buff.d[index]);
}


struct Stds {
    BUFF3 positions;
    BUFF3 normals;
    BUFF2 uvs;
    BUFF4 colors;
};

enum CommonBufferType {
  POSITION, NORMAL, UV, COLOR, MATERIAL1, MATERIAL2, MATERIAL3, MATERIAL4, INDEX, EXTRA0, EXTRA1, EXTRA2, EXTRA3, EXTRA4
};
const std::map<CommonBufferType, int> bufferTypeLength = {{POSITION, 3},  {NORMAL, 3},    {UV, 2},        {COLOR, 4},
                                                          {MATERIAL1, 4}, {MATERIAL2, 4}, {MATERIAL3, 4}, {MATERIAL4, 4},
                                                          {INDEX, 3},  {EXTRA0, 4},   {EXTRA1, 4},    {EXTRA2, 4}, {EXTRA3, 4}, {EXTRA4, 4}};

inline int bufferElementLength(CommonBufferType type) { return bufferTypeLength.at(type); }
inline size_t bufferElementSize(CommonBufferType type) { return type != INDEX ? bufferElementLength(type) * sizeof(FLOAT) : bufferElementLength(type) * sizeof(unsigned int); }


class BufferManager {
    std::unique_ptr<Stds> stds;
    std::unique_ptr<BUFF4> extra0;
    std::unique_ptr<buff4x4> extra;
    std::unique_ptr<IBUFF3> indices;
    std::set<CommonBufferType> activeBuffers;
    void insertValueToSingleBuffer(CommonBufferType type, void *valueAddress);
    void insertDefaultValueToSingleBuffer(CommonBufferType type);


public:
    BufferManager(const BufferManager &other);
    BufferManager & operator=(const BufferManager &other);


    BufferManager(BufferManager &&other) noexcept;
    BufferManager &operator=(BufferManager &&other) noexcept;
    explicit BufferManager(const std::set<CommonBufferType> &activeBuffers = {POSITION, NORMAL, UV, COLOR, INDEX});
    explicit BufferManager(bool materials, const std::set<CommonBufferType> &extras = {} );

    int bufferLength(CommonBufferType type) const;
    size_t bufferSize(CommonBufferType type) const { return bufferLength(type) * bufferElementSize(type); }
    void *firstElementAddress(CommonBufferType type) const;
    bool isActive(CommonBufferType type) const { return activeBuffers.contains(type); }
    bool hasMaterial() const { return isActive(MATERIAL1); }

    int addTriangleVertexIndices(ivec3 ind, int shift = 0);

    int addStdAttributesFromVertex(vec3 pos, vec3 norm, vec2 uv, vec4 col);
    int addMaterialBufferData(const MaterialPhong &mat);
    int addFullVertexData(vec3 pos, vec3 norm, vec2 uv, vec4 col);
    int addFullVertexData(const Vertex &v);
	int addFullVertexData(vec3 pos, vec3 norm, vec2 uv, vec4 col, vec4 extra0Data);
	int addAttributesFromVertex(vec3 pos, vec3 norm, vec2 uv, vec4 col, vec4 extra0Data);
	int addFullVertexData(const Vertex &v, vec4 extra0Data);

    void reserveSpace(int targetSize);
    void reserveSpaceForIndex(int targetSize) { indices->reserve(targetSize); }
    void reserveAdditionalSpace(int extraStorage) { reserveSpace(bufferLength(POSITION) + extraStorage); }
    void reserveAdditionalSpaceForIndex(int extraStorage) { reserveSpaceForIndex(bufferLength(INDEX) + extraStorage); }
    void initialiseExtraBufferSlot(int slot);

    vec3 getPosition(int index) const { return stds->positions[index]; }
    vec3 getNormal(int index) const { return stds->normals[index]; }
    vec2 getUV(int index) const { return stds->uvs[index]; }
    vec4 getColor(int index) const { return stds->colors[index]; }
    vec4 getExtra(int index, int slot = 1) const;
    float getExtraSlot(int index, int slot = 1, int component = 3) const { return getExtra(index, slot)[component]; }
    ivec3 getFaceIndices(int index) const { return (*indices)[index]; }
    Vertex getVertex(int index) const { return Vertex(getPosition(index), getUV(index), getNormal(index), getColor(index)); }

    void setPosition(int index, vec3 value) { stds->positions[index] = value; }
    void setNormal(int index, vec3 value) { stds->normals[index] = value; }
    void setUV(int index, vec2 value) { stds->uvs[index] = value; }
    void setColor(int index, vec4 value) { stds->colors[index] = value; }
    void setColor(int index, float value, int component) { stds->colors[index][component] = value; }
    void setMaterial(int index, mat4 value);
	void setFaceIndices(int index, const ivec3 &in) { indices->at(index) = in; }


    void setExtra(int index, vec4 value, int slot = 1);
    void setExtra(int index, vec3 value, int slot = 1);
    void setExtra(int index, float value, int slot = 1, int component = 3);
	bool hasExtra0() const {return activeBuffers.contains(EXTRA0); }
	bool hasExtra1() const {return activeBuffers.contains(EXTRA1); }
	bool hasExtra2() const {return activeBuffers.contains(EXTRA2); }
	bool hasExtra3() const {return activeBuffers.contains(EXTRA3); }
	bool hasExtra4() const {return activeBuffers.contains(EXTRA4); }
};





class BufferedVertex {
  BufferManager &bufferBoss;
  int index;
public:
  BufferedVertex(BufferManager &bufferBoss, int index) : bufferBoss(bufferBoss), index(index) {}
  BufferedVertex(const BufferedVertex &other) = default;
  BufferedVertex(BufferedVertex &&other) noexcept : bufferBoss(other.bufferBoss), index(other.index) {}
  BufferedVertex(BufferManager &bufferBoss, const Vertex &v);

  int getIndex() const { return index; }
  vec3 getPosition() const { return bufferBoss.getPosition(index); }
  vec3 getNormal() const { return bufferBoss.getNormal(index); }
  vec2 getUV() const { return bufferBoss.getUV(index); }
  vec4 getColor() const { return bufferBoss.getColor(index); }
  vec4 getExtra(int slot = 1) const { return bufferBoss.getExtra(index, slot); }
  Vertex getVertex() const { return Vertex(getPosition(), getUV(), getNormal(), getColor()); }
	vec2 getSurfaceParams() const {return vec2(getColor());}

  void setPosition(vec3 value) { bufferBoss.setPosition(index, value); }
  void setNormal(vec3 value) { bufferBoss.setNormal(index, value); }
  void setUV(vec2 value) { bufferBoss.setUV(index, value); }
  void setColor(vec4 value) { bufferBoss.setColor(index, value); }
  void setColor(float value, int i) { bufferBoss.setColor(index, value, i); }
  void setMaterial(const mat4 &value) { bufferBoss.setMaterial(index, value); }
  void setExtra(vec4 value, int slot = 1) { bufferBoss.setExtra(index, value, slot); }
  void setExtra(vec3 value, int slot = 1) { bufferBoss.setExtra(index, value, slot); }
  void setExtra(float value, int slot = 1, int component = 3) { bufferBoss.setExtra(index, value, slot, component); }
  void applyFunction(const SpaceEndomorphism &f);
  void setVertex(const Vertex &v) { setPosition(v.getPosition()); setUV(v.getUV()); setNormal(v.getNormal()); setColor(v.getColor()); }

};

class WeakSuperMesh;





class IndexedTriangleDeprecated {
  int metaIndex;
  ivec3 indicesWithinPolygroup;

public:
  explicit IndexedTriangleDeprecated(ivec3 indices);
  IndexedTriangleDeprecated(ivec3 indices, const vector<BufferedVertex> &arrayWithinPoly, BufferManager &bufferBoss);


  ivec3 bufferIndices (const vector<BufferedVertex> &arrayWithinPoly) const;
  void addToBuffer(const vector<BufferedVertex> &arrayWithinPoly, BufferManager &bufferBoss);
  ivec3 getLocalIndices() const { return indicesWithinPolygroup; }
  mat2 barMatrix(const vector<BufferedVertex> &arrayWithinPoly) const;
  mat3 orthonormalFrame(const vector<BufferedVertex> &arrayWithinPoly) const;
  vec3 fromPlanar(vec2, const vector<BufferedVertex> &arrayWithinPoly) const;
  vec2 toPlanar(vec3, const vector<BufferedVertex> &arrayWithinPoly) const;
  vec3 fromBars(vec2, const vector<BufferedVertex> &arrayWithinPoly) const;
  vec2 toBars(vec3, const vector<BufferedVertex> &arrayWithinPoly) const;
  std::array<vec3, 3> borderTriangle(float width, const vector<BufferedVertex> &arrayWithinPoly) const;
  Vertex getVertex(int i, const vector<BufferedVertex> &arrayWithinPoly) const { return arrayWithinPoly[indicesWithinPolygroup[i]].getVertex(); }
  vec3 faceNormal(const vector<BufferedVertex> &arrayWithinPoly) const { return normalize(cross(getVertex(1, arrayWithinPoly).getPosition() - getVertex(0, arrayWithinPoly).getPosition(), getVertex(2, arrayWithinPoly).getPosition() - getVertex(0, arrayWithinPoly).getPosition())); }
};



class IndexedTriangle {
    int index;
    BufferManager &bufferBoss;

public:
//    IndexedTriangle(BufferManager &bufferBoss, int index) : index(index), bufferBoss(bufferBoss) {}
    IndexedTriangle(const IndexedTriangle &other) : index(other.index), bufferBoss(other.bufferBoss) {}
    IndexedTriangle(IndexedTriangle &&other) noexcept : index(other.index), bufferBoss(other.bufferBoss) {}
    IndexedTriangle(BufferManager &bufferBoss, ivec3 index, int shift) : index(bufferBoss.addTriangleVertexIndices(index, shift)), bufferBoss(bufferBoss) {}

    ivec3 getVertexIndices() const { return bufferBoss.getFaceIndices(index); }
    Vertex getVertex(int i) const { return bufferBoss.getVertex(getVertexIndices()[i]); }
    mat2 barMatrix() const;
    mat3 orthonormalFrame() const;
    vec3 fromPlanar(vec2 v) const;
    vec2 toPlanar(vec3 v) const;
    vec3 fromBars(vec2 v) const;
    vec2 toBars(vec3 v) const;
    std::array<vec3, 3> borderTriangle(float width) const;
    vec3 faceNormal() const { return normalize(cross(getVertex(1).getPosition() - getVertex(0).getPosition(), getVertex(2).getPosition() - getVertex(0).getPosition())); }
    vec3 center() const { return (getVertex(0).getPosition() + getVertex(1).getPosition() + getVertex(2).getPosition()) / 3.f; }
    float area() const { return 0.5f * length(cross(getVertex(1).getPosition() - getVertex(0).getPosition(), getVertex(2).getPosition() - getVertex(0).getPosition())); }

	bool containsEdge(int i, int j) const;
	bool containsEdge(ivec2 edge) const { return containsEdge(edge.x, edge.y); }
	void setVertexIndices(const ivec3 &in) { bufferBoss.setFaceIndices(index, in); }
	void changeOrientation() { bufferBoss.setFaceIndices(index, ivec3(getVertexIndices().z, getVertexIndices().y, getVertexIndices().x)); }
};



class SmoothParametricSurface;

class WeakSuperMesh {
protected:
  std::unique_ptr<BufferManager> boss;
  std::map<PolyGroupID, vector<BufferedVertex>> vertices = {};
  std::map<PolyGroupID, vector<IndexedTriangle>> triangles = {};
  std::shared_ptr<MaterialPhong> material = nullptr;

public:
  virtual ~WeakSuperMesh() = default;

  WeakSuperMesh();
  WeakSuperMesh(const vector<Vertex> &hardVertices, const vector<ivec3> &faceIndices, const PolyGroupID &id);
  WeakSuperMesh(const char* filename, const PolyGroupID &id);

  void addNewPolygroup(const vector<Vertex> &hardVertices, const vector<ivec3> &faceIndices, const PolyGroupID &id);
  void addNewPolygroup(const vector<Vertex> &hardVertices, const vector<ivec3> &faceIndices, const PolyGroupID &id, const vector<vec4> &extra0);
  void addNewPolygroup(const vector<Vertex> &hardVertices, const vector<ivec3> &faceIndices, const PolyGroupID &id, const vector<vec4> &extra0, const vector<mat4> &extra);
  void addNewPolygroup(const char* filename, const PolyGroupID &id);


  WeakSuperMesh& operator=(const WeakSuperMesh &other);
  WeakSuperMesh(WeakSuperMesh &&other) noexcept;
  WeakSuperMesh& operator=(WeakSuperMesh &&other) noexcept;
  WeakSuperMesh(const WeakSuperMesh &other);


  WeakSuperMesh(const SmoothParametricSurface &surf, int tRes, int uRes, const PolyGroupID &id=randomID());
  void addUniformSurface(const SmoothParametricSurface &surf, int tRes, int uRes, const PolyGroupID &id=randomID());
  void merge (const WeakSuperMesh &other);
  void mergeAndKeepID(const WeakSuperMesh &other);
  void copyPolygroup(const WeakSuperMesh &other, const PolyGroupID &id, const PolyGroupID &newId);
	void copyPolygroup(const PolyGroupID &id, const PolyGroupID &newId);

  bool hasExtra0() const;
  bool hasExtra1() const;
  bool hasExtra2() const;
  bool hasExtra3() const;
  bool hasExtra4() const;
  bool hasExtra(int slot) const;

  void* bufferIndexLocation() const;
  size_t bufferIndexSize() const;
  int bufferIndexLength() const;
  const void* getBufferLocation(CommonBufferType type) const;
  unsigned int getBufferLength(CommonBufferType type) const;
  size_t getBufferSize(CommonBufferType type) const;
  vector<PolyGroupID> getPolyGroupIDs() const;
  BufferManager& getBufferBoss() const;
  bool isActive(CommonBufferType type) const;
  bool hasGlobalTextures() const;
  BufferedVertex& getAnyVertexFromPolyGroup(const PolyGroupID &id);

  void deformPerVertex(const PolyGroupID &id, const std::function<void(BufferedVertex&)> &deformation);
  void deformPerVertex(const std::function<void(BufferedVertex&)> &deformation);
  void deformPerVertex(const PolyGroupID &id, const std::function<void(int, BufferedVertex&)> &deformation);
  void deformPerId(const std::function<void(BufferedVertex&, PolyGroupID)> &deformation);

  static vec2 getSurfaceParameters(const BufferedVertex &v);
  void encodeSurfacePoint(BufferedVertex &v, const SmoothParametricSurface &surf, vec2 tu);
  void adjustToNewSurface(const SmoothParametricSurface &surf, const PolyGroupID &id);
  void adjustToNewSurface(const SmoothParametricSurface &surf);

  void moveAlongVectorField(const PolyGroupID &id, VectorField X, float delta=1);
  void deformWithAmbientMap(const PolyGroupID &id, SpaceEndomorphism f);
  void deformWithAmbientMap(const SpaceEndomorphism &f);
  void initGlobalTextures() const;

  void affineTransform(const mat3 &M, vec3 v, const PolyGroupID &id);
  void affineTransform(const mat3 &M, vec3 v);
  void shift(vec3 v, const PolyGroupID &id);
  void shift(vec3 v);
  void scale(float s, const PolyGroupID &id);
  void scale(float s);
  void addGlobalMaterial(const MaterialPhong &mat);

  void flipNormals(const PolyGroupID &id);
  void flipNormals() { for (auto &name: getPolyGroupIDs()) flipNormals(name); }
  void pointNormalsInDirection(vec3 dir, const PolyGroupID &id);
  void pointNormalsInDirection(vec3 dir);


  WeakSuperMesh subdivideBarycentric(const PolyGroupID &id) const;
  WeakSuperMesh subdivideEdgecentric(const PolyGroupID &id) const;
  WeakSuperMesh wireframe(PolyGroupID id, PolyGroupID targetId, float width, float heightCenter, float heightSide) const;

  vector<Vertex> getVertices(const PolyGroupID &id) const;
  vector<BufferedVertex> getBufferedVertices(const PolyGroupID &id) const;
  vector<ivec3> getIndices(const PolyGroupID &id) const;
  vector<IndexedTriangle> getTriangles(const PolyGroupID &id) const;
  vec4 getIntencities() const;
  MaterialPhong getMaterial() const;

  vector<int> findVertexNeighbours(int i, const PolyGroupID &id) const;
  vector<int> findVertexParentTriangles(int i, const PolyGroupID &id) const;
  void recalculateNormal(int i, const PolyGroupID &id);
  void recalculateNormalsNearby(int i, const PolyGroupID &id);
  void recalculateNormals(const PolyGroupID &id);
  void recalculateNormals();
  void orientFaces(const PolyGroupID &id);
  void orientFaces();


  vector<int> findNeighboursSorted(int i, const PolyGroupID &id) const;
  bool checkIfHasCompleteNeighbourhood(int i, const PolyGroupID &id) const;
  float meanCurvature(int i, const PolyGroupID &id) const;
  vec3 meanCurvatureVector(int i, const PolyGroupID &id) const;
  void meanCurvatureFlowDeform(float dt, const PolyGroupID &id);
  float GaussCurvature(int i, const PolyGroupID &id) const;
  void paintMeanCurvature(const PolyGroupID &id);
  void paintMeanCurvature();

  template<typename T>
  T integrateOverTriangles(const std::function<T(const IndexedTriangle &)> &f, PolyGroupID id) const;

  vec3 centerOfMass(PolyGroupID id) const;
  vec3 centerOfMass() const;

  mat3 inertiaTensorCMAppBd(PolyGroupID id) const;
  mat3 inertiaTensorAppBd(PolyGroupID id, vec3 p) const;
};


WeakSuperMesh _wireframe(const SmoothParametricSurface &s, float width, int n, int m, int curve_res_rad, int curve_res_hor);


std::function<void(float, float)> deformationOperator (const std::function<void(BufferedVertex&, float, float)> &deformation, WeakSuperMesh &mesh, const PolyGroupID &id);
std::function<void(float)> deformationOperator (const std::function<void(BufferedVertex&, float)> &deformation, WeakSuperMesh &mesh, const PolyGroupID &id);
std::function<void(float, float)> moveAlongCurve(const SmoothParametricCurve &curve, WeakSuperMesh &mesh, const PolyGroupID &id);

template<typename T>
T WeakSuperMesh::integrateOverTriangles(const std::function<T(const IndexedTriangle &)> &f, PolyGroupID id) const {
    T sum = T(0);
    for (const IndexedTriangle &t: triangles.at(id))
        sum += f(t)*t.area();
    return sum;
}


class Wireframe : public WeakSuperMesh {
	SmoothParametricSurface surf;
	float width;
	int n, m, curve_res_rad, curve_res_hor;
public:
	Wireframe(const SmoothParametricSurface &surf, float width, int n, int m, int curve_res_rad, int curve_res_hor);
	void changeBaseSurface(const SmoothParametricSurface &newsurf);
	vec2 getSurfaceParameters(const BufferedVertex &v) const;
};



class PlanarFlowLines : public WeakSuperMesh {
	VectorFieldR2 X;
	float dt;
	int steps;
	std::function<float(float, float, float, vec2, vec2)> width; //w(t, t0, speed, x, x0)
	std::function<vec4(float, float, float, vec2, vec2)> color;;
	vector<vec2> startPoints;
	vector<float> startTimes;
	vector<PolyGroupID> ids;

	// uv = (t, w)
	// pos = (x, y, 0)
	// n = (0, 0, 1)
	// col = (color, speed)
	// extra1 = (t0, x0, y0, len)

public:
	PlanarFlowLines(const VectorFieldR2 &X, float dt, int steps, const std::function<float(float, float, float, vec2, vec2)> &width, const std::function<vec4(float, float, float, vec2, vec2)> &color);
	void generateGrid(vec2 v_min, vec2 v_max, ivec2 res);
	void generateRandomUniform(vec2 v_min, vec2 v_max, int n);
	void generateStartTimesAll0();
	void generateStartTimesUniform(float t_max);
	void generateLine(int i);
	void generateLines();

	static float getTimeRelative(const BufferedVertex &v);
	static float getT0(const BufferedVertex &v);
	static float getTimeAbsolute(const BufferedVertex &v);
	static vec2 getPos(const BufferedVertex &v);
	static vec2 getStartPoint(const BufferedVertex &v);
	static float getSpeed(const BufferedVertex &v);
	static float getLength(const BufferedVertex &v);
	static vec4 getColor(const BufferedVertex &v);
	static float getWidth(const BufferedVertex &v);
};



class PlanarDiffusedInterval : public WeakSuperMesh {
	VectorFieldR2 X;
	float dt;
	int steps;
	vec4 color;
	HOM(float, float) time_dump;
	HOM(float, float) width_ratio_dump;
	vec2 a, b;
	float t0;
	PolyGroupID id = randomID();

	// uv = (t, t0)
	// pos = (x, y, 0)
	// n = (0, 0, 1)
	// col = (color)

public:
	PlanarDiffusedInterval(const VectorFieldR2 &X, float dt, int steps, vec4 color, const HOM(float, float) &time_dump, const HOM(float, float) &width_ratio_dump, vec2 a, vec2 b, float t0);
};


class PlanarDiffusedCurve : public WeakSuperMesh {
	VectorFieldR2 X;
	float dt;
	int steps;
	vec4 color;
	HOM(float, float) time_dump;
	HOM(float, float) width_ratio_dump;
	SmoothParametricPlaneCurve curve;
	vec2 domain;
	int resolution;
	float t0;

	// uv = (t, t0)
	// pos = (x, y, 0)
	// n = (0, 0, 1)
	// col = (color)

public:
	PlanarDiffusedCurve(const VectorFieldR2 &X, float dt, int steps, vec4 color, const HOM(float, float) &time_dump, const HOM(float, float) &width_ratio_dump, const SmoothParametricPlaneCurve &curve, vec2 domain, int resolution, float t0);
};


class PlanarDiffusedPatterns : public WeakSuperMesh {
	// uv = (t, t0)
	// pos = (x, y, 0)
	// n = (0, 0, 1)
	// col = (color)

public:
	PlanarDiffusedPatterns(const VectorFieldR2 &X, float dt, int steps,
		const vector<vec4> &colors, const vector<vec2> &shifts,
		const HOM(float, float) &time_dump, const HOM(float, float) &width_ratio_dump,
		const SmoothParametricPlaneCurve &curve_pattern, vec2 domain, int resolution, float t0);
};


class PipeCurveVertexShader : public WeakSuperMesh {
	float r;
public:
	PipeCurveVertexShader(const SmoothParametricCurve &curve, float r, int horRes, int radialRes);
	PipeCurveVertexShader(const RealFunction &plot, vec2 dom, float r, int horRes, int radialRes);
	PipeCurveVertexShader(const DiscreteRealFunction &plot, float r, int radialRes);

	void updateCurve(const SmoothParametricCurve &curve);
	void updateCurve(const DiscreteRealFunction &plot);

	void updateRadius(const HOM(float, float) &r);
};

class SurfacePlotDiscretisedMesh : public WeakSuperMesh {
public:
	explicit SurfacePlotDiscretisedMesh(const DiscreteRealFunctionR2 &plot);
};









// uv = (t, t0)
	// pos = (x, y, 0)
	// n = (0, 0, 1)
	// col = (color)
