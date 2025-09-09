#pragma once
#include <array>
#include "../engine/specific.hpp"

enum FACE {DOWN = 0, UP = 1, RIGHT = 2, LEFT = 3, FRONT = 4, BACK = 5};

class Quadrilateral {
	Vertex p0, p1, p2, p3;
public:
	Quadrilateral(vec3 p0, vec3 p1, vec3 p2, vec3 p3) : p0(p0), p1(p1), p2(p2), p3(p3) {}
	std::array<Vertex, 4> getVertices() const;
	static std::array<glm::ivec3, 2> getTriangles() { return {glm::ivec3(0, 1, 2), glm::ivec3(0, 2, 3)}; }
};




inline FACE opposite(FACE f) {
	if (f == DOWN) return UP;
	if (f == UP) return DOWN;
	if (f == RIGHT) return LEFT;
	if (f == LEFT) return RIGHT;
	if (f == FRONT) return BACK;
	if (f == BACK) return FRONT;
	throw std::invalid_argument("Invalid face");
}


inline mat3 toMat3(const std::array<vec3, 3> &m) { return mat3(m[0], m[1], m[2]); }


typedef std::array<vec3, 3> arr33;

class IndexedQuadrilateral {
	glm::ivec4 indices;
	HOM(int, Vertex) bufferManagerInterface;
public:
	IndexedQuadrilateral(glm::ivec4 indices, HOM(int, Vertex) bufferManagerInterface) : indices(indices), bufferManagerInterface(std::move(bufferManagerInterface)) {}
	std::array<Vertex, 4> getVertices() const { return {bufferManagerInterface(indices[0]), bufferManagerInterface(indices[1]), bufferManagerInterface(indices[2]), bufferManagerInterface(indices[3])}; }
	std::array<glm::ivec3, 2> getTriangles() const { return {glm::ivec3(indices[0], indices[1], indices[2]), glm::ivec3(indices[0], indices[2], indices[3])}; }
	std::array<std::array<vec3, 3>, 2> getTriangleVertexPositions() const { return {bufferManagerInterface(indices[0]).getPosition(), bufferManagerInterface(indices[1]).getPosition(), bufferManagerInterface(indices[2]).getPosition()}; }
	float surfaceArea() const { std::array<arr33, 2> v = getTriangleVertexPositions(); return abs(det(toMat3(v[0]))) +  abs(det(toMat3(v[1]))); }
	std::array<vec3, 4> vertexPositions() const {return {bufferManagerInterface(indices[0]).getPosition(), bufferManagerInterface(indices[1]).getPosition(), bufferManagerInterface(indices[2]).getPosition(), bufferManagerInterface(indices[3]).getPosition()}; }
	vec3 vertexPosition(int i) const { return bufferManagerInterface(indices[i]).getPosition(); }
	vec3 normal() const { return normalize(cross(vertexPosition(1) - vertexPosition(0), vertexPosition(2) - vertexPosition(0))); }
	glm::ivec4 getIndices() const { return indices; }
	int vertexIndex(int i) const { return indices[i]; }

	float supDistance (vec3 p) const;
	float supDistance (const IndexedQuadrilateral &q) const;
};




class IndexedHexahedron {
		ivec6 facesInd;
		HOM(int, IndexedQuadrilateral) faceSampler;

public:
	IndexedHexahedron(const ivec6 &faces, HOM(int, IndexedQuadrilateral) faceCallback) : facesInd(faces), faceSampler(std::move(faceCallback)) {}
	IndexedHexahedron(int down, int up, int right, int left, int front, int back, HOM(int, IndexedQuadrilateral) faceCallback) : facesInd(down, up, right, left, front, back), faceSampler(std::move(faceCallback)) {}

	IndexedQuadrilateral getFaceDown() const { return faceSampler(facesInd[0]); }
	IndexedQuadrilateral getFaceUp()   const { return faceSampler(facesInd[1]); }
	IndexedQuadrilateral getFaceRight()const  { return faceSampler(facesInd[2]); }
	IndexedQuadrilateral getFaceLeft() const  { return faceSampler(facesInd[3]); }
	IndexedQuadrilateral getFaceFront()const  { return faceSampler(facesInd[4]); }
	IndexedQuadrilateral getFaceBack() const  { return faceSampler(facesInd[5]); }
	int vertexIndex(int i) const { return i < 5 ? getFaceDown().vertexIndex(i) : getFaceUp().vertexIndex(i-4); }
	std::array<Vertex, 8> getVertices() const { return {getFaceDown().getVertices()[0], getFaceDown().getVertices()[1], getFaceDown().getVertices()[2], getFaceDown().getVertices()[3], getFaceUp().getVertices()[0], getFaceUp().getVertices()[1], getFaceUp().getVertices()[2], getFaceUp().getVertices()[3]}; }
	std::array<vec3, 8> getVerticesPos() const {return {getFaceDown().getVertices()[0].getPosition(), getFaceDown().getVertices()[1].getPosition(), getFaceDown().getVertices()[2].getPosition(), getFaceDown().getVertices()[3].getPosition(), getFaceUp().getVertices()[0].getPosition(), getFaceUp().getVertices()[1].getPosition(), getFaceUp().getVertices()[2].getPosition(), getFaceUp().getVertices()[3].getPosition()}; }
	std::array<IndexedQuadrilateral, 6> getFaces() const { return {getFaceDown(), getFaceUp(), getFaceRight(), getFaceLeft(), getFaceFront(), getFaceBack()}; }
	IndexedQuadrilateral getFace(FACE fc) const { return fc == DOWN ? getFaceDown() : fc == UP ? getFaceUp() : fc == RIGHT ? getFaceRight() : fc == LEFT ? getFaceLeft() : fc == FRONT ? getFaceFront() : getFaceBack(); }
	std::array<glm::ivec3, 12> getTriangles() const;
	std::array<glm::ivec2, 8> getEdges() const;
	ivec8 vertexIndices() const { return ivec8(vertexIndex(0), vertexIndex(1), vertexIndex(2), vertexIndex(3), vertexIndex(4), vertexIndex(5), vertexIndex(6), vertexIndex(7)); }
	ivec6 faceIndices() const { return facesInd; }
	int getFaceIndex(FACE fc) const { return facesInd[fc]; }

	bool operator==(const IndexedHexahedron &q) const { return faceIndices() == q.faceIndices(); }

	float volume() const;
	float surfaceArea() const;
	vec3 centerOfFace(FACE i) const;
	vec3 centerOfMass() const;
	float radiusOfFaceCenter(FACE i) const;
	mat3 inertiaTensor(float mass) const;
	mat3 inertiaTensor(std::array<float, 8> vertMasses) const;
	mat3 inertiaTensorAroundPoint(vec3 p) const;
	bool contains(vec3 p) const;
	vec3 faceCenterNormalPoint(int i) const;
	vec3 interpolateFromVertices(std::array<float, 8> values) const;
	vec3 interpolateFromVertices(std::array<float, 8> values, vec3 p) const;
	vec3 interpolateFromEdges(std::array<float, 12> values) const;
	vec3 interpolateFromEdges(std::array<float, 12> values, vec3 p) const;
	vec3 interpolateFromFaces(std::array<float, 6> values) const;
	vec3 interpolateFromFaces(std::array<float, 6> values, vec3 p) const;
	HOM(int, SmoothParametricSurface) pencilOfSections() const;

	float supDistance (vec3 p) const;
	float supDistance (const IndexedHexahedron &q) const;
};


class HexahedronWithNbhd : public IndexedHexahedron {
	HOM(int, HexahedronWithNbhd*) hexSampler;
	std::map<FACE, int> nbhd;
public:
	HexahedronWithNbhd(const ivec6 &faces, HOM(int, IndexedQuadrilateral) faceCallback, HOM(int, HexahedronWithNbhd*) hexCallback, const std::map<FACE, int> &neighbours);

	void setNeighbour(FACE fc, int neighbour) { nbhd[fc] = neighbour; }
	bool hasNeighbour(FACE fc) const { return nbhd.contains(fc) && nbhd.at(fc) != -1; }
	HexahedronWithNbhd* getNeighbour(FACE fc) const { return nbhd.at(fc) == -1 ? nullptr : hexSampler(nbhd.at(fc)); }
	int getNeighbourIndex(FACE fc) const { return nbhd.at(fc); }
	std::vector<HexahedronWithNbhd*> getNeighbours() const;
	std::vector<int> getNeighboursIndices() const { return {nbhd.at(DOWN), nbhd.at(UP), nbhd.at(RIGHT), nbhd.at(LEFT), nbhd.at(FRONT), nbhd.at(BACK)}; }
};


class IndexedHexahedralVolumetricMesh {
	std::vector<Vertex> vertices;
	std::vector<HexahedronWithNbhd> hexahedra;
	std::vector<IndexedQuadrilateral> faces;
	HOM(int, Vertex) bufferManagerAccess;
	HOM(int, IndexedQuadrilateral) bufferManagerFace;
	HOM(int, HexahedronWithNbhd*) bufferManagerHexa;
public:
	IndexedHexahedralVolumetricMesh(const std::vector<HexahedronWithNbhd>& hexahedra, std::vector<IndexedQuadrilateral> faces, std::vector<Vertex> vertices);
	IndexedHexahedralVolumetricMesh(const std::vector<ivec6>& hexahedra, const std::vector<glm::ivec4> &faces, std::vector<Vertex> vertices);

	IndexedQuadrilateral initFace(glm::ivec4 indices) const { return IndexedQuadrilateral(indices, bufferManagerAccess); }
//	HexahedronWithNbhd initHexahedron(const ivec6 &ind) const { return HexahedronWithNbhd(ind, bufferManagerAccess, bufferManagerHexa, {}); }
	HexahedronWithNbhd initHexahedron(const ivec6 &ind, const std::map<FACE, int>& nbhd) const;

//	void addNeighbour(int nbhdInd, int target, FACE side) { hexahedra[target].setNeighbour(side, &nbhdInd); }

	void addVertex(const Vertex& v) { vertices.push_back(v); }
	void addCell(const HexahedronWithNbhd& h, const std::map<FACE, int>& nbhdFaces);
	void addFace(const IndexedQuadrilateral &q, bool bd);
	int size() const { return hexahedra.size(); }

	void applyPerCell(const HOM(HexahedronWithNbhd&, void)& f) { for (auto &hex : hexahedra) f(hex); }
	void subdivide();
	void subdivideCell(int i);

	HexahedronWithNbhd getCell(int i) const { return hexahedra[i]; }
	IndexedQuadrilateral getFace(int i) const { return faces[i]; }
	Vertex getVertex(int i) const { return vertices[i]; }
	std::map<FACE, IndexedQuadrilateral> getCellFaces(int i) const { return { {DOWN, hexahedra[i].getFaceDown()}, {UP, hexahedra[i].getFaceUp()}, {RIGHT, hexahedra[i].getFaceRight()}, {LEFT, hexahedra[i].getFaceLeft()}, {FRONT, hexahedra[i].getFaceFront()}, {BACK, hexahedra[i].getFaceBack()} }; }
	std::map<FACE, HexahedronWithNbhd*> getCellNeighbours(int i) const { return { {DOWN, hexahedra[i].getNeighbour(DOWN)}, {UP, hexahedra[i].getNeighbour(UP)}, {RIGHT, hexahedra[i].getNeighbour(RIGHT)}, {LEFT, hexahedra[i].getNeighbour(LEFT)}, {FRONT, hexahedra[i].getNeighbour(FRONT)}, {BACK, hexahedra[i].getNeighbour(BACK)} }; }
	std::array<Vertex, 8> getCellVertices(int i) const { return hexahedra[i].getVertices(); }
	std::array<Vertex, 4> getFaceVertices(int i) const { return faces[i].getVertices(); }

	IndexedHexahedralVolumetricMesh coproduct(const IndexedHexahedralVolumetricMesh &M) const;
	IndexedHexahedralVolumetricMesh glueFaces(const IndexedHexahedralVolumetricMesh &M, std::vector<glm::ivec2> glueMap) const;
	IndexedHexahedralVolumetricMesh glueCells(const IndexedHexahedralVolumetricMesh &M, std::vector<glm::ivec2> glueMap) const;
	IndexedHexahedralVolumetricMesh glueVertices(const IndexedHexahedralVolumetricMesh &M, std::vector<glm::ivec2> glueMap) const;

	IndexedHexahedralVolumetricMesh glueSimilarFaces(const IndexedHexahedralVolumetricMesh &M, float threshold) const;
	IndexedHexahedralVolumetricMesh glueSimilarCells(const IndexedHexahedralVolumetricMesh &M, float threshold) const;
	IndexedHexahedralVolumetricMesh glueSimilarVertices(const IndexedHexahedralVolumetricMesh &M, float threshold) const;
};

class HexVolumetricMeshWithBoundary{
public:
	IndexedHexahedralVolumetricMesh mesh;
	std::vector<int> bdVertices;
	std::shared_ptr<IndexedMesh> boundary;
	std::map<int, std::vector<int>> bdVerticesCells;
	PolyGroupID boundaryPolygroup = 0;

	explicit HexVolumetricMeshWithBoundary(const std::vector<ivec6> &hexahedra, const std::vector<glm::ivec4> &faces,
		const std::vector<Vertex> &vertices,
		const std::vector<int>& bdVertices);

	std::vector<int> getBoundaryCellIndsOfVertex(int i) const { return bdVerticesCells.at(i); }
};




















class Cell {

	int index;
	std::map<FACE, int> neighbours;
	std::map<std::array<FACE, 3>, vec3> corners;
	std::vector<FACE> bdFaces;
	std::map<std::string, float> attributes;
	PolyGroupID id;
public:

	Cell( int index,
		  const std::map<FACE, int> &neighbours,
		  const std::map<std::array<FACE, 3>, vec3> &corners,
		  const std::vector<FACE> &bdFaces,
		  const std::map<std::string, float> &attributes);

	std::array<vec3, 8> getCorners() const { return vectorToArray<vec3, 8>(valuesVec(corners)); }
	std::array<vec3, 4> getFaceCorners(FACE i) const;
	vec3 faceCenter(FACE i) const;
	vec3 center() const;
	float getFaceArea(FACE i) const;
	float attrValue(const std::string &attr) const { return attributes.at(attr); }
	std::vector<std::string> getAttributeNames() const { return keys(attributes); }

	float lengthBetweenFaceCenters(FACE dir) const;

	std::vector<FACE> getBdDirections() const { return bdFaces; }
	std::vector<std::array<vec3, 4>> getBdFaces() const;
	std::vector<std::array<Vertex, 4>> getBdVertices() const;
	bool isBdCell() const { return !bdFaces.empty(); }
	bool isBd(FACE i) const { return std::ranges::find(bdFaces, i) != bdFaces.end(); }

	int getIndex() const { return index; }
	Cell* getNeighbour(FACE i, std::vector<Cell> &cells) const { return neighbours.at(i) == -1 ? nullptr : &cells[neighbours.at(i)]; }
	float gradientOnFace(const std::string& attr, FACE i, std::vector<Cell> &cells) const;

	void updateAttribute(const std::string &attr, float value) { attributes[attr] = value; }
	void addToAttribute(const std::string &attr, float value) { attributes[attr] += value; }
	void updateIndex(int i) { index = i; }
	void increaseIndex() { index++; }
	void decreaseIndex() { index--; }

	void reindexing(int index, int newIndex);
	void reindexingAfterRemoval(int index);

	PolyGroupID getID() const { return id; }
};

class CellMesh {
public:
	std::vector<Cell> cells;
	std::vector<std::string> attributes;

	explicit CellMesh(const std::vector<Cell> &cells) : cells(cells), attributes(cells[0].getAttributeNames()) {}

	Cell* cellNeighbour(const Cell &c, FACE f) { return c.getNeighbour(f, cells); }
	float gradientOnFace(const Cell &c, const std::string &attr, FACE i) { return c.gradientOnFace(attr, i, cells); }
	float Laplacian(const Cell &c, const std::string &attr);
	IndexedMesh bdMesh(const std::vector<std::string>& attrSavedAsColor) const;
	static vec4 colorFromAttributes(const Cell &c, const std::vector<std::string>& attrSavedAsColor) ;

	static void updateBdMeshAtCell(const Cell &c, const std::vector<std::string>& attrSavedAsColor, IndexedMesh &mesh);
	void updateBdMesh(const std::vector<std::string>& attrSavedAsColor, IndexedMesh &mesh) const;

	void removeCell(int i);
};
