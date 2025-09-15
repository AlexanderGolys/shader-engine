#version 330 core

in vec3 v_position;

layout(location = 0) out vec4 col;



uniform mat4 mvp;
uniform vec3[__N__] params;
uniform vec3 camPosition;
uniform mat4 light1;


int SPHERE = 0;
int BOX = 1;


    float dot2(vec3 a) {
        return dot(a, a);
    }

vec3 lightPosition(mat4 m)
{
    return (m*vec4(1., 0., 0., 0.)).xyz;
}

vec4 lightColor(mat4 m)
{
    return m*vec4(0., 1., 0., 0.);
}

float lightPower(mat4 m)
{
    return (m*vec4(1., 0., 0., 0.)).w;
}

vec3 lightIntencities(mat4 m)
{
    return m[2].xyz;
}


__SDF__



vec3 grad_sdf(vec3 x, float eps) {
    return normalize(vec3(
    sdf(x + vec3(eps, 0, 0)) - sdf(x - vec3(eps, 0, 0)),
    sdf(x + vec3(0, eps, 0)) - sdf(x - vec3(0, eps, 0)),
    sdf(x + vec3(0, 0, eps)) - sdf(x - vec3(0, 0, eps))
    ));
}

float cosAngle_sdf(vec3 x, vec3 ray, float eps) {
    return max(0, dot(-ray, grad_sdf(x, eps)));
}





struct TraceResult {
    vec3 position; // point of min achieved distance in case of hit
    vec3 normal;
    float distance; // min distance in case of no hit
    float cosAngle;
};



TraceResult raytraceFull(vec3 p0, vec3 dir, float maxDistance, int maxIter, float tolerance) {
    dir = normalize(dir);
    float travel = 0.0;
    float minDistance = sdf(p0);
    vec3 minPos = p0;

    for (int i = 0; i < maxIter; i++) {
        float d = sdf(p0);
        if (d <= tolerance) {
            return TraceResult(p0, grad_sdf(p0, tolerance), 0, cosAngle_sdf(p0, dir, tolerance));
        }
        p0 += dir * d;
        travel += d;
        if (d < minDistance) {
            minDistance = d;
            minPos = p0;
        }
        if (travel >= maxDistance) {
            break;
        }
    }
    return TraceResult(minPos, grad_sdf(minPos, tolerance), minDistance, cosAngle_sdf(minPos, dir, tolerance));
}

    float AO(vec3 p, vec3 n, float step) {
        float distSum = 0;
        float maxDist = 0;
        for (int i = 0; i < 6; i++) {
            distSum += abs(sdf(p + n * step*i))/pow(2., i);
            maxDist += step*i/pow(2., i);
        }
        return distSum/maxDist;
    }





void main()
{
    float tol = 0.001;
    vec4 ambientColor = vec4(0.05, 0.05, 0.2, 1);

    TraceResult tr = raytraceFull(camPosition, normalize(v_position - camPosition), 100.0, 1000, tol);
    if (tr.distance > 0)
    {
        discard;
    }
    float ao = AO(tr.position, tr.normal, .1);
    vec3 lightDir = normalize(lightPosition(light1) - tr.position);
    float cosTheta = max(0, dot(tr.normal, lightDir));
    float lightDist = length(lightPosition(light1) - tr.position);
    vec3 light_c = lightIntencities(light1);
    col = ambientColor*ao + lightColor(light1)*cosTheta/(light_c.x + lightDist*light_c.y + lightDist*lightDist*light_c.z);
    col.a = 1;
}
