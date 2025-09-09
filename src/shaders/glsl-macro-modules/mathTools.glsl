
/* -----------BASIC FUNCTIONS------------------ */

    float dot2(vec3 a) {
        return dot(a, a);
    }

float circularSmoothMin( float a, float b, float k )
{
    k *= 1.0/(1.0-sqrt(0.5));
    float h = max( k-abs(a-b), 0.0 )/k;
    return min(a,b) - k*0.5*(1.0+h-sqrt(1.0-h*(h-2.0)));
}

float circularSmoothMax( float a, float b, float k )
{
    return -circularSmoothMin( -a, -b, k );
}

float negated(float a) {
    return -a;
}
