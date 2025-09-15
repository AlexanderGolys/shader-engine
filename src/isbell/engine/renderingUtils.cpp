
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
// #include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <glm/glm.hpp>
#include <string>
#include <vector>
#include <array>
#include <set>
#include <random>
#include <algorithm>
#include <chrono>



#include "renderingUtils.hpp"

#include "utils/randomUtils.hpp"


using namespace glm;
using std::vector, std::array, std::string, std::set, std::pair, std::unique_ptr, std::shared_ptr, std::make_shared, std::make_unique, std::to_string;




TriangleR3::TriangleR3() {
	this->vertices = { vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f) };
	this->normals = { vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f,1.0f) };
	this->uvs = { vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f) };
	this->vertexColors = { vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f) };
}

TriangleR3::TriangleR3(const vector<vec3> &vertices) : TriangleR3()
{
	this->vertices = vertices;
	vec3 n = normalize(cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
	this->normals = { n, n, n };
}




TriangleR3::TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals) : TriangleR3()
{
	this->vertices = vertices;
	this->normals = normals;
}


TriangleR3::TriangleR3(const vector<vec3> &vertices, const vector<vec4> &colors) : TriangleR3(vertices)
{
	this->vertexColors = colors;
}

TriangleR3::TriangleR3(const vector<vec3> &vertices, vec3 normal) : TriangleR3(vertices)
{
	this->normals = { normal, normal, normal };
}

TriangleR3::TriangleR3(const vector<vec3> &vertices, const vector<vec2> &uvs) : TriangleR3(vertices, vec3(0, 0, 1))
{
	this->uvs = uvs;
}

TriangleR3::TriangleR3(const vector<vec3> &vertices, vec4 color) :  TriangleR3(vertices, vec3(0, 0, 1))
{
	this->vertexColors = { color, color, color };
}



TriangleR3::TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, vec4 color) : TriangleR3(vertices, normals)
{
	this->vertexColors = { color, color, color };
}

TriangleR3::TriangleR3(const vector<vec3> &vertices, vec3 normal, vec4 color) : TriangleR3(vertices, normal)
{
	this->vertexColors = { color, color, color };
}

TriangleR3::TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, const vector<vec4> &colors) : TriangleR3(vertices, normals)
{
	this->vertexColors = colors;
}

TriangleR3::TriangleR3(const vector<vec3> &vertices, vec3 normal, const vector<vec4> &colors) : TriangleR3(vertices, normal)
{
	this->vertexColors = colors;
}




TriangleR3::TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, vec4 color, const vector<vec2> &uvs) : TriangleR3(vertices, normals, color)
{
	this->uvs = uvs;
}


TriangleR3::TriangleR3(const vector<vec3> &vertices, vec3 normal, vec4 color,
                       const vector<vec2> &uvs)
    : TriangleR3(vertices, normal, color) {
  this->uvs = uvs;
}

TriangleR3::TriangleR3(const vector<vec3> &vertices,
                       const vector<vec3> &normals,
                       const vector<vec4> &colors,
                       const vector<vec2> &uvs, MaterialPhong material) :  TriangleR3(vertices, normals, colors, uvs) {
	this->material = material;
}

TriangleR3::TriangleR3(vector<vec3> vertices,
                       vector<vec3> normal, MaterialPhong material) {}

TriangleR3::TriangleR3(vector<vec3> vertices,
                       MaterialPhong material) {}

bool TriangleR3::hasMaterial() const {return material.has_value();}

void TriangleR3::addMaterial(MaterialPhong material) {this->material = material;}

TriangleR3::TriangleR3(const vector<vec3> &vertices, const vector<vec3> &normals, const vector<vec4> &colors, const vector<vec2> &uvs) : TriangleR3(vertices, normals, colors)
{
	this->uvs = uvs;
}

TriangleR3::TriangleR3(const vector<vec3> &vertices, vec3 normal, const vector<vec4> &colors, const vector<vec2> &uvs) : TriangleR3(vertices, normal, colors)
{
	this->uvs = uvs;
}




vec3 TriangleR3::operator[](int i) const {

	return this->vertices.at(i);
}


vec3 TriangleR3::barycenter() const {
	return (vertices[0] + vertices[1] + vertices[2]) / 3.0f;
}

float TriangleR3::area() const {
	return 0.5f * norm(cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
}




vec3 TriangleR3::barycentricToWorld(vec3 coords) const {
	return coords[0] * vertices[0] + coords[1] * vertices[1] + coords[2] * vertices[2];
}

vec3 TriangleR3::worldToBarycentric(vec3 point) const {
	vec3 v0 = vertices[1] - vertices[0];
	vec3 v1 = vertices[2] - vertices[0];
	vec3 v2 = point - vertices[0];
	float d00 = dot(v0, v0);
	float d01 = dot(v0, v1);
	float d11 = dot(v1, v1);
	float d20 = dot(v2, v0);
	float d21 = dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;
	return vec3(u, v, w);
}


TriangleR3 TriangleR3::operator*(const mat4 &M) const {
	vector<vec3> newVertices = vector<vec3>();
	vector<vec3> newNormals = vector<vec3>();
	newVertices.reserve(3);
	newNormals.reserve(3);
	for (int i = 0; i < 3; i++) {
		newVertices.emplace_back(M * vec4(vertices[i], 1.0f));
		newNormals.emplace_back(M * vec4(normals[i], 0.0f));
	}
	return TriangleR3(newVertices, newNormals, vertexColors, uvs);
}

bool TriangleR3::operator<(const TriangleR3& t) const
{
	if (this->vertices[0] == t.vertices[0])
		if (this->vertices[1] == t.vertices[1])
			return vecToVecHeHe(this->vertices[2]) < vecToVecHeHe(t.vertices[2]);
	return vecToVecHeHe(this->vertices[1]) < vecToVecHeHe(t.vertices[1]);
	return vecToVecHeHe(this->vertices[0]) < vecToVecHeHe(t.vertices[0]);
}

void TriangleR3::recalculateNormal()
{
	vec3 faceNormal = normalise(cross(vertices[0] - vertices[1], vertices[0] - vertices[2]));
	this->normals = { faceNormal, faceNormal, faceNormal };
}

void TriangleR3::flipNormals()
{
	this->normals = { -normals[0], -normals[1], -normals[2] };
}

void TriangleR3::addExtraData(const string &name, const array<vec4, 3> &data) {
	extraData[name] = data;
}

void TriangleR3::setExtraData(const string &name, int i, vec4 data) {
	extraData[name][i] = data;
}

array<vec4, 3> TriangleR3::getExtraData(const string &name) {
	return extraData[name];
}

TriangleR3 TriangleR3::operator+(vec3 v) const {
	vector<vec3> newVertices = { vertices[0] + v, vertices[1] + v, vertices[2] + v };
	return TriangleR3(newVertices, this->normals, this->vertexColors, this->uvs);
}




Vertex::Vertex(vec3 position, vec2 uv, vec3 normal, vec4 color, std::map<string, vec4> extraData, std::optional<MaterialPhongConstColor> material) {
	this->position = position;
	this->normal = normal;
	this->uv = uv;
	this->color = color;
	this->material = material;
	this->extraData = extraData; }

Vertex::Vertex(vec3 position, vec3 normal, vec4 color, std::map<string, vec4> extraData, std::optional<MaterialPhongConstColor> material) {
	this->position = position;
	this->normal = normal;
	this->uv = vec2(position.x, position.y);
	this->color = color;
	this->material = material;
	this->extraData = extraData; }

string Vertex::hash() const {
    return to_string(this->position.x) + to_string(this->position.y) + to_string(this->position.z) +
	to_string(this->normal.x) + to_string(this->normal.y) + to_string(this->normal.z) +
	to_string(this->uv.x) + to_string(this->uv.y) +
	to_string(this->color.x) + to_string(this->color.y) + to_string(this->color.z) + to_string(this->color.w);
}

bool Vertex::operator<(const Vertex& v) const
{
    if (this->position.x != v.position.x) return this->position.x < v.position.x;
    if (this->position.y != v.position.y) return this->position.y < v.position.y;
    if (this->position.z != v.position.z) return this->position.z < v.position.z;
    if (this->normal.x != v.normal.x) return this->normal.x < v.normal.x;
    if (this->normal.y != v.normal.y) return this->normal.y < v.normal.y;
    if (this->normal.z != v.normal.z) return this->normal.z < v.normal.z;
    if (this->uv.x != v.uv.x) return this->uv.x < v.uv.x;
    if (this->uv.y != v.uv.y) return this->uv.y < v.uv.y;
    if (this->color.x != v.color.x) return this->color.x < v.color.x;
    if (this->color.y != v.color.y) return this->color.y < v.color.y;
    if (this->color.z != v.color.z) return this->color.z < v.color.z;
    return this->color.w < v.color.w;
}

bool Vertex::operator==(const Vertex &v) const {
    return this->position == v.position && this->normal == v.normal && this->uv == v.uv && this->color == v.color;
}

void Vertex::setIndex(int i) { index = i; }

int Vertex::getIndex() const { return index; }

bool Vertex::hasIndex() const { return index != -1; }

void Vertex::gotAddedAsVertex(std::weak_ptr<IndexedTriangle> triangle, int i) { triangles.push_back({triangle, i}); }

vector<std::pair<std::weak_ptr<IndexedTriangle>, int>> Vertex::getTriangles() const { return triangles; }

vec3 Vertex::getPosition() const { return position; }

vec3 Vertex::getNormal() const { return normal; }

vec2 Vertex::getUV() const { return uv; }

vec4 Vertex::getColor() const { return color; }

MaterialPhong Vertex::getMaterial() const { return material.value(); }

mat4 Vertex::getMaterialMat() const { return material.value().compressToMatrix(); }

vec4 Vertex::getExtraData(const string &name) const { return extraData.at(name); }

vec3 Vertex::getExtraData_xyz(const string &name) const { return vec3(extraData.at(name)); }

float Vertex::getExtraData(const string &name, int i) const { return extraData.at(name)[i]; }

float Vertex::getExtraLast(const string &name) const { return extraData.at(name)[3]; }

bool Vertex::hasExtraData() const { return !extraData.empty(); }

bool Vertex::hasExtraData(const string &name) const { return extraData.contains(name); }

void Vertex::addExtraData(const string &name, vec4 data) {
	if (!extraData.contains(name)) extraData.insert({name, data});
	else extraData[name] = data;
}

void Vertex::addExtraData(const string &name, vec3 data) {
    if (extraData.contains(name)) {
        extraData[name].x = data.x;
        extraData[name].y = data.y;
        extraData[name].z = data.z;
    }
    else
        extraData.insert({name, vec4(data.x, data.y, data.z, 0)});
}

void Vertex::addExtraData(const string &name, float data, int i) {
    if (!extraData.contains(name))
        extraData.insert({name, vec4(0)});
    extraData[name][i] = data;
}

void Vertex::translate(vec3 v) { this->position += v; }

vector<string> Vertex::getExtraDataNames() {
	vector<string> names;
	for (const auto &pair : extraData)
		names.emplace_back(pair.first);
	return names;
}

bool Vertex::hasMaterial() const { return material.has_value(); }

void Vertex::transform(const SpaceEndomorphism &M) {
	this->normal = M.df(this->position)*this->normal;
	this->position = M(this->position);
}


Vertex Vertex::operator+(vec3 v) const {
    return Vertex(this->position + v, this->uv, this->normal, this->color, this->extraData, this->material );
}

void Vertex::operator+=(vec3 v) { this->translate(v); }

void Vertex::setMaterial(MaterialPhongConstColor material) { this->material = material; }

void Vertex::setPosition(vec3 position) { this->position = position; }

void Vertex::setNormal(vec3 normal) { this->normal = normal; }

void Vertex::setUV(vec2 uv) { this->uv = uv; }

void Vertex::setColor(vec4 color) { this->color = color; }


//void Vertex::appendToBuffers(StdAttributeBuffers &buffers, MaterialBuffers &materialBuffers) {
//	appendToBuffers(buffers);
//	if (this->material.has_value()) {
//		materialBuffers.ambientColors.push_back(this->material->ambientColor);
//		materialBuffers.diffuseColors.push_back(this->material->diffuseColor);
//		materialBuffers.specularColors.push_back(this->material->specularColor);
//		materialBuffers.intencitiesAndShininess.push_back(this->material->compressIntencities());
//	}
//}

void Vertex::appendToBuffers(StdAttributeBuffers &buffers) {
    setIndex(buffers.positions.size());
	buffers.positions.push_back(this->position);
	buffers.normals.push_back(this->normal);
	buffers.uvs.push_back(this->uv);
	buffers.colors.push_back(this->color);
}

void Vertex::appendExtraDataToBuffer(const string &name, vector<vec4> &buffer) const {
    if (extraData.contains(name))
        buffer.push_back(extraData.at(name));
}
void Vertex::appendToList(vector<Vertex> &list) {
    setIndex(list.size());
    list.push_back(*this);
}
void Vertex::setAllParametricCurveExtras(float t, CurveSample &sample) {
    setCurveParameter(t);
    setCurvePosition(sample.getPosition());
    setCurveNormal(sample.getNormal());
    setCurveTangent(sample.getTangent());
}

Vertex barycenter(Vertex v1, const Vertex &v2, const Vertex &v3) {
    std::map<string, vec4> extraData = {};
    for (auto& key : v1.getExtraDataNames()) {
        vec4 newData = barycenter(v1.getExtraData(key), v2.getExtraData(key), v3.getExtraData(key));
        extraData.insert({key, newData});
    }
    vec3 n = normalize(cross(v2.getPosition() - v1.getPosition(), v3.getPosition() - v1.getPosition()));
    n = n*sign(dot(n, v1.getNormal()));
    return Vertex(barycenter(v1.getPosition(), v2.getPosition(), v3.getPosition()),
                  barycenter(v1.getUV(), v2.getUV(), v3.getUV()),
                  n,
                  barycenter(v1.getColor(), v2.getColor(), v3.getColor()),extraData,
                  MaterialPhongConstColor(barycenter(v1.getMaterialMat(), v2.getMaterialMat(), v3.getMaterialMat()))
                    );
}

Vertex center(Vertex v1, const Vertex &v2) {
    std::map<string, vec4> extraData = {};
    for (auto& key : v1.getExtraDataNames()) {
        vec4 newData =(v1.getExtraData(key) + v2.getExtraData(key))/2.0f;
        extraData.insert({key, newData});
    }
    if (v1.hasMaterial() && v2.hasMaterial() && !v1.getMaterial().textured() && !v2.getMaterial().textured())
        return Vertex((v1.getPosition() + v2.getPosition())/2.0f,
                      (v1.getUV() + v2.getUV())/2.0f,
                      normalize(v1.getNormal() + v2.getNormal()),
                      (v1.getColor() + v2.getColor())/2.0f,
                      extraData,
                      MaterialPhongConstColor((v1.getMaterialMat() + v2.getMaterialMat())/2.0f)
                        );
    return Vertex((v1.getPosition() + v2.getPosition())/2.0f,
                  (v1.getUV() + v2.getUV())/2.0f,
                  normalize(v1.getNormal() + v2.getNormal()),
                  (v1.getColor() + v2.getColor())/2.0f, extraData);
}


TriangularMesh::TriangularMesh()
{
	this->triangles = vector<TriangleR3>();
	this->posBuff = vector<vec3>();
	this->normBuff = vector<vec3>();
	this->colorBuff = vector<vec4>();
	this->uvBuff = vector<vec2>();
	this->bufferLocations = array<void*, 4>();
	this->bufferSizes = array<int, 4>();
}





TriangularMesh::TriangularMesh(const vector<TriangleR3> &triangles) : TriangularMesh()
{
	this->triangles = triangles;
}


PointLight::PointLight(vec3 position, float intensity_constant, float intensity_linear, float intensity_quadratic, float softShadowRadius, vec4 color) :
	Light(position, color, vec4(intensity_constant, intensity_linear, intensity_quadratic, 0), 0, softShadowRadius) {}

PointLight::PointLight(vec3 position, vec3 intensities, float r, vec4 color)
: PointLight(position, intensities[0], intensities[1], intensities[2], r, color) {}

PointLight::PointLight(vec3 position, float intensity_linear, float intensity_quadratic, vec4 color)
: PointLight(position, 1, intensity_linear, intensity_quadratic, 3.3f, color) {}

std::map<string, int> countEstimatedBufferSizesInOBJFile(const char *filename) {
    int positions = 0;
    int normals = 0;
    int faces = 0;
    int uvs = 0;

    FILE* file = fopen(filename, "r");

    while (1) {
        char lineHeader[1024];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;

        if (strcmp(lineHeader, "v") == 0)
            positions++;
        else if (strcmp(lineHeader, "vn") == 0)
            normals++;
        else if (strcmp(lineHeader, "vt") == 0)
            uvs++;
        else if (strcmp(lineHeader, "_f") == 0)
            faces++;
    }
    fclose(file);
    return std::map<string, int> {{"positions", positions}, {"normals", normals}, {"uvs", uvs}, {"faces", faces}};
}

TriangularMesh::TriangularMesh(const char* filename, MeshFormat format) : TriangularMesh()
{
	if (format == 0)
	{
		FILE* file = fopen(filename, "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return;
		}

			auto pos = vector<vec3>();
			auto norms = vector<vec3>();
			auto uvs = vector<vec2>();
			this->triangles = vector <TriangleR3>();
			bool vertices_loading_started = false;
			bool vertices_loading_ended = false;

			while (1) {
				char lineHeader[1024];
				// read the first word of the line
				int res = fscanf(file, "%s", lineHeader);
				if (res == EOF)
					break;

				if (strcmp(lineHeader, "v") == 0) {
					vec3 vertex;
					fscanf(file, "%_f %_f %_f\n", &vertex.x, &vertex.y, &vertex.z);
					pos.push_back(vertex);
				}


				if (vertices_loading_started && !vertices_loading_ended)
				{
					vertices_loading_ended = true;
					int nVertices = pos.size();
					norms.reserve(nVertices);
					uvs.reserve(nVertices);
				}

				else if (strcmp(lineHeader, "vt") == 0) {
					vec2 uv;
					fscanf(file, "%_f %_f\n", &uv.x, &uv.y);
					uvs.push_back(uv);
				}
				else if (strcmp(lineHeader, "vn") == 0) {
					vec3 normal;
					fscanf(file, "%_f %_f %_f\n", &normal.x, &normal.y, &normal.z);
					norms.push_back(normal);
				}

				else if (strcmp(lineHeader, "_f") == 0) {
					ivec3 vertexIndex, uvIndex, normalIndex;
					int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					if (matches != 9) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return;
					}
					vertexIndex -= ivec3(1, 1, 1);
					uvIndex -= ivec3(1, 1, 1);
					normalIndex -= ivec3(1, 1, 1);

					vector<vec2> uvsVec = { uvs.at(uvIndex[0]), uvs.at(uvIndex[1]), uvs.at(uvIndex[2]) };
					vector<vec3> normsVec = { norms.at(normalIndex[0]), norms.at(normalIndex[1]), norms.at(normalIndex[2]) };
					vector<vec4> colorsVec = { vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f) };
					vector<vec3> posVec = { pos.at(vertexIndex[0]), pos.at(vertexIndex[1]), pos.at(vertexIndex[2]) };

					this->triangles.push_back(TriangleR3(posVec, normsVec, colorsVec, uvsVec));

				}
			}
	}
	else
	{
		std::cout << "Format not supported" << std::endl;
	}
}

TriangularMesh::TriangularMesh(ComplexCurve* curve, int nSegments, float h_middle, float w_middle, float w_side)
{
	float middleRatio = w_middle / (w_middle + 2.f*w_side);
	this->triangles = vector<TriangleR3>();
	auto verticesExtruded = vector<vec2>();
	verticesExtruded.reserve(4 * nSegments+4);

	auto points = curve->sample(nSegments);
	if (curve->cyclic)
	{
		points.push_back(points.at(0));
		points.push_back(points.at(1));
	}
	else
	{
		points.insert(points.begin(), points.at(0) - points.at(1) + points.at(0));
		points.push_back(points.at(points.size() - 1) + points.at(points.size() - 1) - points.at(points.size() - 2));
	}
	for (int i=1; i<points.size() - 2; i++)
	{
		vec2 n1 = orthogonalComplement(points.at(i).z - points.at(i-1).z);
		vec2 n2 = orthogonalComplement(points.at(i+1).z - points.at(i).z);
		vec2 n3 = orthogonalComplement(points.at(i+2).z - points.at(i+1).z);
		vec2 N1 = normalize(n1+n2);
		vec2 N2 = normalize(n2+n3);
		float w = w_middle / 2.f + w_side;

		vec2 p0 = points.at(i).z + N1*w;
		vec2 p3 = points.at(i).z - N1*w;
		vec2 p1 = points.at(i).z + N1*w_middle/2.f;
		vec2 p2 = points.at(i).z - N1*w_middle/2.f;

		vec2 q0 = points.at(i+1).z + N2*w;
		vec2 q3 = points.at(i+1).z - N2*w;
		vec2 q1 = points.at(i+1).z + N2*w_middle/2.f;
		vec2 q2 = points.at(i+1).z - N2*w_middle/2.f;

		TriangleR3 tr1 = TriangleR3({ vec3(p0, 0), vec3(q0, 0), vec3(q1, h_middle) }, vector<vec2>({p0, q0, q1}));
		TriangleR3 tr2 = TriangleR3({ vec3(p0, 0), vec3(q1, h_middle), vec3(p1, h_middle) }, vector<vec2>({p0, q1, p1}));
		TriangleR3 tr3 = TriangleR3({ vec3(p1, h_middle), vec3(q1, h_middle), vec3(p2, h_middle) }, vector<vec2>({p1, q1, p2}));
		TriangleR3 tr4 = TriangleR3({ vec3(q1, h_middle), vec3(q2, h_middle), vec3(p2, h_middle) }, vector<vec2>({ q1, q2, p2 }));
		TriangleR3 tr5 = TriangleR3({ vec3(p2, h_middle), vec3(q2, h_middle), vec3(p3, 0) }, vector<vec2>({ p2, q2, p3 }));
		TriangleR3 tr6 = TriangleR3({ vec3(q2, h_middle), vec3(q3, 0), vec3(p3, 0) }, vector<vec2>({ q2, q3, p3 }));
		this->triangles.push_back(tr1);
		this->triangles.push_back(tr2);
		this->triangles.push_back(tr3);
		this->triangles.push_back(tr4);
		this->triangles.push_back(tr5);
		this->triangles.push_back(tr6);

	}
	recalculateNormals();
}

vector<TriangleR3> TriangularMesh::getTriangles() const {
	return triangles;
}

TriangularMesh TriangularMesh::operator+(vec3 v) const
{
	TriangularMesh newMesh = TriangularMesh(*this);
		newMesh.translate(v);
		return newMesh;

}

TriangularMesh TriangularMesh::operator*(const mat4 &M) const
{
		TriangularMesh newMesh = TriangularMesh(*this);
		newMesh.transform(M);
		return newMesh;

}

TriangularMesh TriangularMesh::operator*(const mat3 &M) const
{
	mat4 m = mat4(M);
	return *this * m;
}


void TriangularMesh::translate(vec3 v)
{
	for (int i = 0; i < triangles.size(); i++) {
		triangles.at(i) = triangles.at(i) + v;
	}
}

void TriangularMesh::transform(const mat4 &M)
{
	for (int i = 0; i < triangles.size(); i++) {
		triangles.at(i) = triangles.at(i) * M;
	}
}


vector<vec3> TriangularMesh::calculatePositionBuffer() const
{
	auto positions = vector<vec3>();
	positions.reserve(3 * triangles.size());
	for (TriangleR3 t : this->triangles) {
		positions.emplace_back(t[0]);
		positions.emplace_back(t[1]);
		positions.emplace_back(t[2]);
	}
	return positions;
}

vector<vec3> TriangularMesh::calculateNormalBuffer() const
{
	auto normals = vector<vec3>();
	normals.reserve(3 * triangles.size());
	for (TriangleR3 t : this->triangles) {
		normals.emplace_back(t.normals[0]);
		normals.emplace_back(t.normals[1]);
		normals.emplace_back(t.normals[2]);
	}
	return normals;
}

vector<vec4> TriangularMesh::calculateColorBuffer() const
{
	auto clr = vector<vec4>();
	clr.reserve(3 * triangles.size());
		for (TriangleR3 t : triangles) {
			clr.emplace_back(t.vertexColors[0]);
			clr.emplace_back(t.vertexColors[1]);
			clr.emplace_back(t.vertexColors[2]);
		}
		return clr;
}

vector<vec2> TriangularMesh::calculateUVBuffer() const
{
	auto uv = vector<vec2>();
	uv.reserve(3 * triangles.size());
		for (TriangleR3 t : triangles) {
			uv.emplace_back(t.uvs[0]);
			uv.emplace_back(t.uvs[1]);
			uv.emplace_back(t.uvs[2]);
		}
		return uv;

}
void TriangularMesh::recomputeBuffers() {
	recomputeBuffers(true, true, true, true);
}

void TriangularMesh::recomputeBuffers(bool pos, bool norm, bool color, bool uv) {
	if (pos) {
		this->posBuff = calculatePositionBuffer();
		this->bufferLocations[0] = &posBuff[0];
		this->bufferSizes[0] = posBuff.size();
	}

	if (norm) {
		this->normBuff = calculateNormalBuffer();
		this->bufferLocations[1] = &normBuff[0];
		this->bufferSizes[1] = normBuff.size();
	}
	if (color) {
		this->colorBuff = calculateColorBuffer();
		this->bufferLocations[2] = &colorBuff[0];
		this->bufferSizes[2] = colorBuff.size();
	}
	if (uv) {
		this->uvBuff = calculateUVBuffer();
		this->bufferLocations[3] = &uvBuff[0];
		this->bufferSizes[3] = uvBuff.size();
	}
}

void TriangularMesh::precomputeBuffers()
{
	if (!this->posBuff.empty())
		return;
	recomputeBuffers();
}

void TriangularMesh::precomputeExtraBuffer(const string &name) {
	extraBuff[name] = vector<vec4>();
	extraBuff[name].reserve(3 * triangles.size());
	for (TriangleR3 t : triangles) {
		auto data = t.getExtraData(name);
		extraBuff[name].push_back(data[0]);
		extraBuff[name].push_back(data[1]);
		extraBuff[name].push_back(data[2]);
	}
}


void TriangularMesh::randomizeFaceColors(vec3 min, vec3 max)
{
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> distribution(0, 1);
	for (int i = 0; i < triangles.size(); i++) {
		vec4 faceColor = vec4(distribution(generator)*(max.x - min.x) + min.x,
			distribution(generator) * (max.y - min.y) + min.y,
			distribution(generator) * (max.z - min.z) + min.z, 1.0f);
		triangles.at(i).vertexColors = { faceColor, faceColor, faceColor };
	}

}

void TriangularMesh::cleanUpEmptyTriangles()
{
	for (int i = 0; i < this->triangles.size(); i++)
	{
		if (triangles[i].vertices.size() != 3)
		{
			triangles.erase(triangles.begin() + i);
			i--;
		}
	}
}

void TriangularMesh::flipNormals()
{
	for (int i = 0; i < this->triangles.size(); i++)
	{
		triangles[i].flipNormals();
	}
}

void TriangularMesh::recalculateNormals()
{
	for (int i = 0; i < this->triangles.size(); i++)
	{
		triangles[i].recalculateNormal();
	}
}

void TriangularMesh::applyMap(const std::function<vec3(vec3)> &f) {
	for (int i = 0; i < this->triangles.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			triangles[i].vertices[j] = f(triangles[i].vertices[j]);
		}
	}
	for (int i = 0; i < this->posBuff.size(); i++)
	{
		this->posBuff[i] = f(this->posBuff[i]);
	}
}

void TriangularMesh::applyMap(const std::function<vec3(vec3)> &f,
                              const std::function<vec3(vec3, vec3)> &f_normal)
{
	for (int i = 0; i < this->triangles.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			triangles[i].vertices[j] = f(triangles[i].vertices[j]);
			triangles[i].normals[j] = f_normal(triangles[i].vertices[j], triangles[i].normals[j]);
		}
	}
	for (int i = 0; i < this->posBuff.size(); i++)
	{
		this->normBuff[i] = f_normal(this->posBuff[i], this->normBuff[i]);
		this->posBuff[i] = f(this->posBuff[i]);
	}
}

void TriangularMesh::applyMap(const std::function<vec3(vec3)> &f, const string &customDomain) {
	for (int i = 0; i < this->triangles.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vec3 shift = triangles[i].vertices[j] - vec3(triangles[i].getExtraData(customDomain)[j]);
			triangles[i].vertices[j] = f(vec3(triangles[i].getExtraData(customDomain)[j])) + shift;
		}
	}
	for (int i = 0; i < this->posBuff.size(); i++)
	{
		vec3 shift = posBuff[i] - vec3(extraBuff[customDomain][i]);
		posBuff[i] = f(vec3(extraBuff[customDomain][i])) + shift;
		extraBuff[customDomain][i] = vec4(f(vec3(extraBuff[customDomain][i])), 0);
	}
}

void TriangularMesh::applyMap(std::function<vec3(vec3)> f,
    std::function<vec3(vec3, vec3)> f_normal, string customDomain) {
}




MaterialPhong::MaterialPhong(): MaterialPhong(vec4(0), vec4(0), vec4(0), 0, 0, 0, 0) {}

shared_ptr<Texture> MaterialPhong::constAmbientTexture(vec4 color) {
	return make_shared<Texture>(color, 0, "texture_ambient");
}

shared_ptr<Texture> MaterialPhong::constDiffuseTexture(vec4 color) {
	return make_shared<Texture>(color, 1, "texture_diffuse");
}

shared_ptr<Texture> MaterialPhong::constSpecularTexture(vec4 color) {
	return make_shared<Texture>(color, 2, "texture_specular");
}

shared_ptr<Texture> MaterialPhong::constAmbientTexture(vec3 color) {
	return constAmbientTexture(vec4(color, 1));
}

shared_ptr<Texture> MaterialPhong::constDiffuseTexture(vec3 color) {
	return constDiffuseTexture(vec4(color, 1));
}

shared_ptr<Texture> MaterialPhong::constSpecularTexture(vec3 color) {
	return constSpecularTexture(vec4(color, 1));
}

shared_ptr<Texture> MaterialPhong::makeAmbientTexture(const string &filename, bool alpha) {
	return std::make_shared<Texture>(filename, 0, "texture_ambient", alpha);
}

shared_ptr<Texture> MaterialPhong::makeDiffuseTexture(const string &filename, bool alpha) {
	return std::make_shared<Texture>(filename, 1, "texture_diffuse", alpha);
}

shared_ptr<Texture> MaterialPhong::makeSpecularTexture(const string &filename, bool alpha) {
	return std::make_shared<Texture>(filename, 2, "texture_specular", alpha);
}

MaterialPhong::MaterialPhong(const shared_ptr<Texture> &texture_ambient, const shared_ptr<Texture> &texture_diffuse,
							 const shared_ptr<Texture> &texture_specular, float ambientIntensity, float diffuseIntensity,
							 float specularIntensity, float shininess) {
    this->texture_ambient = texture_ambient;
    this->texture_diffuse = texture_diffuse;
    this->texture_specular = texture_specular;

    this->ambientIntensity = ambientIntensity;
    this->diffuseIntensity = diffuseIntensity;
    this->specularIntensity = specularIntensity;
    this->shininess = shininess;

	initTextures();
}

MaterialPhong::MaterialPhong(const string &textureFilenameAmbient, const string &textureFilenameDiffuse, const string &textureFilenameSpecular, float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess, bool alphaAmbient, bool alphaDiffuse, bool alphaSpecular)
:  MaterialPhong(makeAmbientTexture(textureFilenameAmbient, alphaAmbient),
				makeDiffuseTexture(textureFilenameDiffuse, alphaDiffuse),
				makeSpecularTexture(textureFilenameSpecular, alphaSpecular),
				ambientIntensity, diffuseIntensity, specularIntensity, shininess) {}

MaterialPhong::MaterialPhong(vec4 ambient, vec4 diffuse, vec4 specular, float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess)
: MaterialPhong(constAmbientTexture(ambient), constDiffuseTexture(diffuse), constSpecularTexture(specular), ambientIntensity, diffuseIntensity, specularIntensity, shininess) {}

MaterialPhong::MaterialPhong(vec4 ambient, float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess)
: MaterialPhong(ambient, ambient, ambient, ambientIntensity, diffuseIntensity, specularIntensity, shininess) {}



vec4 MaterialPhong::compressIntencities() const
{
	return vec4(ambientIntensity, diffuseIntensity, specularIntensity, shininess);
}

void MaterialPhong::initTextures() {
	texture_ambient->load();
	texture_diffuse->load();
	texture_specular->load();
}

void MaterialPhong::bindTextures() {
	texture_ambient->bind();
	texture_diffuse->bind();
	texture_specular->bind();
}

bool MaterialPhong::textured() const {
	return true;
}

void MaterialPhong::setAmbientTexture(const shared_ptr<Texture> &texture) {
	texture_ambient->deleteTexture();
	texture_ambient = texture;
	texture_ambient->load();
}

void MaterialPhong::setDiffuseTexture(const shared_ptr<Texture> &texture) {
	texture_diffuse->deleteTexture();
	texture_diffuse = texture;
	texture_diffuse->load();
}

void MaterialPhong::setSpecularTexture(const shared_ptr<Texture> &texture) {
	texture_specular->deleteTexture();
	texture_specular = texture;
	texture_specular->load();
}

void MaterialPhong::addAmbientTexture(const string &filename, bool alpha) {
	setAmbientTexture(makeAmbientTexture(filename, alpha));
}

void MaterialPhong::addDiffuseTexture(const string &filename, bool alpha) { setDiffuseTexture(makeDiffuseTexture(filename, alpha));}

void MaterialPhong::addSpecularTexture(const string &filename, bool alpha) { setSpecularTexture(makeSpecularTexture(filename, alpha));}

void MaterialPhong::addAmbientTexture(vec4 color) { setAmbientTexture(constAmbientTexture(color)); }

void MaterialPhong::addDiffuseTexture(vec4 color) { setDiffuseTexture(constDiffuseTexture(color)); }

void MaterialPhong::addSpecularTexture(vec4 color) { setSpecularTexture(constSpecularTexture(color)); }

MaterialPhongConstColor::MaterialPhongConstColor(vec4 ambient, vec4 diffuse, vec4 specular, float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess): MaterialPhong(ambient, diffuse, specular, ambientIntensity, diffuseIntensity, specularIntensity, shininess),
																																													   ambientColor(ambient), diffuseColor(diffuse), specularColor(specular) {}

MaterialPhongConstColor::MaterialPhongConstColor(mat4 mat): MaterialPhongConstColor(mat[0], mat[1], mat[2], mat[3][0], mat[3][1], mat[3][2], mat[3][3]) {}

MaterialPhongConstColor::MaterialPhongConstColor(): MaterialPhongConstColor(vec4(0), vec4(0), vec4(0), 0, 0, 0, 0) {}

mat4 MaterialPhongConstColor::compressToMatrix() const {return mat4(ambientColor, diffuseColor, specularColor, compressIntencities());}

bool MaterialPhongConstColor::textured() const {return false;}




Model3D::Model3D()
{
	this->mesh = nullptr;
	this->material = nullptr;
	this->transform = mat4(1.0f);
}

Model3D::Model3D(TriangularMesh &mesh, MaterialPhong &material, const mat4 &transform)
{
	this->mesh = std::make_shared<TriangularMesh>(mesh);
	this->material = std::make_shared<MaterialPhong>(material);
	this->transform = transform;
}

void Model3D::addTransform(const mat4 &transform)
{
	this->transform = this->transform * transform;
}



vec2 TriangleR2::operator[](int i)
{
	return this->vertices[i];
}

TriangleR2::TriangleR2(const vector<vec2> &vertices, const vector<vec4> &vertexColors, const vector<vec2> &uvs)
{
	this->vertices = vertices;
	this->vertexColors = vertexColors;
	this->uvs = uvs;
}



TriangleR2::TriangleR2(const vector<vec2> &vertices)
{
	this->vertices = vertices;
	this->vertexColors = { vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f) };
	this->uvs = vertices;
}
TriangleR2::TriangleR2(const vector<vec2> &vertices, const vector<vec4> &colors)
{
	this->vertices = vertices;
	this->vertexColors = colors;
	this->uvs = vertices;
}

TriangleR2::TriangleR2(const vector<vec2> &vertices, const vector<vec2> &uvs)
{
	this->vertices = vertices;
	this->vertexColors = { vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f) };
	this->uvs = uvs;
}



TriangleR2::TriangleR2(vec2 v1, vec2 v2, vec2 v3)
{
	this->vertices = { v1, v2, v3 };
	this->vertexColors = { vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f) };
	this->uvs = { v1, v2, v3 };
}

TriangleR2::TriangleR2()
{
	this->vertices = vector<vec2>();
	this->vertexColors = vector<vec4>();
	this->uvs = vector<vec2>();
}


TriangleR2 TriangleR2::operator+(vec2 v)
{
	vector<vec2> newVertices = { vertices[0] + v, vertices[1] + v, vertices[2] + v };
	return TriangleR2(newVertices, { vertexColors[0], vertexColors[1], vertexColors[2] }, { uvs[0], uvs[1], uvs[2] });
}



TriangleR3 TriangleR2::embeddInR3(float z)
{
	vector<vec3> newVertices = { vec3(vertices[0].x, vertices[0].y, z), vec3(vertices[1].x, vertices[1].y, z), vec3(vertices[2].x, vertices[2].y, z) };
	return TriangleR3(newVertices, vec3(0.0f, 0.0f, 1.0f), vertexColors, uvs);
}

float TriangleR2::area() const {
	return norm(cross(vec3(vertices[1] - vertices[0], 0.0f), vec3(vertices[2] - vertices[0], 0.0f))) / 2.0f;
}

TriangleComplex::TriangleComplex(const array<Complex, 3> &vertices, const array<vec4, 3> &colors, const array<vec2, 3> &uvs) {
	this->vertices = vertices;
	this->vertexColors = colors;
	this->uvs = uvs;
}

TriangleComplex::TriangleComplex(const array<Complex, 3> &vertices, const array<vec2, 3> &uvs):
TriangleComplex(vertices, { vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f) }, uvs) {}

TriangleComplex::TriangleComplex(const array<Complex, 3> &vertices) : TriangleComplex(vertices, { vec2(vertices[0]), vec2(vertices[1]), vec2(vertices[2]) }) {}

TriangleComplex::operator TriangleR2() const {
	vector<vec2> vertices = { vec2(this->vertices[0].x(), this->vertices[0].y()),
		vec2(this->vertices[1].x(), this->vertices[1].y()),
		vec2(this->vertices[2].x(), this->vertices[2].y()) };
	vector<vec4> colors = { this->vertexColors[0], this->vertexColors[1], this->vertexColors[2] };
	vector<vec2> uvs = { this->uvs[0], this->uvs[1], this->uvs[2] };
	return TriangleR2(vertices, colors, uvs);
}

TriangleComplex TriangleComplex::operator+(Complex v) const {
	return TriangleComplex({ vertices[0] + v, vertices[1] + v, vertices[2] + v }, vertexColors, uvs);
}

TriangleComplex TriangleComplex::operator*(Complex M) const {
	return TriangleComplex({ vertices[0] * M, vertices[1] * M, vertices[2] * M }, vertexColors, uvs);
}

// TriangleComplex TriangleComplex::operator*(const Matrix<Complex, 2> &M) const {
// 	return TriangleComplex({ M.mobius(vertices[0]), M.mobius(vertices[1]), M.mobius(vertices[2]) }, vertexColors, uvs);
// }

TriangleComplex TriangleComplex::operator*(const Meromorphism &f) const {
	return TriangleComplex({ f(vertices[0]), f(vertices[1]), f(vertices[2]) }, vertexColors, uvs);
}

TriangleR3 TriangleComplex::embeddInR3(float z) const {
	return TriangleR2(*this).embeddInR3(z);
}

array<TriangleComplex, 3> TriangleComplex::subdivide() const {
	Complex center = (vertices[0] + vertices[1] + vertices[2]) / 3.0f;
	vec2 centerUV = (uvs[0] + uvs[1] + uvs[2]) / 3.0f;
	vec4 centerColor = (vertexColors[0] + vertexColors[1] + vertexColors[2]) / 3.0f;
	return {TriangleComplex({vertices[0], vertices[1], center},	{vertexColors[0], vertexColors[1], centerColor}, {uvs[0], uvs[1], centerUV}),
			TriangleComplex({vertices[1], vertices[2], center},	{vertexColors[1], vertexColors[2], centerColor}, {uvs[1], uvs[2], centerUV}),
			TriangleComplex({vertices[2], vertices[0], center},	{vertexColors[2], vertexColors[0], centerColor}, {uvs[2], uvs[0], centerUV})};
}

vector<TriangleComplex> TriangleComplex::subdivideTriangulation(const vector<TriangleComplex> &triangles) {
	vector<TriangleComplex> newTriangles;
	newTriangles.reserve(triangles.size() * 3);
	for (int i = 0; i < triangles.size(); i++) {
		auto subTriangles = triangles[i].subdivide();
		newTriangles.push_back(subTriangles[0]);
		newTriangles.push_back(subTriangles[1]);
		newTriangles.push_back(subTriangles[2]);
	}
	return newTriangles;
}

Vertex::Vertex(const Vertex &other): position(other.position),
									 normal(other.normal),
									 uv(other.uv),
									 color(other.color),
									 material(other.material),
									 extraData(other.extraData),
									 index(other.index),
									 triangles(other.triangles) {}

Vertex::Vertex(Vertex &&other) noexcept: position(std::move(other.position)),
										 normal(std::move(other.normal)),
										 uv(std::move(other.uv)),
										 color(std::move(other.color)),
										 material(std::move(other.material)),
										 extraData(std::move(other.extraData)),
										 index(other.index),
										 triangles(std::move(other.triangles)) {}

Vertex & Vertex::operator=(const Vertex &other) {
	if (this == &other)
		return *this;
	position = other.position;
	normal = other.normal;
	uv = other.uv;
	color = other.color;
	material = other.material;
	extraData = other.extraData;
	index = other.index;
	triangles = other.triangles;
	return *this;
}

Vertex & Vertex::operator=(Vertex &&other) noexcept {
	if (this == &other)
		return *this;
	position = std::move(other.position);
	normal = std::move(other.normal);
	uv = std::move(other.uv);
	color = std::move(other.color);
	material = std::move(other.material);
	extraData = std::move(other.extraData);
	index = other.index;
	triangles = std::move(other.triangles);
	return *this;
}


PlanarMeshWithBoundary::PlanarMeshWithBoundary()
{
	auto triangles = vector<TriangleR2>();
	auto boundaries = vector<vector<vec2>>();
	auto boundaryCyclic = vector<bool>();
}

PlanarMeshWithBoundary::PlanarMeshWithBoundary(const vector<TriangleR2> &triangles, const vector<vector<vec2>> &boundaries, const vector<bool> &boundaryCyclic)
{
	this->triangles = triangles;
	this->boundaries = boundaries;
	this->boundaryCyclic = boundaryCyclic;

}

PlanarMeshWithBoundary::PlanarMeshWithBoundary(const vector<TriangleR2> &triangles, const vector<vector<vec2>> &boundaries)
{
	this->triangles = triangles;
	this->boundaries = boundaries;
	this->boundaryCyclic = vector<bool>();
	for (auto bd : boundaries)
	{
		this->boundaryCyclic.push_back(true);
	}
}

PlanarMeshWithBoundary::PlanarMeshWithBoundary(const vector<TriangleR2> &triangles, vector<vec2> boundary, bool cyclic)
{
	this->triangles = triangles;
	this->boundaries = { boundary };
	this->boundaryCyclic = { cyclic };
}

PlanarMeshWithBoundary::PlanarMeshWithBoundary(const vector<TriangleR2> &triangles)
{
	this->triangles = triangles;
	this->boundaries = vector<vector<vec2>>();
	this->boundaryCyclic = vector<bool>();
}



void PlanarMeshWithBoundary::addVectorField(const VectorFieldR2 &vectorField)
{
	for (int j = 0; j < triangles.size(); j++)
	{
		auto tr = triangles[j];
		for (int i = 0; i < 3; i++)
		{
			tr.vertexColors[i] = vec4(vectorField(tr.vertices[i]), 0.0f, 1.0f);
		}
	}
	this->encodesVectorField = true;
}

TriangularMesh PlanarMeshWithBoundary::embeddInR3(float z) const
{
	vector<TriangleR3> trs = vector<TriangleR3>();
	trs.reserve(triangles.size());
	for (TriangleR2 tr : triangles)
	{
		trs.push_back(tr.embeddInR3(z));
	}
	return TriangularMesh(trs);
}

TriangularMesh PlanarMeshWithBoundary::embeddInR3LowerByArea(float z, float factor) const
{
	vector<TriangleR3> trs = vector<TriangleR3>();
	trs.reserve(triangles.size());
	for (TriangleR2 tr : triangles)
	{
		trs.push_back(tr.embeddInR3(z - tr.area()*factor));
	}
	return TriangularMesh(trs);
}



PlanarMeshWithBoundary PlanarMeshWithBoundary::offsetBoundaryMesh(float w)
{
	vector<TriangleR2> triang = vector<TriangleR2>();
	vector<vector<vec2>> newBoundaries = vector<vector<vec2>>();
	vector<bool> cyclics = vector<bool>();

	for (int j = 0; j < boundaries.size(); j++)
	{
		auto bd = boundaries[j];
		auto outerPts = vector<vec2>();
		auto innerPts = vector<vec2>();
		if (boundaryCyclic[j])
		{
			for (int i = 0; i < bd.size(); i++)
			{
				vec2 v1 = bd[(i - 1) % bd.size()];
				vec2 v2 = bd[i];
				vec2 v3 = bd[(i + 1) % bd.size()];
				vec2 n1 = normalize(orthogonalComplement(v2 - v1));
				vec2 n2 = normalize(orthogonalComplement(v3 - v2));
				vec2 pOuter = intersectLines(v1 + n1 * w, v2 + n1 * w, v2 + n2 * w, v3 + n2 * w);
				vec2 pInner = intersectLines(v1 - n1 * w, v2 - n1 * w, v2 - n2 * w, v3 - n2 * w);
				outerPts.push_back(pOuter);
				innerPts.push_back(pInner);
			}

			for (int i = 0; i < outerPts.size(); i++)
			{
				triang.push_back(TriangleR2(outerPts[i], outerPts[(i + 1) % bd.size()], innerPts[(i + 1) % bd.size()] ));
				triang.push_back(TriangleR2( outerPts[i], innerPts[i], innerPts[(i + 1) % bd.size()] ));
			}

			newBoundaries.push_back(outerPts);
			newBoundaries.push_back(innerPts);
			cyclics.push_back(true);
			cyclics.push_back(true);
		}


		else
		{
			vector<vec2> entireNewBd = vector<vec2>();
			vec2 endingDir = normalize(bd[0] - bd[1]);
			vec2 n1 = orthogonalComplement(endingDir);
			vec2 p1 = bd[0] + n1 * w;
			vec2 p2 = p1 + endingDir * w;
			vec2 p4 = bd[0] - n1 * w;
			vec2 p3 = p4 + endingDir * w;
			triang.push_back(TriangleR2(vector<vec2>({ p1, p2, p3 })));
			triang.push_back(TriangleR2(vector<vec2>({ p1, p3, p4 })));
			entireNewBd.push_back(p1);
			entireNewBd.push_back(p2);
			entireNewBd.push_back(p3);
			entireNewBd.push_back(p4);
			for (int i = 1; i < bd.size(); i++)
			{
				vec2 v1 = bd[(i - 1) % bd.size()];
				vec2 v2 = bd[i];
				vec2 v3 = bd[(i + 1) % bd.size()];
				vec2 n1 = normalize(orthogonalComplement(v2 - v1));
				vec2 n2 = normalize(orthogonalComplement(v3 - v2));
				vec2 pOuter = intersectLines(v1 + n1 * w, v2 + n1 * w, v2 + n2 * w, v3 + n2 * w);
				vec2 pInner = intersectLines(v1 - n1 * w, v2 - n1 * w, v2 - n2 * w, v3 - n2 * w);
				outerPts.push_back(pOuter);
				innerPts.push_back(pInner);
				entireNewBd.push_back(pInner);
			}
			for (int i = 0; i < outerPts.size() - 1; i++)
			{
				triang.push_back(TriangleR2(outerPts[i], outerPts[(i + 1)], innerPts[(i + 1)]));
				triang.push_back(TriangleR2(outerPts[i], innerPts[i], innerPts[(i + 1)]));
			}
			endingDir = normalize(bd[bd.size() - 1] - bd[bd.size() - 2]);
			vec2 n2 = orthogonalComplement(endingDir);
			vec2 q1 = bd[bd.size() - 1] - n1 * w;
			vec2 q2 = q1 + endingDir * w;
			vec2 q4 = bd[bd.size() - 1] + n1 * w;
			vec2 q3 = q4 + endingDir * w;
			triang.push_back(TriangleR2(q1, q2, q3 ));
			triang.push_back(TriangleR2(q1, q3, q4 ));
			entireNewBd.push_back(q1);
			entireNewBd.push_back(q2);
			entireNewBd.push_back(q3);
			entireNewBd.push_back(q4);
			for (int i = outerPts.size() - 1; i >= 0; i--)
			{
				entireNewBd.push_back(outerPts[i]);
			}

			newBoundaries.push_back(entireNewBd);
			cyclics.push_back(true);
		}
	}


		return PlanarMeshWithBoundary(triang, newBoundaries, cyclics);
	}


vector<TriangularMesh> PlanarMeshWithBoundary::kerbBoundaryEmbedding(float w, float h, float outerMargin, float skewness) {
		auto autMeshes = vector<TriangularMesh>();
		for (int j = 0; j < boundaries.size(); j++)
		{
			auto bd = boundaries[j];
			vector<TriangleR3> triang = vector<TriangleR3>();
			vector<vec3> row1 = vector<vec3>();
			vector<vec3> row2 = vector<vec3>();
			vector<vec3> row3 = vector<vec3>();
			vector<vec3> row4 = vector<vec3>();
			for (int i = 0; i < bd.size(); i++)
			{
				vec2 prev = bd[i == 0 ? bd.size() - 1 : i - 1];
				vec2 pt = bd[i % bd.size()];
				vec2 next = bd[(i + 1) % bd.size()];
				vec2 n1 = orthogonalComplement(pt - prev);
				vec2 n2 = orthogonalComplement(next - pt);
				vec2 n = normalise(n1 + n2);
				if (dot(cross(vec3(n, 0), vec3(pt-prev, 0)), vec3(0, 0, 1)) < 0)
				{
					n = -n;
				}

				vec3 p1 = vec3(pt.x, pt.y, h) + vec3(n.x, n.y, 0) * h * skewness;
				row1.push_back(p1);

				vec3 p2 = p1 + vec3(n.x, n.y, 0) * w;
				row2.push_back(p2);

				vec3 p3 = p2 - vec3(0, 0, h) + vec3(n.x, n.y, 0) * h * skewness;
				row3.push_back(p3);

				vec3 p4 = p3 + vec3(n.x, n.y, 0) * outerMargin;
				row4.push_back(p4);
			}
			for (int i = 0; i < row1.size()-1; i++)
			{
				vec3 p0 = vec3(bd[i% bd.size()].x, bd[i % bd.size()].y, 0);
				vec3 q0 = vec3(bd[(i + 1) % bd.size()].x, bd[(i + 1) % bd.size()].y, 0);
				int next = (i + 1) % bd.size();
				triang.push_back(TriangleR3({ p0, q0, row1[next] }));
				triang.push_back(TriangleR3({ p0, row1[i], row1[next] }));
				triang.push_back(TriangleR3({ row1[i], row1[next], row2[next] }));
				triang.push_back(TriangleR3({ row1[i], row2[i], row2[next] }));
				triang.push_back(TriangleR3({ row2[i], row3[next], row2[next] }));
				triang.push_back(TriangleR3({ row3[i], row2[i], row3[next] }));
				triang.push_back(TriangleR3({ row3[i], row3[next], row4[next] }));
				triang.push_back(TriangleR3({ row3[i], row4[i], row4[next] }));
			}
			int last = row1.size() - 1;
			vec3 p0 = vec3(bd[last].x, bd[last].y, 0);
			vec3 q0 = vec3(bd[0].x, bd[0].y, 0);
			triang.push_back(TriangleR3({ p0, q0, row1[0] }));
			triang.push_back(TriangleR3({ p0, row1[last], row1[0] }));
			triang.push_back(TriangleR3({ row1[last], row1[0], row2[0] }));
			triang.push_back(TriangleR3({ row1[last], row2[last], row2[0] }));
			triang.push_back(TriangleR3({ row2[last], row2[0], row3[0] }));
			triang.push_back(TriangleR3({ row2[last], row3[last], row3[0] }));
			triang.push_back(TriangleR3({ row3[last], row3[0], row4[0] }));
			triang.push_back(TriangleR3({ row3[last], row4[last], row4[0] }));

			for (int i = 0; i < triang.size(); i++)
			{
				triang[i].recalculateNormal();
				if (dot(triang[i].normals[0], vec3(0, 0, 1)) < 0)
				{
					triang[i].flipNormals();
				}
			}


			autMeshes.push_back(TriangularMesh(triang));
		}
		return autMeshes;
	}

vector<TriangularMesh> PlanarMeshWithBoundary::stylisedBoundaryEmbedding(const BoundaryEmbeddingStyle &style) {
	return kerbBoundaryEmbedding(style.width, style.height, style.outerMargin, style.skewness); // todo more styles
}

MeshFamily1P::MeshFamily1P(const shared_ptr<TriangularMesh> &mesh,
                           const std::function<std::function<vec3(vec3)>(float, float)> &time_operator,
                           const std::function<std::function<vec3(vec3, vec3)>(float, float)> &time_operator_normal, float t) {
	this->mesh = mesh;
	this->time_operator = time_operator;
	this->time_operator_normal = time_operator_normal;
	this->_t = t;
}

MeshFamily1P::MeshFamily1P(const shared_ptr<TriangularMesh> &embedded_mesh, std::function<PlaneSmoothEndomorphism(float, float)> time_operator, vec3 embedding_shift, float t) {
	this->mesh = embedded_mesh;
	this->time_operator = [time_operator, embedding_shift](float t, float s) {
		return [time_operator, embedding_shift, t, s](vec3 w) {
			vec3 v = w - embedding_shift;
			vec2 p = vec2(v.x, v.y);
			vec2 q = time_operator(t, s)(p);
			return vec3(q.x, q.y, 0) + embedding_shift;
		};
	};
	this->time_operator_normal = [](float t, float s) {
		return [](vec3 w, vec3 n) {
			return n;
		};
	};
	planar = true;
	this->_t = t;
}

MeshFamily1P::MeshFamily1P(const shared_ptr<TriangularMesh> &embedded_mesh, std::function<PlaneAutomorphism(float)> time_operator_from0, vec3 embedding_shift) {
	this->mesh = embedded_mesh;
	this->time_operator = [time_operator_from0, embedding_shift](float t, float s) {
		return [time_operator_from0, embedding_shift, t, s](vec3 w) {
			vec3 v = w - embedding_shift;
			vec2 p = vec2(v.x, v.y);
			vec2 q0 = time_operator_from0(t).f_inv(p);
			vec2 q = time_operator_from0(s)(q0);
			return vec3(q.x, q.y, 0) + embedding_shift;
		};
	};
	this->time_operator_normal = [](float t, float s) {
		return [](vec3 w, vec3 n) {
			return n;
		};
	};
	planar = true;
	this->_t = 0;
}

float MeshFamily1P::time() const {
	return _t;
}

void MeshFamily1P::transformMesh(float new_t) {
	if (planar)
		mesh->applyMap(time_operator(_t, new_t));
	else
		mesh->applyMap(time_operator(_t, new_t), time_operator_normal(_t, new_t));
	_t = new_t;
}

void MeshFamily1P::meshDeformation(float dt) {
	transformMesh(_t + dt);
}

MeshFamily1PExtraDomain::MeshFamily1PExtraDomain(const shared_ptr<TriangularMesh> &mesh, const string &domain,
	const std::function<std::function<vec3(vec3)>(float, float)> &time_operator,
	const std::function<std::function<vec3(vec3, vec3)>(float, float)> &time_operator_normal, float t) :
 MeshFamily1P(mesh,  time_operator, time_operator_normal, t) {
	this->domain = domain;
}

MeshFamily1PExtraDomain::MeshFamily1PExtraDomain(const shared_ptr<TriangularMesh> &embedded_mesh, const string &domain,
	const std::function<PlaneSmoothEndomorphism(float, float)> &time_operator, vec3 embedding_shift, float t) :
	MeshFamily1P(embedded_mesh, time_operator, embedding_shift, t) {
	this->domain = domain;
}

MeshFamily1PExtraDomain::MeshFamily1PExtraDomain(const shared_ptr<TriangularMesh> &embedded_mesh, const string &domain,
	const std::function<PlaneAutomorphism(float)> &time_operator_from0, vec3 embedding_shift) : MeshFamily1P(embedded_mesh, time_operator_from0, embedding_shift) {
	this->domain = domain;
}

void MeshFamily1PExtraDomain::transformMesh(float new_t) {
	if (planar)
		mesh->applyMap(time_operator(_t, new_t), domain);
	else
		mesh->applyMap(time_operator(_t, new_t), time_operator_normal(_t, new_t), domain);
	_t = new_t;
}

void MeshFamily1PExtraDomain::meshDeformation(float dt) {
	transformMesh(_t + dt);
}







SuperMesh::SuperMesh() { }

SuperMesh::SuperMesh(TriangularMesh &mesh) {
	triangleGroups[PolyGroupID(0)] = mesh.getTriangles();
}

SuperMesh::SuperMesh(const char *filename, MeshFormat format) {
	auto mesh = TriangularMesh(filename, format);
	triangleGroups[PolyGroupID(0)] = mesh.getTriangles();

}

SuperMesh::SuperMesh(TriangularMesh &mesh, const MaterialPhongConstColor &material) : SuperMesh(mesh) {
	doPerTriangle([material](TriangleR3 &tr) {
		tr.addMaterial(material);
	});
}

SuperMesh::SuperMesh(const char *filename, const MaterialPhongConstColor &material, MeshFormat format) : SuperMesh(filename, format) {
	materials[PolyGroupID(0)] = material;
}


SuperMesh::SuperMesh(PlanarMeshWithBoundary &mesh, const MaterialPhongConstColor &material, MaterialPhongConstColor &material_bd, const BoundaryEmbeddingStyle &style) {
	TriangularMesh embedded = mesh.embeddInR3(0.0f);
	auto a = PolyGroupID(0);
	triangleGroups[PolyGroupID(0)] = embedded.getTriangles();
	materials[PolyGroupID(0)] = material;
	auto bds = mesh.stylisedBoundaryEmbedding(style);
	for (int i = 0; i < bds.size(); i++)
	{
		boundaryGroups[bdGroup(i)] = bds[i].getTriangles();
		materials[bdGroup(i)] = material_bd;
	}
}

SuperMesh::SuperMesh(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	triangleGroups[PolyGroupID(0)] = triangles;
	materials[PolyGroupID(0)] = material;
}

SuperMesh::SuperMesh(const std::map<PolyGroupID, vector<TriangleR3>> &triangleGroups, const std::map<PolyGroupID, MaterialPhongConstColor> &materials) {
	this->triangleGroups = triangleGroups;
	this->materials = materials;

}

void SuperMesh::addPolyGroup(PolyGroupID id, const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	triangleGroups[id] = triangles;
	materials[id] = material;
}

void SuperMesh::addBdGroup(PolyGroupID id, const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	boundaryGroups[id] = triangles;
	materials[id] = material;
}

void SuperMesh::addEmbeddedCurve(PolyGroupID id, const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	embedded_curves[id] = triangles;
	materials[id] = material;
}

void SuperMesh::addEmbeddedPoint(PolyGroupID id, const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	embedded_points[id] = triangles;
	materials[id] = material;
}

void SuperMesh::addPolyGroup(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	PolyGroupID id = static_cast<int>(triangleGroups.size());
	addPolyGroup(id, triangles, material);
}

void SuperMesh::addPolyGroup(const TriangularMesh &mesh, const MaterialPhongConstColor &material) {
	addPolyGroup(mesh.getTriangles(), material);
}

void SuperMesh::addBdGroup(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	addBdGroup(bdGroup(boundaryGroups.size()), triangles, material);
}

void SuperMesh::addEmbeddedCurve(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	addEmbeddedCurve(curveGroup(embedded_curves.size()), triangles, material);
}

void SuperMesh::addEmbeddedPoint(const vector<TriangleR3> &triangles, const MaterialPhongConstColor &material) {
	addEmbeddedPoint(static_cast<int>(-embedded_points.size() - 1), triangles, material);
}

void SuperMesh::embedCurve(ComplexCurve *curve, int nSegments, float h_middle, float w_middle, float w_side, const MaterialPhongConstColor &material) {
	auto embedded = TriangularMesh(curve, nSegments, h_middle, w_middle, w_side);
	PolyGroupID id = curveGroup(embedded_curves.size());
	embedded_curves[id] = embedded.getTriangles();
	materials[id] = material;
}

void SuperMesh::precomputeBuffers(bool materials, bool extra) {
	if (this->stdAttributeBuffers.positions.size() == 0) {
		this->stdAttributeBuffers = { vector<vec3>(), vector<vec3>(), vector<vec4>(), vector<vec2>() };
		doPerTriangle([this](const TriangleR3 &tr) {
			for (int i = 0; i < 3; i++) {
				this->stdAttributeBuffers.positions.push_back(tr.vertices[i]);
				this->stdAttributeBuffers.normals.push_back(tr.normals[i]);
				this->stdAttributeBuffers.colors.push_back(tr.vertexColors[i]);
				this->stdAttributeBuffers.uvs.push_back(tr.uvs[i]);
			}
		});
		this->bufferLocations = { &this->stdAttributeBuffers.positions[0],
			&this->stdAttributeBuffers.normals[0],
			&this->stdAttributeBuffers.colors[0], &this->stdAttributeBuffers.uvs[0] };

		this->bufferSizes = {   (int) this->stdAttributeBuffers.positions.size(), (int) this->stdAttributeBuffers.normals.size(),
			                    (int) this->stdAttributeBuffers.colors.size(),    (int) this->stdAttributeBuffers.uvs.size() };
	}

	if (materials && this->materialBuffers.ambientColors.size() == 0) {
		this-> materialBuffers = {vector<vec4>(), vector<vec4>(), vector<vec4>(), vector<vec4>()};

		for (auto &pair : this->triangleGroups)
		{
			PolyGroupID id = pair.first;
			MaterialPhongConstColor material = this->materials[id];
			for (int i = 0; i < 3*pair.second.size(); i++) {
				this->materialBuffers.ambientColors.push_back(material.ambientColor);
				this->materialBuffers.diffuseColors.push_back(material.diffuseColor);
				this->materialBuffers.specularColors.push_back(material.specularColor);
				this->materialBuffers.intencitiesAndShininess.push_back(material.compressIntencities());
				}

		}
		for (auto &pair : this->boundaryGroups)
		{
			PolyGroupID id = pair.first;
			MaterialPhongConstColor material = this->materials[id];
			for (int i = 0; i < 3*pair.second.size(); i++) {
				this->materialBuffers.ambientColors.push_back(material.ambientColor);
				this->materialBuffers.diffuseColors.push_back(material.diffuseColor);
				this->materialBuffers.specularColors.push_back(material.specularColor);
				this->materialBuffers.intencitiesAndShininess.push_back(material.compressIntencities());
			}

		}
		for (auto &pair : this->embedded_curves)
		{
			PolyGroupID id = pair.first;
			MaterialPhongConstColor material = this->materials[id];
			for (int i = 0; i < 3*pair.second.size(); i++) {
				this->materialBuffers.ambientColors.push_back(material.ambientColor);
				this->materialBuffers.diffuseColors.push_back(material.diffuseColor);
				this->materialBuffers.specularColors.push_back(material.specularColor);
				this->materialBuffers.intencitiesAndShininess.push_back(material.compressIntencities());
			}

		}
		for (auto &pair : this->embedded_points)
		{
			PolyGroupID id = pair.first;
			MaterialPhongConstColor material = this->materials[id];
			for (int i = 0; i < 3*pair.second.size(); i++) {
				this->materialBuffers.ambientColors.push_back(material.ambientColor);
				this->materialBuffers.diffuseColors.push_back(material.diffuseColor);
				this->materialBuffers.specularColors.push_back(material.specularColor);
				this->materialBuffers.intencitiesAndShininess.push_back(material.compressIntencities());
			}

		}


		this->bufferLocations = { &this->stdAttributeBuffers.positions[0],
					&this->stdAttributeBuffers.normals[0],
					&this->stdAttributeBuffers.colors[0], &this->stdAttributeBuffers.uvs[0],
					&this->materialBuffers.ambientColors[0], &this->materialBuffers.diffuseColors[0],
					&this->materialBuffers.specularColors[0], &this->materialBuffers.intencitiesAndShininess[0] };

		this->bufferSizes = {   (int) this->stdAttributeBuffers.positions.size(),  (int) this->stdAttributeBuffers.normals.size(),
				                (int) this->stdAttributeBuffers.colors.size(),     (int) this->stdAttributeBuffers.uvs.size(),
				                (int) this->materialBuffers.ambientColors.size(),  (int) this->materialBuffers.diffuseColors.size(),
				                (int) this->materialBuffers.specularColors.size(), (int) this->materialBuffers.intencitiesAndShininess.size() };
	}
}





void SuperMesh::merge(const SuperMesh &other) {
	auto prefix_rand = randomStringNumeric();
	for (auto &pair : other.triangleGroups)
		triangleGroups[prefix(pair.first, prefix_rand)] = pair.second;
	for (auto &pair : other.boundaryGroups)
		boundaryGroups[prefix(pair.first, prefix_rand)] = pair.second;
	for (auto &pair : other.embedded_curves)
		embedded_curves[prefix(pair.first, prefix_rand)] = pair.second;

	for (auto &pair : other.embedded_points)
		embedded_points[prefix(pair.first, prefix_rand)] = pair.second;
	for (auto &pair : other.materials)
		materials[prefix(pair.first, prefix_rand)] = pair.second;
	for (auto &pair : other.extraBuff)
		extraBuff[pair.first] = pair.second;
	for (auto &pair : other.extraBufferIndices)
		extraBufferIndices[pair.first] = pair.second;
	// stdAttributeBuffers = {};
	// materialBuffers = {};
}

void SuperMesh::precomputeExtraBuffer(string name) {
	doPerTriangle([this, name](TriangleR3 &tr) {
			for (int i = 0; i < 3; i++) {
				this->extraBuff[name].push_back(tr.getExtraData(name)[i]);
			}
		});
}


void SuperMesh::actOnPositions(std::function<vec3(vec3)> f) {
	doPerTriangle([f](TriangleR3 &tr) {
		for (int i = 0; i < 3; i++) {
			tr.vertices[i] = f(tr.vertices[i]);
		}
	});
	for (int i = 0; i < stdAttributeBuffers.positions.size(); i++) {
		stdAttributeBuffers.positions[i] = f(stdAttributeBuffers.positions[i]);
	}
}

void SuperMesh::actAtEmbeddedPlane(const Meromorphism& f) {
	auto ff = [f](vec3 w) {
		vec2 p = vec2(w.x, w.y);
		vec2 q = f(p).z;
		return vec3(q.x, q.y, w.z);
	};
	doPerTriangle([ff](TriangleR3 &tr) {
		for (int i = 0; i < 3; i++) {
			tr.vertices[i] = ff(tr.vertices[i]);
		}
	});
	for (int i = 0; i < stdAttributeBuffers.positions.size(); i++) {
		stdAttributeBuffers.positions[i] = ff(stdAttributeBuffers.positions[i]);
	}
}



void SuperMesh::translate(vec3 v) {
	doPerTriangle([v](TriangleR3 &tr) {
		for (int i = 0; i < 3; i++) {
			tr.vertices[i] += v;
		}
	});
}

void SuperMesh::doPerTriangle(const std::function<void(TriangleR3 &)> &f) {
  for (auto &pair : triangleGroups)
    for (TriangleR3 &tr : pair.second)
      f(tr);
  for (auto &pair : boundaryGroups)
    for (TriangleR3 &tr : pair.second)
      f(tr);
  for (auto &pair : embedded_curves)
    for (TriangleR3 &tr : pair.second)
      f(tr);
  for (auto &pair : embedded_points)
    for (TriangleR3 &tr : pair.second)
      f(tr);
}
void SuperMesh::doPerTriangle(std::variant<int, string> grp, const std::function<void(TriangleR3 &)> &f) {
    if (triangleGroups.contains(grp))
        for (TriangleR3 &tr: triangleGroups[grp])
            f(tr);
    if (boundaryGroups.contains(grp))
        for (TriangleR3 &tr: triangleGroups[grp])
            f(tr);
    if (embedded_curves.contains(grp))
        for (TriangleR3 &tr: triangleGroups[grp])
            f(tr);
    if (embedded_points.contains(grp))
        for (TriangleR3 &tr: triangleGroups[grp])
            f(tr);
}


void SuperMesh::actOnEmbeddedCurve(const SpaceEndomorphism &f, bool buffOnly) {
    if(!buffOnly)
        for (auto &pair : embedded_curves)
		    for (TriangleR3 &tr : pair.second) {

			    for (int i = 0; i < 3; i++) {
				    vec3 v = vec3(tr.getExtraData("curvePoint")[i]);
				    vec3 p = f(v);
				    tr.vertices[i] = p + tr.vertices[i] - v;
			    }
		    }
	for (int i = 0; i < stdAttributeBuffers.positions.size(); i++) {
		stdAttributeBuffers.normals[i] = f.df(vec3(extraBuff["curvePoint"][i]))*stdAttributeBuffers.normals[i];
		stdAttributeBuffers.positions[i] = f(vec3(extraBuff["curvePoint"][i])) + stdAttributeBuffers.positions[i] - vec3(extraBuff["curvePoint"][i]);
		extraBuff["curvePoint"][i] = vec4(f(vec3(extraBuff["curvePoint"][i])), extraBuff["curvePoint"][i].w);
	}
}

void SuperMesh::randomizeMaterials(MaterialPhongConstColor &min, MaterialPhongConstColor &max) {
	// precomputeBuffers(true, false);

	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> distribution(0, 1);
	for (int i = 0; i < materialBuffers.ambientColors.size(); i++) {
		materialBuffers.ambientColors[i] = vec4(distribution(generator)*(max.ambientColor.x - min.ambientColor.x) + min.ambientColor.x,
			distribution(generator)*(max.ambientColor.y - min.ambientColor.y) + min.ambientColor.y,
			distribution(generator)*(max.ambientColor.z - min.ambientColor.z) + min.ambientColor.z, 1.0f);
		materialBuffers.diffuseColors[i] =  vec4(distribution(generator)*(max.diffuseColor.x - min.diffuseColor.x) + min.diffuseColor.x,
			distribution(generator)*(max.diffuseColor.y - min.diffuseColor.y) + min.diffuseColor.y,
			distribution(generator)*(max.diffuseColor.z - min.diffuseColor.z) + min.diffuseColor.z, 1.0f);
		materialBuffers.specularColors[i] = vec4(distribution(generator)*(max.specularColor.x - min.specularColor.x) + min.specularColor.x,
			distribution(generator)*(max.specularColor.y - min.specularColor.y) + min.specularColor.y,
			distribution(generator)*(max.specularColor.z - min.specularColor.z) + min.specularColor.z, 1.0f);
		materialBuffers.intencitiesAndShininess[i] = max.compressIntencities();
	}
}

void SuperMesh::randomiseMaterialsDynamically(float stepMax) {
    precomputeBuffers(true, false);

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<float> distribution(-stepMax, stepMax);
    for (int i = 3; i < materialBuffers.ambientColors.size(); i += 3) {
        float p = distribution(generator);
        materialBuffers.ambientColors[i] += materialBuffers.ambientColors[i - 1] * p;
        materialBuffers.ambientColors[i + 1] += materialBuffers.ambientColors[i - 1] * p;
        materialBuffers.ambientColors[i + 2] += materialBuffers.ambientColors[i - 1] * p;
        p = distribution(generator);
        materialBuffers.diffuseColors[i] += materialBuffers.diffuseColors[i - 1] * p;
        materialBuffers.diffuseColors[i + 1] += materialBuffers.diffuseColors[i - 1] * p;
        materialBuffers.diffuseColors[i + 2] += materialBuffers.diffuseColors[i - 1] * p;
        p = distribution(generator);
        materialBuffers.specularColors[i] += materialBuffers.specularColors[i - 1] * p;
        materialBuffers.specularColors[i + 1] += materialBuffers.specularColors[i - 1] * p;
        materialBuffers.specularColors[i + 2] += materialBuffers.specularColors[i - 1] * p;
        p = distribution(generator);
        materialBuffers.intencitiesAndShininess[i] += materialBuffers.intencitiesAndShininess[i - 1] * p;
        materialBuffers.intencitiesAndShininess[i + 1] += materialBuffers.intencitiesAndShininess[i - 1] * p;
        materialBuffers.intencitiesAndShininess[i + 2] += materialBuffers.intencitiesAndShininess[i - 1] * p;
    }
}




















void SuperPencilPlanar::makePencil(const std::function<Biholomorphism(float)> &_time_operator, float t) {
	this->_time_operator = _time_operator;
	this->_t = t;
}

void SuperPencilPlanar::transformMesh(float new_t) {
	actAtEmbeddedPlane(_time_operator(new_t).compose(_time_operator(_t).inv()));
	_t = new_t;
}

void SuperPencilPlanar::deformMesh(float dt) {
	transformMesh(_t + dt);
	_t += dt;
}


Texture::Texture(int width, int height, int bpp, int slot, const char* sampler)
{
	this->width = width;
	this->height = height;
	this->data = nullptr;
	this->bpp = bpp;

	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	this->textureSlot = GL_TEXTURE0 + slot;
	abs_slot = slot;
	this->samplerName = sampler;
	this->frameBufferID = 0;
	glGenFramebuffers(1, &frameBufferID);
}

Texture::Texture(vec4 color, int slot, const char *sampler) {
    width = 1;
    height = 1;
    bpp = 4;
    this->textureSlot = GL_TEXTURE0 + slot;
	abs_slot = slot;
    this->samplerName = sampler;
    this->frameBufferID = 0;
    this->textureID = 0;

    data = new unsigned char[4];
    data[0] = (unsigned char) (color.z * 255);
    data[1] = (unsigned char) (color.y * 255);
    data[2] = (unsigned char) (color.x * 255);
    data[3] = (unsigned char) (color.w * 255);

	load();
}

Texture::Texture(const char* filename, int slot, const char* sampler, bool alpha)
{
	unsigned char header[54];
	FILE* file = fopen(filename, "rb");

	if (!file)
		throw FileNotFoundError(filename, __FILE__, __LINE__);

	if (fread(header, 1, 54, file) != 54)
		throw InvalidFileError(filename, "Not a correct BMP file (54 bytes header not found)", __FILE__, __LINE__);

	if (header[0] != 'B' || header[1] != 'M')
		throw InvalidFileError(filename, "Not a correct BMP file (first two bytes of header invalid)", __FILE__, __LINE__);


    bpp = alpha ? 4 : 3;
	unsigned int dataPos = *(int*)&(header[0x0A]);
	int size = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (size == 0)
		size = width * height * bpp;
	if (dataPos == 0)
		dataPos = 54;

	this->data = new unsigned char[size];
	fread(this->data, 1, size, file);
	fclose(file);

	this->textureSlot = GL_TEXTURE0 + slot;
	abs_slot = slot;
	this->samplerName = sampler;
	this->frameBufferID = 0;
    this->textureID = 0;

	load();
}

Texture::Texture(const string &filename, int slot, const char *sampler, bool alpha)
: Texture(filename.c_str(), slot, sampler, alpha) {}

void Texture::deleteTexture()
{
	free(this->data);
	glDeleteTextures(1, &this->textureID);
}

Texture::~Texture() {
	deleteTexture();
}

void Texture::addFilters(GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT) {
	if (minFilter == GL_LINEAR_MIPMAP_LINEAR)
		glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

void Texture::bind() const
{
	glActiveTexture(this->textureSlot);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
}


void Texture::bindToFrameBuffer() const {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferID);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textureID, 0);
	glViewport(0, 0, this->width, this->height);
}

void Texture::calculateMipmap() const {
	glGenerateMipmap(this->textureID);
}

void Texture::load() {

    glGenTextures(1, &this->textureID);
	glActiveTexture(this->textureSlot);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
	addFilters( GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

    if (bpp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width, this->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, this->data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, this->width, this->height, 0, GL_BGR, GL_UNSIGNED_BYTE, this->data);

    glGenerateMipmap(GL_TEXTURE_2D);
}


//
// IndexedMesh::IndexedMesh(const char* filename, MeshFormat format)
// {
// 	this->posBuff = vector<vec3>();
// 	this->normBuff = vector<vec3>();
// 	this->colorBuff = vector<vec4>();
// 	this->uvBuff = vector<vec2>();
//
// 	if (format == 0) {
// 		auto pos = vector<vec3>();
// 		auto norms = vector<vec3>();
// 		auto uvs = vector<vec2>();
// 		set<Vertex> uniqueVertices = set<Vertex>();
//
// 		FILE* file = fopen(filename, "r");
// 		if (file == NULL) {
// 			printf("Impossible to open the file !\n");
// 			return;
// 		}
//
// 		while (1) {
// 			char lineHeader[1024];
// 			// read the first word of the line
// 			int res = fscanf(file, "%s", lineHeader);
// 			if (res == EOF)
// 				break;
//
// 			if (strcmp(lineHeader, "v") == 0) {
// 				vec3 vertex;
// 				fscanf(file, "%_f %_f %_f\n", &vertex.x, &vertex.y, &vertex.z);
// 				pos.push_back(vertex);
// 			}
//
// 			else if (strcmp(lineHeader, "vt") == 0) {
// 				vec2 uv;
// 				fscanf(file, "%_f %_f\n", &uv.x, &uv.y);
// 				uvs.push_back(uv);
// 			}
// 			else if (strcmp(lineHeader, "vn") == 0) {
// 				vec3 normal;
// 				fscanf(file, "%_f %_f %_f\n", &normal.x, &normal.y, &normal.z);
// 				norms.push_back(normal);
// 			}
//
// 			else if (strcmp(lineHeader, "_f") == 0) {
// 				ivec3 posIndex, uvIndex, normalIndex;
// 				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &posIndex[0], &uvIndex[0], &normalIndex[0], &posIndex[1], &uvIndex[1], &normalIndex[1], &posIndex[2], &uvIndex[2], &normalIndex[2]);
// 				if (matches != 9) {
// 					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
// 					return;
// 				}
// 				posIndex -= ivec3(1, 1, 1);
// 				uvIndex -= ivec3(1, 1, 1);
// 				normalIndex -= ivec3(1, 1, 1);
//
// 				uniqueVertices.insert(Vertex(pos.at(posIndex[0]), norms.at(normalIndex[0]), uvs.at(uvIndex[0])));
// 				uniqueVertices.insert(Vertex(pos.at(posIndex[1]), norms.at(normalIndex[1]), uvs.at(uvIndex[1])));
// 				uniqueVertices.insert(Vertex(pos.at(posIndex[2]), norms.at(normalIndex[2]), uvs.at(uvIndex[2])));
// 			}
// 		}
//
// 		this->vertices = vector<Vertex>(uniqueVertices.begin(), uniqueVertices.end());
//
// 		file = fopen(filename, "r");
// 		this->faceIndices = vector<array<int, 3>>();
// 		this->faceIndices.reserve(this->vertices.size());
//
// 		while (1) {
// 			char lineHeader[1024];
// 			int res = fscanf(file, "%s", lineHeader);
// 			if (res == EOF)
// 				break;
//
// 			if (strcmp(lineHeader, "v") == 0) {
// 				vec3 vertex;
// 				fscanf(file, "%_f %_f %_f\n", &vertex.x, &vertex.y, &vertex.z);
//
// 			}
//
// 			else if (strcmp(lineHeader, "vt") == 0) {
// 				vec2 uv;
// 				fscanf(file, "%_f %_f\n", &uv.x, &uv.y);
//
// 			}
// 			else if (strcmp(lineHeader, "vn") == 0) {
// 				vec3 normal;
// 				fscanf(file, "%_f %_f %_f\n", &normal.x, &normal.y, &normal.z);
//
// 			}
//
// 			if (strcmp(lineHeader, "_f") == 0) {
// 				ivec3 posIndex, uvIndex, normalIndex;
// 				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &posIndex[0], &uvIndex[0], &normalIndex[0], &posIndex[1], &uvIndex[1], &normalIndex[1], &posIndex[2], &uvIndex[2], &normalIndex[2]);
// 				if (matches != 9) {
// 					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
// 					return;
// 				}
// 				posIndex -= ivec3(1, 1, 1);
// 				uvIndex -= ivec3(1, 1, 1);
// 				normalIndex -= ivec3(1, 1, 1);
//
// 				auto inds = array<int, 3>();
// 				for (int i = 0; i < 3; i++)
// 					inds[i] = binSearch(this->vertices, Vertex(pos.at(posIndex[i]), norms.at(normalIndex[i]), uvs.at(uvIndex[i])));
//
// 				this->faceIndices.push_back(inds);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		std::cout << "Format not supported" << std::endl;
// 	}
// }






SuperCurve::SuperCurve(const SmoothParametricCurve &curve, const std::function<float(float)> &width,
                       const std::function<MaterialPhongConstColor(float)> &material, int segments,
                       float t0, float t1, bool periodic) {
    samples = sampleCurve(curve, width, material, t0, t1, segments, periodic);
    this->t0 = t0;
    this->t1 = t1;
    id = curve.getID();
}
SuperCurve::SuperCurve(const SmoothParametricCurve &curve, float width, MaterialPhongConstColor material, int nSegments, float t0, float t1, bool periodic) {
    samples = sampleCurve(curve, width, material, t0, t1, nSegments, periodic);
    this->t0 = t0;
    this->t1 = t1;
    id = curve.getID();
}
SuperMesh SuperCurve::generateMeshTube(int radialSegments) const {
	SuperMesh mesh = SuperMesh();
	 for (int i = 0; i < samples.size()-1; i++) {
		for (int j = 0; j < radialSegments; j++) {
			float theta0 = TAU * j / radialSegments;
			float theta1 = TAU * (j + 1) / radialSegments;
			vec3 n0a = cos(theta0) *  samples[i].getNormal() + sin(theta0) * samples[i].getBinormal();
			vec3 n1a = cos(theta1) * samples[i].getNormal() + sin(theta1) * samples[i].getBinormal();
			vec3 n0b = cos(theta0) * samples[i + 1].getNormal() + sin(theta0) * samples[i + 1].getBinormal();
			vec3 n1b = cos(theta1) * samples[i + 1].getNormal() + sin(theta1) * samples[i + 1].getBinormal();

			vec3 pos_a = samples[i].getPosition();
			vec3 pos_b = samples[i + 1].getPosition();

			vec3 p0a = pos_a + samples[i].getWidth() * n0a;
			vec3 p0b = pos_b + samples[i + 1].getWidth() * n0b;
			vec3 p1b = pos_b + samples[i + 1].getWidth() * n1b;
			vec3 p1a = pos_a + samples[i].getWidth() * n1a;
			auto tr1 = TriangleR3({p0a, p1a, p0b}, vector({n0a, n1a, n0b}));
			auto tr2 = TriangleR3({p0b, p1a, p1b}, vector({n0b, n1a, n1b}));
			float t_a = t0 + (t1 - t0) * i / samples.size();
			float t_b = t_a + (t1 - t0) / samples.size();

		    vec3 na = samples[i].getNormal();
		    vec3 nb = samples[i + 1].getNormal();
		    vec3 ta = samples[i].getTangent();
		    vec3 tb = samples[i + 1].getTangent();

			tr1.addExtraData("curvePoint", {vec4(pos_a, t_a), vec4(pos_a, t_a), vec4(pos_b, t_b)});
			tr2.addExtraData("curvePoint", {vec4(pos_b, t_b), vec4(pos_a, t_a), vec4(pos_b, t_b)});
		    tr1.addExtraData("curveNormal", {vec4(na, cos(theta0)), vec4(na, cos(theta1)), vec4(nb, cos(theta0))});
		    tr2.addExtraData("curveNormal", {vec4(nb, cos(theta0)), vec4(na, cos(theta1)), vec4(nb, cos(theta1))});
		    tr1.addExtraData("curveTangent", {vec4(ta, theta0), vec4(ta, theta1), vec4(tb, theta0)});
		    tr2.addExtraData("curveTangent", {vec4(tb, theta0), vec4(ta, theta1), vec4(tb, theta1)});

			mesh.addEmbeddedCurve({tr1, tr2}, samples[i].getMaterial());
		}
	 }
	return mesh;
}

void SuperCurve::generateMesh(int radialSegments, CurveEmbeddingTypeID type) {
    embeddingType = type;
    switch (type) {
        case TUBE:
            _mesh = std::make_shared<SuperMesh>(generateMeshTube(radialSegments));
        break;
        case PLANAR:
            throw NotImplementedVariantError("PLANAR", "curve embedding as SuperMesh", __FILE__, __LINE__);
        case NOT_EMBEDDED:
            throw IllegalVariantError("NOT_EMBEDDED", "embedding type", "generating embedding as SuperMesh", __FILE__, __LINE__);
        default:
            throw UnknownVariantError(embeddingTypeName(type), "curve embedding as SuperMesh", __FILE__, __LINE__);

    }
}

void SuperCurve::transformMeshByAmbientMap(const SpaceEndomorphism &f) {
	if (_mesh != nullptr)
		_mesh->actOnEmbeddedCurve(f);
}





 vector<CurveSample> sampleCurve(SmoothParametricCurve curve, std::function<float(float)> width,
                                     std::function<MaterialPhongConstColor(float)> material, float t0, float t1, int n, bool periodic) {
	vector<CurveSample> samples = vector<CurveSample>();
	samples.reserve(n);
	for (int i = 0; i <= n; i++) {
		float t = t0 + (t1 - t0) * i / n;
		vec3 pos = curve(t);
		vec3 tangent = curve.tangent(t);
		vec3 normal = curve.normal(t);
		float w = width(t);

		MaterialPhongConstColor mat = material(t);
		samples.emplace_back(pos, normal, tangent, mat, w);
		samples.at(i).updateExtra(t);
	}
	if (periodic) {
		vec3 pos = curve(t0);
		vec3 tangent = curve.tangent(t0);
		vec3 normal = curve.normal(t0);
		float w = width(t0);
		MaterialPhongConstColor mat = material(t0);
		samples.push_back(CurveSample(pos, normal, tangent, mat, w));
		samples.at(n).updateExtra(t0);
	}
	return samples;
}

CurveSample::CurveSample(const CurveSample &other) :
    position(other.position), normal(other.normal), tangent(other.tangent), material(other.material), width(other.width),
    extraInfo(other.extraInfo) {}

CurveSample::CurveSample(CurveSample &&other) noexcept :
    position(std::move(other.position)), normal(std::move(other.normal)), tangent(std::move(other.tangent)),
    material(std::move(other.material)), width(other.width), extraInfo(std::move(other.extraInfo)) {}

CurveSample &CurveSample::operator=(const CurveSample &other) {
    if (this == &other)
        return *this;
    position = other.position;
    normal = other.normal;
    tangent = other.tangent;
    material = other.material;
    width = other.width;
    extraInfo = other.extraInfo;
    return *this;
}

CurveSample &CurveSample::operator=(CurveSample &&other) noexcept {
    if (this == &other)
        return *this;
    position = std::move(other.position);
    normal = std::move(other.normal);
    tangent = std::move(other.tangent);
    material = std::move(other.material);
    width = other.width;
    extraInfo = std::move(other.extraInfo);
    return *this;
}

vector<CurveSample> sampleCurve(SmoothParametricCurve curve, float width,
                                MaterialPhongConstColor material, float t0, float t1,
                                int n, bool periodic) {
  return sampleCurve(
      curve, [w=width](float t) { return w; },
      [m=material](float t) { return m; }, t0, t1, n, periodic);
}

// SuperPencilCurve::SuperPencilCurve(const SuperCurve &c) : SuperPencilCurve(c){}

void SuperPencilCurve::addAmbientDeformation(End2P _ambient_operator, float t){
	this->_ambient_operator = make_unique<End2P>(_ambient_operator);
	this->_t = t;
}

void SuperPencilCurve::addLocalDeformation(End1P _local_operator, float t) {
	this->_ambient_operator = make_unique<End2P>([op=_local_operator](float t1, float t2) {return op(t2-t1); });
	this->_t = t;
}

void SuperPencilCurve::addDeformationAlongVectorField(VectorField vectorField, float t) {
	auto local_operator = [vectorField](float dt) {
		return SpaceEndomorphism([vectorField, dt](vec3 p) {
			return p + vectorField(p) * dt;
		});
	};
	addLocalDeformation(local_operator, t);
}

void SuperPencilCurve::addPencil(std::function<SmoothParametricCurve(float)> family, float t) {
    _parametric_operator = make_unique<std::function<SmoothParametricCurve(float)>>(family);
    _t = t;
}

void SuperCurve::precomputeBuffers() {
    if (_mesh != nullptr) {
        _mesh->precomputeBuffers();
        _mesh->precomputeExtraBuffer("curvePoint");
        _mesh->precomputeExtraBuffer("curveNormal");
        _mesh->precomputeExtraBuffer("curveTangent");
    }
    else
        throw std::logic_error("Curve has no associated mesh containing the buffers.");
}


std::function<void(float)> SuperCurve::pencilDeformerWeak(std::function<SmoothParametricCurve(float)> pencil) {
    return [c=pencil, this](float t) {
        auto curva = c(t);
        curva.setID(this->id);
        // updateCurve(curva);
    };
}

float SuperPencilCurve::time() const {
	return _t;
}

void SuperPencilCurve::transformMesh(float new_t) {
	if(_ambient_operator != nullptr)
		transformMeshByAmbientMap((*_ambient_operator)(_t, new_t));
	if (_parametric_operator != nullptr) {
		auto slice = (*_parametric_operator)(new_t);
		// updateCurve(slice);
	}
	_t = new_t;
}
