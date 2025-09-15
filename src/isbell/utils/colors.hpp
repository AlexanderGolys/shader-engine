#pragma once
#include "../../external/glm-0.9.7.1/glm/glm.hpp"
#include "../../external/glm-0.9.7.1/glm/gtc/matrix_transform.hpp"
#include "../../external/glm-0.9.7.1/glm/gtx/transform.hpp"

#include <array>
#include <vector>

using std::vector, std::array;
using glm::vec4, glm::vec3, glm::ivec3;


struct COLOR_PALETTE {
	vec4 mainColor;
	vec4 second;
	vec4 third;
	vec4 accent;
	vec4 accent2;

	COLOR_PALETTE(vec4 mainColor, vec4 second, vec4 third, vec4 accent, vec4 accent2)
	{
		this->mainColor = mainColor;
		this->second = second;
		this->third = third;
		this->accent = accent;
		this->accent2 = accent2;
	}
	COLOR_PALETTE(vec3 mainColor, vec3 second, vec3 third, vec3 accent, vec3 accent2)  : COLOR_PALETTE(vec4(mainColor, 1.0f), vec4(second, 1.0f), vec4(third, 1.0f), vec4(accent, 1.0f), vec4(accent2, 1.0f) ) {}
	COLOR_PALETTE(ivec3 mainColor, ivec3 second, ivec3 third, ivec3 accent, ivec3 accent2)
	{
		this->mainColor = vec4(mainColor.x / 255.f, mainColor.y / 255.f, mainColor.z / 255.f, 1.0f);
		this->second = vec4(second.x / 255.f, second.y / 255.f, second.z / 255.f, 1.0f);
		this->third = vec4(third.x / 255.f, third.y / 255.f, third.z / 255.f, 1.0f);
		this->accent = vec4(accent.x / 255.f, accent.y / 255.f, accent.z / 255.f, 1.0f);
		this->accent2 = vec4(accent2.x / 255.f, accent2.y / 255.f, accent2.z / 255.f, 1.0f);
	}
	vector<vec4> colors()
	{
		return std::vector({ mainColor, second, third, accent, accent2 });
	}
	vec4 operator[] (int i)
	{
		return colors()[i];
	}
};

struct COLOR_PALETTE10 {
	array<vec4, 10> cls;

	explicit COLOR_PALETTE10(const array<vec4, 10> &colors) : cls(colors) {}
	COLOR_PALETTE10(COLOR_PALETTE p1, COLOR_PALETTE p2) : cls({p1[0], p1[1], p1[2], p1[3], p1[4], p2[0], p2[1], p2[2], p2[3], p2[4]}) {}
	COLOR_PALETTE10(ivec3 c1, ivec3 c2, ivec3 c3, ivec3 c4, ivec3 c5, ivec3 c6, ivec3 c7, ivec3 c8, ivec3 c9, ivec3 c10)
	{
		cls = {vec4(c1.x / 255.f, c1.y / 255.f, c1.z / 255.f, 1.0f),
			   vec4(c2.x / 255.f, c2.y / 255.f, c2.z / 255.f, 1.0f),
			   vec4(c3.x / 255.f, c3.y / 255.f, c3.z / 255.f, 1.0f),
			   vec4(c4.x / 255.f, c4.y / 255.f, c4.z / 255.f, 1.0f),
			   vec4(c5.x / 255.f, c5.y / 255.f, c5.z / 255.f, 1.0f),
			   vec4(c6.x / 255.f, c6.y / 255.f, c6.z / 255.f, 1.0f),
			   vec4(c7.x / 255.f, c7.y / 255.f, c7.z / 255.f, 1.0f),
			   vec4(c8.x / 255.f, c8.y / 255.f, c8.z / 255.f, 1.0f),
			   vec4(c9.x / 255.f, c9.y / 255.f, c9.z / 255.f, 1.0f),
			   vec4(c10.x / 255.f, c10.y / 255.f, c10.z / 255.f, 1.0f)};

	}
	vector<vec4> colors() const { return vector(cls.begin(), cls.end()); }
	vec4 operator[] (int i) const { return cls[i]; }
};

#define BLACK vec4(0, 0, 0, 1)
#define WHITE vec4(1, 1, 1, 1)
#define RED vec4(1, 0, 0, 1)
#define GREEN vec4(0, 1, 0, 1)
#define BLUE vec4(0, 0, 1, 1)
#define YELLOW vec4(1, 1, 0, 1)
#define CYAN vec4(0, 1, 1, 1)
#define MAGENTA vec4(1, 0, 1, 1)
#define ORANGE vec4(1, .5, 0, 1)
#define PINK vec4(1, .5, .5, 0)
#define LIGHT_BLUE vec4(.5, .5, 1, 1)
#define LIGHT_GREEN vec4(.5, 1, .5, 1)
#define LIGHT_RED vec4(1, .5, .5, 1)
#define LIGHT_YELLOW vec4(1, 1, .5, 1)
#define LIGHT_CYAN vec4(.5, 1, 1, 1)
#define LIGHT_MAGENTA vec4(1, .5, 1, 1)
#define DARK_PURPLE vec4(54._f/255, 31._f/255, 39._f/255, 0)
#define PURPLE vec4(82._f/255, 25._f/255, 69._f/255, 1)
#define BROWN_SUGAR vec4(169._f/255, 113._f/255, 75._f/255, 1)
#define POWDER_PINK vec4(255._f/255, 179._f/255, 198._f/255, 1)
#define DARKDARKBLUE vec4(0, 0.016, .045, 1)

#define INTENSE_DARKER_RED_PALLETTE COLOR_PALETTE(ivec3(173, 40, 49), ivec3(128, 14, 19), ivec3(100, 13, 20), ivec3(56, 4, 14), ivec3(37, 9, 2))
#define BLUE_PALLETTE COLOR_PALETTE10(ivec3(226, 239, 246), ivec3(183, 215, 234), ivec3(141, 190, 220), ivec3(100, 164, 206), ivec3(72, 147, 198), ivec3(45, 130, 189), ivec3(36, 118, 177), ivec3(25, 101, 160), ivec3(15, 85, 143), ivec3(0, 58, 112))
#define GREEN_PALLETTE COLOR_PALETTE10(ivec3(219, 234, 215), ivec3(195, 220, 188), ivec3(171, 206, 161), ivec3(148, 193, 134), ivec3(124, 179, 107), ivec3(102, 162, 83), ivec3(85, 135, 69), ivec3(68, 108, 55), ivec3(51, 81, 42), ivec3(32, 51, 26))
#define REDPINK_PALLETTE COLOR_PALETTE10(ivec3(243, 219, 234), ivec3(236, 197, 221), ivec3(204, 154, 181), ivec3(179, 116, 149), ivec3(153, 77, 116), ivec3(128, 39, 84), ivec3(102, 0, 51), ivec3(85, 6, 45), ivec3(67, 12, 39), ivec3(52, 9, 30))
#define GRAY_PALLETTE COLOR_PALETTE10(ivec3(248, 249, 250), ivec3(233, 236, 239), ivec3(222, 226, 230), ivec3(206, 212, 218), ivec3(173, 181, 189), ivec3(108, 117, 125), ivec3(73, 80, 87), ivec3(52, 58, 64), ivec3(33, 37, 41), ivec3(22, 26, 29))
