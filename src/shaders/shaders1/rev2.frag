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


float TAU = 6.28318530718;

float mod(float a, float b)
{
    return a - b * floor(a / b);
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

vec4 colorFactorFromSingleLight(mat4 light, vec3 camPos, vec3 pos, vec3 n, vec2 uv)
{
	vec3 light_direction = normalize(lightPosition(light) - pos);
//    float cosTheta = abs(dot(n, light_direction));

    float cosTheta = max(abs(dot(n, light_direction)), 0.);
	float distanceSquared = dot(lightPosition(light) - pos, lightPosition(light) - pos);
	vec3 reflectDirection = normalize(reflect(-light_direction, n));
	vec3 viewDirection = normalize(camPos - pos);
	float cosAlpha = max(abs(dot(viewDirection, reflectDirection)), 0.);

    return  texture(texture_ambient, uv) * intencities.x +
    texture(texture_diffuse, uv) * intencities.y * cosTheta * lightPower(light) * lightColor(light) / distanceSquared+
    texture(texture_specular, uv) * intencities.z  * pow(cosAlpha, intencities.w) * lightColor(light) * lightPower(light) / distanceSquared;

}



void main()
{
//    float t = time + (1-bdColor.a);
//    vec4 c = bdColor;
//    c.a = 1.;
	vec3 normal = normalize(v_normal);

    float t= time*4.;
    if (v_color.x >= t/2)
    {
       discard;
    }
	color = colorFactorFromSingleLight(light1, camPosition, v_position, normal, 1.5*v_uv+0.5*vec2(-v_color.z, v_color.z)*t) +
			colorFactorFromSingleLight(light2, camPosition, v_position, normal, 1.5*v_uv+0.5*vec2(-v_color.z, v_color.z)*t) +
			colorFactorFromSingleLight(light3, camPosition, v_position, normal, 1.5*v_uv+0.5*vec2(-v_color.z, v_color.z)*t);

	color.a = 1.;
	vec4 red = vec4(.89, .4, .3, 1);
	vec4 blue = vec4(.3, .5, .85, 1);
	vec4 color_blue = min(color*blue*2, vec4(1));
	vec4 color_red =  min(color*red*2, vec4(1));

//	color = color*(.8+abs(v_color.w)/2);

//	color = color*color*1.9;

    float collar = smoothstep(0.1, 2, v_color.z);
    color = mix(color, color_red, collar);
    collar = smoothstep(2, 10, v_color.z);
    color = mix(color, red, collar);

    collar = smoothstep(.1, 2, -v_color.z);
    color = mix(color, color_blue, collar);
    collar = smoothstep(2,10, -v_color.z);
    color = mix(color, blue, collar);
}
