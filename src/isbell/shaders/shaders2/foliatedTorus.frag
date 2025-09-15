#version 330 core

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;
in vec4 v_extra0;
in vec4 v_extra1;

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


vec4 saturate(vec4 color) {
    return clamp(color, 0., 1.);
}

float saturate(float x) {
    return clamp(x, 0., 1.);
}

int lightMode(mat4 m) {
    return m[3][0] < 1.1 ? 1 : 2;
}

vec3 lightPosition(mat4 m) {
    return (m[0]).xyz;
}
vec3 lightIntencities(mat4 m) {
    return (m[2]).xyz;
}

vec4 lightFactor(mat4 light, vec3 position) {
    vec3 lightPos = lightPosition(light);
    float distance = length(lightPos - position);
    vec4 color = light[1];


    float constant = (light[2]).x;
    float linear = (light[2]).y;
    float quadratic = (light[2]).z;
    return color / (1 + linear * distance + quadratic * distance * distance);
}


vec4 colorFromPointlight(mat4 light, vec3 camPos, vec3 pos, vec3 n, vec2 uv, vec4 col)
{
    vec3 lightPos = lightPosition(light);
	vec3 light_direction = normalize(lightPos - pos);
    float cosTheta = abs(dot(n, light_direction));
//    float cosTheta = max(dot(n, -light_direction), 0.);

	vec3 reflectDirection = normalize(reflect(-light_direction, n));
	vec3 viewDirection = normalize(camPos - pos);
//	float cosAlpha = max(dot(viewDirection, reflectDirection), 0.);
    float cosAlpha = abs(dot(viewDirection, reflectDirection));
//
    return col * intencities.x +
    col * intencities.y * cosTheta * lightFactor(light, pos) +
    col * intencities.z  * pow(cosAlpha, intencities.w) * lightFactor(light, pos);
}


float PI = 3.14159265358979323846;

void main()
{
    float t0= v_uv.x;
    float t1 = v_color.x;
    if (t0+t1*1.5 > -2+2*time) discard;
	vec3 normal = v_normal/length(v_normal);
    vec4 c = v_extra0;
	color = saturate(colorFromPointlight(light1, camPosition, v_position, normal, v_uv, c)) +
            saturate(colorFromPointlight(light2, camPosition, v_position, normal, v_uv, c)) +
            saturate(colorFromPointlight(light3, camPosition, v_position, normal, v_uv, c));
//    color = saturate(color/3);
    color.a = 1.0;

}
