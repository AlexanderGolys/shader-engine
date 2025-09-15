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


vec4 saturate(vec4 color) {
    return clamp(color, 0., 1.);
}

vec3 saturate(vec3 color) {
    return clamp(color, 0., 1.);
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


vec4 colorFromPointlight(mat4 light, vec3 camPos, vec3 pos, vec3 n, vec2 uv, float gamma)
{
    vec3 lightPos = lightPosition(light);
	vec3 light_direction = normalize(lightPos - pos);
//    float cosTheta = abs(dot(n, light_direction));
    float cosTheta = max(dot(n, light_direction), 0.);

	vec3 reflectDirection = normalize(reflect(-light_direction, n));
	vec3 viewDirection = normalize(camPos - pos);
	float cosAlpha = max(dot(viewDirection, reflectDirection), 0.);
//    float cosAlpha = abs(dot(viewDirection, reflectDirection));

    return pow(texture(texture_ambient,  uv), vec4(gamma)) * intencities.x +
   pow(texture(texture_diffuse,  uv), vec4(gamma)) * intencities.y * cosTheta * lightFactor(light, pos) +
    pow(texture(texture_specular,  uv), vec4(gamma)) * intencities.z  * pow(cosAlpha, intencities.w) * lightFactor(light, pos);
}



void main()
{
    float t = time/10.;
//	if ( v_uv.x > t-1)
//   {
//      discard;
//   }
	vec3 normal = normalize(v_normal);
	color = colorFromPointlight(light1, camPosition, v_position, normal, v_uv, 1) +
			        colorFromPointlight(light2, camPosition, v_position, normal, v_uv, 1) +
			        colorFromPointlight(light3, camPosition, v_position, normal, v_uv, 1);


//    color.rgb = pow(color.rgb, vec3(1.0/2.2));
//    float bd = smoothstep(-.08, .04, v_uv.x- t+2);
//    float bd2 = smoothstep(.0, 0.04, v_uv.x- t+2);
//    vec3 c = mix(vec3(0.4, 0.8, 0.98), color.rgb, .1);
//    color.rgb = mix(c, color.rgb, 1-bd);
//    color.a = (1-bd2)*.9 + .1;
//    color.a = .9;
}
