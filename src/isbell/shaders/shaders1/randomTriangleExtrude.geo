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
    v_normal = n;
    float t = time + phase[i];
    v_position = gl_in[i].gl_Position.xyz + n * power * (sin(t * speed + phase[i])*sin(t * speed + phase[i]));
    gl_Position = mvp * vec4(v_position, 1.0);

    EmitVertex();
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

    // emmitSide(0);
    // EndPrimitive();
    // emmitSide(1);
    // EndPrimitive();
    // emmitSide(2);
    // EndPrimitive();


    shiftVertex(0, normalArr[0]);
    shiftVertex(1, normalArr[1]);
    shiftVertex(2, normalArr[2]);
    EndPrimitive();
}
