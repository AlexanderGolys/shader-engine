#pragma once
#include <cassert>

#include "../utils/logging.hpp"

template <typename T, typename... Args>
bool assertEqual_UT_(const T& A0, const Args&... args) {
	try {
		for (const auto& A : {args...})
			if(A0 != A)
				return false;
		return true;
	} catch (...) {
		return false;
	}
};


template <typename T, typename... Args>
bool assertNearlyEqual_UT_(const T& A0, const Args&... args) {
	try {
		for (const auto& A : {args...})
			if(!nearlyEqual(A0, A))
				return false;
		return true;
	} catch (...) {
		return false;
	}
};

template <typename T, typename U>
bool assertLess_UT_(const T& x, const U& y) {
	try {
		return x < y;
	} catch (...) {
		return false;
	}
};

template <typename T, typename U>
bool assertLessOrEqual_UT_(const T& x, const U& y) {
	try {
		return x <= y;
	} catch (...) {
		return false;
	}
};

template <typename T, typename U>
bool assertMore_UT_(const T& x, const U& y) {
	try {
		return x > y;
	} catch (...) {
		return false;
	}
};

template <typename T, typename U>
bool assertMoreOrEqual_UT_(const T& x, const U& y) {
	try {
		return x >= y;
	} catch (...) {
		return false;
	}
};

template <typename T, typename U>
bool assertNotEqual_UT_(const T& x, const U& y) {
	try {
		return x != y;
	} catch (...) {
		return false;
	}
};

inline bool assertTrue_UT_(bool x) {
	try {
		return x;
	} catch (...) {
		return false;
	}
};

inline bool assertFalse_UT_(bool x) {
	try {
		return ~x;
	} catch (...) {
		return false;
	}
};


// purely for better highlighting in IDE
#define assertEqual_UT(...) assertEqual_UT_(__VA_ARGS__)
#define assertNearlyEqual_UT(...) assertNearlyEqual_UT_(__VA_ARGS__)
#define assertLess_UT(...) assertLess_UT_(__VA_ARGS__)
#define assertLessOrEqual_UT(...) assertLessOrEqual_UT_(__VA_ARGS__)
#define assertMore_UT(...) assertMore_UT_(__VA_ARGS__)
#define assertMoreOrEqual_UT(...) assertMoreOrEqual_UT_(__VA_ARGS__)
#define assertNotEqual_UT(...) assertNotEqual_UT_(__VA_ARGS__)
#define assertTrue_UT(...) assertTrue_UT_(__VA_ARGS__)
#define assertFalse_UT(...) assertFalse_UT_(__VA_ARGS__)

struct UnitTestResult {
	int passed, all;
	UnitTestResult(int p, int a): passed(p), all(a) {}
	UnitTestResult(): passed(0), all(0) {}

	UnitTestResult operator+(const UnitTestResult& other) const {
		return UnitTestResult(passed + other.passed, all + other.all);
	}

	void operator+=(const UnitTestResult& other) {
		passed += other.passed;
		all += other.all;
	}
};



inline void printTestResult(const string& test_name, const UnitTestResult& result) {
	if (result.all == 0)
		return;
	if (result.passed == result.all)
		LOG_TEST_OK("Test " + test_name + " passed [" + std::to_string(result.passed) + "/" + std::to_string(result.all) + "]");
	else
		LOG_TEST_FAIL("Test " + test_name + " failed [" + std::to_string(result.passed) + "/" + std::to_string(result.all) + "]");
}
