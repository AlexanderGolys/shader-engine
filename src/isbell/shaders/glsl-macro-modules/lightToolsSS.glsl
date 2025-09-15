/* -----------LIGHT TOOLS------------------ */

PointLight parsePointLight(mat4 m)
{
    return PointLight(m[0].xyz, m[1], m[2].x, m[2].y, m[2].z, m[3].w);
}

vec3 lightDirection(PointLight light, vec3 point)
{
    return normalize(point - light.position);
}

float lightDistance(PointLight light, vec3 point)
{
    return length(point - light.position);
}

float lightAttenuation(PointLight light, vec3 point)
{
    float d = lightDistance(light, point);
    return 1.0 / (light.constFactor + light.linearFactor * d + light.quadraticFactor * d * d);
}




vec4 scatterLightColor(vec3 p, PointLight light, int maxIter, float tolerance, float bd_eps)
{
    vec4 c = light.color*lightAttenuation(light, p);
    float dist = 0;
    float maxDist = length(p-light.position);
    vec3 dir = normalize(p-light.position);
    vec3 pos = light.position;
    for (int i = 0; i < maxIter; i++) {
        PointAndDist outside = traceOutside(pos, dir, maxDist, maxIter, tolerance);
        dist += outside.dist + bd_eps;
        if (dist >= maxDist - tolerance) {
            return c;
        }
        pos += (outside.dist+bd_eps) * dir;
        PointAndDist inside = traceInside(pos, dir, maxDist, maxIter, tolerance);
        Material m = material(inside.obj);
        float factor = exp(-inside.dist*m.translucency);
        c = mix(m.diffuseColor*factor, c, factor);

        dist += inside.dist + bd_eps;
        pos += (inside.dist + bd_eps) * dir;
        if (dist >= maxDist - tolerance || c.a < tolerance) {
            return c;
        }
    }
    return c;
}

vec4 ambientColor(Material mat, PointLight light, TraceResult point, float ao_step, int maxIter, float tolerance, float bd_eps)
{
    vec4 c = scatterLightColor(point.position, light, maxIter, tolerance, bd_eps);
    return clamp(mat.ambientColor * AO(point.position, point.normal, ao_step) * c, 0.0, 1.0);
}

vec4 diffuseColor(Material mat, PointLight light, TraceResult point, int maxIter, float tolerance, float bd_eps)

{
    vec3 lightDir = lightDirection(light, point.position);
    float diff = max(0, dot(point.normal, -lightDir));
    return clamp(mat.diffuseColor * scatterLightColor(point.position, light, maxIter, tolerance, bd_eps)*diff, 0.0, 1.0);
}





vec4 colorSoftShadows(Material mat, PointLight light, TraceResult point, float ao_step, int maxIter, float tolerance, float bd_eps)
{
    return ambientColor(mat, light, point, ao_step, maxIter, tolerance, bd_eps) + diffuseColor(mat, light, point, maxIter, tolerance, bd_eps);
//        return diffuseColor(mat, light, point, maxIter, tolerance, bd_eps);

}








vec4 rayTraceColor(TraceResult p0, PointLight light, float ao_step, float step_eps, float maxDistance, int maxIter, float tolerance, int steps)
{
    return colorSoftShadows(material(p0.position), light, p0, ao_step, maxIter, tolerance, step_eps);
}
