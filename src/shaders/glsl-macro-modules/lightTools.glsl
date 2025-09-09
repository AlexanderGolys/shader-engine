/* -----------LIGHT TOOLS------------------ */

PointLight parsePointLight(mat4 m)
{
    return PointLight(m[0].xyz, m[1], m[2].x, m[2].y, m[2].z, m[3].y);
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

vec4 ambientColor(Material mat, PointLight light, TraceResult point, float ao_step)
{
    return clamp(mat.ambientColor * AO(point.position, point.normal, ao_step) * lightAttenuation(light, point.position) * light.color, 0.0, 1.0);
}

vec4 diffuseColor(Material mat, PointLight light, TraceResult point)
{
    vec3 lightDir = lightDirection(light, point.position);
    float diff = max(0.0, dot(point.normal, -lightDir));
    return clamp(mat.diffuseColor * light.color * diff * lightAttenuation(light, point.position), 0.0, 1.0);
}

float softShadowFactor(vec3 point, vec3 lightPos, float radius, int maxIter, float tolerance)
{
    float shadowFactor = 1.0;
    vec3 dir = normalize(lightPos - point);
    float travel = radius/5.0;
    float maxTravel = length(lightPos - point);
    for (int i = 0; i < maxIter && travel < maxTravel; i++) {
        float d = sdf(point + dir * travel);
        if (d < tolerance) {
            return 0.0;
        }
        shadowFactor = min(shadowFactor, d/(travel*radius));
        travel += d;
    }
    return shadowFactor;
}

vec4 diffuseColorSoftShadows(Material mat, PointLight light, TraceResult point, int maxIter, float tolerance)
{
    float shadowFactor = softShadowFactor(point.position, light.position, light.softShadowRadius, maxIter, tolerance);
    return diffuseColor(mat, light, point) * shadowFactor;
}


vec3 refract(vec3 dir, vec3 normal, float ratio)
{
    float c = dot(dir, -normal);
    if (c < 0.0) {
        normal = -normal;
        c = -c;
    }
    return normalize(dir*ratio + normal*(ratio * c - sqrt(1.0 - ratio * ratio * (1.0 - c * c))));
}

vec4 colorSoftShadows(Material mat, PointLight light, TraceResult point, float ao_step, int maxIter, float tolerance)
{
    return ambientColor(mat, light, point, ao_step) + diffuseColorSoftShadows(mat, light, point, maxIter, tolerance);
}

vec4 colorNoShadows(Material mat, PointLight light, TraceResult point, float ao_step, int maxIter, float tolerance)
{
    return ambientColor(mat, light, point, ao_step) + diffuseColor(mat, light, point);
}


TraceResult propagate(TraceResult point, float step_eps, float maxDistance, int maxIter, float tolerance)
{
    vec3 dir;
    Material mat = material(point.position);

    if (sdf(point.position - point.rayDir * step_eps) >= 0.0) {
        dir = refract(point.rayDir, point.normal, 1/mat.ior);
    } else {
        dir = refract(point.rayDir, point.normal, mat.ior);
    }
    vec3 p_shift = point.position + dir * step_eps;
    if (sdf(p_shift) >= 0.0) {
        return raytraceFull(p_shift, dir, maxDistance, maxIter, tolerance);
    }
    return raytraceFullReverse(p_shift, dir, maxDistance, maxIter, tolerance);
}

vec4 refractedColor(TraceResult p0, PointLight light, float ao_step, float step_eps, float maxDistance, int maxIter, float tolerance, int steps)
{
    Material mat = material(p0.position);
    vec4 col = colorSoftShadows(mat, light, p0, ao_step, maxIter, tolerance)*mat.transparency*(1-mat.reflectivity);
    float mult = 1-mat.transparency;
    for (int i = 0; i < steps; i++) {
        if (p0.distance > tolerance) {
            break;
        }
        p0 = propagate(p0, step_eps, maxDistance, maxIter, tolerance);

        mat = material(p0.position);
        if (i>0) col += colorNoShadows(mat, light, p0, ao_step, maxIter, tolerance)*mat.transparency*mult;
        else col += colorSoftShadows(mat, light, p0, ao_step, maxIter, tolerance)*mat.transparency*mult;
        mult *= 1-mat.transparency;

    }
    return col;
}

vec4 reflectedColor(TraceResult p0, PointLight light, float ao_step, float step_eps, float maxDistance, int maxIter, float tolerance)
{
    Material mat = material(p0.position);
    vec3 dir_reflect = reflect(p0.rayDir, p0.normal);
    TraceResult tr_reflect = raytraceFull(p0.position+dir_reflect*step_eps, dir_reflect, maxDistance, maxIter, tolerance);

    if (tr_reflect.distance > tolerance) {
        return vec4(0, 0, 0, 1);
    }
    Material m_reflect = material(tr_reflect.position);
    vec3 dir_reflect2 = reflect(p0.rayDir, tr_reflect.normal);
    TraceResult tr_reflect2 = raytraceFull(tr_reflect.position+dir_reflect2*step_eps, dir_reflect2, maxDistance, maxIter, tolerance);
    if (tr_reflect2.distance > tolerance) {
        return colorSoftShadows(m_reflect, light, tr_reflect, ao_step, maxIter, tolerance)*mat.reflectivity*mat.transparency*(1-m_reflect.reflectivity);
    }
    Material m_reflect2 = material(tr_reflect2.position);
    return (colorSoftShadows(m_reflect, light, tr_reflect, ao_step, maxIter, tolerance)*(1-m_reflect.reflectivity) +
    colorNoShadows(m_reflect2, light, tr_reflect2, ao_step, maxIter, tolerance)*m_reflect.reflectivity)*mat.reflectivity*mat.transparency;
}

vec4 rayTraceColor(TraceResult p0, PointLight light, float ao_step, float step_eps, float maxDistance, int maxIter, float tolerance, int steps)
{
    return refractedColor(p0, light, ao_step, step_eps, maxDistance, maxIter, tolerance, steps)
    + reflectedColor(p0, light, ao_step, step_eps, maxDistance, maxIter, tolerance);
}
