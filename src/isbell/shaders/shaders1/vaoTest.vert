#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec4 color;



out vec4 v_color;
out vec3 v_position;
out vec3 v_normal;
out vec4 v_ambientColor;
out vec4 v_diffuseColor;
out vec4 v_specularColor;
out vec4 v_intencities;

uniform mat4 mvp;
uniform mat4 light1;
uniform mat4 light2;
uniform mat4 light3;
uniform vec3 camPosition;
uniform float t;


vec3 normalise(vec3 v)
{
	return v / length(v);
}

void main()
{
	v_normal = normalise(normal);
	gl_Position = mvp * vec4(position, 1.0);
	v_position = position;
	v_color = color;
	v_ambientColor = color;
	v_diffuseColor = color;
	v_specularColor = vec4(1.0, 1.0, 1.0, 1.0);
	v_intencities = vec4(.1, .7, .6, 20.0);
}

