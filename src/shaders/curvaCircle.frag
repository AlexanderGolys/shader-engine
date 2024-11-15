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




vec3 lightPosition(mat4 m)
{
	return (m*vec4(1., 0., 0., 0.)).xyz;
}

float lightPower(mat4 m)
{
	return (m*vec4(1., 0., 0., 0.)).w;
}

vec4 lightColor(mat4 m)
{
	return m*vec4(0., 1., 0., 0.);
}

vec4 colorFactorFromSingleLight(mat4 light, vec3 camPos, vec3 pos, vec3 n, vec2 uv)
{
	vec3 light_direction = normalize(lightPosition(light) - pos);
    float cosTheta = abs(dot(n, light_direction));

//    float cosTheta = max(dot(n, light_direction), 0.);
	float distanceSquared = dot(lightPosition(light) - pos, lightPosition(light) - pos);
	vec3 reflectDirection = normalize(reflect(-light_direction, n));
	vec3 viewDirection = normalize(camPos - pos);
	float cosAlpha = max(dot(viewDirection, reflectDirection), 0.);

    return  texture(texture_ambient, uv) * intencities.x +
    texture(texture_diffuse, uv) * intencities.y * cosTheta * lightPower(light) * lightColor(light) / distanceSquared+
    texture(texture_specular, uv) * intencities.z  * pow(cosAlpha, intencities.w) * lightColor(light) * lightPower(light) / distanceSquared;

}




void main()
{

	vec3 normal = normalize(v_normal);
	color = colorFactorFromSingleLight(light1, camPosition, v_position, normal, v_uv*2.f) +
			colorFactorFromSingleLight(light2, camPosition, v_position, normal, v_uv*2.f) +
			colorFactorFromSingleLight(light3, camPosition, v_position, normal, v_uv*2.f);

}
