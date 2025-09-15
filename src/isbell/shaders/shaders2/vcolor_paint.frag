#version 330 core

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;

layout(location = 0) out vec4 color;

uniform mat4 mvp;
uniform mat4 light1;
uniform mat4 light2;
uniform mat4 light3;
uniform vec3 camPosition;
uniform vec4 intencities;
uniform sampler2D texture_ambient;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform float time;



void main()
{
    color = v_color;
    color.a=1.0;
}
