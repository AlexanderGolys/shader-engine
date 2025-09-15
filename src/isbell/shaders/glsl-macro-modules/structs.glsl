

/* -----------STRUCTS------------------ */


struct TraceResult {
    vec3 position; // point of min achieved distance in case of hit
    vec3 normal;
    float distance; // min distance in case of no hit
    float cosAngle;
    vec3 rayDir;
    int obj;
};

struct PointLight {
    vec3 position;
    vec4 color;
    float constFactor;
    float linearFactor;
    float quadraticFactor;
    float softShadowRadius;
};

struct Material {
    vec4 ambientColor;
    vec4 diffuseColor;
    float reflectivity;
    float transparency;
    float ior;
    float translucency;
};

struct PointAndDist{
    vec3 point;
    float dist;
    int obj;
};

Material materialConstructor(vec4 m1, vec4 m2, vec4 m3){
    return Material(m1, m2, m3.r, m3.g, m3.b, m3.a);
}
