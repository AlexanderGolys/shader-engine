#version 330 core

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;
in vec4 v_extra0;

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
    return m[0].xyz;
}

vec4 lightFactor(mat4 light, vec3 position) {
    int mode = lightMode(light);
    vec3 lightPos = lightPosition(light);
    float distance = length(lightPos - position);
    vec4 color = light[1];
    if (mode == 1) {
        float power = (light*vec4(1., 0., 0., 0.)).w;
        return color * power / distance / distance;
    }

    float constant = (light[2]).x;
    float linear = (light[2]).y;
    float quadratic = (light[2]).z;
    return color / (constant + linear * distance + quadratic * distance * distance);
}


vec4 colorFromPointlight(mat4 light, vec3 camPos, vec3 pos, vec3 n, vec2 uv)
{
    vec3 lightPos = lightPosition(light);
	vec3 light_direction = normalize(lightPos - pos);
    float cosTheta = abs(dot(n, light_direction));
//    float cosTheta = max(dot(n, light_direction), 0.);

	vec3 reflectDirection = normalize(reflect(-light_direction, n));
	vec3 viewDirection = normalize(camPos - pos);
//	float cosAlpha = max(dot(viewDirection, reflectDirection), 0.);
    float cosAlpha = abs(dot(viewDirection, reflectDirection));

    return texture(texture_ambient,  uv) * intencities.x +
   texture(texture_diffuse,  uv) * intencities.y * cosTheta * lightFactor(light, pos) +
    texture(texture_specular,  uv) * intencities.z  * pow(cosAlpha, intencities.w) * lightFactor(light, pos);
}



void main()
{

	vec3 normal = normalize(v_normal);
	float t = v_uv.x;
	    if (t > time-1.2) {
        discard;
    }
	float width = v_uv.y;
	float t0 = v_extra0.x;
	vec2 pos0 = v_extra0.yz;
	float len  = v_extra0.w;
	float speed = v_color.a;
	vec4 col = vec4(v_color.rgb, 1.);

	color = mix(col*.6, col, smoothstep(0., .1, speed));
	color = saturate(col/(.8 + .8*pow(time-t, 4)));
    color.a = 1;
}
