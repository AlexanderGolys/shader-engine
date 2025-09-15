#pragma once
#pragma GCC diagnostic ignored "-Wliteral-suffix"

#include <chrono>
#include <exception>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <stack>

#include "exceptions.hpp"
#include "concepts.hpp"
#include "monads.hpp"




inline string removeWhitespace(const string &s)
{
	string result;
	for (char c : s)
		if (!isspace(c)) result += c;
	return result;
}

inline string operator""s(const char* str) {
	return string(str);
}


inline string vecToString(vec2 v) {
	return "(" + to_string(v.x) + ", " + to_string(v.y) + ")"; }

inline string vecToString(vec3 v) {
	return "(" + to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z) + ")"; }

inline string vecToString(vec4 v) {
	return  "(" + to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z) + ", " + to_string(v.w) + ")"; }

inline string vecToString(ivec2 v) {
	return "(" + to_string(v.x) + ", " + to_string(v.y) + ")"; }

inline string vecToString(ivec3 v) {
	return "(" + to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z) + ")"; }

inline string vecToString(ivec4 v) {
	return  "(" + to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z) + ", " + to_string(v.w) + ")"; }




template<typename X>
void add_all(vector<X> a, vector<X> b) {
	a.insert(a.end(), b.begin(), b.end());
}

// template<typename X>
// vector<X> concat(vector<X> a, vector<X> b) {
// 	auto c = a;
// 	c.insert(c.end(), b.begin(), b.end());
// 	return c;
// }

namespace glm {
    inline vec3 xyz(const vec4& v) {
        return vec3(v.x, v.y, v.z);
    }
    inline vec3 yzw(const vec4& v) {
        return vec3(v.y, v.z, v.w);
    }
    inline vec2 xy(const vec4& v) {
        return vec2(v.x, v.y);
    }
    inline vec2 yz(const vec4& v) {
        return vec2(v.y, v.z);
    }
    inline vec2 zw(const vec4& v) {
        return vec2(v.z, v.w);
    }
    inline vec2 xy(const vec3& v) {
        return vec2(v.x, v.y);
    }
    inline vec2 yz(const vec3& v) {
        return vec2(v.y, v.z);
    }

    inline vec4 xyz1(const vec3& v) {
        return vec4(v, 1);
    }

	inline vec2 operator*(const vec2& v, int i) { return v*(1.f*i); }
	inline vec3 operator*(const vec3& v, int i) { return v*(1.f*i); }
	inline vec4 operator*(const vec4& v, int i) { return v*(1.f*i); }
	inline mat2 operator*(const mat2& v, int i) { return v*(1.f*i); }
	inline mat3 operator*(const mat3& v, int i) { return v*(1.f*i); }
	inline mat4 operator*(const mat4& v, int i) { return v*(1.f*i); }
	inline vec2 operator/(const vec2& v, int i) { return v/(1.f*i); }
	inline vec3 operator/(const vec3& v, int i) { return v/(1.f*i); }
	inline vec4 operator/(const vec4& v, int i) { return v/(1.f*i); }
	inline mat2 operator/(const mat2& v, int i) { return v/(1.f*i); }
	inline mat3 operator/(const mat3& v, int i) { return v/(1.f*i); }
	inline mat4 operator/(const mat4& v, int i) { return v/(1.f*i); }


}

inline int factorial(int n) {
	if (n < 0) throw std::invalid_argument("Negative factorial");
	if (n == 0 || n == 1) return 1;
	int res = 1;
	for (int i = 2; i <= n; i++)
		res *= i;
	return res;
}

inline int binom(int a, int b) {
	return a >= b ? factorial(a) / (factorial(b) * factorial(a - b)) : 0;
}

    template<typename T>
    void printVector(vector<T> v, string title="vector")
    {
        std::cout << title << " [" << v.size() << "]: ";
        for (int i = 0; i < v.size(); i++)
        {
            std::cout << v[i] << ", ";
        }
        std::cout << std::endl;
    }

constexpr PolyGroupID DEFAULT_POLY_GROUP_ID = PolyGroupID(0);
constexpr RP1 inf = std::nullopt;
constexpr RP1 unbounded = std::nullopt;
const vec3 e1 = vec3(1, 0, 0);
const vec3 e2 = vec3(0, 1, 0);
const vec3 e3 = vec3(0, 0, 1);
const vec3 ORIGIN = vec3(0, 0, 0);
const vec2 PLANE_ORIGIN = vec2(0, 0);
constexpr PolyGroupID DFLT_CURV = PolyGroupID(420);
const vector std_basis3 = {e1, e2, e3};

class ivec8 {
	ivec4 a, b;
public:
	ivec8(ivec4 a, ivec4 b) : a(a), b(b) {}
	ivec8(int a, int b, int c, int d, int e, int f, int g, int h) : a(a, b, c, d), b(e, f, g, h) {}
	explicit ivec8(int i) : a(i), b(i) {}
	int operator[](int i) const { return i < 4 ? a[i] : b[i - 4]; }
	ivec8 operator+(int i) const { return ivec8(a + i, b + i); }
	ivec8 operator-(int i) const { return ivec8(a - i, b - i); }
	ivec8 operator*(int i) const { return ivec8(a * i, b * i); }
	ivec8 operator/(int i) const { return ivec8(a / i, b / i); }
	ivec8 operator+(ivec8 v) const { return ivec8(a + v.a, b + v.b); }
	ivec8 operator-(ivec8 v) const { return ivec8(a - v.a, b - v.b); }
	ivec8 operator*(ivec8 v) const { return ivec8(a * v.a, b * v.b); }
	ivec8 operator/(ivec8 v) const { return ivec8(a / v.a, b / v.b); }
	ivec8 operator-() const { return ivec8(-a, -b); }
	ivec8 operator%(ivec8 v) const { return ivec8(a % v.a, b % v.b); }
	static int size() { return 8; }
//	friend int dot(ivec8 a, ivec8 b) { return dot(1.F*a.a, b.a) + dot(a.b, b.b); }
	ivec4 xyzw() const { return a; }
	ivec4 stuv() const { return b; }
	ivec3 xyz() const { return ivec3(a); }
	ivec3 yzw() const { return ivec3(a.y, a.z, a.w); }
	ivec3 stv() const { return ivec3(b); }
	ivec3 tvu() const { return ivec3(b.y, b.z, b.w); }
	ivec2 xy() const { return ivec2(a); }
	ivec2 yz() const { return ivec2(a.y, a.z); }
	ivec2 zw() const { return ivec2(a.z, a.w); }
	ivec2 st() const { return ivec2(b); }
	ivec2 tu() const { return ivec2(b.y, b.z); }
	ivec2 uv() const { return ivec2(b.z, b.w); }
	int x() const { return a.x; }
	int y() const { return a.y; }
	int z() const { return a.z; }
	int w() const { return a.w; }
};

class ivec6 {
	ivec3 a, b;
public:
	ivec6(ivec3 a, ivec3 b) : a(a), b(b) {}
	ivec6(int a, int b, int c, int d, int e, int f) : a(a, b, c), b(d, e, f) {}
	explicit ivec6(int i) : a(i), b(i) {}
	int operator[](int i) const { return i < 3 ? a[i] : b[i - 3]; }
	ivec6 operator+(int i) const { return ivec6(a + i, b + i); }
	ivec6 operator-(int i) const { return ivec6(a - i, b - i); }
	ivec6 operator*(int i) const { return ivec6(a * i, b * i); }
	ivec6 operator/(int i) const { return ivec6(a / i, b / i); }
	ivec6 operator+(ivec6 v) const { return ivec6(a + v.a, b + v.b); }
	ivec6 operator-(ivec6 v) const { return ivec6(a - v.a, b - v.b); }
	ivec6 operator*(ivec6 v) const { return ivec6(a * v.a, b * v.b); }
	ivec6 operator/(ivec6 v) const { return ivec6(a / v.a, b / v.b); }
	ivec6 operator-() const { return ivec6(-a, -b); }
	ivec6 operator%(ivec6 v) const { return ivec6(a % v.a, b % v.b); }
	static int size() { return 6; }
	bool operator==(ivec6 v) const { return a == v.a && b == v.b; }
//	friend int dot(ivec6 a, ivec6 b) { return dot(a.a, b.a) + dot(a.b, b.b); }
	ivec3 xyz() const { return ivec3(a); }
	ivec3 stv() const { return ivec3(b); }
	ivec2 xy() const { return ivec2(a); }
	ivec2 yz() const { return ivec2(a.y, a.z); }
	ivec2 st() const { return ivec2(b); }
	ivec2 tu() const { return ivec2(b.y, b.z); }
	int x() const { return a.x; }
	int y() const { return a.y; }
	int z() const { return a.z; }
	int s() const { return b.x; }
	int t() const { return b.y; }
	int u() const { return b.z; }

	vector<int> toVec() const { return {a.x, a.y, a.z, b.x, b.y, b.z}; }
};



inline vector<int> range(int a, int b, int step=1) {
	if (step == 0) throw std::invalid_argument("step cannot be 0");

	vector<int> res = vector<int>();
	res.reserve(abs(b - a) / abs(step));

	if (step > 0) {
		for (int i = a; i < b; i += step) res.push_back(i);
		return res;
	}

	for (int i = b; i > a; i += step) res.push_back(i);
	return res;
}


inline vector<int> range(int b) {
	return range(0, b); }


class Permutation {
public:
	vector<int> perm;
	explicit Permutation(const vector<int>& direct_img) : perm(direct_img) {}
	explicit Permutation(const vector<int>& cycle, int size) : perm(range(size)) {
		for (int i = 0; i < cycle.size(); i++)
			perm[cycle[i]] = cycle[(i + 1) % cycle.size()];
	}
	explicit Permutation(const vector<vector<int>>& cycles, int size) : perm(range(size)) {
		for (auto& cycle : cycles)
			for (int i = 0; i < cycle.size(); i++)
				perm[cycle[i]] = cycle[(i + 1) % cycle.size()];
	}
	int operator()(int i) const { return perm[i]; }
	HOM(int, int) func() const { return [this](int i) { return perm[i]; }; }
	int size() const { return perm.size(); }
	Permutation operator&(const Permutation &p) const;
	Permutation operator~() const;
	// Permutation concat(const Permutation &p) const {
	// 	return Permutation(concat<int>(perm, p.perm));
	// }
};


template<typename T>
vector<T> flattened2DVector(vector<vector<T>> v)
{
	vector<T> res;
	for (auto& row : v)
		res.insert(res.end(), row.begin(), row.end());
	return res;
}


inline int flattened2DVectorIndex(int i, int j, ivec2 size) {
	if (i < 0) return flattened2DVectorIndex(size.x + i, j, size);
	if (j < 0) return flattened2DVectorIndex(i, size.y + j, size);
	return i * size.y + j;
}


template<typename T>
T flattened2DVectorSample(vector<T> flattened, int i, int j, ivec2 size) {
	return flattened[flattened2DVectorIndex(i, j, size)];
}



template<typename T>
vector<T> flattened3DVector(vector<vector<vector<T>>> v)
{
	vector<T> res;
	for (auto& row : v)
		for (auto& col : row)
			res.insert(res.end(), col.begin(), col.end());
	return res;
}


inline int flattened3DVectorIndex(int i, int j, int k,  ivec3 size) {
	if (i < 0) return flattened3DVectorIndex(size.x + i, j, k, size);
	if (j < 0) return flattened3DVectorIndex(i, size.y + j, k, size);
	if (k < 0) return flattened3DVectorIndex(i, j, size.z + k, size);
	if (i >= size.x || j >= size.y || k >= size.z) throw IndexOutOfBounds(format("({},{},{})", i, j, k), format("({},{},{})", size[0], size[1],size[2]),"flattened3DVectorIndex", __FILE__, __LINE__);
	return i * size.y * size.z + j * size.z + k;
}


template<typename T>
T flattened3DVectorSample(vector<T> flattened, int i, int j, int k, ivec3 size) {
	return flattened[flattened3DVectorIndex(i, j, k, size)];
}

inline int flattened4DVectorIndex(int i, int j, int k, int m,  ivec4 size) {
	if (i < 0) return flattened4DVectorIndex(size.x + i, j, k,m, size);
	if (j < 0) return flattened4DVectorIndex(i, size.y + j, k,m, size);
	if (k < 0) return flattened4DVectorIndex(i, j, size.z + k,m, size);
	if (m < 0) return flattened4DVectorIndex(i, j, k, size.w + m, size);
	if (i >= size.x || j >= size.y || k >= size.z || m>size.w) throw IndexOutOfBounds(format("({},{},{},{})", i, j, k, m), format("({},{},{},{})", size[0], size[1],size[2], size[3]),"flattened4DVectorIndex", __FILE__, __LINE__);

	return i * size.y * size.z * size.w + j * size.z * size.w + k * size.w + m;
}




template<typename T>
T flattened4DVectorSample(vector<T> flattened, int i, int j, int k, int m, ivec4 size) {
	return flattened[flattened4DVectorIndex(i, j, k, m, size)];
}

template<typename T>
bool contains(const vector<T> &v, T x) {
	return std::find(v.begin(), v.end(), x) != v.end();
}

template<typename T>
bool containsAll(const vector<T> &big, const vector<T> &subset) {
	for (auto& x : subset)
		if (!contains(big, x))
			return false;
	return true;
}

template<typename T, typename U>
vector<T> keys(const std::map<T, U> &m) {
	vector<T> res;
	for (auto& [k, v] : m)
		res.push_back(k);
	return res;
}

template<typename T, typename U>
std::set<U> values(const std::map<T, U> &m) {
	std::set<U> res;
	for (auto& [k, v] : m)
		res.insert(v);
	return res;
}

template<typename T, typename U>
vector<U> valuesVec(const std::map<T, U> &m) {
	vector<U> res;
	for (auto& [k, v] : m)
		res.push_back(v);
	return res;
}

template<typename T>
vector<T> setToVector(const std::set<T> &s) {
	vector<T> res;
	for (auto& x : s)
		res.push_back(x);
	return res;
}

template<typename T, int n>
array<T, n> vectorToArray(const vector<T> &s) {
	array<T, n> res;
	for (int i = 0; i < n; i++)
		res[i] = s[i];
	return res;
}

template<typename T, int n, int m=n>
array<array<T, n>, m> vecVecToArray(const vector<T> &s) {
	array<array<T, n>, m> res;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			res[i][j] = s[i * n + j];
	return res;
}

template<typename T, int n, int m=n>
vector<vector<T>> arrayToVecVec(const array<array<T, n>, m> &s) {
	vector<vector<T>> res;
	res.reserve(m);
	for (int i = 0; i < m; i++)
		res.emplace_back(s[i].begin(), s[i].end());
	return res;
}

template<typename T, int n>
array<T, n> setToArray(const std::set<T> &s) {
	return vectorToArray<T, n>(setToVector(s));
}

inline float dot(float x, float y) { return x * y; }

template<typename  V>
float norm2(V v) {
	return dot(v, v);
}

inline float norm2(mat2 m) {
	return m[0][0]*m[0][0] + m[0][1]*m[0][1] + m[1][0]*m[1][0] + m[1][1]*m[1][1];
}

inline float norm2(mat3 m) {
	float res = 0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			res += m[i][j] * m[i][j];
	return res;
}

inline float norm2(mat4 m) {
	float res = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			res += m[i][j] * m[i][j];
	return res;
}

template <typename  T>
float norm(T v) {
	return sqrt(norm2(v));
}

inline array<array<float, 2>, 2> mat2ToArray(mat2 m) {
	return vectorToArray<array<float, 2>, 2>({{m[0][0], m[0][1]}, {m[1][0], m[1][1]} }); }

inline array<array<float, 3>, 3> mat3ToArray(mat3 m) {
	return vectorToArray<array<float, 3>, 3>({{m[0][0], m[0][1], m[0][2]}, {m[1][0], m[1][1], m[1][2]}, {m[2][0], m[2][1], m[2][2]} }); }

//inline array<array<float, 4>, 4> mat4ToArray(mat4 m) {
//	return {{{m[0][0], m[0][1], m[0][2], m[0][3]}, {m[1][0], m[1][1], m[1][2], m[1][3]}, {m[2][0], m[2][1], m[2][2], m[2][3]}, {m[3][0], m[3][1], m[3][2], m[3][3]} }};}


inline MATR$X matToVecVec(const mat2 &m) { return { {m[0][0], m[0][1]}, {m[1][0], m[1][1]} }; }
inline MATR$X matToVecVec(const mat3 &m) { return { {m[0][0], m[0][1], m[0][2]}, {m[1][0], m[1][1], m[1][2]}, {m[2][0], m[2][1], m[2][2]} }; }
//inline MATR$X matToVecVec(const mat4 &m) { return arrayToVecVec(mat4ToArray(m)); }

template<typename T>
vector<T> flatten  (vector<vector<T>> v) {
	vector<T> res;
	for (auto& row : v)
		res.insert(res.end(), row.begin(), row.end());
	return res;
}

template<typename T>
vector<vector<T>> grid (T x0, T d1, T d2, int n1, int n2) {
	vector<vector<T>> res;
	for (int i = 0; i < n1; i++) {
		vector<T> row;
		for (int j = 0; j < n2; j++)
			row.push_back(x0 + d1 * i + d2 * j);
		res.push_back(row);
	}
	return res;
}

template<typename T>
vector<vector<vector<T>>> grid (T x0, T d1, T d2, T d3, int n1, int n2, int n3) {
	vector<vector<vector<T>>> res;
	for (int i = 0; i < n1; i++) {
		vector<vector<T>> row;
		for (int j = 0; j < n2; j++) {
			vector<T> col;
			for (int k = 0; k < n3; k++)
				col.push_back(x0 + d1 * i + d2 * j + d3 * k);
			row.push_back(col);
		}
		res.push_back(row);
	}
	return res;
}

inline bool contains(int i, ivec3 x) { return i == x.x || i == x.y || i == x.z; }
inline bool contains(int i, ivec2 x) { return i == x.x || i == x.y; }
inline bool contains(int i, ivec4 x) { return i == x.x || i == x.y || i == x.z || i == x.w; }
inline bool contains(ivec3 x, int i) { return contains(i, x);}
inline bool contains(ivec2 x, int i) { return contains(i, x);}
inline bool contains(ivec4 x, int i) { return contains(i, x);}

inline int setMinus(ivec3 x, ivec2 y) {
	if (x.x == y.x) return x.y == y.y ? x.z : x.y;
	if (x.x == y.y) return x.z;
	return x.x;
}

template<typename T>
vector<T> setMinus(vector<T> x, T y) {
	vector<T> res;
	res.reserve(x.size()-1);
	for (auto& z : x)
		if (z != y)
			res.push_back(z);
	return res;
}

template<typename T>
vector<T> setMinus(vector<T> x, vector<T> y) {
	vector<T> res;
	for (auto& z : x)
		if (!contains<T>(y, z))
			res.push_back(z);
	return res;
}

inline int setMinus(ivec4 x, ivec3 y) {
	if (x.x == y.x) return x.y == y.y ? x.z == y.z ? x.w : x.z : x.y;
	if (x.x == y.y) return x.z == y.z ? x.w : x.z;
	if (x.x == y.z) return x.w;
	return x.x;
}

inline int setMinus(ivec2 x, int y) {
	return x.x == y ? x.y : x.x;
}

inline ivec2 setMinus(ivec3 x, int y) {
	if (x.x == y) return ivec2(x.y, x.z);
	if (x.y == y) return ivec2(x.x, x.z);
	return ivec2(x.x, x.y);
}

inline ivec3 setMinus(ivec4 x, int y) {
	if (x.x == y) return ivec3(x.y, x.z, x.w);
	if (x.y == y) return ivec3(x.x, x.z, x.w);
	if (x.z == y) return ivec3(x.x, x.y, x.w);
	return ivec3(x.x, x.y, x.z);
}

inline ivec2 setMinus(ivec4 x, ivec2 y) {
	if (x.x == y.x) return x.y == y.y ? ivec2(x.z, x.w) : ivec2(x.y, x.z);
	if (x.x == y.y) return ivec2(x.y, x.w);
	return ivec2(x.x, x.y);
}

inline string hash_ivec3(ivec3 v) {
	return to_string(v.x) + "--" + to_string(v.y) + "--" + to_string(v.z);
}

inline string replaceAll(const string &target, const string &old, const string &new_)
{
	    string res = target;
    size_t pos = 0;
    while ((pos = res.find(old, pos)) != string::npos) {
        res.replace(pos, old.length(), new_);
        pos += new_.length();
    }
    return res;
}

template<typename T>
vector<T> concat(const vector<T> &a, const vector<T> &b) {
    vector<T> res = a;
    res.insert(res.end(), b.begin(), b.end());
    return res;
}

template<typename T>
void addAll(vector<T> &target, const vector<T> &addition) {
    target.insert(target.end(), addition.begin(), addition.end());
}

inline long long timestampNow() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline int timestampNowTruncated() {
	long long t0 = 1734327390240LL;
	return (int)(timestampNow() - t0);
}


//class SmallestUniqueNumber{
//public:
//
//	static int count;
//	static int generate() { return ++count; }
//};
//int SmallestUniqueNumber::count = 0;
//
//inline int smallestUniqueNumber() {
//    return SmallestUniqueNumber::generate();
//}
//
//inline string smallestUniqueNumberStr() {
//	return to_string(SmallestUniqueNumber::generate());
//}

template<typename T>
int indexOf(T x, const vector<T> &v) {
	for (int i = 0; i < v.size(); i++)
		if (v[i] == x)
			return i;
	throw ValueError("Element not found in vector.", __FILE__, __LINE__);
}

template<typename T>
string str(T x) {
	return to_string(x);
}



template<typename V>
V lerp(V a, V b, float t) { return b * t + a * (1.f - t); }

// template<typename V>
// V clamp(V a, V b, float v) { return min(b, max(a, v)); }
template<typename V>
V lerpClamp(V a, V b, float t, V max_v = V(1), V min_v = V(0)) { return lerp(a, b, clamp(min_v, max_v, t)); }

template<typename V>
V mean(vector<V> points) {
	V m = V(0);
	for (V p: points)
		m = m + p;
	return m / (1.f * points.size());
}


template<typename V>
vector<V> linspace(V a, V b, int n, bool includeEnd = true) {
	vector<V> res;
	res.reserve(n);
	if (includeEnd)
		for (int i = 0; i < n; i++)
			res.emplace_back(lerp(a, b, i * 1.f / (n - 1.f)));
	else
		for (int i = 0; i < n; i++)
			res.emplace_back(lerp(a, b, i * 1.f / n));
	return res;
}

template<AbelianSemigroup V>
vector<vector<V> > linspace2D(V p0, V v0, V v1, int n, int m) {
	vector<vector<V> > res;
	res.reserve(n);
	for (int i = 0; i < n; i++) {
		res.push_back(vector<V>());
		res[i].reserve(m);
		for (int j = 0; j < m; j++)
			res[i].push_back(p0 + v0 * i + v1 * j);
	}
	return res;
}



template<TotallyOrderedAbelianSemigroup A>
vector<A> arange(A a, A b, A step) {
	vector<A> res;
	for (A i = a; i < b; i += step) {
		res.push_back(i);
		if (res.size() > 1000000) throw std::format_error("arange didn'v terminate in milion iterations");
	}
	return res;
}




template<typename A=float, typename container>
A sum(container arr) {
	A res = arr[0];
	for (int i = 1; i < arr.size(); i++) res = res + arr[i];
	return res;
}

template<typename A=float>
vector<A> pointwise_vector_sum(vector<A> v1, vector<A> v2) {
	vector v = v1;
	for (int i = 0; i < v1.size(); i++) v[i] = v[i] + v2[i];
	return v;
}

template<typename container, typename A>
A combine(container arr, std::function<A(A, A)> binaryOperator) {
	A res = arr[0];
	for (int i = 1; i < arr.size(); i++) res = binaryOperator(res, arr[i]);
	return res;
}

template<Semigroup A, typename container>
A mult(container arr) {return combine<container, A>(arr, [](A a, A b) { return a * b; }); }

template<typename A, int k, typename homspace>
array<A, k> arrayComprehension(const homspace &f) {
	array<A, k> res = {f(0)};
	for (int i = 1; i < k; i++) res[i] = f(i);
	return res;
}

template<typename A, typename homspace>
vector<A> vectorComprehension(const homspace &f, int n) {
	vector res = {f(0)};
	res.reserve(n);
	for (int i = 1; i < n; i++) res[i] = f(i);
	return res;
}
//
// template<typename A=float>
//     A clamp(A x, A a, A b) {
// 		return x < a ? a : x > b ? b : x;
// 	}


template<typename Dom=float, typename Cod=float>
vector<Cod> map_cr(vector<Dom> v, const HOM(const Dom&, Cod) &f) {
	vector<Cod> res = vector<Cod>();
	res.reserve(v.size());
	for (auto x : v) res.push_back(f(x));
	return res;
}

template<typename Dom=float, typename Cod=float>
vector<Cod> map(vector<Dom> v, const HOM(Dom, Cod) &f) {
	vector<Cod> res;
	res.reserve(v.size());
	for (auto x : v) res.push_back(f(x));
	return res;
}

template<typename Dom, typename Cod, int k>
array<Cod, k> map(array<Dom, k> v, const std::function<Cod(const Dom &)> &f) {
	array<Cod, k> res;
	for (int i = 0; i < k; i++) res[i] = f(v[i]);
	return res;
}

template<TotallyOrderedAbelianSemigroup Dom, typename Cod>
vector<Cod> mapArange(Dom a, Dom b, Dom step, const std::function<Cod(const Dom &)> &f) {
	return map<Dom, Cod>(arange(a, b, step), f);
}




template<typename Cod>
vector<Cod> mapRange(int a, int b, int step, const std::function<Cod(int)> &f) {
	return map<int, Cod>(range(a, b, step), f);
}

template<typename Cod>
vector<Cod> mapRange(int a, int b, const std::function<Cod(int)> &f) {
	return mapRange(a, b, 1, f);
}

template<typename Cod>
vector<Cod> mapRange(int b, const std::function<Cod(int)> &f) {
	return mapRange(0, b, 1, f);
}

template<VectorSpaceConcept<float> Dom=float, typename Cod=float>
vector<Cod> mapLinspace(Dom a, Dom b, int steps, const HOM(Dom, Cod) &f, bool includeEnd=true) {
	return map<Dom, Cod>(linspace<Dom>(a, b, steps, includeEnd), f);
}

template<VectorSpaceConcept<float> Dom=float, typename Cod=float>
vector<Cod> mapLinspace(Dom a, Dom b, int steps, const HOM(const Dom&, Cod) &f, bool includeEnd=true) {
	return map_cref(linspace<Dom>(a, b, steps, includeEnd), f);
}

template<typename Dom, typename Cod, int k>
array<Cod, k> mapByVal(array<Dom, k> v, const std::function<Cod(Dom)> &f) {
	array<Cod, k> res;
	for (int i = 0; i < k; i++) res[i] = f(v[i]);
	return res;
}

template<typename T>
class FlatteningIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    FlatteningIterator(vector<vector<T>>& nestedVec) {
        for (auto it = nestedVec.rbegin(); it != nestedVec.rend(); ++it) {
            if (!it->empty()) {
                stack.push({it->begin(), it->end()});
            }
        }
        advanceToNext();
    }

	FlatteningIterator() : FlatteningIterator(vector<vector<T>>()) {}

    reference operator*() const { return *stack.top().first; }
    pointer operator->() { return &(*stack.top().first); }

    // Prefix increment
    FlatteningIterator& operator++() {
        ++stack.top().first;
        advanceToNext();
        return *this;
    }

    // Postfix increment
    FlatteningIterator operator++(int) {
        FlatteningIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const FlatteningIterator& a, const FlatteningIterator& b) {
        return a.stack == b.stack;
    }

    friend bool operator!=(const FlatteningIterator& a, const FlatteningIterator& b) {
        return !(a == b);
    }

private:
    std::stack<std::pair<typename vector<T>::iterator, typename vector<T>::iterator>> stack;

    void advanceToNext() {
        while (!stack.empty() && stack.top().first == stack.top().second) {
            stack.pop();
            if (!stack.empty()) {
                ++stack.top().first;
            }
        }
    }
};


class MultiIndex {
	int dim;
	vector<int> indices;

public:
	explicit MultiIndex(const vector<int> &indices) : dim(indices.size()), indices(indices) {}
	explicit MultiIndex(int dim) : dim(dim), indices(dim, 0) {}
	explicit MultiIndex(int dim, int i) : dim(dim), indices(dim, 0) {indices[i] = 1; }

	int operator[](int i) const { return indices[i]; }
	int& operator[](int i) { return indices[i]; }

	MultiIndex operator+(MultiIndex v) const {
		MultiIndex res = *this;
		for (int i = 0; i < dim; i++)
			res.indices[i] += v[i];
		return res;
	}
	MultiIndex operator-(MultiIndex v) const {
		MultiIndex res = *this;
		for (int i = 0; i < dim; i++)
			res.indices[i] -= v[i];
		return res;
	}
	MultiIndex operator*(int i) const {
		MultiIndex res = *this;
		for (int j = 0; j < dim; j++)
			res.indices[j] *= i;
		return res;
	}
	void operator+=(MultiIndex v) {
		for (int i = 0; i < dim; i++)
			indices[i] += v[i];
	}
	void operator-=(MultiIndex v) {
		for (int i = 0; i < dim; i++)
			indices[i] -= v[i];
	}
	void operator*=(int i) {
		for (int j = 0; j < dim; j++)
			indices[j] *= i;
	}
	MultiIndex operator-() const {
		MultiIndex res = *this;
		for (int i = 0; i < dim; i++)
			res.indices[i] = -res.indices[i];
		return res;
	}
	void operator=(MultiIndex v) {
		for (int i = 0; i < dim; i++)
			indices[i] = v[i];
	}
	void set(int i, int j) {
		if (i < 0 || i >= dim) throw std::invalid_argument("Index out of bounds");
		indices[i] = j;
	}
	bool operator==(MultiIndex v) const {
		for (int i = 0; i < dim; i++)
			if (indices[i] != v[i])
				return false;
		return true;
	}
	bool operator!=(MultiIndex v) const {
		for (int i = 0; i < dim; i++)
			if (indices[i] != v[i])
				return true;
		return false;
	}
	bool operator<=(MultiIndex v) const {
		for (int i = 0; i < dim; i++)
			if (indices[i] > v[i])
				return false;
		return true;
	}
	bool operator>=(MultiIndex v) const {
		for (int i = 0; i < dim; i++)
			if (indices[i] < v[i])
				return false;
		return true;
	}
	int order() const {
		int res = 0;
		for (int i = 0; i < dim; i++)
			res += indices[i];
		return res;
	}
	int size() const {
		return dim;
	}
	int abs() const {
		return order();
	}
	int factorial() const {
		int res = 1;
		for (int i = 0; i < dim; i++)
			for (int j=1; j<=indices[i]; j++)
				res *= j;
		return res;
	}
	friend int binom(const MultiIndex &a, const MultiIndex &b) {
		int res = 1;
		for (int i = 0; i < a.dim; i++)
			res *= binom(a[i], b[i]);
		return res;
	}
	int multinomial() const {
		return ::factorial(abs()) / factorial();
	}
};


inline vector<string> split(const string &s, char delim) {
	vector<string> res;
	std::stringstream ss(s);
	string item;
	while (std::getline(ss, item, delim)) {
		res.push_back(item);
	}
	return res;
}

inline vector<string> split(const string &s, const string &delim) {
	vector<string> res;
	size_t start = 0;
	size_t end = s.find(delim);
	while (end != string::npos) {
		res.push_back(s.substr(start, end - start));
		start = end + delim.length();
		end = s.find(delim, start);
	}
	res.push_back(s.substr(start, end));
	return res;
}

inline float angleBetween(vec3 vec3, glm::vec3 down) {
	return  acos(glm::dot(vec3, down)/length(vec3));
}
