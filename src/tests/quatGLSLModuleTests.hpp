#pragma once
#include "../utils/func.hpp"
#include <cassert>
#include "unittests.hpp"
#include <iostream>
#include "../utils/formatters.hpp"

using namespace glm;

namespace testsHelpers {
  inline vec4 multH(vec4 p, vec4 q) {
    return vec4(
        p.x * q.x - p.y * q.y - p.z * q.z - p.w * q.w,
        p.x * q.y + p.y * q.x + p.z * q.w - p.w * q.z,
        p.x * q.z - p.y * q.w + p.z * q.x + p.w * q.y,
        p.x * q.w + p.y * q.z - p.z * q.y + p.w * q.x
    );
  }

  inline vec4 conjH(vec4 q) {
    return vec4(q.x, -q.y, -q.z, -q.w);
  }

  inline vec4 invH(vec4 q) {
    return conjH(q) / dot(q, q);
  }

  inline vec3 imagH(vec4 q) {
    return vec3(q.y, q.z, q.w);
  }

  inline vec3 rotateVectorByQuaternion(vec3 v, vec4 q) {
    vec4 qv = vec4(0.0, v);
    vec4 qConj = conjH(q);
    vec4 rotatedQ = multH(multH(q, qv), qConj);
    return vec3(rotatedQ.y, rotatedQ.z, rotatedQ.w);
  }

  inline mat3 doubleCoverSO3(vec4 q) {
    float x0 = q.x;
    float x1 = -q.y;
    float x2 = -q.z;
    float x3 = -q.w;

    return mat3(
       x0*x0 + x1*x1 - x2*x2 - x3*x3, 2.0*(x1*x2 - x0*x3), 2.0*(x1*x3 + x0*x2),
         2.0*(x1*x2 + x0*x3), x0*x0 - x1*x1 + x2*x2 - x3*x3, 2.0*(x2*x3 - x0*x1),
         2.0*(x1*x3 - x0*x2), 2.0*(x2*x3 + x0*x1), x0*x0 - x1*x1 - x2*x2 + x3*x3
    );
  }

  inline float normMat3(mat3 m) {
    return std::sqrt(m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2] +
                     m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2] +
                     m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2]);
  }

  inline bool nearlyEqualM3(const mat3 &a, const mat3 &b) {
    return nearlyEqual(normMat3(a - b), 0.f);
  }

  inline bool isSpecialOrthogonal(const mat3 &m) {
    return nearlyEqual(det(m), 1) && nearlyEqualM3(transpose(m) * m, mat3(1));
  }

  inline vec4 randomUnitQuaternion() {

    auto v = vec4(randomFloat(-1, 1),
                randomFloat(-1, 1),
                randomFloat(-1, 1),
                randomFloat(-1, 1));
    return v / length(v);
  }

  inline vec3 randomVec3() {

    auto v = vec3(randomFloat(-1, 1),
                randomFloat(-1, 1),
                randomFloat(-1, 1));
    return v;
  }

  struct SE3 {
    vec4 quat;
    vec3 v;
  };

  inline SE3 randomSE3() {

    return SE3{randomUnitQuaternion(), randomVec3()};
  }


  inline SE3 multSE3(const SE3 &a, const SE3 &b) {
    return SE3(
        multH(a.quat, b.quat),
        a.v + rotateVectorByQuaternion(b.v, a.quat)
    );
  }

  inline SE3 inv_SE3(const SE3 &a) {
    return SE3(
        invH(a.quat),
        rotateVectorByQuaternion(-a.v, conjH(a.quat))
    );
  }

  inline SE3 id_SE3() {
    return SE3(vec4(1, 0, 0, 0), vec3(0.0));
  }

  inline vec3 transformPoint(const SE3 &tf, vec3 p) {
    return rotateVectorByQuaternion(p, tf.quat) + tf.v;
  }

  inline bool nearlyEqualSE3(const SE3 &a, const SE3 &b) {
    return nearlyEqual(a.v,  b.v) && nearlyEqualM3(doubleCoverSO3(a.quat), doubleCoverSO3(b.quat));
  }
}





inline bool multTests()
  {
    try {
      bool passed = true;
      auto one = vec4(1, 0, 0, 0);
      auto i = vec4(0, 1, 0, 0);
      auto j = vec4(0, 0, 1, 0);
      auto k = vec4(0, 0, 0, 1);

      passed &= assertNearlyEqual_UT(testsHelpers::multH(one, one), one);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(one, i), i);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(one, j), j);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(one, k), k);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(i, i), -one);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(j, j), -one);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(k, k), -one);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(i, j), k);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(j, i), -k);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(j, k), i);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(k, j), -i);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(k, i), j);
      passed &= assertNearlyEqual_UT(testsHelpers::multH(i, k), -j);

    return passed;
  }
    catch (...) {
      return false;
    }
  }

inline bool doubleCoverTest()
{
  try {
    bool passed = true;
    for (int _ = 0; _ < 10; _++) {
      auto q = testsHelpers::randomUnitQuaternion();
      auto m = testsHelpers::doubleCoverSO3(q);
      passed &= assertTrue_UT(testsHelpers::isSpecialOrthogonal(m));
      if (!assertTrue_UT(testsHelpers::isSpecialOrthogonal(m))) {
        LOG("Matrix is not in SO(3):");
        LOG(format("{}", m));
        LOG("Determinant: " + std::to_string(det(m)));
        LOG(format("m^T * m = {}", transpose(m) * m));
      }

    }

    return passed;
  }
  catch (...) {
    return false;
  }
}
inline bool rotationsQuatTest()
{
  try {
    bool passed = true;
    for (int _ = 0; _ < 10; _++) {
      auto q = testsHelpers::randomUnitQuaternion();
      auto m = testsHelpers::doubleCoverSO3(q);
      auto x = testsHelpers::imagH(testsHelpers::randomUnitQuaternion());
      passed &= assertNearlyEqual_UT(testsHelpers::rotateVectorByQuaternion(x, q), m * x);

      if (!assertNearlyEqual_UT(testsHelpers::rotateVectorByQuaternion(x, q), m * x)) {
        LOG("Quaternion rotation and matrix rotation do not agree:");
        LOG("Vector: " + format("{}", x));
        LOG(format("Quaternion rotation: {}", testsHelpers::rotateVectorByQuaternion(x, q)));
        LOG("Matrix rotation: " + format("{}", m * x));
      }
    }

    return passed;
  }
  catch (...) {
    return false;
  }
}

inline bool SE3Test()
{
  try {
    bool passed = true;
    for (int _ = 0; _ < 10; _++) {
      auto g1 = testsHelpers::randomSE3();
      auto g2 = testsHelpers::randomSE3();
      auto g3 = testsHelpers::randomSE3();
      auto p = testsHelpers::randomVec3();
      auto g1_inv = testsHelpers::inv_SE3(g1);

      passed &= assertTrue_UT(testsHelpers::nearlyEqualSE3(
          testsHelpers::multSE3(testsHelpers::multSE3(g1, g2), g3),
          testsHelpers::multSE3(g1, testsHelpers::multSE3(g2, g3))
      ));
      auto a = testsHelpers::multSE3(testsHelpers::multSE3(g1, g2), g3);
      auto b = testsHelpers::multSE3(g1, testsHelpers::multSE3(g2, g3));

      passed &= assertTrue_UT(testsHelpers::nearlyEqualSE3(
        testsHelpers::multSE3(g1, testsHelpers::inv_SE3(g1)),
        testsHelpers::id_SE3() ));


      passed &= assertNearlyEqual_UT(
        testsHelpers::transformPoint(g1_inv, testsHelpers::transformPoint(g1, p)), p);
    }
    return passed;
  }
  catch (...) {
    return false;
  }
}
inline UnitTestResult quatGLSLModuleTests__all()
  {
    int passed = 0;
    int all = 4;

    if (multTests())
      ++passed;
    if (doubleCoverTest())
      ++passed;
    if (rotationsQuatTest())
      ++passed;
    if (SE3Test())
      ++passed;

  return UnitTestResult(passed, all);
  }
