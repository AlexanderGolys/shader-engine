#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;


out vec4 v_color;
out vec3 v_position;
out vec3 v_normal;
out vec2 v_complexPlaneCoords;

uniform mat4 mvp;
uniform mat4 material;
uniform mat4 light1;
uniform mat4 light2;
uniform mat4 light3;
uniform vec3 camPosition;
uniform float gobluego;
uniform vec2 currentPt;
uniform vec3 currentPtEmbedded;
uniform vec4 secondaryColor;
uniform float radiusBulb;
uniform float extrudeNearHotspotSize;

vec3 normalise(vec3 v)
{
	return v / length(v);
}

void main()
{
    vec3 v = normalise(position - currentPtEmbedded );
	v_complexPlaneCoords = uv;
	float p = smoothstep(0., 1., 1. - length(v_complexPlaneCoords - currentPt) / radiusBulb);
	v_normal = normalise(normal + (camPosition-position)*(color.g/10 - color.r/50) + color.g/50.*gobluego);
	gl_Position = mvp * (vec4(position + vec3(0, color.g/80., color.r/40. - color.g/80.)*(1.-gobluego), 1.0) + vec4(v*extrudeNearHotspotSize*p, 0));
	v_position = position + vec3(0, color.g/80., color.r/40. - color.g/80.)*(1.-gobluego) + v*extrudeNearHotspotSize*p;
	v_color = vec4(color.r, color.g/2., color.b/4. , 1.0);
}

