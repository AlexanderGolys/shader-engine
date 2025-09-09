#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec4 color;



out vec4 v_color;
out vec3 v_position;
out vec3 v_normal;
out vec2 v_uv;

uniform mat4 mvp;
uniform mat4 light1;
uniform mat4 light2;
uniform mat4 light3;
uniform vec3 camPosition;


vec3 normalise(vec3 v)
{
	return v / length(v);
}

void main()
{
	v_normal = normal;
	gl_Position = mvp * vec4(position, 1.0);
	v_position = position;
	v_color = color;
	v_uv = uv;
}
