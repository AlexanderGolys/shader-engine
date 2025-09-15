#version 330 core

in vec3 v_position;

layout(location = 0) out vec4 color;



uniform mat4 mvp;
uniform vec3[__P__+1] params;
uniform vec4[3*__N__] materials;
uniform vec3 camPosition;
uniform mat4 light1;

__STRUCTS__
__MATHTOOLS__

__SDF__

__SDFTOOLS__
__LIGHTTOOLS__








void main()
{
    float tol = 0.00001;
    float mat_bd_eps = 0.0003;
    int maxIter = 4000;
    float ao_step = 0.15;
    float maxDistance = 1000.0;
    int steps = 6;

    vec3 dir = normalize(v_position - camPosition);
    TraceResult tr = raytraceFull(camPosition, dir, maxDistance, maxIter, tol);

    if (tr.distance > tol)
    {
        discard;
    }

    PointLight light = parsePointLight(light1);
    color = rayTraceColor(tr, light, ao_step, mat_bd_eps, maxDistance, maxIter, tol, steps);
   color = clamp(color, 0., 1.);
    color.a = 1.;


}
