#version 330 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 6) out;

uniform float lineWidth;
uniform mat4 mvp;

out vec3 v_position;
out vec3 v_normal;


vec4 normalise(vec4 v)
{
    if (length(v) == 0.0)
        return vec4(0.0);
    return v / length(v);
}

void main()
{
    vec4 normal1 = normalise(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec4 normal2 = normalise(gl_in[3].gl_Position - gl_in[2].gl_Position);
    gl_Position = gl_in[1].gl_Position - normal1*lineWidth/2.;
    v_position = (gl_in[1].gl_Position - normal1*lineWidth/2.).xyz;
    v_normal = vec3(0., 0. , 1.);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position - normal2*lineWidth/2.;
    v_position = (gl_in[2].gl_Position - normal2*lineWidth/2.).xyz;
    v_normal = vec3(0., 0. , 1.);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    v_position = gl_in[1].gl_Position.xyz;
    v_normal = vec3(0., 0. , 1.);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    v_position = gl_in[2].gl_Position.xyz;
    v_normal = vec3(0., 0. , 1.);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + normal1*lineWidth/2.;
    v_position = (gl_in[1].gl_Position + normal1*lineWidth/2.).xyz;
    v_normal = vec3(0., 0. , 1.);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position +  normal2*lineWidth/2.;
    v_position = (gl_in[2].gl_Position +  normal2*lineWidth/2.).xyz;
    v_normal = vec3(0., 0. , 1.);
    EmitVertex();
    EndPrimitive();
}
