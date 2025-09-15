#pragma once
#include "../geometry/smoothImplicit.hpp"
#include "../utils/macros.hpp"

#include <array>
#include <map>
#include <variant>
#include <vector>
#include <string>
// #include <glm/glm.hpp>

#include <memory>
#include <optional>

#include <GL/glew.h>




enum GLSLType {
	FLOAT,
	INT,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4,
	SAMPLER1D,
	SAMPLER2D,
	SAMPLER3D
};

// TODO: check if textures are not copied
class Texture {
public:
	int width, height;
	unsigned char* data;
	int bpp;
	GLuint textureID;
	GLenum textureSlot;
	int abs_slot;
	const char* samplerName;
	GLuint frameBufferID;

	Texture(int width, int height, int bpp, int slot, const char* sampler);
    explicit Texture(vec3 color, int slot, const char* sampler);
    explicit Texture(vec4 color, int slot, const char* sampler);
	explicit Texture(const char* filename, int slot, const char* sampler, bool alpha=false);
	explicit Texture(const string& filename, int slot, const char* sampler, bool alpha=false);

	void deleteTexture();
	~Texture();

	static void addFilters(GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT);
	void bind() const;
	void bindToFrameBuffer() const;
	void calculateMipmap() const;
    void load();

};


class MaterialPhong {
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float shininess;

public:
	shared_ptr<Texture> texture_ambient;
	shared_ptr<Texture> texture_diffuse;
	shared_ptr<Texture> texture_specular;

	virtual ~MaterialPhong() = default;

	MaterialPhong();

	static shared_ptr<Texture> constAmbientTexture(vec4 color);
	static shared_ptr<Texture> constDiffuseTexture(vec4 color);
	static shared_ptr<Texture> constSpecularTexture(vec4 color);
	static shared_ptr<Texture> constAmbientTexture(vec3 color);
	static shared_ptr<Texture> constDiffuseTexture(vec3 color);
	static shared_ptr<Texture> constSpecularTexture(vec3 color);
	static shared_ptr<Texture> makeAmbientTexture(const string &filename, bool alpha=false);
	static shared_ptr<Texture> makeDiffuseTexture(const string &filename, bool alpha=false);
	static shared_ptr<Texture> makeSpecularTexture(const string &filename, bool alpha=false);

  MaterialPhong(const shared_ptr<Texture> &texture_ambient, const shared_ptr<Texture> &texture_diffuse, const shared_ptr<Texture> &texture_specular,
                float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess);



	MaterialPhong(const string& textureFilenameAmbient, const string& textureFilenameDiffuse, const string& textureFilenameSpecular,
		float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess,
		bool alphaAmbient=false, bool alphaDiffuse=false, bool alphaSpecular=false);



	MaterialPhong(vec4 ambient, vec4 diffuse, vec4 specular, float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess);
	MaterialPhong(vec4 ambient, float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess);

	vec4 compressIntencities() const;
    void initTextures();
	void bindTextures();

	virtual bool textured() const;

	void setAmbientTexture(const shared_ptr<Texture> &texture);
	void setDiffuseTexture(const shared_ptr<Texture> &texture);
	void setSpecularTexture(const shared_ptr<Texture> &texture);

	void addAmbientTexture(const string &filename, bool alpha=false);
	void addDiffuseTexture(const string &filename, bool alpha=false);
	void addSpecularTexture(const string &filename, bool alpha=false);

	void addAmbientTexture(vec4 color);
	void addDiffuseTexture(vec4 color);
	void addSpecularTexture(vec4 color);
};

class MaterialPhongConstColor : public MaterialPhong {
public:
	vec4 ambientColor, diffuseColor, specularColor;
	MaterialPhongConstColor(vec4 ambient, vec4 diffuse, vec4 specular, float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess);

	explicit MaterialPhongConstColor(mat4 mat);
	MaterialPhongConstColor();

	mat4 compressToMatrix() const;
	bool textured() const override;
};


class TriangleR3 {
public:
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	vector<vec4> vertexColors;
	std::map<string, array<vec4, 3>> extraData = {};
	std::optional<MaterialPhong> material = std::nullopt;


	TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, const vector<vec4> &colors, const vector<vec2> &uvs, MaterialPhong material);
	TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, const vector<vec4> &colors, const vector<vec2> &uvs);
	TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, const vector<vec4> &colors);
	TriangleR3(const vector<vec3> &vertices, const vector<vec4> &colors);
	TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normal);
	TriangleR3(vector<vec3> vertices, vector<vec3> normal, MaterialPhong material);
	explicit TriangleR3(const vector<vec3> &vertices);
	TriangleR3(vector<vec3> vertices, MaterialPhong material);
	TriangleR3(const vector<vec3> &vertices, const vector<vec2> &uvs);
	TriangleR3(const vector<vec3> &vertices, vec3 normal, const vector<vec4> &colors, const vector<vec2> &uvs);
	TriangleR3(const vector<vec3> &vertices, vec3 normal, const vector<vec4> &colors);
	TriangleR3(const vector<vec3> &vertices, vec3 normal);
	TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, vec4 color, const vector<vec2> &uvs);
	TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, vec4 color);
	TriangleR3(const vector<vec3> &vertices, vec4 color);
	TriangleR3(const vector<vec3> &vertices, vec3 normal, vec4 color, const vector<vec2> &uvs);
	TriangleR3(const vector<vec3> &vertices, vec3 normal, vec4 color);
	TriangleR3();

	vec3 operator[](int i) const;
	vec3 barycenter() const;
	float area() const;
	vec3 barycentricToWorld(vec3 coords) const;
	vec3 worldToBarycentric(vec3 point) const;
	TriangleR3 operator+(vec3 v) const;
	TriangleR3 operator*(const mat4 &M) const;
	bool operator<(const TriangleR3& t) const;
	bool hasMaterial() const;

	void recalculateNormal();
	void flipNormals();

	void addExtraData(const string &name, const array<vec4, 3> &data);
    void setExtraData(const string &name, int i, vec4 data);
	array<vec4, 3> getExtraData(const string &name);
	void addMaterial(MaterialPhong material);

};


class TriangleR2 {
public:
	vector<vec2> vertices;
	vector<vec2> uvs;
	vector<vec4> vertexColors;

	TriangleR2(const vector<vec2> &vertices, const vector<vec4> &colors, const vector<vec2> &uvs);
	TriangleR2(const vector<vec2> &vertices, const vector<vec4> &colors);
	explicit TriangleR2(const vector<vec2> &vertices);
	TriangleR2(const vector<vec2> &vertices, const vector<vec2> &uvs);
	TriangleR2(vec2 v1, vec2 v2, vec2 v3);
	TriangleR2();

	vec2 operator[](int i);
	TriangleR2 operator+(vec2 v);
	TriangleR3 embeddInR3(float z = 0);
	float area() const;
};

class Meromorphism;

class TriangleComplex {
public:
	array<Complex, 3> vertices;
	array<vec2, 3> uvs;
	array<vec4, 3> vertexColors;

	TriangleComplex(const array<Complex, 3> &vertices, const array<vec4, 3> &colors, const array<vec2, 3> &uvs);
	TriangleComplex(const array<Complex, 3> &vertices, const array<vec2, 3> &uvs);
	TriangleComplex(const array<Complex, 3> &vertices);

	operator TriangleR2() const;
	TriangleComplex operator+(Complex v) const;
	TriangleComplex operator*(Complex M) const;
	TriangleComplex operator*(const Matrix<Complex> &M) const;
	TriangleComplex operator*(const Meromorphism &f) const;
	TriangleR3 embeddInR3(float z = 0) const;

	array<TriangleComplex, 3> subdivide() const;
	static vector<TriangleComplex> subdivideTriangulation(const vector<TriangleComplex> &triangles);
};

struct MaterialBuffers {
    vector<vec4> ambientColors;
	vector<vec4> diffuseColors;
	vector<vec4> specularColors;
	vector<vec4> intencitiesAndShininess;
};

struct StdAttributeBuffers {
	vector<vec3>  positions;
	vector<vec3> normals;
	vector<vec4> colors;
	vector<vec2>  uvs;
};

class IndexedTriangle;
class CurveSample;

class Vertex {
public:
	Vertex(const Vertex &other);
	Vertex(Vertex &&other) noexcept;
	Vertex & operator=(const Vertex &other);
	Vertex & operator=(Vertex &&other) noexcept;

private:
	vec3 position;
	vec3 normal;
	vec2 uv;
	vec4 color;
    maybeMaterial material;
    std::map<string, vec4> extraData = {};
    int index = -1;
    vector<std::pair<std::weak_ptr<IndexedTriangle>, int>> triangles = {};
public:
	Vertex(vec3 position, vec2 uv, vec3 normal=e3, vec4 color=BLACK, std::map<string, vec4> extraData = {}, maybeMaterial material=std::nullopt);
	explicit Vertex(vec3 position, vec3 normal=e3, vec4 color=BLACK, std::map<string, vec4> extraData = {}, maybeMaterial material=std::nullopt);

	// explicit Vertex(vec3 position, vec3 normal=vec3(0,0,1),  vec4 color=vec4(0,0,0,1),  maybeMaterial material=std::nullopt)
	// : Vertex(position, vec2(position.x, position.y), normal, color, material) {}

	string hash() const;
	bool operator<(const Vertex& v) const;
	bool operator==(const Vertex& v) const;
    void setIndex(int i);
	int getIndex() const;
	bool hasIndex() const;
	void gotAddedAsVertex (std::weak_ptr<IndexedTriangle> triangle, int i);
	vector<std::pair<std::weak_ptr<IndexedTriangle>, int>> getTriangles() const;
	vector<std::weak_ptr<Vertex>> getNeighbours() const;
    void recomputeNormals(bool weightByArea=true);

	vec3 getPosition() const;
	vec3 getNormal() const;
	vec2 getUV() const;
	vec4 getColor() const;
	MaterialPhong getMaterial() const;
	mat4 getMaterialMat() const;
	vec4 getExtraData(const string &name) const;
	vec3 getExtraData_xyz(const string &name) const;
	float getExtraData(const string &name, int i) const;
	float getExtraLast(const string &name) const;
	bool hasExtraData() const;
	bool hasExtraData(const string &name) const;
	vector<string> getExtraDataNames();
    bool hasMaterial() const;

	void addExtraData(const string &name, vec4 data);
	void addExtraData(const string &name, vec3 data);
    void addExtraData(const string &name, float data, int i=3);
	void translate(vec3 v);
	void transform(const SpaceEndomorphism &M);
	Vertex operator+(vec3 v) const;
	void operator+=(vec3 v);
	void setMaterial(MaterialPhongConstColor material);
	void setPosition(vec3 position);;
    void setNormal(vec3 normal);
	void setUV(vec2 uv);
	void setColor(vec4 color);
	void appendToBuffers(StdAttributeBuffers &buffers, MaterialBuffers &materialBuffers);
    void appendToBuffers(StdAttributeBuffers &buffers);
    void appendExtraDataToBuffer(const string &name, vector<vec4> &buffer) const;

    void appendToList(vector<Vertex> &list);
    void addTriangle(shared_ptr<IndexedTriangle> triangle, int i);

    void setCurveParameter(float t) { addExtraData("curvePoint", t); }
    void setCurvePosition(vec3 pos) { addExtraData("curvePoint", pos); }
    void setCurveTangent(vec3 v) { addExtraData("curveTangent", v); }
    void setCurveNormal(vec3 v) { addExtraData("curveNormal", v); }
    void setCurveNormalAngle(float a) { addExtraData("curveTangent", a); }
    void setAllParametricCurveExtras(float t, CurveSample &sample);

    float getCurveParameter() { return getExtraLast("curvePoint"); }
    vec3 getCurvePosition() { return getExtraData_xyz("curvePoint"); }
    vec3 getCurveTangent() { return getExtraData_xyz("curveTangent"); }
    vec3 getCurveNormal() { return getExtraData_xyz("curveNormal"); }
    float getCurveNormalAngle() { return getExtraLast("curveTangent"); }
    float getCurveWidth() { return norm(getPosition() - getCurvePosition()); }
};

Vertex barycenter(Vertex v1, const Vertex &v2, const Vertex &v3);
Vertex center(Vertex v1, const Vertex &v2);




MaterialPhong lerp(MaterialPhong m0, MaterialPhong m1, float t);



class MaterialFamily1P {
	MaterialPhong* m0 = nullptr;
    MaterialPhong *m1 = nullptr;
public:
	MaterialFamily1P(MaterialPhong *m0, MaterialPhong *m1);
	MaterialFamily1P(vec4 c1, vec4 c2, float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess);
	MaterialPhong operator()(float t) const;
};



class Light {
protected:
	vec3 position;
	vec4 color;
	vec4 intensities;
	float mode;
	float softShadowRadius;
public:
	Light(vec3 position, vec4 color, vec4 intensities, float mode, float r) : position(position), color(color), intensities(intensities), mode(mode), softShadowRadius(r) {}
	virtual ~Light() = default;
	virtual mat4 compressToMatrix() const { return mat4(vec4(position, 0), color, intensities, vec4(mode, softShadowRadius, 0, 0)); }
	vec3 getPosition() const { return position; }
	vec4 getColor() const { return color; }
};

class PointLightQuadric : public Light {
	float intensity;
public:
	PointLightQuadric(vec3 position, vec4 color, float intensity, float r=.1) : Light(position, color, vec4(intensity, 0, 0, 0), 0, r), intensity(intensity) {}
	PointLightQuadric() : PointLightQuadric(vec3(0, 0, 0), vec4(1, 1, 1, 1), 1) {}
	using Light::compressToMatrix;
};

class PointLight: public Light {
public:
	PointLight(vec3 position, float intensity_constant, float intensity_linear, float intensity_quadratic, float softShadowRadius=.1f, vec4 color=WHITE);
	PointLight(vec3 position, vec3 intensities, float r=.1f, vec4 color=WHITE);
	PointLight(vec3 position, float intensity_linear, float intensity_quadratic, vec4 color=WHITE);
};





enum MeshFormat {
	OBJ = 0
};

std::map<string, int> countEstimatedBufferSizesInOBJFile(const char *filename);




class TriangularMesh {
public:
	vector <TriangleR3> triangles;

	vector <vec3> posBuff;
	vector <vec3> normBuff;
	vector <vec4> colorBuff;
	vector <vec2> uvBuff;

	vector <vec3> calculatePositionBuffer() const;
	vector <vec3> calculateNormalBuffer() const;
	vector <vec4> calculateColorBuffer() const;
	vector <vec2> calculateUVBuffer() const;

	std::map<string, vector<vec4>> extraBuff = {};

    array<void*, 4> bufferLocations;
    array<int, 4> bufferSizes;

	TriangularMesh();
	explicit TriangularMesh(const vector <TriangleR3> &triangles);
	TriangularMesh(vector <Vertex> vertices, vector <ivec3> faceIndices);
	explicit TriangularMesh(const char* filename, MeshFormat format = OBJ);
	TriangularMesh(ComplexCurve* curve, int nSegments, float h_middle, float w_middle, float w_side);
	vector<TriangleR3> getTriangles() const;

	// --- transforms ---
	void translate(vec3 v);
	void transform(const mat4 &M);
	TriangularMesh operator+(vec3 v) const;
	TriangularMesh operator*(const mat4 &M) const;
	TriangularMesh operator*(const mat3 &M) const;
	void operator+=(vec3 v) { this->translate(v); }
	void operator*=(const mat4 &M) { this->transform(M); }
	void operator*=(const mat3 &M) { this->transform(mat4(M)); }
    void applyMap(const std::function<vec3(vec3)> &f);
    void applyMap(const std::function<vec3(vec3)> &f, const std::function<vec3(vec3, vec3)> &f_normal);
	void applyMap(const std::function<vec3(vec3)> &f, const string &customDomain);
	void applyMap(std::function<vec3(vec3)> f, std::function<vec3(vec3, vec3)> f_normal, string customDomain);

	// --- buffer generators ---

	void precomputeBuffers();
	void precomputeExtraBuffer(const string &name);
	void recomputeBuffers();
	void recomputeBuffers(bool pos, bool norm, bool color, bool uv);

	// --- modifiers ---
	void randomizeFaceColors(vec3 min = vec3(0, 0, 0), vec3 max = vec3(1, 1, 1));
	void cleanUpEmptyTriangles();
	void flipNormals();
	void recalculateNormals();
};




class Model3D {
public:
	shared_ptr<TriangularMesh> mesh;
	shared_ptr<MaterialPhong> material;
	mat4 transform;
	Model3D();
	Model3D(TriangularMesh &mesh, MaterialPhong &material, const mat4 &transform);
	void addTransform(const mat4 &transform);
};

enum BoundaryEmbeddingType {
	KERB,
	PIPE,
	FLAT,
	CURVE
};
struct BoundaryEmbeddingStyle {
	BoundaryEmbeddingType type;
	float width = 0;
	float height = 0;
	float outerMargin = 0;
	float skewness = 0;
	float width_middle = 0;
	float height_middle = 0;
	float width_side = 0;
	int nSegments = 60;
};

class PlanarMeshWithBoundary {
public:
	vector<TriangleR2> triangles;
	vector < vector<vec2>> boundaries;
	vector<bool> boundaryCyclic;
	bool encodesVectorField = false;


	PlanarMeshWithBoundary();
	PlanarMeshWithBoundary(const vector<TriangleR2> &triangles, const vector < vector<vec2>> &boundaries, const vector<bool> &boundaryCyclic);
	PlanarMeshWithBoundary(const vector<TriangleR2> &triangles, const vector < vector<vec2>> &boundaries);
	PlanarMeshWithBoundary(const vector<TriangleR2> &triangles, vector<vec2> boundary, bool cyclic = true);
	explicit PlanarMeshWithBoundary(const vector<TriangleR2> &triangles);

	void addVectorField(const VectorFieldR2 &vectorField);

	TriangularMesh embeddInR3(float z = 0) const;
	TriangularMesh embeddInR3LowerByArea(float z=0, float factor=2) const;

	PlanarMeshWithBoundary offsetBoundaryMesh(float w);
	vector<TriangularMesh> kerbBoundaryEmbedding(float w, float h, float outerMargin, float skewness);
	vector<TriangularMesh> stylisedBoundaryEmbedding(const BoundaryEmbeddingStyle &style); // only kerb for now implemented
};

class MeshFamily1P{
protected:
	float _t=0;
	shared_ptr<TriangularMesh> mesh;
	std::function<std::function<vec3(vec3)>(float, float)> time_operator;
	std::function<std::function<vec3(vec3, vec3)>(float, float)> time_operator_normal;
	bool planar=false;
public:
	virtual ~MeshFamily1P() = default;
	MeshFamily1P(const shared_ptr<TriangularMesh> &mesh, const std::function<std::function<vec3(vec3)>(float, float)> &time_operator, const std::function<std::function<vec3(vec3, vec3)>(float, float)> &time_operator_normal, float t=0);
	MeshFamily1P(const shared_ptr<TriangularMesh> &embedded_mesh, std::function<PlaneSmoothEndomorphism(float, float)> time_operator, vec3 embedding_shift=vec3(0, 0, 0), float t=0);
	MeshFamily1P(const shared_ptr<TriangularMesh> &embedded_mesh, std::function<PlaneAutomorphism(float)> time_operator_from0, vec3 embedding_shift=vec3(0, 0, 0));

	float time() const;
	virtual void transformMesh(float new_t);
	virtual void meshDeformation(float dt);
};


class MeshFamily1PExtraDomain : public MeshFamily1P {
protected:
	string domain;
public:
	MeshFamily1PExtraDomain(const shared_ptr<TriangularMesh> &mesh, const string &domain, const std::function<std::function<vec3(vec3)>(float, float)> &time_operator, const std::function<std::function<vec3(vec3, vec3)>(float, float)> &time_operator_normal, float t=0);
	MeshFamily1PExtraDomain(const shared_ptr<TriangularMesh> &embedded_mesh, const string &domain, const std::function<PlaneSmoothEndomorphism(float, float)> &time_operator, vec3 embedding_shift=vec3(0, 0, 0), float t=0);
	MeshFamily1PExtraDomain(const shared_ptr<TriangularMesh> &embedded_mesh, const string &domain, const std::function<PlaneAutomorphism(float)> &time_operator_from0, vec3 embedding_shift=vec3(0, 0, 0));

	void transformMesh(float new_t) override;
	void meshDeformation(float dt) override;
};


constexpr BoundaryEmbeddingStyle STD_KERB = BoundaryEmbeddingStyle{KERB, .01f, .02f, .01f, .2};
constexpr BoundaryEmbeddingStyle STD_CURVE = BoundaryEmbeddingStyle{CURVE, .0f, .0f, .0f, .0f, .005f, .005f, .005f};


class SuperMesh {
	std::map<PolyGroupID, vector<TriangleR3>> triangleGroups={};
	std::map<PolyGroupID, vector<TriangleR3>> boundaryGroups={}; // "bdPoint": extra attribute for unextruded pt
	std::map<PolyGroupID, vector<TriangleR3>> embedded_curves={}; // "curvePoint": extra attribute for unextruded pt
	std::map<PolyGroupID, vector<TriangleR3>> embedded_points={}; // "loc": extra const attribute for unextruded pt
	std::map<PolyGroupID, MaterialPhongConstColor> materials = std::map<PolyGroupID, MaterialPhongConstColor>();

	MaterialBuffers materialBuffers = {{}, {}, {}, {}};
	StdAttributeBuffers stdAttributeBuffers = {{}, {}, {}, {}};
	std::map<string, vector<vec4>> extraBuff = {};
	std::map<string, int> extraBufferIndices = {};

public:
	vector<void*> bufferLocations;
	vector<int> bufferSizes;
	SuperMesh();
	explicit SuperMesh(TriangularMesh &mesh);
	SuperMesh(const char* filename, MeshFormat format = OBJ);
	SuperMesh(TriangularMesh &mesh, const MaterialPhongConstColor &material);
	SuperMesh(const char* filename, const MaterialPhongConstColor &material, MeshFormat format = OBJ);
	SuperMesh(PlanarMeshWithBoundary &mesh, const MaterialPhongConstColor &material, MaterialPhongConstColor &material_bd, const BoundaryEmbeddingStyle &embeddingStyle = STD_KERB);
	SuperMesh(const vector <TriangleR3> &triangles, const MaterialPhongConstColor &material);
	SuperMesh(const std::map<PolyGroupID, vector<TriangleR3>> &triangleGroups, const std::map<PolyGroupID, MaterialPhongConstColor> &materials);

	void addPolyGroup(PolyGroupID id, const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material);
	void addPolyGroup(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material);
	void addPolyGroup(const TriangularMesh &mesh, const MaterialPhongConstColor &material);
	void addBdGroup(PolyGroupID id, const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material);
	void addBdGroup(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material);
	void addEmbeddedCurve(PolyGroupID id, const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material);
	void addEmbeddedCurve(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material);
	void addEmbeddedPoint(PolyGroupID id, const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material);
	void addEmbeddedPoint(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material);
	void embedCurve(ComplexCurve* curve, int nSegments, float h_middle, float w_middle, float w_side, const MaterialPhongConstColor &material);
	void precomputeBuffers(bool materials=true, bool extra=true);
	void merge(const SuperMesh &other);
	void precomputeExtraBuffer(string name);

	void actOnPositions(std::function<vec3(vec3)> f);
	void actAtEmbeddedPlane(const Meromorphism &f);
	void actOnPositionsWithCustomShift(std::function<vec3(vec3)> f, std::map<PolyGroupID, string> useShiftOfCustomDomain = {});
	void translate(vec3 v);
	void doPerTriangle(const std::function<void( TriangleR3&)> &f);
	void doPerTriangle(PolyGroupID grp, const std::function<void(TriangleR3&)> &f);

	void actOnEmbeddedCurve(const SpaceEndomorphism &f, bool buffOnly=false);


	void randomizeMaterials(MaterialPhongConstColor &min, MaterialPhongConstColor &max);
	void randomiseMaterialsDynamically(float stepMax);
};



class SuperPencilPlanar : public SuperMesh{
	float _t=0;
	std::function<Biholomorphism(float)> _time_operator = [](float t){return Biholomorphism::linear(1+0i, 0i);};

public:
	using SuperMesh::SuperMesh;
	void makePencil(const std::function<Biholomorphism(float)> &_time_operator, float t=0);
	void transformMesh(float new_t);
	void deformMesh(float dt);

};

class CurveSample {
	vec3 position;
	vec3 normal;
	vec3 tangent;
	mat4 material;
	float width;
	vec4 extraInfo = vec4(0, 0, 0, 0);

public:
	CurveSample(vec3 position, vec3 normal, vec3 tangent, MaterialPhongConstColor material, float width) : position(position), normal(normal), tangent(tangent), material(material.compressToMatrix()), width(width) {}
    CurveSample(const CurveSample &other);
    CurveSample(CurveSample &&other) noexcept;
    CurveSample &operator=(const CurveSample &other);
    CurveSample &operator=(CurveSample &&other) noexcept;

    float getWidth() const {return width;}
	vec3 getBinormal() const {return cross(tangent, normal);}
    vec3 getPosition() const {return position;}
    vec3 getNormal() const {return normal;}
    vec3 getTangent() const {return tangent;}
    MaterialPhongConstColor getMaterial() const { return MaterialPhongConstColor(material); }
    mat4 getMaterialMatrix() const { return material; }
	float readExtra(int position) const {return extraInfo[position];}
	float readExtraLast() const	{return extraInfo.w;}
    vec4 readExtra() const {return extraInfo;}
    vec3 readExtraXYZ() const {return vec3(extraInfo);}

	void updatePosition(vec3 new_position) {position = new_position;}
	void updateNormal(vec3 new_normal) {normal = new_normal;}
	void updateTangent(vec3 new_tangent) {tangent = new_tangent;}
	void updateMaterial(const MaterialPhongConstColor &new_material) {material = new_material.compressToMatrix();}
	void updateWidth(float new_width) {width = new_width;}
    void updateExtra(vec4 extra) {extraInfo = extra;}
    void updateExtra(vec3 extra) {extraInfo = vec4(extra, extraInfo.w);}
    void updateExtra(float extra, int position=3) {extraInfo[position] = extra;}
};

vector<CurveSample> sampleCurve(SmoothParametricCurve curve, std::function<float(float)> width,
	std::function<MaterialPhongConstColor(float)> material, float t0=0, float t1=1, int n=100, bool periodic=false);



vector<CurveSample> sampleCurve(SmoothParametricCurve curve, float width,
                                     MaterialPhongConstColor material, float t0=0, float t1=1, int n=100, bool periodic=false);



enum CurveEmbeddingTypeID {
  PLANAR,
  TUBE,
  NOT_EMBEDDED
};

const std::map<CurveEmbeddingTypeID, string> curveEmbeddingTypes = {
        {PLANAR, "PLANAR"}, {TUBE, "TUBE"}, {NOT_EMBEDDED, "NOT EMBEDDED"}};

inline string embeddingTypeName(CurveEmbeddingTypeID type) {
  if (curveEmbeddingTypes.contains(type))
    return curveEmbeddingTypes.at(type);
  throw UnknownVariantError("Embedding type with id " + std::to_string(type) + " not recognised as valid variant.", __FILE__, __LINE__);
};


class SuperCurve{
  float t0, t1;
  vector<CurveSample> samples;
  shared_ptr<SuperMesh> _mesh = nullptr;
  PolyGroupID id;
  CurveEmbeddingTypeID embeddingType = NOT_EMBEDDED;

  SuperMesh generateMeshTube(int radialSegments) const;
  void initMesh() { _mesh = std::make_shared<SuperMesh>(); }

public:
  SuperCurve(const SmoothParametricCurve &curve, const std::function<float(float)> &width, const std::function<MaterialPhongConstColor(float)> &material, int segments, float t0=0, float t1=1,  bool periodic=true);
  SuperCurve(const SmoothParametricCurve &curve, float width, MaterialPhongConstColor material, int nSegments, float t0=0, float t1=1, bool periodic=true);
  // SuperCurve(const SuperCurve &other) : t0(other.t0), t1(other.t1), samples(other.samples), _mesh(other._mesh), _weak_mesh(other._weak_mesh), id(other.id), embeddingType(other.embeddingType) {}
  // SuperCurve(SuperCurve &&other) noexcept : t0(other.domain().x), t1(other.domain().y), samples(std::move(other.samples)), _mesh(std::move(other._mesh)) {}

  void generateMesh(int radialSegments, CurveEmbeddingTypeID type);

  void transformMeshByAmbientMap( const SpaceEndomorphism &f); // just composition
  vec2 domain() const {return vec2(t0, t1);}

  void precomputeBuffers();

  void updateCurve(const SmoothParametricCurve &new_curve);
  std::function<void(float)> pencilDeformerWeak(std::function<SmoothParametricCurve(float)> pencil);
};



class SuperPencilCurve : public SuperCurve {
  float _t = 0;
  std::unique_ptr<End2P> _ambient_operator = nullptr;
  std::unique_ptr<std::function<SmoothParametricCurve(float)>> _parametric_operator = nullptr;
public:
  using SuperCurve::SuperCurve;
  SuperPencilCurve(const SuperCurve &other) : SuperCurve(other) {}
  SuperPencilCurve(SuperCurve &&other) noexcept : SuperCurve(std::move(other)) {}

  void addAmbientDeformation(End2P _ambient_operator, float t=0);
  void addLocalDeformation(End1P _local_operator, float t=0);
  void addDeformationAlongVectorField(VectorField vectorField, float t=0);
  void addPencil(std::function<SmoothParametricCurve(float)> family, float t = 0);
  float time() const;
  void transformMesh(float new_t);
};
