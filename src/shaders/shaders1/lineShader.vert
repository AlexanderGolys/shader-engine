#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;


out vec4 v_color;
out vec3 v_position;
out vec3 v_normal;




void main()
{
	gl_Position = vec4(position, 0.);
	v_position = position;
	v_normal = normal;
	v_color = color;
}
