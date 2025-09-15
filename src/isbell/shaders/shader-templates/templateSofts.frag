#version 330 core

in vec3 v_position;

layout(location = 0) out vec4 color;



uniform mat4 mvp;
uniform vec3[__N__] params;
uniform vec3 camPosition;
uniform mat4 light1;

/* -----------TOOLS------------------ */
__STRUCTS__
__MATHTOOLS__

/* -----------FLOOR------------------ */
__FLOOR__

/* -----------WATER------------------ */
__WATER__

/* -----------GLASS------------------ */
__GLASS__

/* -----------TOTAL------------------ */
__SDF__

/* -----------RAYTRACING------------------ */
__SDFTOOLS__

/* -----------LIGHTS------------------ */
__LIGHTTOOLS__








void main()
{
    float tol = 0.0001;
    int maxIter = 1000;
    float ao_step = 0.1;
    float maxDistance = 1000.0;

    Material material = Material(vec4(4, 15, 30, 255)/255, vec4(54, 133, 181, 255)/255, 0.5, 0.5, 1.5);

    TraceResult tr = raytraceFull(camPosition, normalize(v_position - camPosition), maxDistance, maxIter, tol);

    if (tr.distance > tol)
    {
        discard;
    }

    PointLight light = parsePointLight(light1);
    color = clamp(ambientColor(material, light, tr, ao_step) + diffuseColorSoftShadows(material, light, tr, maxIter, tol), 0., 1.);

    color.a = 1;
}
