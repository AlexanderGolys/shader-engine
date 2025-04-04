#pragma once

#include <chrono>
#include <exception>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <random>
#include <stack>

#include "concepts.hpp"


inline std::string vecToString(vec2 v) {
	return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }

inline std::string vecToString(vec3 v) {
	return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")"; }

inline std::string vecToString(vec4 v) {
	return  "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")"; }

inline std::string vecToString(ivec2 v) {
	return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }

inline std::string vecToString(ivec3 v) {
	return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")"; }

inline std::string vecToString(ivec4 v) {
	return  "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")"; }


inline std::string plural (std::string word) {
    if (word.size() == 0)
      return word;
    bool caps = word.back() >= 'A' && word.back() <= 'Z' && word.size() > 1;
    bool es = word.back() == 's'||
              word.back() == 'x' ||
              word.back() == 'z' ||
              (word.size() > 1 && word[word.size()-2] == 'c' && word.back() == 'h') ||
              (word.size() > 1 && word[word.size()-2] == 's' && word.back() == 'h') ||
              word.back() == 'S'||
              word.back() == 'X' ||
              word.back() == 'Z' ||
              (word.size() > 1 && word[word.size()-2] == 'C' && word.back() == 'H') ||
              (word.size() > 1 && word[word.size()-2] == 'S' && word.back() == 'H');
    if (es && !caps)
      return word + "es";
    if (es)
      return word + "ES";
    if (caps)
      return word + "S";
    return word + "s";
}

class ErrorClassWrapper : public std::exception {
	std::string msg_;
public:
	explicit ErrorClassWrapper(const std::string& msg) : msg_(msg) {}
	const char* what() const noexcept override {
		return msg_.c_str();
	}
	string message() const { return msg_; }
};

class NotImplementedError : public ErrorClassWrapper {
public:
    explicit NotImplementedError(const std::string& notImplementedMethodName, const std::string& lackingType="Method")
        : ErrorClassWrapper(lackingType + " " + notImplementedMethodName + " is not implemented yet.") {}
};

class IndexOutOfBounds : public ErrorClassWrapper {
public:
	IndexOutOfBounds(int index, int size, const std::string &indexName="i")
		: ErrorClassWrapper("Index " + indexName + " is out of bounds [" + std::to_string(index) + "/" + std::to_string(size) + "].") {}
	IndexOutOfBounds(const std::string &index, const std::string &size, const std::string &indexName="i")
		: ErrorClassWrapper("Index " + indexName + " is out of bounds [" + index + "/" + size + "].") {}
	IndexOutOfBounds(const ivec2 &index, const ivec2 &size, const std::string &indexName="i")
		: ErrorClassWrapper("Index " + indexName + " is out of bounds [" + vecToString(index) + "/" + vecToString(size) + "].") {}
	IndexOutOfBounds(const ivec3 &index, const ivec3 &size, const std::string &indexName="i")
	: ErrorClassWrapper("Index " + indexName + " is out of bounds [" + vecToString(index) + "/" + vecToString(size) + "].") {}
	IndexOutOfBounds(const ivec4 &index, const ivec4 &size, const std::string &indexName="i")
	: ErrorClassWrapper("Index " + indexName + " is out of bounds [" + vecToString(index) + "/" + vecToString(size) + "].") {}
};

class NotImplementedMethodError : public NotImplementedError {
public:
  explicit NotImplementedMethodError(const std::string& methodName)
      :NotImplementedError(methodName, "Method") {}
};

class NotImplementedFunctionError : public NotImplementedError {
public:
  explicit NotImplementedFunctionError(const std::string& name)
      :NotImplementedError(name, "Function") {}
};

class NotImplementedVariantError : public NotImplementedError {
public:
  NotImplementedVariantError(const std::string& variant, const std::string& ofWhat)
      :NotImplementedError(variant + " of " + ofWhat, "Variant") {}
};

class UnknownVariantError : public ErrorClassWrapper {
public:
  UnknownVariantError(const std::string& variant, const std::string& ofWhat)
      : ErrorClassWrapper("Variant " + variant + " of  " + ofWhat + " is an unknown type in this context.") {}
  explicit UnknownVariantError(const std::string& msg) : ErrorClassWrapper(msg) {}

};

class IllegalVariantError : public ErrorClassWrapper {
public:
  IllegalVariantError(const std::string& variant, const std::string& ofWhat, const std::string& rejectingMethod)
      : ErrorClassWrapper(plural(ofWhat) + " in variant " + variant + " are considered invalid by method " + rejectingMethod + ".") {}
  explicit IllegalVariantError(const std::string& msg) : ErrorClassWrapper(msg) {}

};

class IllegalArgumentError : public ErrorClassWrapper {
public:
	explicit IllegalArgumentError(const std::string& msg) : ErrorClassWrapper(msg) {}
};

class ValueError : public ErrorClassWrapper {
public:
	explicit ValueError(const std::string& msg) : ErrorClassWrapper(msg) {}
};

class SystemError : public ErrorClassWrapper {
public:
	explicit SystemError(const std::string& msg) : ErrorClassWrapper(msg) {}
};

class FileNotFoundError : public SystemError {
public:
	explicit FileNotFoundError(const std::string& filename) : SystemError("File " + filename + " not found.") {}
	FileNotFoundError(const std::string& filename, const std::string& dir) : SystemError("File " + filename + " not found in " + dir) {}
};

template<typename X>
void add_all(vector<X> a, vector<X> b) {
	a.insert(a.end(), b.begin(), b.end());
}

template<typename X>
vector<X> concat(vector<X> a, vector<X> b) {
	auto c = a;
	c.insert(c.end(), b.begin(), b.end());
	return c;
}

class COLOR_PALETTE {
public:
    vec4 mainColor;
    vec4 second;
    vec4 third;
    vec4 accent;
    vec4 accent2;

    COLOR_PALETTE(vec4 mainColor, vec4 second, vec4 third, vec4 accent, vec4 accent2);
    COLOR_PALETTE(vec3 mainColor, vec3 second, vec3 third, vec3 accent, vec3 accent2);
    COLOR_PALETTE(ivec3 mainColor, ivec3 second, ivec3 third, ivec3 accent, ivec3 accent2);
    std::vector<vec4> colors();
    vec4 operator[] (int i);

};

class COLOR_PALETTE10 {
public:
    std::array<vec4, 10> cls;

    explicit COLOR_PALETTE10(std::array<vec4, 10> colors) : cls(colors) {}
    COLOR_PALETTE10(COLOR_PALETTE p1, COLOR_PALETTE p2) : cls({p1[0], p1[1], p1[2], p1[3], p1[4], p2[0], p2[1], p2[2], p2[3], p2[4]}) {}
    COLOR_PALETTE10(ivec3 c1, ivec3 c2, ivec3 c3, ivec3 c4, ivec3 c5, ivec3 c6, ivec3 c7, ivec3 c8, ivec3 c9, ivec3 c10);
    std::vector<vec4> colors() const { return std::vector<vec4>(cls.begin(), cls.end()); }
    vec4 operator[] (int i) const { return cls[i]; }
};


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
    void printVector(std::vector<T> v, std::string title="vector")
    {
        std::cout << title << " [" << v.size() << "]: ";
        for (int i = 0; i < v.size(); i++)
        {
            std::cout << v[i] << ", ";
        }
        std::cout << std::endl;
    }


const PolyGroupID DEFAULT_POLY_GROUP_ID = PolyGroupID(0);
constexpr RP1 inf = std::nullopt;
constexpr RP1 unbounded = std::nullopt;
const vec3 e1 = vec3(1, 0, 0);
const vec3 e2 = vec3(0, 1, 0);
const vec3 e3 = vec3(0, 0, 1);
const vec3 ORIGIN = vec3(0, 0, 0);
const vec2 PLANE_ORIGIN = vec2(0, 0);
const PolyGroupID DFLT_CURV = PolyGroupID(420);
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

	std::vector<int> toVec() const { return {a.x, a.y, a.z, b.x, b.y, b.z}; }
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
	std::vector<int> perm;
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
std::vector<T> flattened2DVector(std::vector<std::vector<T>> v)
{
	std::vector<T> res;
	for (auto& row : v)
		res.insert(res.end(), row.begin(), row.end());
	return res;
}


inline int flattened2DVectorIndex(int i, int j, ivec2 size) {
	if (i < 0) return flattened2DVectorIndex(size.x + i, j, size);
	if (j < 0) return flattened2DVectorIndex(i, size.y + j, size);
	if (i >= size.x) throw IndexOutOfBounds(ivec2(i, j), size, "i");
	if (j >= size.y) throw IndexOutOfBounds(ivec2(i, j), size, "j");
	return i * size.y + j;
}


template<typename T>
T flattened2DVectorSample(std::vector<T> flattened, int i, int j, ivec2 size) {
	return flattened[flattened2DVectorIndex(i, j, size)];
}



template<typename T>
std::vector<T> flattened3DVector(std::vector<std::vector<std::vector<T>>> v)
{
	std::vector<T> res;
	for (auto& row : v)
		for (auto& col : row)
			res.insert(res.end(), col.begin(), col.end());
	return res;
}


inline int flattened3DVectorIndex(int i, int j, int k,  ivec3 size) {
	if (i < 0) return flattened3DVectorIndex(size.x + i, j, k, size);
	if (j < 0) return flattened3DVectorIndex(i, size.y + j, k, size);
	if (k < 0) return flattened3DVectorIndex(i, j, size.z + k, size);
	if (i >= size.x) throw IndexOutOfBounds(ivec3(i, j, k), size, "i");
	if (j >= size.y) throw IndexOutOfBounds(ivec3(i, j, k), size, "j");
	if (k >= size.z) throw IndexOutOfBounds(ivec3(i, j, k), size, "k");
	return i * size.y * size.z + j * size.z + k;
}


template<typename T>
T flattened3DVectorSample(std::vector<T> flattened, int i, int j, int k, ivec3 size) {
	return flattened[flattened3DVectorIndex(i, j, k, size)];
}

inline int flattened4DVectorIndex(int i, int j, int k, int m,  ivec4 size) {
	if (i < 0) return flattened4DVectorIndex(size.x + i, j, k,m, size);
	if (j < 0) return flattened4DVectorIndex(i, size.y + j, k,m, size);
	if (k < 0) return flattened4DVectorIndex(i, j, size.z + k,m, size);
	if (m < 0) return flattened4DVectorIndex(i, j, k, size.w + m, size);
	if (i >= size.x) throw IndexOutOfBounds(ivec4(i, j, k, m), size, "i");
	if (j >= size.y) throw IndexOutOfBounds(ivec4(i, j, k, m), size, "j");
	if (k >= size.z) throw IndexOutOfBounds(ivec4(i, j, k, m), size, "k");
	if (m >= size.w) throw IndexOutOfBounds(ivec4(i, j, k, m), size, "m");
	return i * size.y * size.z * size.w + j * size.z * size.w + k * size.w + m;
}




template<typename T>
T flattened4DVectorSample(std::vector<T> flattened, int i, int j, int k, int m, ivec4 size) {
	return flattened[flattened4DVectorIndex(i, j, k, m, size)];
}

template<typename T>
bool contains(const std::vector<T> &v, T x) {
	return std::find(v.begin(), v.end(), x) != v.end();
}

template<typename T>
bool containsAll(const std::vector<T> &big, const std::vector<T> &subset) {
	for (auto& x : subset)
		if (!contains(big, x))
			return false;
	return true;
}

template<typename T, typename U>
std::vector<T> keys(const std::map<T, U> &m) {
	std::vector<T> res;
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
std::vector<U> valuesVec(const std::map<T, U> &m) {
	std::vector<U> res;
	for (auto& [k, v] : m)
		res.push_back(v);
	return res;
}

template<typename T>
std::vector<T> setToVector(const std::set<T> &s) {
	std::vector<T> res;
	for (auto& x : s)
		res.push_back(x);
	return res;
}

template<typename T, int n>
std::array<T, n> vectorToArray(const std::vector<T> &s) {
	std::array<T, n> res;
	for (int i = 0; i < n; i++)
		res[i] = s[i];
	return res;
}

template<typename T, int n, int m=n>
std::array<std::array<T, n>, m> vecVecToArray(const std::vector<T> &s) {
	std::array<std::array<T, n>, m> res;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			res[i][j] = s[i * n + j];
	return res;
}

template<typename T, int n, int m=n>
std::vector<std::vector<T>> arrayToVecVec(const std::array<std::array<T, n>, m> &s) {
	std::vector<std::vector<T>> res;
	res.reserve(m);
	for (int i = 0; i < m; i++)
		res.emplace_back(s[i].begin(), s[i].end());
	return res;
}

template<typename T, int n>
std::array<T, n> setToArray(const std::set<T> &s) {
	return vectorToArray<T, n>(setToVector(s));
}

inline float dot(float x, float y) { return x * y; }

template<typename  V>
float norm2(V v) {
	return dot(v, v);
}

inline float norm2(mat2 m) { return m[0][0]*m[0][0] + m[0][1]*m[0][1] + m[1][0]*m[1][0] + m[1][1]*m[1][1]; }

template <typename  T>
float norm(T v) { return sqrt(norm2(v)); }

inline std::array<std::array<float, 2>, 2> mat2ToArray(mat2 m) {
	return vectorToArray<std::array<float, 2>, 2>({{m[0][0], m[0][1]}, {m[1][0], m[1][1]} }); }

inline std::array<std::array<float, 3>, 3> mat3ToArray(mat3 m) {
	return vectorToArray<std::array<float, 3>, 3>({{m[0][0], m[0][1], m[0][2]}, {m[1][0], m[1][1], m[1][2]}, {m[2][0], m[2][1], m[2][2]} }); }

//inline std::array<std::array<float, 4>, 4> mat4ToArray(mat4 m) {
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
std::vector<T> setMinus(std::vector<T> x, T y) {
	std::vector<T> res;
	res.reserve(x.size()-1);
	for (auto& z : x)
		if (z != y)
			res.push_back(z);
	return res;
}

template<typename T>
std::vector<T> setMinus(std::vector<T> x, std::vector<T> y) {
	std::vector<T> res;
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

inline std::string hash_ivec3(ivec3 v) {
	return std::to_string(v.x) + "--" + std::to_string(v.y) + "--" + std::to_string(v.z);
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
std::vector<T> concat(const std::vector<T> &a, const std::vector<T> &b) {
    std::vector<T> res = a;
    res.insert(res.end(), b.begin(), b.end());
    return res;
}

template<typename T>
void addAll(std::vector<T> &target, const std::vector<T> &addition) {
    target.insert(target.end(), addition.begin(), addition.end());
}

inline long long timestampNow() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline int timestampNowTruncated() {
	long long t0 = 1734327390240LL;
	return (int)(timestampNow() - t0);
}


inline int randomInt() {
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	auto generator = std::default_random_engine(seed);
	auto distribution = std::uniform_int_distribution<int>(0, 1215752192);
	return distribution(generator);
}

inline long randomLong() {
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	auto generator = std::default_random_engine(seed);
	auto distribution = std::uniform_int_distribution<long>(0, 1215752192);
	return distribution(generator);
}

inline float randomFloat(float a=0, float b=1) {
	auto seed = std::chrono::system_clock::now().time_since_epoch().count()*randomInt();
	auto generator = std::default_random_engine(seed);
	auto distribution = std::uniform_real_distribution<float>(a, b);
	return distribution(generator);
}

inline std::string randomStringNumeric() {
	return std::to_string(randomLong());
}

inline char randomLetter() {
    return (char)randomInt()%('Z' - 'a') + 'a';
}
inline char randomLetterSmall() {
    return (char)randomInt()%('z' - 'a') + 'a';
}
inline char randomLetterBig() {
    return (char)randomInt()%('Z' - 'A') + 'A';
}
inline char randomNumericChar() {
    return (char)randomInt()%('9' - '0') + '0';
}


inline std::string randomStringLetters(int size=10) {
	std::string res;
    for (int i = 0; i < size; i++)
        res += (char)randomInt()%('Z' - 'a') + 'a';
    return res;
}

inline std::string randomStringAllValid(int size=10) {
	std::string res;
	vector<char> validChars = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                               'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                               'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
                               'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                               'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                               'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', '!', '@', '#', '$', '%', '^', '&', '*', '(',
                               ')', '-', '_', '+', '=', '{', '}', '[', ']', '|', };
	for (int i = 0; i < size; i++)
		res += (char)randomInt()%('Z' - 'a') + 'a';
	return res;
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
//	return std::to_string(SmallestUniqueNumber::generate());
//}

template<typename T>
int indexOf(T x, const std::vector<T> &v) {
	for (int i = 0; i < v.size(); i++)
		if (v[i] == x)
			return i;
	throw ValueError("Element not found in vector.");
}

template<typename T>
std::string str(T x) {
	return std::to_string(x);
}


template<typename Y>
HOM(vec2, Y) wrap(const BIHOM(float, float, Y) &f_) {
	return [f=f_](vec2 v) {
		return f(v.x, v.y); };
}

template<typename Y>
BIHOM(float, float, Y) unwrap(const HOM(vec2, Y) &f_) { return [f=f_](float x, float y) {
	return f(vec2(x, y)); };
}

template<typename Y>
HOM(vec3, Y) wrap(const TRIHOM(float, float, float, Y) &f_) { return [f=f_](vec3 v) {
	return f(v.x, v.y, v.z); }; }

template<typename Y>
TRIHOM(float, float, float, Y) unwrap(const HOM(vec3, Y) &f_) { return [f=f_](float x, float y, float z) {
	return f(vec3(x, y, z)); }; }

template<typename C1, typename C2=C1, typename D>
BIHOM(C1, C2, D) uncurry(HOM(C1, HOM(C2, D)) f) {
	return [f=f](C1 x, C2 y) {
		return f(x)(y); }; }

template<typename C1, typename C2=C1, typename D>
BIHOM(C1, C2, D) uncurry_perm(const HOM(C2, HOM(C1, D)) &f) {
	return [F=f](C1 x, C2 y) {
		return F(y)(x); }; }

template<typename C1, typename C2=C1, typename D>
HOM(C1, HOM(C2, D)) uncurry(const BIHOM(C1, C2, D) &f) {
	return [F=f](C1 x) { return [F, x](C2 y) { return F(x, y); }; }; }

template<typename C1, typename C2=C1, typename D>
HOM(C2, HOM(C1, D)) uncurry_perm(const BIHOM(C1, C2, D) &f) {
	return [F=f](C2 y) { return [F, y](C1 x) { return F(x, y); }; }; }

template<typename V>
V lerp(V a, V b, float t) { return b * t + a * (1.f - t); }

// template<typename V>
// V clamp(V a, V b, float t) { return min(b, max(a, t)); }
template<typename V>
V lerpClamp(V a, V b, float t, V max_v = V(1), V min_v = V(0)) { return lerp(a, b, clamp(min_v, max_v, t)); }

template<typename V>
V mean(std::vector<V> points) {
	V m = V(0);
	for (V p: points)
		m = m + p;
	return m / (1.f * points.size());
}


template<VectorSpaceConcept<float> V>
vector<V> linspace(V a, V b, int n, bool includeEnd = true) {
	vector<V> res;
	res.reserve(n);
	if (includeEnd)
		for (int i = 0; i < n; i++)
			res.push_back(lerp(a, b, i * 1.f / (n - 1.f)));
	else
		for (int i = 0; i < n; i++)
			res.push_back(lerp(a, b, i * 1.f / n));
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
		if (res.size() > 1000000) throw std::format_error("arange didn't terminate in milion iterations");
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
std::array<A, k> arrayComprehension(const homspace &f) {
	std::array<A, k> res = {f(0)};
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
std::array<Cod, k> map(std::array<Dom, k> v, const std::function<Cod(const Dom &)> &f) {
	std::array<Cod, k> res;
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
std::array<Cod, k> mapByVal(std::array<Dom, k> v, const std::function<Cod(Dom)> &f) {
	std::array<Cod, k> res;
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

    FlatteningIterator(std::vector<std::vector<T>>& nestedVec) {
        for (auto it = nestedVec.rbegin(); it != nestedVec.rend(); ++it) {
            if (!it->empty()) {
                stack.push({it->begin(), it->end()});
            }
        }
        advanceToNext();
    }

	FlatteningIterator() : FlatteningIterator(std::vector<std::vector<T>>()) {}

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
    std::stack<std::pair<typename std::vector<T>::iterator, typename std::vector<T>::iterator>> stack;

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
