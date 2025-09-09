#pragma once
#include "func.hpp"

using std::formatter, std::format, std::to_string;

template<>
struct formatter<Complex> : formatter<string> {

	template<typename FormatContext>
	auto format(const Complex &c, FormatContext &ctx) const {
		if (nearlyEqual(c.imag(), 0.f))
			return formatter<string>::format("{}", format(c.real(), ctx), ctx);
		if (nearlyEqual(c.real(), 0.f))
			return formatter<string>::format("{}i", format(c.imag(), ctx), ctx);
		if (c.imag() < 0)
			return formatter<string>::format("{} - {}i", format(c.real(), ctx), format(-c.imag(), ctx), ctx);
		return formatter<string>::format("{} + {}i", format(c.real(), ctx), format(c.imag(), ctx), ctx);+++++++++++
	}
};


template<>
struct formatter<vec2> : formatter<string> {
	template<typename FormatContext>
	auto format(const vec2 &c, FormatContext &ctx) const {
		return formatter<string>::format("({}, {})", format(c.x, ctx), format(c.y, ctx), ctx);
	}
};

template<>
struct formatter<vec3> : formatter<string> {
	template<typename FormatContext>
	auto format(const vec3 &c, FormatContext &ctx) const {
		return formatter<string>::format("({}, {}, {})", format(c.x, ctx), format(c.y, ctx), format(c.z, ctx), ctx);
	}
};

template<>
struct formatter<vec4> : formatter<string> {
	template<typename FormatContext>
	auto format(const vec4 &c, FormatContext &ctx) const {
		return formatter<string>::format("({}, {}, {}, {})", format(c.x, ctx), format(c.y, ctx), format(c.z, ctx), format(c.w, ctx), ctx);
	}
};

template<>
struct formatter<vec5> : formatter<string> {
	template<typename FormatContext>
	auto format(const vec5 &c, FormatContext &ctx) const {
		return formatter<string>::format("({}, {}, {}, {}, {})", format(c.x, ctx), format(c.y, ctx), format(c.z, ctx), format(c.w, ctx), format(c.v, ctx), ctx);
	}
};
