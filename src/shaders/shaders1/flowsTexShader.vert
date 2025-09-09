#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;
layout (location = 4) in vec4 ambientColor;
layout (location = 5) in vec4 diffuseColor;
layout (location = 6) in vec4 specularColor;
layout (location = 7) in vec4 intencities;

out vec2 vectorField;
out vec2 v_uv;
out vec4 v_ambientColor;
out vec4 v_diffuseColor;
out vec4 v_specularColor;
out vec4 v_intencities;



void main()
{
	gl_Position = vec4(position, 0);
	vectorField = color.xy;
	v_uv = uv;
	v_ambientColor = ambientColor;
	v_diffuseColor = diffuseColor;
	v_specularColor = specularColor;
	v_intencities = intencities;
}
