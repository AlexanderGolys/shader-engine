#version 330 core

layout (location = 0) in vec3 position;

out vec3 v_position;


uniform mat4 mvp;


void main()
{
	gl_Position = vec4(position, 1.0);
	vec4 hom_pos = inverse(mvp) * gl_Position;
	v_position = hom_pos.xyz / hom_pos.w;
}
