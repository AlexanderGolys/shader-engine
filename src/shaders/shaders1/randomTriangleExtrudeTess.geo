#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 21) out;

in vec3 normalArr[];
in float phase[];

uniform mat4 mvp;
uniform float time;
uniform float speed;
uniform float power;

out vec3 v_normal;
out vec3 v_position;

vec3 normalise(vec3 v)
{
    return v / length(v);
}

int mod(int a, int b)
{
	return a - b * int(a/b);
}


void emmit(int i)
{
    float t = time + phase[0];
    vec3 mid = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0 + normalArr[0] * power * (sin(t * speed + phase[0])*cos(t * speed + phase[0]));
    vec3 n = normalise(cross(mid - gl_in[i].gl_Position.xyz, mid - gl_in[mod(i+1, 3)].gl_Position.xyz));

    v_position = gl_in[i].gl_Position.xyz;
    v_normal = n;
    gl_Position = mvp * vec4(v_position, 1.0);
    EmitVertex();

    v_position = gl_in[mod(i+1, 3)].gl_Position.xyz;
    v_normal = n;
    gl_Position = mvp * vec4(v_position, 1.0);
    EmitVertex();

    v_position = mid;
    v_normal = n;
    gl_Position = mvp * vec4(v_position, 1.0);
    EmitVertex();

    EndPrimitive();
}


void main()
{
    emmit(0);
    emmit(1);
    emmit(2);
    
}
