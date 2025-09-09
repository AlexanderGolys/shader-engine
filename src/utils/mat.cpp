#include "mat.hpp"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <sstream>
#include <vector>


using namespace glm;
using std::vector, std::string, std::set, std::array, std::pair, std::exp, std::log, std::cos, std::sin, std::cosh, std::sinh, std::sqrt, std::pow, std::atan2, std::abs;


FloatMatrix::operator tvec2<float>() {
	if (std::min(n(), m()) != 1 || std::max(m(), n()) != 2) throw std::format_error("wrong dimension of matrix (" + std::to_string(n()) + ", " + std::to_string(m()) + ")");
	return (*this)[0].size() > 1 ? vec2((*this)[0][0], (*this)[0][1]) : vec2((*this)[0][0], (*this)[1][0]);
}

FloatMatrix::operator tvec3<float>() {
	if (std::min(n(), m()) != 1 || std::max(m(), n()) != 3) throw std::format_error("wrong dimension of matrix (" + std::to_string(n()) + ", " + std::to_string(m()) + ")");
	return (*this)[0].size() > 1 ? vec3((*this)[0][0], (*this)[0][1], (*this)[0][2]) : vec3((*this)[0][0], (*this)[1][0], (*this)[2][0]);
}

FloatMatrix::operator tvec4<float>() {
	if (std::min(n(), m()) != 1 || std::max(m(), n()) != 4) throw std::format_error("wrong dimension of matrix (" + std::to_string(n()) + ", " + std::to_string(m()) + ")");
	return (*this)[0].size() > 1 ? vec4((*this)[0][0], (*this)[0][1], (*this)[0][2], (*this)[0][3]) : vec4((*this)[0][0], (*this)[1][0], (*this)[2][0], (*this)[3][0]);
}

Complex::Complex() {
	z = vec2(0, 0);
}

Complex::Complex(vec2 z) {
	this->z = z;
}

Complex::Complex(float x, float y) : Complex(vec2(x, y)) {}

Complex::Complex(int x, float y) : Complex(vec2(x, y)) {}

Complex::Complex(float x, int y) : Complex(vec2(x, y)) {}

Complex::Complex(int x, int y) : Complex(vec2(x, y)) {}



Complex::Complex(float x) : Complex(x, 0.f) {}

Complex::Complex(int x) : Complex(x, 0) {}

Complex Complex::operator+(Complex c) const {
	return Complex(z + c.z);
}

Complex Complex::operator-(const Complex c) const {
	return Complex(z - c.z);
}



Complex Complex::operator*(Complex c) const {
	return Complex(vec2(z.x * c.z.x - z.y * c.z.y, z.x * c.z.y + z.y * c.z.x));
}


Complex Complex::conj() const {
	return Complex(vec2(z.x, -z.y));
}

Complex Complex::pow(int k) const {
	if (k == 0) return Complex(1, 0);
	if (k == 1) return *this;
	if (k < 0) return inv().pow(-k);
	if (k % 2 == 0) return pow(k / 2) * pow(k / 2);
	return *this * pow(k - 1);
}

Complex Complex::operator~() const {
	return Complex(z.x / norm2(z), -z.y / norm2(z));
}

void Complex::operator+=(const Complex &c) {
	z += c.z;
}

void Complex::operator-=(const Complex &c) {
	z -= c.z;
}

void Complex::operator*=(const Complex &c) {
	z = vec2(z.x * c.z.x - z.y * c.z.y, z.x * c.z.y + z.y * c.z.x);
}

void Complex::operator/=(const Complex &c) {
	*this *= c.inv();
}




Complex Complex::operator-() const {
	return Complex(-z);
}




float Complex::re() const {
	return z.x;
}

float Complex::im() const {
	return z.y;
}




float norm(Complex z) {
	return norm(z.z);
}

std::ostream &operator<<(std::ostream &_stream, const Complex &z) {
	_stream << z.z.x << " + " << z.z.y << "i";
	return _stream;
}

vec5 operator+(const vec5 &a, const vec5 &b) {
	return vec5(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w, a.v + b.v);
}

vec5 operator*(float scalar, const vec5 &a) {
	return a * scalar;
}

Complex toComplex(vec2 v) {
	return Complex(v);
}

float abs(Complex z) {
	return norm(z.z);
}

Complex operator ""i(long double im) {
	return Complex(0.f, static_cast<float>(im));
}

Complex operator ""i(unsigned long long int im) {
	return Complex(0, static_cast<int>(im));
}

float norm2(Complex z) {
	return norm2(z.z);
}


float Complex::arg() const {
	if (nearlyZero()) return 0;
	return atan2(z.y, z.x);
}

Complex Complex::operator/(Complex c) const {
	return c.inv() * (*this);
}

Complex Complex::operator+(float c) const {
	return (*this) + Complex(c);
}

Complex Complex::operator-(float c) const {
	return (*this) - Complex(c);
}

Complex Complex::operator*(float c) const {
	return (*this) * Complex(c);
}

Complex Complex::operator/(float c) const {
	return (*this) / Complex(c);
}

Complex Complex::operator+(int c) const {
	return (*this) + Complex(c);
}

Complex Complex::operator-(int c) const {
	return (*this) - Complex(c);
}

Complex Complex::operator*(int c) const {
	return (*this) * Complex(c);
}

Complex Complex::operator/(int c) const {
	return (*this) / Complex(c);
}


Complex Complex::square() const {
	return (*this) * (*this);
}

vec2 Complex::expForm() const {
	return vec2(norm(z), arg());
}

Complex fromExpForm(vec2 e) {
	return Complex(e.x * cos(e.y), e.x * sin(e.y));
}

Complex fromExpForm(float r, float theta) {
	return fromExpForm(vec2(r, theta));
}

bool Complex::operator==(Complex c) const {
	return z == c.z;
}

bool Complex::operator==(float f) const {
	return z == vec2(f, 0);
}

// Complex Complex::one() { return Complex(1, 0); }
//
// Complex Complex::zero() { return Complex(0, 0); }

Complex Complex::pow(float exponent) const {
	if (exponent == 0) return Complex(1, 0);
	if (exponent == 1) return *this;
	if (exponent == 2) return square();
	if (exponent == 0.5) return sqrt();
	if (exponent == -1) return inv();
	if (exponent == -2) return inv().square();
	if (norm(z) == 0) return Complex(0, 0);
	return fromExpForm(vec2(std::pow(norm(z), exponent), arg() * exponent));
}

Complex Complex::sqrt() const {
	if (z.x == 0 && z.y == 0) return Complex(0, 0);
	return fromExpForm(vec2(std::sqrt(norm(z)), arg() / 2));
}

Complex::operator glm::vec2() const {
	return z;
}

Complex::operator std::string() const {
	std::ostringstream out;
	out.precision(2);
	out << std::fixed << z.x << " + " << z.y << "i";
	return std::move(out).str();
}

bool Complex::nearlyEqual(Complex c) const {
	return abs(norm2(z - c.z)) < 1e-6;
}

Complex operator*(float f, const Complex &c) {
	return c * f;
}

Complex operator/(float f, const Complex &c) {
	return Complex(f) / c;
}

Complex operator+(float f, const Complex &c) {
	return c + f;
}

Complex operator-(float f, const Complex &c) {
	return Complex(f) - c;
}

Complex operator*(int f, const Complex &c) {
	return 1.f * f * c;
}

Complex operator/(int f, const Complex &c) {
	return 1.f * f / c;
}

Complex operator+(int f, const Complex &c) {
	return 1.f * f + c;
}

Complex operator-(int f, const Complex &c) {
	return 1.f * f - c;
}

Complex Complex::inv() const {
	return ~(*this);
}

float Complex::real() const {
	return re();
}

float Complex::imag() const {
	return im();
}


Complex rootOfUnity(int n, int k) {
	return Complex(std::cos(k * TAU / n), std::sin(k * TAU / n));
}

vector<Complex> allRootsOf1(int n) {
	vector<Complex> roots;
	for (int k = 0; k < n; k++) roots.push_back(rootOfUnity(n, k));
	return roots;
}

Complex exp(Complex c) {
	if (c.nearlyZero()) return Complex(1, 0);
	return Complex(std::cos(c.imag()), std::sin(c.imag())) * std::exp(c.real());
}

Complex log(Complex c) {
	return Complex(std::log(norm(c.z)), c.arg());
}

Complex sin(Complex c) {
	return Complex(sin(c.real()) * cosh(c.imag()), cos(c.real()) * sinh(c.imag()));
}

Complex cos(Complex c) {
	return Complex(cos(c.real()) * cosh(c.imag()), -sin(c.real()) * sinh(c.imag()));
}

Complex tan(Complex c) {
	return sin(c) / cos(c);
}

Complex sinh(Complex c) {
	return Complex(sinh(c.real()) * cos(c.imag()), cosh(c.real()) * sin(c.imag()));
}

Complex cosh(Complex c) {
	return Complex(cosh(c.real()) * cos(c.imag()), sinh(c.real()) * sin(c.imag()));
}

Complex tanh(Complex c) {
	return sinh(c) / cosh(c);
}

Complex asin(Complex c) {
	return 1.0i * log(1.0i * c + (1 - c.square()).sqrt()) * (-1);
}

Complex acos(Complex c) {
	return 1.0i * log(c + 1.0i * (1 - c.square()).sqrt()) * (-1);
}

Complex atan(Complex c) {
	return 1.0i / 2 * (log(1 - 1.0i * c) - log(1 + 1.0i * c));
}

Complex asinh(Complex c) {
	return log(c + (c.square() + 1).sqrt());
}

Complex acosh(Complex c) {
	return log(c + (c + 1).sqrt() * (c - 1).sqrt());
}

Complex atanh(Complex c) {
	return (log(1 + c) - log(1 - c)) / 2;
}

Complex sqrt(Complex c) {
	return c.sqrt();
}

Complex sq(Complex c) {
	return c.square();
}

Complex pow2(Complex c) {
	return c.square();
}

Complex pow3(Complex c) {
	return c.pow(3);
}

Complex pow4(Complex c) {
	return c.pow(4);
}

Complex pow(Complex c, float n) {
	return c.pow(n);
}

Complex pow(float x, Complex n) {
	return exp(n * log(x));
}


Quaternion Quaternion::operator*(Quaternion r) const {
	return Quaternion(q.x * r.q.x - q.y * r.q.y - q.z * r.q.z - q.w * r.q.w, q.x * r.q.y + q.y * r.q.x + q.z * r.q.w - q.w * r.q.z, q.x * r.q.z - q.y * r.q.w + q.z * r.q.x + q.w * r.q.y, q.x * r.q.w + q.y * r.q.z - q.z * r.q.y + q.w * r.q.x);
}

Quaternion Quaternion::pow(int p) const {
	if (p == 0) return Quaternion(1);
	if (p < 0) return ~pow(-p);
	if (p == 1) return *this;
	if (p % 2 == 0) return this->pow(p / 2) * this->pow(p / 2);
	return (*this) * this->pow(p / 2) * this->pow(p / 2);
}

vec3 Quaternion::rotate(vec3 im_x) const {
	auto q = normalise();
	auto x = Quaternion(im_x);
	return (q * x * q.conj()).im();
}

std::function<vec3(vec3)> Quaternion::rotation() const {
	return [this](vec3 x){
		return rotate(x);
	};
}

vec3 Quaternion::Hopf_map() const {
	auto q = normalise();
	return (q.conj() * i() * q).im();
}

vec4 Quaternion::rotateS3(vec4 x) const {
	auto q = normalise();
	auto h = Quaternion(x);
	return (q.conj() * h * q).q;
}




vec2 intersectLines(vec2 p1, vec2 p2, vec2 q1, vec2 q2) {
	return (p1 * (q1.y - q2.y) - p2 * (q1.y - q2.y) - q1 * (p1.y - p2.y) + q2 * (p1.y - p2.y)) / ((p1.x - p2.x) * (q1.y - q2.y) - (p1.y - p2.y) * (q1.x - q2.x));
}


Complex intersectLines(Complex p1, Complex p2, Complex q1, Complex q2) {
	return Complex(intersectLines(p1.z, p2.z, q1.z, q2.z));
}

mat3 scaleMatrix3(vec3 s) {
	return mat3(s.x, 0, 0, 0, s.y, 0, 0, 0, s.z);
}

mat3 scaleMatrix3(float s) {
	return scaleMatrix3(vec3(s, s, s));
}

mat3 rotationMatrix3(float angle) {
	return mat3(cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1);
}

mat2 rotationMatrix2(float angle) {
	return mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
}


mat3 rotationMatrix3(vec3 axis, float angle) {
	auto planeBasis = orthogonalComplementBasis(axis);
	mat3 change = changeOfBasis(planeBasis.first, planeBasis.second, axis);
	return inverse(change) * rotationMatrix3(angle) * change;
}

mat3 rotationBetween(vec3 v0, vec3 v1) {
	vec3 axis = normalise(cross(v0, v1));
	float angle = acos(dot(v0, v1) / (norm(v0) * norm(v1)));
	return rotationMatrix3(axis, angle);
}

mat2 rotationBetween(vec2 v0, vec2 v1) {
	float angle = acos(dot(v0, v1) / (norm(v0) * norm(v1)));
	return rotationMatrix2(angle);
}


mat3 GramSchmidtProcess(mat3 m) {
	vec3 u1 = normalise(m[0]);
	vec3 u2 = normalise(m[1] - u1 * dot(u1, m[1]));
	vec3 u3 = normalise(m[2] - u1 * dot(u1, m[2]) - u2 * dot(u2, m[2]));
	mat3 ma = mat3(u1, u2, u3);
	return ma;
}

vec5::vec5(float x, float y, float z, float w, float v) : x(x), y(y), z(z), w(w), v(v) {}

vec5::vec5(std::initializer_list<float> list) {
	if (list.size() != 5) throw std::format_error("vec5 must be initialized with 5 values");
	auto it = list.begin();
	x = *it; it++;
	y = *it; it++;
	z = *it; it++;
	w = *it; it++;
	v = *it;
}

vec5::vec5(float x) : x(x), y(x), z(x), w(x), v(x) {}

vec5::vec5() : x(0), y(0), z(0), w(0), v(0) {}

auto vec5::operator[](int i) const -> float {
	switch (i) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		case 4: return v;
		default: throw IndexOutOfBounds(i, 5, "Index out of bounds in vec5", __FILE__, __LINE__);
	}
}

vec5 vec5::operator-(const vec5 &b) const {
	return vec5(x - b.x, y - b.y, z - b.z, w - b.w, v - b.v);
}

vec5 vec5::operator-() const {
	return vec5(-x, -y, -z, -w, -v);
}

vec5 vec5::operator*(float scalar) const {
	return vec5(x * scalar, y * scalar, z * scalar, w * scalar, v * scalar);
}

vec5 vec5::operator/(float scalar) const {
	if (scalar == 0) throw ZeroDivisionError("Division by zero in vec5 division", __FILE__, __LINE__);
	return vec5(x / scalar, y / scalar, z / scalar, w / scalar, v / scalar);
}

vec5 vec5::zero() { return vec5(0, 0, 0, 0, 0); }



float randomUniform(float a, float b) {
	return a + (b - a) * (rand() / (RAND_MAX + 1.0));
}

vec2 randomUniform(vec2 a, vec2 b) {
	return vec2(randomUniform(a.x, b.x), randomUniform(a.y, b.y));
}

vec3 randomUniform(vec3 a, vec3 b) {
	return vec3(randomUniform(a.x, b.x), randomUniform(a.y, b.y), randomUniform(a.z, b.z));
}

float rotationAngle(const mat3 &M) {
	return acos(rotationCosAngle(M));
}

float rotationCosAngle(mat3 M) {
	return (M[0][0] + M[1][1] + M[2][2] - 1) / 2;
}

vec3 rotationAxis(mat3 M) {
	float sinAngle = sqrt(1 - pow(rotationCosAngle(M), 2));
	return vec3(M[2][1] - M[1][2], M[0][2] - M[2][0], M[1][0] - M[0][1]) / (2 * sinAngle);
}

mat3 spinTensor(vec3 omega) {
	return mat3(0, -omega.z, omega.y, omega.z, 0, -omega.x, -omega.y, omega.x, 0);
}

mat3 rotationMatrix(vec3 axis, float angle) {
	vec3 n = normalise(axis);
	float nx = n.x;
	float ny = n.y;
	float nz = n.z;

	return mat3(cos(angle) + nx * nx * (1 - cos(angle)), nx * ny * (1 - cos(angle)) - nz * sin(angle), nx * nz * (1 - cos(angle)) + ny * sin(angle), ny * nx * (1 - cos(angle)) + nz * sin(angle), cos(angle) + ny * ny * (1 - cos(angle)), ny * nz * (1 - cos(angle)) - nx * sin(angle), nz * nx * (1 - cos(angle)) - ny * sin(angle), nz * ny * (1 - cos(angle)) + nx * sin(angle), cos(angle) + nz * nz * (1 - cos(angle)));
}

mat3 rotationMatrix(vec3 omega) {
	return rotationMatrix(normalise(omega), length(omega));
}


vec3 projectVectorToPlane(vec3 v, vec3 n) {
	return v - dot(v, n) * n;
}

pair<vec3, vec3> orthogonalComplementBasis(vec3 v) {
	vec3 b1 = norm(cross(v, normalise(e1 - e2 + e3))) > norm(cross(v, normalise(e1 + e2))) ? normalise(e1 - e2 + e3) : normalise(e1 + e2);
	vec3 b2 = cross(normalise(v), b1);
	mat3 frame = GramSchmidtProcess(mat3(normalise(v), b1, b2));
	return std::make_pair(frame[1], frame[2]);
}

float pseudorandomizer(float x, float seed) {
	return frac(sin(x + seed) * 43758.5453f + seed);
}


SparseMatrix::SparseMatrix(int n, int m) {
	this->n = n;
	this->m = m;
	this->data = vector<vector<pair<int, float> > >(n);
}

void SparseMatrix::set(int i, int j, float val) {
	this->data[i].emplace_back(j, val);
}

float SparseMatrix::get(int i, int j) {
	for (auto p: this->data[i]) if (p.first == j) return p.second;
	return 0;
}

SparseMatrix SparseMatrix::operator*(float f) {
	SparseMatrix result = SparseMatrix(this->n, this->m);
	for (int i = 0; i < this->n; i++) for (auto p: this->data[i]) result.set(i, p.first, p.second * f);
	return result;
}

SparseMatrix SparseMatrix::operator+(const SparseMatrix &M) {
	SparseMatrix result = SparseMatrix(this->n, this->m);
	for (int i = 0; i < this->n; i++) for (auto p: this->data[i]) result.set(i, p.first, p.second);
	for (int i = 0; i < M.n; i++) for (auto p: M.data[i]) result.set(i, p.first, result.get(i, p.first) + p.second);
	return result;
}

SparseMatrix SparseMatrix::operator-(const SparseMatrix &M) {
	SparseMatrix result = SparseMatrix(this->n, this->m);
	for (int i = 0; i < this->n; i++) for (auto p: this->data[i]) result.set(i, p.first, p.second);
	for (int i = 0; i < M.n; i++) for (auto p: M.data[i]) result.set(i, p.first, result.get(i, p.first) - p.second);
	return result;
}

FloatVector::FloatVector(vec2 data) {
	this->data = vecToVecHeHe(data);
}

FloatVector::FloatVector(vec3 data) {
	this->data = vecToVecHeHe(data);
}

FloatVector::FloatVector(vec4 data) {
	this->data = vecToVecHeHe(data);
}

FloatVector::FloatVector(const vector<vector<float> > &data) {
	this->data = data[0];
	for (int i = 1; i < data.size(); i++) for (float j: data[i]) this->data.push_back(j);
}

FloatVector::FloatVector(int n, float val) {
	data = vector<float>();
	data.reserve(n);
	for (int i = 0; i < n; i++) this->data.push_back(val);
}

FloatVector::FloatVector(int n) : FloatVector(n, 0) {}

FloatVector::FloatVector(FloatVector &&other) noexcept : data(std::move(other.data)) {}

FloatVector &FloatVector::operator=(const FloatVector &other) {
	if (this == &other) return *this;
	data = other.data;
	return *this;
}

FloatVector &FloatVector::operator=(FloatVector &&other) noexcept {
	if (this == &other) return *this;
	data = std::move(other.data);
	return *this;
}

float FloatVector::operator[](int i) const {
	return this->data[i];
}

FloatVector FloatVector::operator/(float f) const {
	return *this * (1.f / f);
}

FloatVector FloatVector::operator-() const {
	return *this * -1;
}

void FloatVector::operator+=(const FloatVector &v) {
	for (int i = 0; i < this->data.size(); i++) this->data[i] += v[i];
}

void FloatVector::operator-=(const FloatVector &v) {
	for (int i = 0; i < this->data.size(); i++) this->data[i] -= v[i];
}

void FloatVector::operator*=(float f) {
	for (float &i: this->data) i *= f;
}

void FloatVector::operator/=(float f) {
	*this *= 1.f / f;
}

void FloatVector::append(float f) {
	this->data.push_back(f);
}

void FloatVector::append(const FloatVector &v) {
	for (float f: v.data) this->data.push_back(f);
}

void FloatVector::append(const vector<float> &v) {
	for (float f: v) this->data.push_back(f);
}

vector<float> FloatVector::getVec() const {
	return this->data;
}

int FloatVector::size() {
	return this->data.size();
}

FloatVector concat(const FloatVector &a, const FloatVector &b) {
	FloatVector res = a;
	res.append(b);
	return res;
}

FloatVector FloatVector::operator*(float f) const {
	vector<float> result = vector<float>(this->data.size());
	for (int i = 0; i < this->data.size(); i++) result[i] = this->data[i] * f;
	return FloatVector(result);
}

FloatVector FloatVector::operator+(const FloatVector &v) const {
	vector<float> res;
	for (int i = 0; i < this->data.size(); i++) res.push_back(this->data[i] + v[i]);
	return FloatVector(res);
}

FloatVector FloatVector::operator-(const FloatVector &v) const {
	vector<float> res;
	for (int i = 0; i < this->data.size(); i++) res.push_back(this->data[i] - v[i]);
	return FloatVector(res);
}



FloatMatrix::FloatMatrix(int n, int m) {
	this->data = MATR$X(n, vec2137(m, 0));
}

FloatMatrix FloatMatrix::submatrix(int i, int j) {
	MATR$X sub = MATR$X();
	for (int k = 0; k < n(); k++) {
		if (k == i) continue;
		vector<float> row = vector<float>();
		for (int l = 0; l < m(); l++) {
			if (l == j) continue;
			row.push_back(this->data[k][l]);
		}
		sub.push_back(row);
	}
	return FloatMatrix(sub);
}

FloatMatrix FloatMatrix::diagonalComponent() const {
	FloatMatrix res = FloatMatrix(this->n(), this->m());
	for (int i = 0; i < this->n(); i++) res.set(i, i, this->get(i, i));
	return res;
}

FloatMatrix FloatMatrix::invertedDiagonal() const {
	FloatMatrix res = FloatMatrix(this->n(), this->m());
	for (int i = 0; i < this->n(); i++) res.set(i, i, 1 / this->get(i, i));
	return res;
}

FloatMatrix FloatMatrix::subtractedDiagonal() const {
	return *this - diagonalComponent();
}

vector<float> FloatMatrix::operator[](int i) {
	return this->data[i];
}

ivec2 FloatMatrix::size() const {
	return ivec2(n(), m());
}

int FloatMatrix::n() const {
	return this->data.size();
}

int FloatMatrix::m() const {
	return this->data[0].size();
}

FloatMatrix::operator float() {
	if (n() != m() || n() != 1) throw ValueError("wrong dimension of matrix (not 1x1)", __FILE__, __LINE__);
	return this->data[0][0];
}

int binomial(int n, int k) {
	if (k > n) return 0;
	if (k == 0 || k == n) return 1;
	return binomial(n - 1, k - 1) + binomial(n - 1, k);
}

FloatMatrix::FloatMatrix(const MATR$X &data) {
	this->data = data;
};
FloatMatrix::FloatMatrix(vec69 &&data) : FloatMatrix({data}) {}

void FloatMatrix::set(int i, int j, float val) {
	this->data[i][j] = val;
}

float FloatMatrix::get(int i, int j) const {
	return this->data[i][j];
}

bool FloatMatrix::isSquare() const {
	return this->n() == this->m();
}

float FloatMatrix::det() {
	if (n() != m()) throw std::invalid_argument("Matrix must be square");
	if (n() == 1) return this->data[0][0];
	float result = 0;
	for (int j = 0; j < m(); j++) if (this->data[0][j] != 0) result += this->data[0][j] * submatrix(0, j).det() * (j % 2 == 0 ? 1 : -1);
	return result;
}


FloatMatrix FloatMatrix::transpose() const {
	MATR$X result = MATR$X();
	result.reserve(n());
	for (int i = 0; i < n(); i++) result[i].reserve(m());
	for (int i = 0; i < m(); i++) {
		vector<float> row = vector<float>();
		for (int j = 0; j < n(); j++) row.push_back(this->data[j][i]);
		result.push_back(row);
	}
	return FloatMatrix(data);
}


FloatMatrix FloatMatrix::operator*(float f) const {
	FloatMatrix result = FloatMatrix(this->data);
	for (int j = 0; j < n(); j++) for (int i = 0; i < m(); i++) result.data[j][i] *= f;
	return result;
}

FloatMatrix FloatMatrix::operator+(const FloatMatrix &M) const {
	FloatMatrix result = FloatMatrix(this->data);
	for (int j = 0; j < n(); j++) for (int i = 0; i < m(); i++) result.data[j][i] += M.data[j][i];
	return result;
}

FloatMatrix FloatMatrix::operator-(const FloatMatrix &M) const {
	FloatMatrix result = FloatMatrix(this->data);
	for (int j = 0; j < n(); j++) for (int i = 0; i < m(); i++) result.data[j][i] -= M.data[j][i];
	return result;
}

FloatMatrix FloatMatrix::operator*(const FloatMatrix &M) const {
	if (m() != M.n()) throw std::invalid_argument("Matrix dimensions must agree");
	FloatMatrix result = FloatMatrix(n(), M.m());
	for (int i = 0; i < n(); i++) for (int j = 0; j < M.m(); j++) for (int k = 0; k < m(); k++) result.data[i][j] += this->data[i][k] * M.data[k][j];
	return result;
}

FloatMatrix FloatMatrix::operator*(const vector<vector<float> > &M) const {
	FloatMatrix result = FloatMatrix(n(), M[0].size());
	for (int i = 0; i < n(); i++) for (int j = 0; j < M[0].size(); j++) for (int k = 0; k < m(); k++) result.data[i][j] += this->data[i][k] * M[k][j];
	return result;
}

vector<float> FloatMatrix::operator*(const vector<float> &v) const {
	return (*this * vector<vec69 >{v})[0];
}

FloatVector FloatMatrix::operator*(const FloatVector &v) const {
	return FloatVector(*this * v.getVec());
}

FloatMatrix FloatMatrix::operator-() const {
	return *this * (-1.f);
}

FloatMatrix FloatMatrix::operator/(float x) const {
	return *this * (1.f / x);
}


float dot(const FloatVector &a, const FloatVector &b) {
	float res = 0;
	for (int i = 0; i < a.data.size(); i++) res += a[i] * b[i];
	return res;
}

FloatMatrix operator*(const vector<vector<float> > &M, const FloatMatrix &B) {
	FloatMatrix result = FloatMatrix(M.size(), B.m());
	for (int i = 0; i < M.size(); i++) for (int j = 0; j < B.m(); j++) for (int k = 0; k < M[0].size(); k++) result.data[i][j] += M[i][k] * B.data[k][j];
	return result;
}

FloatMatrix FloatMatrix::inv() {
	if (n() != m()) throw std::invalid_argument("Matrix must be square");
	float d = det();
	if (d == 0) throw std::invalid_argument("Matrix must be invertible");
	FloatMatrix result = FloatMatrix(n(), m());
	for (int i = 0; i < n(); i++) for (int j = 0; j < m(); j++) result.data[i][j] = submatrix(j, i).det() * ((i + j) % 2 == 0 ? 1 : -1);
	return result / d;
}

FloatMatrix FloatMatrix::pow(int p) {
	if (n() != m()) throw std::invalid_argument("Matrix must be square");
	if (p < 0) return ~(this)->pow(-p);
	if (p == 0) return FloatMatrix(n(), m());
	if (p == 1) return *this;
	if (p == 2) return *this * *this;
	if (p % 2 == 0) return this->pow(p / 2) * this->pow(p / 2);
	return (*this) * this->pow(p / 2) * this->pow(p / 2);
}

FloatMatrix FloatMatrix::operator~() {
	return inv();
}

FloatMatrix FloatMatrix::GramSchmidtProcess() {
	FloatMatrix result = FloatMatrix(this->data);
	for (int i = 0; i < n(); i++)
		for (int j = 0; j < m(); j++)
			if (i == 0) result.data[i][j] = this->data[i][j];
			else {
				float sum = 0;
				for (int k = 0; k < i; k++) sum += result.data[k][j] * this->data[i][k];
				result.data[i][j] = this->data[i][j] - sum;
			}
	return result;
}

FloatMatrix::FloatMatrix(vector<vector<float> > &&data) {
	this->data = std::move(data);
}


pair<Complex, Complex> eigenvalues(mat2 m) {
	float tr = m[0][0] + m[1][1];
	float det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	float D = tr * tr - 4.f * det;
	if (D < 0) return {(tr + 1.0i * sqrt(-D)) / 2, (tr - 1.0i * sqrt(-D)) / 2};
	return {Complex(tr + sqrt(D)) / 2.f, Complex(tr - sqrt(D)) / 2.f};
}


pair<vec2, mat2> eigendecomposition(mat2 m) {
	vec2 lambda = eigenvaluesReal(m);
	if (isClose(m[0][1] * m[1][0], 0)) lambda = vec2(m[0][0], m[1][1]);
	vec2 v1 = vec2(lambda.x - m[1][1], m[0][1]);
	vec2 v2 = vec2(m[1][0], lambda.y - m[0][0]);
	return std::make_pair(lambda, mat2(v1, v2));
}

float polarAngle(vec3 v, vec3 t1, vec3 t2) {
	return atan2(dot(v, t2), dot(v, t1));
}

float polarAngle(vec3 v, vec3 n) {
	return polarAngle(v, orthogonalComplementBasis(n).first, orthogonalComplementBasis(n).second);
}

float cot(float x) {
	return 1.f / tan(x);
}

Complex cot(Complex c) {
	return 1.f / tan(c);
}

vec3 stereoProjection(vec4 v) {
	return vec3(v.x, v.y, v.z) / (1.f - v.w);
}

vec3 stereoProjection(Quaternion v) {
	return stereoProjection((vec4) v);
}

vec2 stereoProjection(vec3 v) {
	return vec2(v.x, v.y) / (.5f - v.z);
}

float stereoProjection(vec2 v) {
	return v.x / (.5f - v.y);
}

vec4 stereoProjectionInverse(vec3 v) {
	float d2 = norm2(v);
	return vec4(v.x, v.y, v.z, (d2 - 1) / 2 + .5f) / (d2 + 1);
}

vec3 stereoProjectionInverse(vec2 v) {
	float d2 = norm2(v);
	return vec3(v.x, v.y, (d2 - 1) / 2) / (d2 + 1) + .5f;
}

vec2 stereoProjectionInverse(float t) {
	return vec2(t / (t * t + 1), (t * t - 1) / (t * t + 1) / 2 + .5f);
}
