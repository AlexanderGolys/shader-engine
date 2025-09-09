#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv; // (t, phi)
layout (location = 3) in vec4 color; // (binormal, r)
layout (location = 4) in vec4 extra0;


out vec4 v_color;
out vec3 v_position;
out vec3 v_normal;
out vec2 v_uv;
out vec4 v_extra;


uniform mat4 mvp;
uniform mat4 light1;
uniform mat4 light2;
uniform mat4 light3;
uniform vec3 camPosition;
uniform float time;



vec3 normalise(vec3 v)
{
	return v / length(v);
}

void main()
{
	vec3 binormal = normalise(color.xyz);
	vec3 n = normalise(normal);
	float r = color.w;
	float t0 = length(normal)-1;
	v_position = position + sin(uv.y)*n*r + cos(uv.y)*binormal*r + vec3(0, time-extra0.x, 0)*extra0.y/10.0;
	v_normal = sin(uv.y)*n + cos(uv.y)*binormal;
	gl_Position = (mvp * vec4(v_position, 1.0));
	v_color = color;
	v_uv = uv;
	v_extra = vec4(time-extra0.x, extra0.y, 0, 0);
}
