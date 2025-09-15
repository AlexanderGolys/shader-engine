#pragma once

#include "complexGeo.hpp"


#include "engine/specific.hpp"
#include "utils/func.hpp"


Complex planeToDisk(Complex z);
Complex diskToPlane(Complex z);


const Biholomorphism EXP = Biholomorphism::_EXP();
const Biholomorphism LOG = Biholomorphism::_LOG();
const Biholomorphism IdC = Biholomorphism::linear(Complex(1), Complex(0));
const Biholomorphism ADD1 = Biholomorphism::linear(Complex(1), Complex(1));
const Biholomorphism SQUARE = Biholomorphism::power(2);
const Biholomorphism SQRT = Biholomorphism::power(.5f);
const Matrix<Complex> CAYLEY_MAT = Matrix<Complex>(Complex(1), -1.0i, Complex(1), 1.0i);
const Matrix<Complex> CAYLEY_MAT_INV = CAYLEY_MAT.inv();
const Biholomorphism CAYLEY = Biholomorphism::mobius(Matrix<Complex>(Complex(1), -1.0i, Complex(1), 1.0i));

class PlanarMeshWithBoundary;
class SuperMesh;
class SuperPencilPlanar;
class MaterialPhongConstColor;
struct BoundaryEmbeddingStyle;
class TriangleComplex;
class TriangularMesh;

class HyperbolicPlane {
protected:
	Biholomorphism _toH;
	Complex _center = 1.0i;
public:
	virtual ~HyperbolicPlane() = default;
	HyperbolicPlane();
	explicit HyperbolicPlane(Biholomorphism toH);
	virtual vec2 geodesicEndsH(Complex z0, Complex z1);
	virtual Mob geodesicToVerticalH(Complex z0, Complex z1);
	virtual ComplexCurve geodesic(Complex z0, Complex z1);
	virtual ComplexCurve boundary();
	virtual Complex toH(Complex z);
	virtual Complex fromH(Complex z);
	virtual Complex toDisk(Complex z);
	virtual Complex fromDisk(Complex z);
	virtual float distance(Complex z0, Complex z1);
	virtual PlanarMeshWithBoundary mesh(int radial_res, int vertical_res, float max_r, float cut_bd);

	SuperMesh superMesh(std::vector<std::pair<Complex, Complex>> geodesic_ends, MaterialPhongConstColor &material,
	                    MaterialPhongConstColor &material_bd, MaterialPhongConstColor &material_geo, int radial_res, int vertical_res,
	                    float max_r, float cut_bd, BoundaryEmbeddingStyle style, BoundaryEmbeddingStyle geodesic_style);

	SuperPencilPlanar superPencil(std::vector<std::pair<Complex, Complex>> geodesic_ends, MaterialPhongConstColor &material,
						MaterialPhongConstColor &material_bd, MaterialPhongConstColor &material_geo, int radial_res, int vertical_res,
						float max_r, float cut_bd, BoundaryEmbeddingStyle style, BoundaryEmbeddingStyle geodesic_style);

	virtual HyperbolicPlane transform(Biholomorphism f) const;
	void transformInPlace(Biholomorphism f);
	virtual HyperbolicPlane transformByMobiusH(Mob m);
	virtual Biholomorphism toHTransform() const;
	virtual Biholomorphism toDTransform() const;

};

class PoincareDisk : public HyperbolicPlane {
public:
	PoincareDisk();
	Complex toDisk(Complex z) override;
	Complex fromDisk(Complex z) override;
	ComplexCurve boundary() override;
	PlanarMeshWithBoundary mesh(int radial_res, int vertical_res, float max_r, float cut_bd) override;
};

class HyperbolicBand : public HyperbolicPlane {
public:
	HyperbolicBand();
	ComplexCurve boundary() override;
};

class HyperbolicVerticalBand : public HyperbolicPlane {
public:
	HyperbolicVerticalBand();
	ComplexCurve boundary() override;
};

class HyperbolicHalfBand : public HyperbolicPlane {
public:
	HyperbolicHalfBand();
};


class Circle {
public:
	virtual ~Circle() {};

	Complex center;
	float r;
	Circle();
	Circle(Complex center, float r);
	virtual bool disjoint(Circle &c);
	virtual bool tangent(Circle &c);
	virtual std::pair<std::shared_ptr<Complex>, std::shared_ptr<Complex>> intersection(Circle &c);
	virtual ComplexCurve parametric();
};

class Arc : public Circle {
public:
	Complex z0, z1, center;
	float r, theta0, theta1;
	Arc(Complex z0, Complex z1, Complex center);
	Arc(Complex center, float r, float theta0=0, float theta1=TAU);
	Arc(Circle c, float theta0=0, float theta1=TAU);
	ComplexCurve parametric() override;
	std::pair<std::shared_ptr<Complex>, std::shared_ptr<Complex>> intersection(Circle &c) override;
	bool disjoint(Circle& c) override;
	bool tangent(Circle &c) override;
	bool disjoint(Arc& c);
	bool tangent(Arc& c);
	Complex mid() const;
	Circle extend() const;
	bool contains(Complex z);
	static Arc geodesicH(Complex z0, Complex z1);
	static Arc geodesicDisk(Complex z0, Complex z1);
	float angle(Arc &c);
};




class HyperbolicTriangleH {
	std::array<Complex, 3> vertices;
	static std::vector<HyperbolicTriangleH> subdivideRec(float max_sidelen, int n, std::vector<HyperbolicTriangleH>);
public:
	HyperbolicTriangleH(Complex z0, Complex z1, Complex z2);
	std::array<Complex, 3> getVertices() const;
	std::array<Complex, 2> getEdge(int i) const;
	float edgeCenter(int i) const;
	float edgeRadius(int i) const;
	float edgeEuclideanLength(int i) const;
	vec2 edgeArgs(int i) const;
	int longestEdge() const;
	std::vector<HyperbolicTriangleH> subdivideLongestEdge(int n);
	std::vector<HyperbolicTriangleH> subdivide(float max_sidelen, int n=2) const;
	TriangleComplex flatten() const;
	std::vector<TriangleComplex> triangulation(float max_sidelen, int n=2) const;
};


class SchwarzPolygon {
protected:
	std::vector<Complex> vertices; // on D;
	Mob mobiusToFDDisk;
	std::shared_ptr<HyperbolicPlane> plane;
	std::vector<TriangleComplex> triangulation;

public:
	virtual ~SchwarzPolygon() = default;
	SchwarzPolygon();
	SchwarzPolygon(std::vector<Complex> vertices, Mob mobiusToFDDisk, std::shared_ptr<HyperbolicPlane> plane,
		std::vector<TriangleComplex> triangulation);
	SchwarzPolygon(HyperbolicTriangleH t, float max_len, int n=2);
	SchwarzPolygon(std::vector<Complex> vertices, std::vector<HyperbolicTriangleH> trs, float max_len, int n=2);
	Mob getMob() const;
	std::vector<Complex> getVertices() const;
	virtual SchwarzPolygon transform(Mob m);
	virtual TriangularMesh embedd(float z=0);
	std::shared_ptr<HyperbolicPlane> domain();
	void transformInPlace(Biholomorphism f);

	static SchwarzPolygon halfRingH(float a, float b, int rad, int hor, float cut_bd=.99);
	static SchwarzPolygon pizzaSlice(float angle, int rad, int hor, float cut_bd=.99);
	static SchwarzPolygon stripH(float a, float b, float shift, float h0, float h1, float h_max);
	static SchwarzPolygon stripD(float a, float b, float shift, float h0, float h1, float cutbd);
};

class FuchsianGroup {
protected:
	std::vector<Mob> generatorsD;
	std::function<bool(std::vector<Mob>)> relationDuplicateIndicator = [](std::vector<Mob> v) { return false; };
	std::vector<std::vector<Mob>> extendGeneratingList(int last_len, int n, std::vector<std::vector<Mob>> list);
	Mob multiplyGeneratingSequence(std::vector<Mob> seq);
	bool isValidNewSequence(std::vector<Mob> seq);
	std::vector<Mob> multiplyGeneratingSequences(std::vector<std::vector<Mob>> list);
public:
	virtual ~FuchsianGroup() {};
	FuchsianGroup();
	FuchsianGroup(std::vector<Mob> generators, bool disk=true);
	FuchsianGroup(std::vector<Mob> generators, std::function<bool(std::vector<Mob>)> relationDuplicateIndicator, bool disk=true);
	virtual std::vector<Mob> generateElementsD(int n);
	std::vector<Mob> getGeneratorsD();
	std::vector<Mob> getGeneratorsAndInverses();

	static FuchsianGroup Zn(int n);
	static FuchsianGroup Gm(float a, float b);
	static FuchsianGroup Ga(float a, float b);
	static FuchsianGroup modular();
};


bool isInAutH(Mob m);
bool isInAutD(Mob m);


class HyperbolicTesselation {
	FuchsianGroup G;
	SchwarzPolygon fd;
public:
	HyperbolicTesselation(FuchsianGroup G, SchwarzPolygon fd);
	std::vector<SchwarzPolygon> generateTesselation(int n);
	std::vector<TriangularMesh> realiseTesselation(int n);
	std::vector<TriangularMesh> realiseTesselation(int n, const std::function<float(int)> &height_function);
	std::shared_ptr<HyperbolicPlane> domain();

	static HyperbolicTesselation ringsInH(int radial, int horizontal, float a, float b, float cut_bd=.99);
	static HyperbolicTesselation stripsInH(float a, float b, float shift, float h0, float h1, float h_max);
	static HyperbolicTesselation stripsInD(float a, float b, float shift, float h0, float h1, float cutbd);
	static HyperbolicTesselation cyclicD(int n, int radial, int horizontal, float cut_bd=.99);
	static HyperbolicTesselation modular(float max_len, float bd_up, int subdivisions);

	void transformInPlace(Biholomorphism f);
};
