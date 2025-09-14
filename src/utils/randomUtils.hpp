#pragma once
#include "concepts.hpp"
#include <random>



class RealDistribution {
public:
	virtual ~RealDistribution() = default;
	virtual float operator()() { return 0; }
};

class UniformRealDistribution : public RealDistribution {
	float a, b;
	std::mt19937 gen;
	std::uniform_real_distribution<float> dist;

public:
	UniformRealDistribution(float a, float b) : a(a), b(b), gen(std::random_device{}()), dist(a, b) {}

	float operator()() override {
		return dist(gen);
	}
};


class R2Distribution {
public:
	virtual ~R2Distribution() = default;
	virtual vec2 operator()() = 0;
};



class ProductDistribution : public R2Distribution {
	RealDistribution p_x, p_y;
public:
	ProductDistribution(const RealDistribution &p_x, const RealDistribution &p_y) : p_x(p_x), p_y(p_y) {}
	vec2 operator()() override {
		return vec2(p_x(), p_y());
	}

	vector<vec2> operator()(int n) {
		vector<vec2> res;
		for (int i = 0; i < n; i++)
			res.push_back((*this)());
		return res;
	}
};


inline int randomInt() {
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	auto generator = std::default_random_engine(seed);
	auto distribution = std::uniform_int_distribution(0, 1215752192);
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
inline bool randomBool() {
	return randomInt()%2 == 0;
}

inline char randomLetterSmall() {
    return (char)(randomInt() % 26 + 'a');
}
inline char randomLetterBig() {
    return (char)(randomInt() % 26 + 'A') ;
}
inline char randomNumericChar() {
    return (char)randomInt()%('9' - '0') + '0';
}
inline char randomLetter() {
	if (randomBool())
		return randomLetterBig();
	return randomLetterSmall();
}


inline std::string randomStringLetters(int size=10) {
	std::string res = "";
    for (int i = 0; i < size; i++)
        res += randomLetter();
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


inline vec2 random_vec2(vec2 a, vec2 b)
{
	return vec2(randomFloat(a.x, b.x), randomFloat(a.y, b.y));
}


inline vec3 random_vec3(vec3 a, vec3 b)
{
	return vec3(randomFloat(a.x, b.x), randomFloat(a.y, b.y), randomFloat(a.z, b.z));
}

inline vec4 random_vec4(vec4 a, vec4 b)
{
	return vec4(randomFloat(a.x, b.x), randomFloat(a.y, b.y), randomFloat(a.z, b.z), randomFloat(a.w, b.w));
}

inline vec2 random_sample_disk(float r=1) {
	vec2 x = random_vec2(vec2(-1, -1), vec2(1, 1));
	if (dot(x, x) > 1)
		return random_sample_disk(r);
	return x * r;
}

inline vec3 random_sample_ball(float r=1) {
	vec3 x = random_vec3(vec3(-1, -1, -1), vec3(1, 1, 1));
	if (dot(x, x) > 1)
		return random_sample_ball(r);
	return x * r;
}

inline vec2 random_sample_unit_circle() {
	vec2 x = random_sample_disk();
	return normalize(x);
}

inline vec3 random_sample_unit_sphere() {
	vec3 x = random_sample_ball();
	return normalize(x);
}
