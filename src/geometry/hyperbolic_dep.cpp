#include "hyperbolic_dep.hpp"
#include <random>
#include <chrono>
#include <functional>
#include <iostream>

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;

const Complex I = Complex(0, 1);
const Complex ONE = Complex(1, 0);
const Complex ZERO = Complex(0, 0);

auto planeToDisk(Complex z) -> Complex {
    return CAYLEY(z);
}

Complex diskToPlane(Complex z)
{
    return CAYLEY.inv(z);
}

const Matrix<Complex> Imob = Matrix<Complex>(ONE, ZERO, ZERO, ONE);


HyperbolicPlane::HyperbolicPlane() : _toH(Biholomorphism::linear(ONE, ZERO))
{
    _center = 1.0i;
}

HyperbolicPlane::HyperbolicPlane(Biholomorphism toH) : _toH(toH)
{
    _center = toH.inv(1.0i);
}

vec2 HyperbolicPlane::geodesicEndsH(Complex z0, Complex z1)
{

    if (z0.re() == z1.re())
        return vec2(z0.re(), z0.re());
    float c = (z0.re() + z1.re() - (z1.im()*z1.im() - z0.im()*z0.im()) / (z0.re() - z1.re())) / 2;
    float r = abs(z0 - Complex(c, 0));
    return vec2(c-r, c+r);
}

Mob HyperbolicPlane::geodesicToVerticalH(Complex z0, Complex z1)
{
    if (z0.re() == z1.re())
        return Mob(ONE, -z0.re()+0.0i, ZERO, ONE);
    vec2 ends = geodesicEndsH(z0, z1);
    return Mob(Complex(1, 0), Complex(-ends.y, 0), Complex(1, 0), Complex(-ends.x, 0));
}

ComplexCurve HyperbolicPlane::geodesic(Complex z0, Complex z1)
{
    Mob m = geodesicToVerticalH(toH(z0), toH(z1)).inv();
    float t0 = log(m.inv().mobius(toH(z0)).im());
    float t1 = log(m.inv().mobius(toH(z1)).im());
    ComplexCurve curve = ComplexCurve([this, z0, z1](float t) {
        return this->fromH(geodesicToVerticalH(toH(z0), toH(z1)).inv().mobius(Complex(0, exp(t))));
    }, t0, t1);

    return curve;
}

ComplexCurve HyperbolicPlane::boundary()
{
    return ComplexCurve([this](float t) {return fromH(Complex(t, .01f)); }, -5, 5);
}

Complex HyperbolicPlane::toH(Complex z)
{
    return _toH(z);
}

Complex HyperbolicPlane::fromH(Complex z)
{
    return _toH.inv(z);
}

Complex HyperbolicPlane::toDisk(Complex z)
{
    return planeToDisk(_toH(z));
}

Complex HyperbolicPlane::fromDisk(Complex z)
{
    return _toH.inv((diskToPlane(z)));
}

float HyperbolicPlane::distance(Complex z0, Complex z1)
{
    Complex h1 = toH(z0);
    Complex h2 = toH(z1);
    Complex v1 = geodesicToVerticalH(h1, h2).mobius(h1);
    Complex v2 = geodesicToVerticalH(h1, h2).mobius(h2);
    return abs(log(v1.im()) - log(v2.im()));
}



PlanarMeshWithBoundary HyperbolicPlane::mesh(int radial_res, int vertical_res, float max_r, float cut_bd)
{
    PlanarMeshWithBoundary diskMesh = PlanarUnitDisk(radial_res, vertical_res);
    for (int i = 0; i < diskMesh.triangles.size(); i++) {
        for (int j = 0; j < 3; j++) {
            Complex p = fromDisk(Complex(diskMesh.triangles[i].vertices[j] * cut_bd));
            if (norm(p - _center) > max_r)
                p = _center + (p - _center) * max_r/norm(p - _center);
            diskMesh.triangles[i].vertices[j] = p.z;
        }

    }

	for (int i = 0; i < diskMesh.boundaries.at(0).size(); i++)
		diskMesh.boundaries.at(0).at(i) = fromDisk(Complex(diskMesh.boundaries.at(0).at(i) * cut_bd)).z;
    return diskMesh;
}

SuperMesh HyperbolicPlane::superMesh(std::vector<std::pair<Complex, Complex>> geodesic_ends,
    MaterialPhongConstColor &material, MaterialPhongConstColor &material_bd, MaterialPhongConstColor &material_geo, int radial_res, int vertical_res,
    float max_r, float cut_bd, BoundaryEmbeddingStyle style, BoundaryEmbeddingStyle geodesic_style) {

    PlanarMeshWithBoundary planar_mesh = mesh(radial_res, vertical_res, max_r, cut_bd);

    auto sup = SuperMesh(planar_mesh, material, material_bd, style);
    for (int i = 0; i < geodesic_ends.size(); i++) {
        auto geo = geodesic(geodesic_ends[i].first, geodesic_ends[i].second);
        auto geodesic_mesh = TriangularMesh(&geo, 40, geodesic_style.height_middle, geodesic_style.width_middle, geodesic_style.width_side);
        sup.addEmbeddedCurve(PolyGroupID(-i-1), geodesic_mesh.getTriangles(), material_geo);
    }
    return sup;
}

SuperPencilPlanar HyperbolicPlane::superPencil(std::vector<std::pair<Complex, Complex>> geodesic_ends,
    MaterialPhongConstColor &material, MaterialPhongConstColor &material_bd, MaterialPhongConstColor &material_geo, int radial_res, int vertical_res,
    float max_r, float cut_bd, BoundaryEmbeddingStyle style, BoundaryEmbeddingStyle geodesic_style) {

    PlanarMeshWithBoundary planar_mesh = mesh(radial_res, vertical_res, max_r, cut_bd);
    auto sup = SuperPencilPlanar(planar_mesh, material, material_bd, style);
    for (int i = 0; i < geodesic_ends.size(); i++) {
        auto geo = geodesic(geodesic_ends[i].first, geodesic_ends[i].second);
        auto geodesic_mesh = TriangularMesh(&geo, geodesic_style.nSegments, geodesic_style.height_middle, geodesic_style.width_middle, geodesic_style.width_side);
        sup.addEmbeddedCurve(geodesic_mesh.getTriangles(), material_geo);
    }
    return sup;
}

HyperbolicPlane HyperbolicPlane::transform(Biholomorphism f) const{
    return HyperbolicPlane(_toH.compose(f.inv()));
}

void HyperbolicPlane::transformInPlace(Biholomorphism f) {
    _toH = _toH.compose(f);
}

HyperbolicPlane HyperbolicPlane::transformByMobiusH(Matrix<Complex> m) {
    return transform(Biholomorphism::mobius(m));
}




Biholomorphism HyperbolicPlane::toHTransform() const {
    return _toH;
}

Biholomorphism HyperbolicPlane::toDTransform() const {
    return CAYLEY.compose(_toH);
}


PoincareDisk::PoincareDisk()
{
    this->_toH = CAYLEY.inv();
}


Complex PoincareDisk::toDisk(Complex z)
{
    return z;
}

Complex PoincareDisk::fromDisk(Complex z)
{
    return z;
}


ComplexCurve PoincareDisk::boundary()
{
    return ComplexCurve::circle(Complex(0, 0), 1);
}

PlanarMeshWithBoundary PoincareDisk::mesh(int radial_res, int vertical_res, float max_r, float cut_bd) {
    return PlanarUnitDisk(radial_res, vertical_res);
}

HyperbolicBand::HyperbolicBand()
{
    this->_toH = EXP;
}

// ComplexCurve HyperbolicBand::boundary()
// {
//     ComplexCurve l = ComplexCurve::line(Complex(-5, PI-.01), Complex(5, PI-.01));
//     return ComplexCurve::line(Complex(-5, .01), Complex(5, .01)).disjointUnion(l);
// }

// HyperbolicVerticalBand::HyperbolicVerticalBand()
// {
//     this->_toH = [](Complex z) {return diskToPlane(tan(z)); };
//     this->_fromH = [](Complex z) {return (atan(planeToDisk(z))); };
// }

// ComplexCurve HyperbolicVerticalBand::boundary()
// {
//     ComplexCurve l = ComplexCurve::line(Complex(-PI/4+.01, -5), Complex(-PI/4+.01, 5));
//     return ComplexCurve::line(Complex(PI/4-.01, -5), Complex(PI/4-.01, 5)).disjointUnion(l);
// }

// HyperbolicHalfBand::HyperbolicHalfBand()
// {
//     this->_toH = [](Complex z) {return diskToPlane(sin(z)); };
//     this->_fromH = [](Complex z) {return (asin(planeToDisk(z))); };
// }

Circle::Circle()
{
    center = Complex(0, 0);
    r = 0;
}

Circle::Circle(Complex center, float r)
{
    this->center = center;
    this->r = r;
}

bool Circle::disjoint(Circle &c)
{
    return abs(center - c.center) > r + c.r;
}

bool Circle::tangent(Circle &c)
{
    return abs(abs(center - c.center) - r - c.r) < 0.0001;
}

std::pair<std::shared_ptr<Complex>, std::shared_ptr<Complex>> Circle::intersection(Circle &c)
{
    if (disjoint(c))
        return { nullptr, nullptr };
    if (tangent(c))
        return { std::make_shared<Complex>(center + normalise(c.center - center)*r), nullptr };

    float d = abs(center - c.center);
    float a = (r*r - c.r*c.r + d*d) / (2 * d);
    float h = sqrt(r*r - a*a);
    Complex p2 = center + normalise(c.center - center)*a;
    Complex p3 = p2 + normalise(c.center - center)*Complex(0, 1)*h;
    Complex p4 = p2 - normalise(c.center - center)*Complex(0, 1)*h;
    return { std::make_shared<Complex>(p3), std::make_shared<Complex>(p4) };
}

ComplexCurve Circle::parametric()
{
    return ComplexCurve::circle(center, r);
}

Arc::Arc(Complex z0, Complex z1, Complex center)
{
    this->z0 = z0;
    this->z1 = z1;
    this->center = center;
    this->r = abs(z0 - center);
    this->theta0 = atan2(z0.im() - center.im(), z0.re() - center.re());
    this->theta1 = atan2(z1.im() - center.im(), z1.re() - center.re());
}

Arc::Arc(Complex center, float r, float theta0, float theta1)
{
    this->center = center;
    this->r = r;
    this->theta0 = theta0;
    this->theta1 = theta1;
    this->z0 = center + Complex(cos(theta0), sin(theta0)) * r;
    this->z1 = center + Complex(cos(theta1), sin(theta1)) * r;
}

Arc::Arc(Circle c, float theta0, float theta1)
{
    this->center = c.center;
    this->r = c.r;
    this->theta0 = theta0;
    this->theta1 = theta1;
    this->z0 = center + Complex(cos(theta0), sin(theta0)) * r;
    this->z1 = center + Complex(cos(theta1), sin(theta1)) * r;
}

ComplexCurve Arc::parametric()
{
    return ComplexCurve::arc(center, z0, z1);
}

std::pair<std::shared_ptr<Complex>, std::shared_ptr<Complex>> Arc::intersection(Circle &c)
{
    auto result = extend().intersection(c);
    if (result.first == nullptr)
        return { nullptr, nullptr };
    if (result.second == nullptr)
        if (contains(*result.first))
            return { result.first, nullptr };
        else
            return { nullptr, nullptr };
    if (contains(*result.first))
        if (contains(*result.second))
            return { result.first, result.second };
        return { result.first, nullptr };
    if (contains(*result.second))
        return { result.second, nullptr };
    return { nullptr, nullptr };
}

bool Arc::disjoint(Circle &c)
{
    return intersection(c).first == nullptr;
}

bool Arc::tangent(Circle &c)
{
    return tangent(c);
}

bool Arc::disjoint(Arc &c)
{
    Circle c1 = extend();
    Circle c2 = c.extend();
    if (c1.intersection(c2).first == nullptr)
        return true;
    Complex z = *c1.intersection(c2).first;
    if (z.arg() > theta0 && z.arg() < theta1 && z.arg() > c.theta0 && z.arg() < c.theta1)
        return false;
    if (c1.intersection(c2).second == nullptr)
        return true;
    z = *c1.intersection(c2).second;
    return !(z.arg() > theta0 && z.arg() < theta1 && z.arg() > c.theta0 && z.arg() < c.theta1);
}

bool Arc::tangent(Arc &c)
{
    Circle c1 = extend();
    Circle c2 = c.extend();
    if (c1.intersection(c2).first == nullptr || c1.intersection(c2).second != nullptr)
        return false;
    Complex z = *c1.intersection(c2).first;
    return z.arg() > theta0 && z.arg() < theta1 && z.arg() > c.theta0 && z.arg() < c.theta1;
}

Complex Arc::mid() const {
    float theta = (theta0 + theta1) / 2;
    return center + Complex(cos(theta), sin(theta)) * r;
}

Circle Arc::extend() const {
    return Circle(center, r);
}

bool Arc::contains(Complex z)
{
    return abs(abs(z - center) - r) < 0.0001 && z.arg() > theta0 && z.arg() < theta1;
}

Arc Arc::geodesicH(Complex z0, Complex z1)
{
    HyperbolicPlane H = HyperbolicPlane();
    auto ends = H.geodesicEndsH(z0, z1);
    return Arc(z0, z1, Complex(ends.x + ends.y, 0) / 2);
}

Arc Arc::geodesicDisk(Complex z0, Complex z1)
{
    Arc arcH = geodesicH(z0, z1);
    return Arc(planeToDisk(arcH.z0), planeToDisk(arcH.z1), planeToDisk(arcH.center));
}

float Arc::angle(Arc &c)
{
    if (disjoint(c))
        return 0;
    Complex z = *intersection(c).first;
    float theta1 = (z- center).arg();
    float theta2 = (z - c.center).arg();
    ComplexCurve curve1 = parametric();
    ComplexCurve curve2 = c.parametric();
    vec2 v1 = (curve1._df)(theta1).z;
    vec2 v2 = (curve2._df)(theta2).z;
    return acos(dot(v1, v2) / (norm(v1) * norm(v2)));
}



SchwarzPolygon SchwarzPolygon::halfRingH(float a, float b, int rad, int hor, float cut_bd) {
    vector<Complex> vert = {Complex(-a, cut_bd), Complex(-b,  cut_bd), Complex(b,  cut_bd), Complex(a,  cut_bd)};

    std::vector<TriangleComplex> triangulation = {};
    for (int i = 0; i < rad; i++) {
        for (int j = 0; j < hor; j++) {
            float r0 = lerp(a, b, cut_bd*i / rad);
            float r1 = lerp(a, b, cut_bd*(i + 1) / rad);
            float theta0 = PI*j / hor ;
            float theta1 = PI*(j + 1) / hor;
            triangulation.push_back(TriangleComplex({
                Complex(r0*cos(theta0), r0*sin(theta0)),
                Complex(r0*cos(theta1), r0*sin(theta1)),
                Complex(r1*cos(theta0), r1*sin(theta0) )}));
            triangulation.push_back(TriangleComplex({
               Complex(r0*cos(theta1), r0*sin(theta1)),
               Complex(r1*cos(theta0), r1*sin(theta0)),
               Complex(r1*cos(theta1), r1*sin(theta1) )}));
        }
    }
    return SchwarzPolygon(vert, Imob, make_shared<HyperbolicPlane>(HyperbolicPlane()), triangulation);
}

SchwarzPolygon SchwarzPolygon::pizzaSlice(float angle, int rad, int hor, float cut_bd) {
    vector<Complex> vert = {ZERO, ONE, Complex(cos(angle), sin(angle))};

    std::vector<TriangleComplex> triangulation = {};
    triangulation.reserve(rad*hor*2);
    for (int j = 0; j < rad; j++) {
        float r = cut_bd/hor;
        float theta0 = angle*j / rad;
        float theta1 = angle*(j + 1) / rad;
        triangulation.push_back(TriangleComplex({
            Complex(0, 0),
            Complex(r*cos(theta1), r*sin(theta1)),
            Complex(r*cos(theta0), r*sin(theta0) )}));
    }
    for (int i = 1; i < hor; i++) {
        for (int j = 0; j < rad; j++) {
            float r0 = cut_bd*i/hor;
            float r1 = cut_bd*(i+1)/hor;
            float theta0 = angle*j / rad;
            float theta1 = angle*(j + 1) / rad;
            triangulation.push_back(TriangleComplex({
                Complex(r0*cos(theta0), r0*sin(theta0)),
                Complex(r0*cos(theta1), r0*sin(theta1)),
                Complex(r1*cos(theta0), r1*sin(theta0) )}));
            triangulation.push_back(TriangleComplex({
               Complex(r0*cos(theta1), r0*sin(theta1)),
               Complex(r1*cos(theta0), r1*sin(theta0)),
               Complex(r1*cos(theta1), r1*sin(theta1))}));
        }
    }
    return SchwarzPolygon(vert, Imob, make_shared<HyperbolicPlane>(PoincareDisk()), triangulation);
}

SchwarzPolygon SchwarzPolygon::stripH(float a, float b, float shift, float h0, float h1, float h_max)
{
    vector<Complex> vert = {Complex(a, 0), Complex(b, 0), Complex(b, h0), Complex(b+shift, h1), Complex(b+shift, h_max), Complex(a+shift, h_max), Complex(a+shift, h1), Complex(a, h0)};
    float h2 = h1 + h1 - h0;
    std::vector<TriangleComplex> triangulation = {};
    triangulation.push_back(TriangleComplex({Complex(a, 0.001f), Complex(.66f*a+.34f*b, 0.001f), Complex(b, h0)}));
    triangulation.push_back(TriangleComplex({Complex(.66f*a+.34f*b, 0.001f), Complex(.34f*a+.66f*b, 0.001f), Complex(b, h0)}));
    triangulation.push_back(TriangleComplex({Complex(.34f*a+.66f*b, 0.001f),Complex(b, 0.001f),  Complex(b, h0)}));
    triangulation.push_back(TriangleComplex({Complex(a, 0.001f), Complex(b, h0), Complex(a, h0)}));
    triangulation.push_back(TriangleComplex({Complex(a, h0), Complex(b, h0), Complex(a+shift, h1)}));
    triangulation.push_back(TriangleComplex({Complex(b, h0), Complex(a+shift, h1), Complex(b+shift, h1)}));
    triangulation.push_back(TriangleComplex({Complex(a+shift, h1), Complex(b+shift, h1), Complex(b, h2)}));
    triangulation.push_back(TriangleComplex({Complex(a+shift, h1), Complex(b, h2), Complex(a, h2)}));
    triangulation.push_back(TriangleComplex({Complex(a, h2), Complex(b, h2), Complex(b, h_max)}));
    triangulation.push_back(TriangleComplex({Complex(a, h2), Complex(b, h_max), Complex(a, h_max)}));
    vector<TriangleComplex> subdivision = TriangleComplex::subdivideTriangulation(TriangleComplex::subdivideTriangulation(triangulation));
    return SchwarzPolygon(vert, Imob, make_shared<HyperbolicPlane>(HyperbolicPlane()), subdivision);
}

SchwarzPolygon SchwarzPolygon::stripD(float a, float b, float shift, float h0, float h1, float cutbd)
{
    vector<Complex> vert = {planeToDisk(Complex(a, 0)), planeToDisk(Complex(b, 0)), planeToDisk(Complex(b, h0)), planeToDisk(Complex(b+shift, h1)), -I, planeToDisk(Complex(a+shift, h1)), planeToDisk(Complex(a, h0))};
    float h2 = h1 + h1 - h0;
    std::vector<TriangleComplex> triangulation = {};
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(a, 0))*cutbd,    cutbd*planeToDisk(Complex(.66f*a+.34f*b, 0)),    planeToDisk(Complex(b, h0))}));
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(.66f*a+.34f*b, 0))*cutbd,   cutbd*planeToDisk(Complex(.34f*a+.66f*b, 0)),    planeToDisk(Complex(b, h0))}));
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(.34f*a+.66f*b, 0))*cutbd,   cutbd*planeToDisk(Complex(b, 0)),              planeToDisk(Complex(b, h0))}));
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(a, 0))*cutbd,    planeToDisk(Complex(b, h0)),                 planeToDisk(Complex(a, h0))}));
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(a, h0)),                planeToDisk(Complex(b, h0)),                 planeToDisk(Complex(a+shift, h1))}));
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(b, h0)),                planeToDisk(Complex(a+shift, h1)),           planeToDisk(Complex(b+shift, h1))}));
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(a+shift, h1)),          planeToDisk(Complex(b+shift, h1)),           planeToDisk(Complex(b, h2))}));
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(a+shift, h1)),          planeToDisk(Complex(b, h2)),                 planeToDisk(Complex(a, h2))}));
    triangulation.push_back(TriangleComplex({planeToDisk(Complex(a, h2)),                planeToDisk(Complex(b, h2)),                 0.0i+cutbd}));
    vector<TriangleComplex> subdivision = TriangleComplex::subdivideTriangulation(TriangleComplex::subdivideTriangulation(TriangleComplex::subdivideTriangulation(triangulation)));

    return SchwarzPolygon(vert, Imob, make_shared<HyperbolicPlane>(PoincareDisk()), subdivision);
}

std::vector<HyperbolicTriangleH> HyperbolicTriangleH:: subdivideRec(float max_sidelen, int n, std::vector<HyperbolicTriangleH> tr) {
    auto result = std::vector<HyperbolicTriangleH>();
    result.reserve(tr.size());
    for (int i = 0; i < tr.size(); i++) {
        float m = tr[i].edgeEuclideanLength(tr[i].longestEdge());
        if (m > max_sidelen) {
            auto new_tr = tr[i].subdivideLongestEdge(n);
            for (int j = 0; j < new_tr.size(); j++)
                result.push_back(new_tr[j]);
        }
        else
            result.push_back(tr[i]);
    }
    if (result.size() == tr.size())
        return result;
    return subdivideRec(max_sidelen, n, result);
}

HyperbolicTriangleH::HyperbolicTriangleH(Complex z0, Complex z1, Complex z2) {
    this->vertices = {z0, z1, z2};
}




std::array<Complex, 3> HyperbolicTriangleH::getVertices() const {
    return vertices;
}

std::array<Complex, 2> HyperbolicTriangleH::getEdge(int i) const {
    return {vertices[i], vertices[(i + 1) % 3]};
}

float HyperbolicTriangleH::edgeCenter(int i) const {
    vec2 ends = HyperbolicPlane().geodesicEndsH(vertices[i], vertices[(i + 1) % 3]);
    return (ends.x + ends.y) / 2;
}

vec2 HyperbolicTriangleH::edgeArgs(int i) const {
    Complex v1 = vertices[i];
    Complex v2 = vertices[(i + 1) % 3];
    if (v1.re() > v2.re())
        std::swap(v1, v2);
    Complex center = Complex(edgeCenter(i), 0);
    return vec2((v1-center).arg(), (v2-center).arg());
}

float HyperbolicTriangleH::edgeRadius(int i) const {
    return norm(vertices[i] - Complex(edgeCenter(i), 0));
}

float HyperbolicTriangleH::edgeEuclideanLength(int i) const {
    vec2 args = edgeArgs(i);
    float r = edgeRadius(i);
    return abs(args.y-args.x)*r;
}

int HyperbolicTriangleH::longestEdge() const {
    std::array <float, 3> lengths = {edgeEuclideanLength(0), edgeEuclideanLength(1), edgeEuclideanLength(2)};
    if (lengths[0] >= lengths[1] && lengths[0] >= lengths[2])
        return 0;
    if (lengths[1] >= lengths[0] && lengths[1] >= lengths[2])
        return 1;
    return 2;
}

std::vector<HyperbolicTriangleH> HyperbolicTriangleH::subdivideLongestEdge(int n) {
    int i = longestEdge();
    vec2 args = edgeArgs(i);
    Complex center = Complex(edgeCenter(i), 0);
    std::vector<HyperbolicTriangleH> result = {};
    for (int j = 0; j < n; j++) {
        float t0 = lerp(args.x, args.y, 1.f*j / n);
        float t1 = lerp(args.x, args.y, 1.f*(j + 1) / n);
        Complex v0 = center + Complex(cos(t0), sin(t0)) * edgeRadius(i);
        Complex v1 = center + Complex(cos(t1), sin(t1)) * edgeRadius(i);
        result.push_back(HyperbolicTriangleH(vertices[(i+2)%3], v0, v1));
    }
    return result;
}

std::vector<HyperbolicTriangleH> HyperbolicTriangleH::subdivide(float max_sidelen, int n) const {
    return subdivideRec(max_sidelen, n, {HyperbolicTriangleH(vertices[0], vertices[1], vertices[2])});
}

TriangleComplex HyperbolicTriangleH::flatten() const {
    return TriangleComplex(vertices);
}

std::vector<TriangleComplex> HyperbolicTriangleH::triangulation(float max_sidelen, int n) const {
    auto tr = subdivide(max_sidelen, n);
    std::vector<TriangleComplex> result;
    result.reserve(tr.size());
    for (auto t: tr)
        result.push_back(t.flatten());
    return result;
}



SchwarzPolygon::SchwarzPolygon(): mobiusToFDDisk(Imob) {
}

SchwarzPolygon::SchwarzPolygon(std::vector<Complex> vertices, Matrix<Complex> mobiusToFDDisk,
                               std::shared_ptr<HyperbolicPlane> plane, std::vector<TriangleComplex> triangulation): mobiusToFDDisk(mobiusToFDDisk) {
    this->vertices = vertices;
    this->plane = plane;
    this->triangulation = triangulation;
    this->mobiusToFDDisk = mobiusToFDDisk;
}

SchwarzPolygon::SchwarzPolygon(HyperbolicTriangleH t, float max_len, int n): mobiusToFDDisk(1+.0i, 0+.0i, 0+.0i, 1+.0i) {
    this->vertices = {t.getVertices()[0], t.getVertices()[1], t.getVertices()[2]};
    this->plane = make_shared<HyperbolicPlane>(HyperbolicPlane());
    this->triangulation = t.triangulation(max_len, n);
    this->mobiusToFDDisk = Imob;
}

SchwarzPolygon::SchwarzPolygon(std::vector<Complex> vertices, std::vector<HyperbolicTriangleH> trs, float max_len, int n): mobiusToFDDisk(1+.0i, 0+.0i, 0+.0i, 1+.0i) {
    this->vertices = vertices;
    this->plane = make_shared<HyperbolicPlane>(HyperbolicPlane());
    this->triangulation = {};
    for (auto t: trs) {
        std::vector<TriangleComplex> tr = t.triangulation(max_len, n);
        triangulation.insert(triangulation.end(), tr.begin(), tr.end());
    }
}

SchwarzPolygon SchwarzPolygon::transform(Matrix<Complex> m) {
    vector<Complex> vert = vertices;
    std::vector<Complex> newVertices;
    newVertices.reserve(vert.size());
    for (auto v: vert) {
        Complex newV = plane->toDisk(v);
        newV = m.mobius(newV);
        newVertices.push_back(plane->fromDisk(newV));
    }

    std::vector<TriangleComplex> newTriangulation;
    newTriangulation.reserve(triangulation.size());
    for (auto t: triangulation) {
        TriangleComplex newT = t;
        for (int i = 0; i < 3; i++) {
            newT.vertices[i] = plane->toDisk(t.vertices[i]);
            newT.vertices[i] = m.mobius(newT.vertices[i]);
            newT.vertices[i] = plane->fromDisk(newT.vertices[i]);
        }
        newTriangulation.push_back(newT);
    }
    return SchwarzPolygon(newVertices, m.inv()*mobiusToFDDisk, plane, newTriangulation);
}

TriangularMesh SchwarzPolygon::embedd(float z) {
    vector<TriangleR3> tr = {};
    tr.reserve(this->triangulation.size());
    for (auto t: this->triangulation) {
        tr.push_back(t.embeddInR3(z));
    }
    return TriangularMesh(tr);
}

std::shared_ptr<HyperbolicPlane> SchwarzPolygon::domain() {
    return plane;
}

void SchwarzPolygon::transformInPlace(Biholomorphism f) {
    for (int i = 0; i < vertices.size(); i++)
        vertices[i] = f(vertices[i]);
    plane = make_shared<HyperbolicPlane>(plane->transform(f));
    for (int i = 0; i < triangulation.size(); i++) {
        for (int j = 0; j < 3; j++)
            triangulation[i].vertices[j] = f(triangulation[i].vertices[j]);
    }
}

Mob SchwarzPolygon::getMob() const {
    return mobiusToFDDisk;
}

std::vector<Complex> SchwarzPolygon::getVertices() const {
    return vertices;
}


std::vector<std::vector<Matrix<Complex>>> FuchsianGroup::extendGeneratingList(int last_len, int n, std::vector<std::vector<Matrix<Complex>>> list) {
    auto newList = list;
    for (auto g: generatorsD) {
        for (auto l: list) {
            if(l.size() == last_len) {
                auto newL = l;
                newL.push_back(g);
                if (isValidNewSequence(newL))
                    newList.push_back(newL);
                if (newList.size() >= n)
                    return newList;
                // newL = l;
                // newL.push_back(g.inv());
                // if (isValidNewSequence(newL))
                //     newList.push_back(newL);
                // if (newList.size() >= n)
                //     return newList;
            }
        }
    }
    if (newList.size() == list.size())
        return newList;
    return extendGeneratingList(last_len + 1, n, newList);
}

Matrix<Complex> FuchsianGroup::multiplyGeneratingSequence(std::vector<Matrix<Complex>> seq) {
    Mob result = Imob;
    for (auto m: seq)
        result = result * m;
    return result;
}

bool FuchsianGroup::isValidNewSequence(std::vector<Matrix<Complex>> seq) {
    if (relationDuplicateIndicator(seq))
        return false;
    if (seq.size() >= 2 && seq[seq.size() - 1] == seq[seq.size() - 2].inv())
        return false;
    return true;
}

std::vector<Matrix<Complex>> FuchsianGroup::multiplyGeneratingSequences(std::vector<std::vector<Matrix<Complex>>> list) {
    std::vector<Matrix<Complex>> result;
    result.reserve(list.size());
    for (const auto l: list)
        result.push_back(multiplyGeneratingSequence(l));
    return result;
}

FuchsianGroup::FuchsianGroup() {
    this->generatorsD = vector<Mob>();
}

std::vector<Matrix<Complex>> FuchsianGroup::generateElementsD(int n) {
    std::vector<std::vector<Matrix<Complex>>> list = {};
    list.reserve(n);
    for (auto g: getGeneratorsD())
        list.push_back({g});
    list = extendGeneratingList(1, n-1, list);
    list.insert(list.begin(), {Imob});
    return multiplyGeneratingSequences(list);
}




FuchsianGroup::FuchsianGroup(std::vector<Matrix<Complex>> generators, std::function<bool(std::vector<Matrix<Complex>>)> relationDuplicateIndicator, bool disk) {
    this->generatorsD = generators;
    this->relationDuplicateIndicator = relationDuplicateIndicator;
    if (!disk)
        for (int i = 0; i < generators.size(); i++)
            this->generatorsD[i] = CAYLEY_MAT * generators[i] * CAYLEY_MAT_INV;
}

FuchsianGroup::FuchsianGroup(std::vector<Mob> generators, bool disk) : FuchsianGroup(generators, [](std::vector<Mob> v) { return false; }, disk) {}

vector<Mob> FuchsianGroup::getGeneratorsD() {
    return generatorsD;
}

std::vector<Matrix<Complex>> FuchsianGroup::getGeneratorsAndInverses() {
    vector<Mob> gens = getGeneratorsD();
    gens.reserve(gens.size()*2);
    for (auto g: generatorsD)
        if (g.nearly_equal(g.inv()))
            gens.push_back(g.inv());
    return gens;
}

FuchsianGroup FuchsianGroup::Zn(int n) {
    return FuchsianGroup({Mob(Complex (cos(2*PI/n), sin(2*PI/n)), 0+.0i, 0+.0i, 1+.0i)}, [n](vector<Mob> v) { return v.size() >= n;});
}

FuchsianGroup FuchsianGroup::Gm(float a, float b) {
    return FuchsianGroup(  {Mob(b+.0i, 0+.0i, 0+.0i, a+.0i)}, false);
}

FuchsianGroup FuchsianGroup::Ga(float a, float b) {
    return FuchsianGroup({Mob(1+.0i, b - a+.0i, 0+.0i, 1+.0i)}, false);
}

FuchsianGroup FuchsianGroup::modular() {
    return FuchsianGroup({Mob(0+.0i, 1+.0i, -1+.0i, 0+.0i), Mob(0+.0i, 1+.0i, -1+.0i, 0+.0i)*Mob(1+.0i, 1+.0i, 0+.0i, 1+.0i)}, [](vector<Mob> v) {
        return v.size() >= 3 && nearlyEqual_mat(v[v.size() - 1]*v[v.size() - 2], v[v.size() - 3].inv()) ||
            v.size() >= 3 && nearlyEqual_mat(v[v.size() - 1]*v[v.size() - 2], -v[v.size() - 3].inv()) ||
                v.size() >= 2 && nearlyEqual_mat(v[v.size() - 1], -v[v.size() - 2].inv());
    }, false);
}


HyperbolicTesselation::HyperbolicTesselation(FuchsianGroup G, SchwarzPolygon fd) {
    this->G = G;
    this->fd = fd;
}

std::vector<SchwarzPolygon> HyperbolicTesselation::generateTesselation(int n) {
    std::vector<SchwarzPolygon> tesselation = {};
    auto elements = G.generateElementsD(n);
    for (auto m: elements) {
        tesselation.push_back(fd.transform(m));
    }
    return tesselation;
}

std::vector<TriangularMesh> HyperbolicTesselation::realiseTesselation(int n) {
    auto tess = generateTesselation(n);
    std::vector<TriangularMesh> meshes = {};
    meshes.reserve(tess.size());
    for (auto sp: tess) {
        meshes.push_back(sp.embedd());
    }
    return meshes;
}

std::vector<TriangularMesh> HyperbolicTesselation::
realiseTesselation(int n, const std::function<float(int)> &height_function) {
    auto tess = generateTesselation(n);
    std::vector<TriangularMesh> meshes = {};
    meshes.reserve(tess.size());
    for (int i = 0; i < tess.size(); i++) {
        meshes.push_back(tess[i].embedd(height_function(i)));
    }
    return meshes;
}

std::shared_ptr<HyperbolicPlane> HyperbolicTesselation::domain() {
    return fd.domain();
}


HyperbolicTesselation HyperbolicTesselation::ringsInH(int radial, int horizontal, float a, float b, float cut_bd) {
    return HyperbolicTesselation(FuchsianGroup::Gm(a, b), SchwarzPolygon::halfRingH(a, b, radial, horizontal, cut_bd));
}

HyperbolicTesselation HyperbolicTesselation::stripsInH(float a, float b, float shift, float h0, float h1, float h_max) {
    return HyperbolicTesselation(FuchsianGroup::Ga(a, b), SchwarzPolygon::stripH(a, b, shift, h0, h1, h_max));
}

HyperbolicTesselation HyperbolicTesselation::stripsInD(float a, float b, float shift, float h0, float h1, float cutbd) {
    return HyperbolicTesselation(FuchsianGroup::Ga(a, b), SchwarzPolygon::stripD(a, b, shift, h0, h1, cutbd));
}

HyperbolicTesselation HyperbolicTesselation::cyclicD(int n, int radial, int horizontal, float cut_bd) {
    return HyperbolicTesselation(FuchsianGroup::Zn(n), SchwarzPolygon::pizzaSlice(2*PI/n, radial, horizontal, cut_bd));
}

HyperbolicTesselation HyperbolicTesselation::modular(float max_len, float bd_up, int subdivisions) {
    HyperbolicTriangleH t = HyperbolicTriangleH(Complex(0, bd_up), Complex(-.5f, (float)sqrt(3)/2), Complex(.5f, (float)sqrt(3)/2));
    return HyperbolicTesselation(FuchsianGroup::modular(), SchwarzPolygon(t, max_len, subdivisions));
}

void HyperbolicTesselation::transformInPlace(Biholomorphism f) {
    fd.transformInPlace(f);
}
