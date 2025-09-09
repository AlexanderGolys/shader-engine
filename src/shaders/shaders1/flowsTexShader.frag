#version 330 core

in vec2 vectorField;
in vec2 v_uv;

layout(location = 0) out vec4 color;

uniform vec4 bg_color;
uniform float flowPower;
uniform sampler2D tex;


vec4 lerp(vec4 a, vec4 b, float t)
{
	return a * (1. - t) + b * t;
}
void main()
{
	vec4 col = texture(tex, v_uv);
	float red = color.r;
	vec4 inColor = texture(tex, v_uv - vectorField * flowPower);
	color = lerp(col, bg_color, red);
	color = lerp(color, inColor, inColor.r);
}
