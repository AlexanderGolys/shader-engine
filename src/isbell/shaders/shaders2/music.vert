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
uniform float time;


vec3 normalise(vec3 v)
{
	return v / length(v);
}

float C = 10.0;

void main()
{
	v_normal = normalise(normal);
	v_position = position;
//	v_position.y = (v_position.y)*1;
		v_position.y = v_position.y - time + 15;

//    if (v_position.y > 0.0) {
//        v_position.z *= exp(-v_position.y*C);
//    }
	v_uv = uv;
	gl_Position = (mvp * vec4(v_position, 1.0));
	v_color = color;
}
