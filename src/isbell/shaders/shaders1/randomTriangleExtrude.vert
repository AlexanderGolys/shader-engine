#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;


out vec3 normalArr;
out float phase;

void main()
{
	gl_Position = vec4(position, 1.0);
	normalArr = normal;
	phase = 3.*(color.x - 0.5);
}
