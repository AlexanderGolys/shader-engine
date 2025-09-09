#include "solidMeshes.hpp"

#include <map>

#include <utility>

using namespace glm;


std::array<ivec3, 12> IndexedHexahedron::getTriangles() const {
	auto res = std::array<ivec3, 12>();
	for (int i = 0; i < 6; i++) {
		auto face = getFace(FACE(i));
		auto tris = face.getTriangles();
		res[2 * i] = tris[0];
		res[2 * i + 1] = tris[1];
	}
	return res;
}


float IndexedHexahedron::volume() const {
	return abs(dot(getFaceDown().getVertices()[0].getPosition() - getFaceUp().getVertices()[0].getPosition(), cross(getFaceDown().getVertices()[1].getPosition() - getFaceUp().getVertices()[0].getPosition(), getFaceDown().getVertices()[2].getPosition() - getFaceUp().getVertices()[0].getPosition())))/6.f;
}
float IndexedHexahedron::surfaceArea() const {
	return sum<float, std::array<float, 6>>(mapByVal<IndexedQuadrilateral, float, 6>(getFaces(), [](IndexedQuadrilateral q) { return q.surfaceArea(); }));
}


vec3 IndexedHexahedron::centerOfMass() const {
	return sum<vec3, std::array<vec3, 8>>(getVerticesPos())/8.f;
}



vec3 IndexedHexahedron::centerOfFace(FACE i) const {
	return sum<vec3, std::array<vec3, 4>>(getFace(i).vertexPositions());
}

float IndexedHexahedron::radiusOfFaceCenter(FACE i) const {
	return norm(projectVectorToPlane(centerOfFace(i) - centerOfMass(), getFace(i).normal()));
}

mat3 IndexedHexahedron::inertiaTensor(float mass) const {
	auto perVert = [](vec3 pos) {
		return mat3( pos.y * pos.y + pos.z * pos.z, -pos.x * pos.y, -pos.x * pos.z,
						-pos.x * pos.y, pos.x * pos.x + pos.z * pos.z, -pos.y * pos.z,
						-pos.x * pos.z, -pos.y * pos.z, pos.x * pos.x + pos.y * pos.y ); };
	return sum<mat3, std::array<mat3, 8>>(map<vec3, mat3, 8>(getVerticesPos(), perVert)) * mass/8.f;
}

mat3 IndexedHexahedron::inertiaTensor(std::array<float, 8> vertMasses) const {
	auto perVert = [](vec3 pos) {
		return mat3( pos.y * pos.y + pos.z * pos.z, -pos.x * pos.y, -pos.x * pos.z,
						-pos.x * pos.y, pos.x * pos.x + pos.z * pos.z, -pos.y * pos.z,
						-pos.x * pos.z, -pos.y * pos.z, pos.x * pos.x + pos.y * pos.y ); } ;
	auto vert = getVerticesPos();
	return sum<mat3, std::array<mat3, 8>>(arrayComprehension<mat3, 8, HOM(int, mat3)>([&perVert, &vertMasses, &vert](int i){return perVert(vert[i]*vertMasses[i]);}));
}

mat3 IndexedHexahedron::inertiaTensorAroundPoint(vec3 p) const {throw std::logic_error("Not v implemented"); }; // TODO


bool IndexedHexahedron::contains(vec3 p) const { throw std::logic_error("Not implemented"); }
vec3 IndexedHexahedron::faceCenterNormalPoint(int i) const { throw std::logic_error("Not implemented"); }
vec3 IndexedHexahedron::interpolateFromVertices(std::array<float, 8> values) const { throw std::logic_error("Not implemented"); }
vec3 IndexedHexahedron::interpolateFromVertices(std::array<float, 8> values, vec3 p) const { throw std::logic_error("Not implemented"); }
vec3 IndexedHexahedron::interpolateFromEdges(std::array<float, 12> values) const { throw std::logic_error("Not implemented"); }
vec3 IndexedHexahedron::interpolateFromEdges(std::array<float, 12> values, vec3 p) const { throw std::logic_error("Not implemented"); }
vec3 IndexedHexahedron::interpolateFromFaces(std::array<float, 6> values) const { throw std::logic_error("Not implemented"); }
vec3 IndexedHexahedron::interpolateFromFaces(std::array<float, 6> values, vec3 p) const { throw std::logic_error("Not implemented"); }
std::function<SmoothParametricSurface(int)> IndexedHexahedron::pencilOfSections() const { throw std::logic_error("Not implemented"); }

HexahedronWithNbhd::HexahedronWithNbhd(const ivec6 &faces, HOM(int, IndexedQuadrilateral) faceCallback, HOM(int, HexahedronWithNbhd*) hexCallback, const std::map<FACE, int> &neighbours)
	: IndexedHexahedron(faces, std::move(faceCallback)), hexSampler(std::move(hexCallback)), nbhd(neighbours) { }


//std::vector<HexahedronWithNbhd *> HexahedronWithNbhd::getNeighbours() const {
//	std::vector<HexahedronWithNbhd*> nbhd = {};
//	for (auto &fc : {DOWN, UP, RIGHT, LEFT, FRONT, BACK})
//		if (hasNeighbour(fc))
//			nbhd.emplace_back(*getNeighbour(fc), hexSampler, std::map_cref<FACE, glm::ivec3*>());
//	return nbhd;
//}


void IndexedHexahedralVolumetricMesh::addCell(const HexahedronWithNbhd &h, const std::map<FACE, int> &nbhdFaces) {
	hexahedra.push_back(h);
	for (auto &fc : nbhdFaces)
		hexahedra.back().setNeighbour(fc.first, fc.second);
}


void IndexedHexahedralVolumetricMesh::subdivideCell(int i) {
	vec3 p1 = hexahedra[i].faceCenterNormalPoint(DOWN);
	vec3 p2 = hexahedra[i].faceCenterNormalPoint(UP);
	vec3 p3 = hexahedra[i].faceCenterNormalPoint(RIGHT);
	vec3 p4 = hexahedra[i].faceCenterNormalPoint(LEFT);
	vec3 p5 = hexahedra[i].faceCenterNormalPoint(FRONT);
	vec3 p6 = hexahedra[i].faceCenterNormalPoint(BACK);
	vec3 c = hexahedra[i].centerOfMass();

//	addPoint(p1);
//	auto lowRightBack  = IndexedHexahedron()
}

HexVolumetricMeshWithBoundary::HexVolumetricMeshWithBoundary(
	const vector<ivec6> &hexahedra,
	const vector<ivec4> &faces,
	const vector<Vertex> &vertices,
	const vector<int> &bdVertices):

mesh(hexahedra, faces, vertices),
bdVertices(bdVertices)

{
	vector<Vertex> bdVerticesRealised = {};
	bdVerticesRealised.reserve(bdVertices.size());
	vector<ivec3> bdTrs = {};
	std::map<int, int> newBdIndices = {};
	bdVerticesCells = {};

	for (int i: bdVertices){
		bdVerticesRealised.push_back(vertices[i]);
		newBdIndices[i] =  bdVerticesRealised.size()-1;
		bdVerticesCells[bdVerticesRealised.size()-1] = {};
	}

	for (ivec4 face: faces) {
		if (contains(bdVertices, face.x) && contains(bdVertices, face.y) && contains(bdVertices, face.z)) {
			bdTrs.emplace_back(newBdIndices[face.x], newBdIndices[face.y], newBdIndices[face.z]);
			bdTrs.emplace_back(newBdIndices[face.x], newBdIndices[face.w], newBdIndices[face.z]);
		}
	}

	for (int i = 0; i < hexahedra.size(); i++)
		for (int j = 0; j < 6; j++)
			for (int k = 0; k < 4; k++) {
				int kk = faces[hexahedra[i][j]][k];
				if (contains(bdVertices, kk))
					if (!contains(bdVerticesCells[newBdIndices[kk]], i))
						bdVerticesCells[newBdIndices[kk]].push_back(i);
			}

	boundary = make_shared<IndexedMesh>(bdVerticesRealised, bdTrs, boundaryPolygroup);
}

Cell::Cell(int index,
           const std::map<FACE, int> &neighbours,
           const std::map<std::array<FACE, 3>, vec3> &corners,
           const std::vector<FACE> &bdFaces,
           const std::map<std::string, float> &attributes):

		index(index),
		neighbours(neighbours),
		corners(corners),
		bdFaces(bdFaces),
		attributes(attributes) {
	id = randomID();
}

std::array<vec3, 4> Cell::getFaceCorners(FACE i) const {
	switch (i) {
		case DOWN: return {corners.at({DOWN, LEFT, FRONT}), corners.at({DOWN, RIGHT, FRONT}), corners.at({DOWN, RIGHT, BACK}), corners.at({DOWN, LEFT, BACK})};
		case UP: return {corners.at({UP, LEFT, FRONT}), corners.at({UP, RIGHT, FRONT}), corners.at({UP, RIGHT, BACK}), corners.at({UP, LEFT, BACK})};
		case LEFT: return {corners.at({DOWN, LEFT, FRONT}), corners.at({UP, LEFT, FRONT}), corners.at({UP, LEFT, BACK}), corners.at({DOWN, LEFT, BACK})};
		case RIGHT: return {corners.at({DOWN, RIGHT, FRONT}), corners.at({UP, RIGHT, FRONT}), corners.at({UP, RIGHT, BACK}), corners.at({DOWN, RIGHT, BACK})};
		case FRONT: return {corners.at({DOWN, LEFT, FRONT}), corners.at({UP, LEFT, FRONT}), corners.at({UP, RIGHT, FRONT}), corners.at({DOWN, RIGHT, FRONT})};
		case BACK: return {corners.at({DOWN, LEFT, BACK}), corners.at({UP, LEFT, BACK}), corners.at({UP, RIGHT, BACK}), corners.at({DOWN, RIGHT, BACK})};
		default: throw std::logic_error("Invalid face");
	}
}

vec3 Cell::faceCenter(FACE i) const { return sum<vec3, std::array<vec3, 4>>(getFaceCorners(i))/4.f; }
vec3 Cell::center() const { return sum<vec3, std::array<vec3, 8>>(getCorners())/8.f; }
float Cell::getFaceArea(FACE i) const { return .5*norm(cross(getFaceCorners(i)[1] - getFaceCorners(i)[0], getFaceCorners(i)[2] - getFaceCorners(i)[0])) + .5*norm(cross(getFaceCorners(i)[1] - getFaceCorners(i)[3], getFaceCorners(i)[2] - getFaceCorners(i)[3])); }
float Cell::lengthBetweenFaceCenters(FACE dir) const {
	if (dir == UP || dir == DOWN)
		return norm(faceCenter(UP) - faceCenter(DOWN));
	if (dir == LEFT || dir == RIGHT)
		return norm(faceCenter(LEFT) - faceCenter(RIGHT));
	return norm(faceCenter(FRONT) - faceCenter(BACK));
}

std::vector<std::array<vec3, 4>> Cell::getBdFaces() const {
	vector<std::array<vec3, 4>> res = {};
	for (auto &f : bdFaces)
		res.push_back(getFaceCorners(f));
	return res;
}
std::vector<std::array<Vertex, 4>> Cell::getBdVertices() const {
	vector<std::array<Vertex, 4>> res = {};
	for (auto &f : getBdFaces()) {
		vec3 normal = normalise(cross(f[1] - f[0], f[2] - f[0]));
		res.push_back({Vertex(f[0], vec2(0, 0), normal),
						    Vertex(f[1], vec2(1, 0), normal),
							Vertex(f[2], vec2(0, 1), normal),
							Vertex(f[3], vec2(1, 1), normal)});
	}
	return res;
}
float Cell::gradientOnFace(const std::string& attr, FACE i, std::vector<Cell> &cells) const {
	if (isBd(i))
		return 0;
	return (getNeighbour(i, cells)->attrValue(attr) - attrValue(attr))/norm(center()-getNeighbour(i, cells)->center());
}

void Cell::reindexing(int index, int newIndex) {
	if (this->index == index)
		this->index = newIndex;
	for (auto &nb : neighbours)
		if (nb.second == index)
			nb.second = newIndex;
}

void Cell::reindexingAfterRemoval(int index) {
	if (this->index > index)
		this->index--;
	for (auto &val: neighbours){
		if (val.second == index) {
			val.second = -1;
			bdFaces.push_back(val.first);
		}
		if (val.second > index)
			neighbours.at(val.first) = val.second-1;
	}
}


float CellMesh::Laplacian(const Cell &c, const std::string &attr) {
	float dxx = -(gradientOnFace(c, attr, RIGHT) + gradientOnFace(c, attr, LEFT))/norm(c.faceCenter(RIGHT) - c.faceCenter(LEFT));
	float dyy = -(gradientOnFace(c, attr, UP) + gradientOnFace(c, attr, DOWN))/norm(c.faceCenter(UP) - c.faceCenter(DOWN));
	float dzz = -(gradientOnFace(c, attr, FRONT) + gradientOnFace(c, attr, BACK))/norm(c.faceCenter(FRONT) - c.faceCenter(BACK));
	return dxx + dyy + dzz;
}

vec4 CellMesh::colorFromAttributes(const Cell &c, const std::vector<std::string> &attrSavedAsColor) {
	vec4 color = vec4(0, 0, 0, 1);
	for (int i = 0; i < attrSavedAsColor.size(); i++)
		color[i] = c.attrValue(attrSavedAsColor[i]);
	return color;
}


IndexedMesh CellMesh::bdMesh(const std::vector<std::string>& attrSavedAsColor) const {
	IndexedMesh mesh = IndexedMesh();
	for (auto &cell : cells) {
		auto bdVertices = cell.getBdVertices();
		if (!bdVertices.empty()) {
			vector<Vertex> vertices = {};
			vector<ivec3> trs = {};
			vec4 color = colorFromAttributes(cell, attrSavedAsColor);
			for (auto &v : bdVertices) {
				for (int i = 0; i < 4; i++) {
					v.at(i).setColor(color);
					vertices.push_back(v.at(i));
				}
				trs.emplace_back(vertices.size()-4, vertices.size()-3, vertices.size()-2);
				trs.emplace_back(vertices.size()-4, vertices.size()-1, vertices.size()-2);
			}
			mesh.addNewPolygroup(vertices, trs, cell.getID());
		}
	}
	return mesh;
}

void CellMesh::updateBdMeshAtCell(const Cell &c, const std::vector<std::string> &attrSavedAsColor, IndexedMesh &mesh) {
	vec4 color = colorFromAttributes(c, attrSavedAsColor);
	auto def = [color](BufferedVertex &v) { v.setColor(color); };
	mesh.deformPerVertex(c.getID(), def);
}

void CellMesh::updateBdMesh(const std::vector<std::string> &attrSavedAsColor, IndexedMesh &mesh) const {
	for (auto &c : cells)
		updateBdMeshAtCell(c, attrSavedAsColor, mesh);
}

void CellMesh::removeCell(int i) {
	cells.erase(cells.begin() + i);
	for (auto &c : cells)
		c.reindexingAfterRemoval(i);

}



IndexedHexahedralVolumetricMesh::IndexedHexahedralVolumetricMesh(const std::vector<HexahedronWithNbhd>& hexahedra, std::vector<IndexedQuadrilateral> faces, std::vector<Vertex> vertices) {
	throw std::logic_error("Not implemented");
}

HexahedronWithNbhd IndexedHexahedralVolumetricMesh::initHexahedron(const ivec6 &ind, const std::map<FACE, int> &nbhd) const {
	return HexahedronWithNbhd(ind, bufferManagerFace, bufferManagerHexa, nbhd);
}

IndexedHexahedralVolumetricMesh::IndexedHexahedralVolumetricMesh(
	const std::vector<ivec6>& hexahedra,
	const std::vector<ivec4> &faces,
	std::vector<Vertex> vertices) {

	this->vertices = std::move(vertices);
	this->faces = {};
	this->hexahedra = {};
	bufferManagerAccess = [this](int i) { return this->vertices[i]; };
	bufferManagerFace = [this](int i) { return this->faces[i]; };
	bufferManagerHexa = [this](int i) { return &this->hexahedra[i]; };

	for (auto face : faces)
		this->faces.push_back(initFace(face));
	for (const auto & i : hexahedra)
		this->hexahedra.push_back(initHexahedron(i,{}) );
	for (auto &h1 : this->hexahedra)
		for (int i = 0; i < hexahedra.size(); i++){
			auto& h2 = this->hexahedra[i];
			if (h1 == h2) continue;
			if (h1.getFaceIndex(UP) == h2.getFaceIndex(DOWN)) h1.setNeighbour(UP, i);
			if (h1.getFaceIndex(DOWN) == h2.getFaceIndex(UP)) h1.setNeighbour(DOWN, i);
			if (h1.getFaceIndex(LEFT) == h2.getFaceIndex(RIGHT)) h1.setNeighbour(LEFT, i);
			if (h1.getFaceIndex(RIGHT) == h2.getFaceIndex(LEFT)) h1.setNeighbour(RIGHT, i);
			if (h1.getFaceIndex(FRONT) == h2.getFaceIndex(BACK)) h1.setNeighbour(FRONT, i);
			if (h1.getFaceIndex(BACK) == h2.getFaceIndex(FRONT)) h1.setNeighbour(BACK, i);
		}
}
