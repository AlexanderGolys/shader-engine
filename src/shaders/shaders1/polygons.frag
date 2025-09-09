#version 330 core

in vec4 v_color;
in vec3 v_normal;
in vec3 v_position;


layout(location = 0) out vec4 color;

uniform mat4 mvp;
uniform mat4 material;
uniform mat4 light1;
uniform mat4 light2;
uniform mat4 light3;
uniform vec3 camPosition;


vec4 ambientColor(mat4 m)
{
	return m * vec4(1., 0., 0., 0.);
}

vec4 diffuseColor(mat4 m)
{
	return m * vec4(0., 1., 0., 0.);
}

vec4 specularColor(mat4 m)
{
	return m * vec4(0., 0., 1., 0.);
}

float ambientIntencity(mat4 m)
{
	return (m * vec4(0., 0., 0., 1.)).x;
}

float diffuseIntencity(mat4 m)
{
	return (m * vec4(0., 0., 0., 1.)).y;
}

float specularIntencity(mat4 m)
{
	return (m * vec4(0., 0., 0., 1.)).z;
}

float shininess(mat4 m)
{
	return (m * vec4(0., 0., 0., 1.)).w;
}

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

vec4 colorFactorFromSingleLight(mat4 mat, mat4 light, vec3 camPos, vec3 pos, vec3 n)
{
	vec3 light_direction = normalize(lightPosition(light) - pos);
	float cosTheta = max(dot(n, light_direction), 0.);
	float distanceSquared = dot(lightPosition(light) - pos, lightPosition(light) - pos);
	vec3 reflectDirection = reflect(-light_direction, n);
	vec3 viewDirection = normalize(camPos - pos);
	float cosAlpha = max(dot(viewDirection, reflectDirection), 0.);
	return ambientColor(mat) * ambientIntencity(mat) +
		diffuseColor(mat) * diffuseIntencity(mat) * cosTheta * lightPower(light) * lightColor(light) / distanceSquared+
		specularColor(mat) * specularIntencity(mat) * pow(cosAlpha, shininess(mat)) * lightColor(light) * lightPower(light) / distanceSquared;

}



void main()
{
	vec3 normal = normalize(v_normal);
	color = colorFactorFromSingleLight(material, light1, camPosition, v_position, normal) +
			colorFactorFromSingleLight(material, light2, camPosition, v_position, normal) +
			colorFactorFromSingleLight(material, light3, camPosition, v_position, normal);

}