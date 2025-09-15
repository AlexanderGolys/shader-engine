
/* -----------SDF METHODS------------------ */


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


TraceResult raytraceFull(vec3 p0, vec3 dir, float maxDistance, int maxIter, float tolerance) {
    dir = normalize(dir);
    float travel = 0.0;
    float minDistance = sdf(p0);
    vec3 minPos = p0;

    for (int i = 0; i < maxIter; i++) {
        float d = sdf(p0);
        if (d <= tolerance) {
            return TraceResult(p0, grad_sdf(p0, tolerance), 0, cosAngle_sdf(p0, dir, tolerance), dir, closestObject(p0));
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
    return TraceResult(minPos, grad_sdf(minPos, tolerance), max(0, minDistance), cosAngle_sdf(minPos, dir, tolerance), dir, closestObject(minPos));
}

TraceResult raytraceFullReverse(vec3 p0, vec3 dir, float maxDistance, int maxIter, float tolerance) {
    int objInd = closestObject(p0);
    dir = normalize(dir);
    float travel = 0.0;
    float minDistance = -sdf(p0, objInd);
    vec3 minPos = p0;

    for (int i = 0; i < maxIter; i++) {
        float d = -sdf(p0, objInd);
        if (d <= tolerance) {
            return TraceResult(p0, -grad_sdf(p0, tolerance), 0, cosAngle_sdf(p0, dir, tolerance), dir, objInd);
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
    return TraceResult(minPos, -grad_sdf(minPos, tolerance), max(0, minDistance), cosAngle_sdf(minPos, dir, tolerance), dir, objInd);
}

    float AO(vec3 p, vec3 n, float step) {
        float distSum = 0;
        float maxDist = 0;
        for (int i = 1; i < 10; i++) {
            distSum += max(0, sdf(p + n * step*i))/i;
            maxDist += step;
        }
        return distSum/maxDist;
    }

PointAndDist traceOutside(vec3 p0, vec3 dir, float maxDistance, int maxIter, float tolerance) {
    dir = normalize(dir);
    float travel = 0.0;
    float minDistance = sdf(p0);
    vec3 minPos = p0;

    for (int i = 0; i < maxIter; i++) {
        float d = sdf(p0);
        if (d <= tolerance) {
            return PointAndDist(p0, travel, closestObject(p0));
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
    return PointAndDist(minPos, 2000000, closestObject(minPos));
}

PointAndDist traceInside(vec3 p0, vec3 dir, float maxDistance, int maxIter, float tolerance) {
    int objInd = closestObject(p0);
    dir = normalize(dir);
    float travel = 0.0;
    float minDistance = -sdf(p0, objInd);
    vec3 minPos = p0;

    for (int i = 0; i < maxIter; i++) {
        float d = -sdf(p0, objInd);
        if (d <= tolerance) {
            return PointAndDist(p0, travel, objInd);
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
    return PointAndDist(minPos, 0, objInd);
}
