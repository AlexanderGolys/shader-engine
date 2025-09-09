#version 330 core

in vec4 v_color;
in vec3 v_normal;
in vec3 v_position;
in vec4 v_ambientColor;
in vec4 v_diffuseColor;
in vec4 v_specularColor;
in vec4 v_intencities;

layout(location = 0) out vec4 color;

uniform mat4 mvp;
uniform mat4 light1;
uniform mat4 light2;
uniform mat4 light3;
uniform vec3 camPosition;




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

vec4 colorFactorFromSingleLight(mat4 light, vec3 camPos, vec3 pos, vec3 n)
{
	vec3 light_direction = normalize(lightPosition(light) - pos);
	float cosTheta = max(dot(n, light_direction), 0.);
	float distanceSquared = dot(lightPosition(light) - pos, lightPosition(light) - pos);
	vec3 reflectDirection = normalize(reflect(-light_direction, n));
	vec3 viewDirection = normalize(camPos - pos);
	float cosAlpha = max(dot(viewDirection, reflectDirection), 0.);
	return v_ambientColor * v_intencities.x/3 +
		v_diffuseColor * v_intencities.y * cosTheta * lightPower(light) * lightColor(light) / distanceSquared+
		v_specularColor * v_intencities.z  * pow(cosAlpha, v_intencities.w) * lightColor(light) * lightPower(light) / distanceSquared;

}




void main()
{

	vec3 normal = normalize(v_normal);
	color = colorFactorFromSingleLight(light1, camPosition, v_position, normal) +
			colorFactorFromSingleLight(light2, camPosition, v_position, normal) +
			colorFactorFromSingleLight(light3, camPosition, v_position, normal);

}