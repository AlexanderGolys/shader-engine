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


void shiftVertex(int i, vec3 n)
{
    
}

void dontShiftVertex(int i, vec3 n)
{
    v_normal = n;
    v_position = gl_in[i].gl_Position.xyz;
    gl_Position = mvp * vec4(v_position, 1.0);

    EmitVertex();
}

void emmitSide(int i)
{
    vec3 n = normalise(cross(normalArr[i], gl_in[i].gl_Position.xyz - gl_in[mod(i+1, 3)].gl_Position.xyz));
    if (dot(n, gl_in[mod(i+2, 3)].gl_Position.xyz - gl_in[mod(i+1, 3)].gl_Position.xyz/2. - gl_in[i].gl_Position.xyz/2. ) > 0.0)
	{
		n = -n;
	}
	dontShiftVertex(i, n);
    dontShiftVertex(mod(i+1, 3), n);
    shiftVertex(i, n);
            EndPrimitive();

    dontShiftVertex(mod(i+1, 3), n);
    shiftVertex(mod(i+1, 3), n);
    shiftVertex(i, n);
}


void main()
{

    float t0 = time + phase[0];
    float t1 = time + phase[1];
    float t2 = time + phase[2];
    vec3 v_position0 = gl_in[0].gl_Position.xyz + normalArr[0] * power * (sin(t0 * speed + phase[0])*sin(t0 * speed + phase[0]));
    vec3 v_position1 = gl_in[1].gl_Position.xyz + normalArr[1] * power * (sin(t1 * speed + phase[1])*sin(t1 * speed + phase[1]));
    vec3 v_position2 = gl_in[2].gl_Position.xyz + normalArr[2] * power * (sin(t2 * speed + phase[2])*sin(t2 * speed + phase[2]));
    vec3 n = normalise(cross(v_position0 - v_position1, v_position0 - v_position2));

    v_position = v_position0;
    v_normal = n;
    gl_Position = mvp * vec4(v_position, 1.0);
    EmitVertex();

    v_position = v_position1;
    v_normal = n;
    gl_Position = mvp * vec4(v_position, 1.0);
    EmitVertex();

    v_position = v_position2;
    v_normal = n;
    gl_Position = mvp * vec4(v_position, 1.0);
    EmitVertex();

    EndPrimitive();
}
