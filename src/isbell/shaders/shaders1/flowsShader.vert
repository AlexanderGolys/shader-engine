#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;


out vec3 v_position;
out vec3 v_normal;
out vec2 v_uv;

uniform mat4 mvp;
uniform mat4 material;
uniform vec3 camPosition;
uniform sampler2D tex;


void main()
{
	gl_Position = mvp * vec4(position, 1.0);
	v_position = position;
	v_normal = normal;
	v_uv = uv;
}
