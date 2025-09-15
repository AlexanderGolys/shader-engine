#pragma once
#include <format>
#include "func.hpp"

using std::formatter, std::format, std::to_string;

namespace std
{
	template<>
	struct formatter<Complex>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const Complex& c, FormatContext& ctx) const
		{
			if (nearlyEqual(c.imag(), 0.f))
				return format_to(ctx.out(), "{}", c.real());
			if (nearlyEqual(c.real(), 0.f))
				return format_to(ctx.out(), "{}i", c.imag());
			if (c.imag() < 0)
				return format_to(ctx.out(), "{} - {}i", c.real(), -c.imag());
			return format_to(ctx.out(), "{} + {}i", c.real(), c.imag());
		}
	};

	template<>
	struct formatter<glm::vec3>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const glm::vec3& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
		}
	};

	template<>
	struct formatter<vec2>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const vec2& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "({}, {})", v.x, v.y);
		}
	};

	template<>
	struct formatter<vec4>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const vec4& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
		}
	};

	template<>
	struct formatter<vec5>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const vec5& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "({}, {}, {}, {}, {})", v.x, v.y, v.z, v.w, v.v);
		}
	};

	template<>
	struct formatter<mat2>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const mat2& m, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "| {} {} |\n| {} {} |", m[0][0], m[0][1], m[1][0], m[1][1]);
		}
	};

	template<>
	struct formatter<glm::mat3>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const glm::mat3& m, FormatContext& ctx) const
		{
			return format_to(
				ctx.out(),
				"mat3([{}, {}, {}], [{}, {}, {}], [{}, {}, {}])",
				m[0][0], m[0][1], m[0][2],
				m[1][0], m[1][1], m[1][2],
				m[2][0], m[2][1], m[2][2]
			);
		}
	};

	template<>
	struct formatter<mat4>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const mat4& m, FormatContext& ctx) const
		{
			return format_to(
				ctx.out(),
				"| {} {} {} {} |\n| {} {} {} {} |\n| {} {} {} {} |\n| {} {} {} {} |",
				m[0][0], m[0][1], m[0][2], m[0][3],
				m[1][0], m[1][1], m[1][2], m[1][3],
				m[2][0], m[2][1], m[2][2], m[2][3],
				m[3][0], m[3][1], m[3][2], m[3][3]
			);
		}
	};
}
