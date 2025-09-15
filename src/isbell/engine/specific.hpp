#pragma once
// #include "glslUtils.hpp"
#include "../utils/flows.hpp"


#include <chrono>
#include <iosfwd>
#include <vector>

#include "indexedRendering.hpp"
#include "renderingUtils.hpp"




// const VectorField dabbaX = VectorField::constant(vec3(1, 0, 0));
// const VectorField dabbaY = VectorField::constant(vec3(0, 1, 0));
// const VectorField dabbaZ = VectorField::constant(vec3(0, 0, 1));
//

/**
 * @brief Planar surfaces with boundary
 */
PlanarMeshWithBoundary PlanarUnitDisk(int radial_res, int vertical_res);
PlanarMeshWithBoundary PlanarConvexPolygon(const std::vector<vec2> &verts);
PlanarMeshWithBoundary PlanarRing(int radial_res, int vertical_res, vec2 center, float radiusBig, float radiusSmall);



/**
 * \brief Planar curves
 */
SmoothParametricPlaneCurve circle(float r, vec2 center=PLANE_ORIGIN, float eps=.01);
SmoothParametricPlaneCurve ellipse(float a, float b, vec2 center=PLANE_ORIGIN, float eps=.01);
SmoothParametricPlaneCurve epitrochoid(float r, float R, float d, float eps = .01);
SmoothParametricPlaneCurve epicycloid(float r, float R, float eps = .01);
SmoothParametricPlaneCurve cardioid(float r, float eps=.01);
SmoothParametricPlaneCurve nephroid(float r, float eps=.01);
SmoothParametricPlaneCurve trefoiloid(float r, float eps=.01);
SmoothParametricPlaneCurve quatrefoloid(float r, float eps=.01);
SmoothParametricPlaneCurve hypotrochoid(float r, float R, float d, float eps = .01);
SmoothParametricPlaneCurve hypocycloid(float r, float R, float eps = .01);
SmoothParametricPlaneCurve astroid(float r, float eps=.01);
SmoothParametricPlaneCurve deltoid(float r, float eps=.01);
SmoothParametricPlaneCurve pentoid(float r, float eps=.01);
SmoothParametricPlaneCurve exoid(float r, float eps=.01);
SmoothParametricPlaneCurve LissajousCurve(float a, float b, float delta=PI/2, float r1=1, float r2=1, float eps = .01);
SmoothParametricPlaneCurve GernsterWave(float a, float b, float k, float c);


/**
 * \brief Space curves
 */
SmoothParametricCurve VivaniCurve(float r, float eps = .01);
SmoothParametricCurve sphericalSpiral(float a, float t_max, PolyGroupID id, float eps = .001);
SmoothParametricCurve sphericalSpiral(float a, float r, float t_max, PolyGroupID id, float eps = .001);
SmoothParametricCurve trefoil(float r, float R, float eps = .01);
SmoothParametricCurve torusKnot23(float scale, float R, float eps = .01);
SmoothParametricCurve torusKnot_pq(int p, int q,  float R, float scale, float eps = .01);
SmoothParametricCurve circle(float r, vec3 center, vec3 v1 = e1, vec3 v2 = e2, float eps = .01);
SmoothParametricCurve PLCurve(std::vector<vec3> points);
SmoothParametricCurve segment(vec3 p0, vec3 p1, float t0=0, float t1=1);
SmoothParametricCurve segment(vec3 p0, vec3 p1, float t0, float t1);

/**
 * \brief Primitive meshes
 * (triangles, quads, disks, icospheres, icosahedra, boxes, particles)
 */
IndexedMesh singleTrig(vec3 v0, vec3 v1, vec3 v2, PolyGroupID id=randomID());
IndexedMesh singleQuadShadeSmooth(vec3 outer1, vec3 inner1, vec3 inner2, vec3 outer2, PolyGroupID id);
IndexedMesh singleQuadShadeFlat(vec3 outer1, vec3 inner1, vec3 inner2, vec3 outer2, MaterialPhong &material, PolyGroupID id);
IndexedMesh singleQuadShadeFlat(vec3 outer1, vec3 inner1, vec3 inner2, vec3 outer2, MaterialPhong &material1, MaterialPhong &material2, PolyGroupID id);
IndexedMesh singleQuad(vec3 outer1, vec3 inner1, vec3 inner2, vec3 outer2, MaterialPhong &materiaInner1, MaterialPhong &materialInner2, MaterialPhong &materialOuter1, MaterialPhong &materialOuter2, bool shadeSmooth, PolyGroupID id);
IndexedMesh singleQuadShadeFlat(vec3 inner1, vec3 outer1, vec3 inner2, vec3 outer2, PolyGroupID id=randomID());
IndexedMesh singleQuadShadeFlat(vec3 inner1, vec3 outer1, vec3 inner2, vec3 outer2, vec4 color, PolyGroupID id=randomID());
IndexedMesh singleQuadShadeFlat(const vector<vec3> &corners, PolyGroupID id=randomID());
IndexedMesh icosahedron(float r, vec3 center, PolyGroupID id);
IndexedMesh icosphere(float r, int n, vec3 center, PolyGroupID id, vec4 color=BLACK);
IndexedMesh disk3d(float r, vec3 center, vec3 v1, vec3 v2, int radial_res, int vertical_res, const PolyGroupID &id);
IndexedMesh generate_random_particle_mesh(int n, vec3 bound1, vec3 bound2, float radius);
IndexedMesh box(vec3 size, vec3 center, PolyGroupID id);
IndexedMesh box(vec3 size, vec3 center, vec4 color, PolyGroupID id);

IndexedMesh box(vector<vec3> cornersUp, vector<vec3> cornersDown, vec4 color=BLACK);
IndexedMesh paraleblahblapid(vec3 corner, vec3 dir1, vec3 dir2, vec3 dir3);
IndexedMesh pyramid(const vector<vec3> &cornersDown, vec3 apex, PolyGroupID id=randomID());


/**
 * \brief Parametric surfaces
 */
SmoothParametricSurface plane(vec3 corner, vec3 side1, vec3 side2, float eps=.01);
SmoothParametricSurface sphere(float r, vec3 center=ORIGIN, float cutdown=0, float eps=.01);
SmoothParametricSurface ellipsoid(float rx,float ry,float rz, vec3 center=ORIGIN, float eps=.01);
SmoothParametricSurface DupinCyclide(float Ra, float Rb, float r, float delta_r, float eps=.01);
SmoothParametricSurface disk(float r, vec3 center, vec3 v1, vec3 v2, float eps);
SmoothParametricSurface cylinder(float r, vec3 c1, vec3 c2, vec3 v1, vec3 v2, float eps);
SmoothParametricSurface hyperbolic_helicoid(float a, float eps=.01); // todo check out
SmoothParametricSurface LawsonTwist(float alpha, Quaternion q, vec2 range_u, float eps=.01);
SmoothParametricSurface coolLawson(float eps=0.01);
SmoothParametricSurface sudaneseMobius(float eps=0.01);
SmoothParametricSurface parametricPlane2ptHull(vec3 v1=e1, vec3 v2=e2, float normal_shift=0, float eps=.01);
SmoothParametricSurface LawsonKleinBottle(float eps=0.01);
SmoothParametricSurface catenoid(float a, float eps=.01);
SmoothParametricSurface helicoid(float a, float eps=.01);
SmoothParametricSurface enneper(float k, float l, float eps=.01);
SmoothParametricSurface twistedTorus(float a, float m, float n, int dommul1, int dommul2, float eps=.01);
SmoothParametricSurface polyTorus(float r, float n, float alpha, float eps=.01);
SmoothParametricSurface ellipticTorus(float c, float eps=.01);
SmoothParametricSurface limpetTorus(float eps=.01);
SmoothParametricSurface fig8(float c, float eps=.01);
SmoothParametricSurface doubleTorus(float eps=.01);
SmoothParametricSurface saddleTorus(float eps=.01);
SmoothParametricSurface kinkyTorus(float eps=.01);
SmoothParametricSurface GraysKlein(float a, float n, float m, float eps=.01);
SmoothParametricSurface bowTie(float eps=.01);
SmoothParametricSurface bohemianDome(float a, float b, float c, float eps=.01);
SmoothParametricSurface horn(float eps=.01);
SmoothParametricSurface crescent(float eps=.01);
SmoothParametricSurface seaShell(int n, float a, float b, float c, float eps=.01);
SmoothParametricSurface cone(const SmoothParametricCurve &base, vec3 apex, float eps);
SmoothParametricSurface coneSide(float r, float h, vec3 center, vec3 v1, vec3 v2, float eps);
SmoothParametricSurface torus(float r, float R, vec3 center=ORIGIN, float eps=.01);;


/**
 * \brief Implicit surfaces
 */
SmoothImplicitSurface sphereImplicit(float r, vec3 center, float eps=.01);
SmoothImplicitSurface torusImplicit(float r, float R, vec3 center, float eps=.01);
SmoothImplicitSurface genus2Implicit(float eps=.01);
SmoothImplicitSurface wineGlass(float eps=.01);
SmoothImplicitSurface equipotentialSurface(vector<vec3> points, vector<float> charges, float potential, float eps=.01);
SmoothImplicitSurface chair(float k=5, float a=.95, float b=.82, float eps=.01);
SmoothImplicitSurface tangleCube(float eps=.01);
SmoothImplicitSurface wineImplicit(float eps=.01);
SmoothImplicitSurface gumdrop(float eps=.01);
SmoothImplicitSurface genus2Implicit2(float eps=.01);
SmoothImplicitSurface genus2Implicit3(float c, float d, float eps=.01);
SmoothImplicitSurface genus2Implicit4(float d=.01, float eps=.01);
SmoothImplicitSurface superellipsoid(float alpha1, float alpha2, float a, float b, float c, float r=1, float eps=.01);
SmoothImplicitSurface superQuadric(float alpha, float beta, float gamma, float a, float b, float c, float r=1, float eps=.01);
SmoothImplicitSurface K3Surface222(float eps=.01);
SmoothImplicitSurface planeImplicit(vec3 normal, float d, vec3 center=ORIGIN, float eps=.01);


/**
 * \brief Non-geometric meshes
 */
IndexedMesh arrow(vec3 start, vec3 head, float radius, float head_len, float head_radius, int radial, int straight, float eps, std::variant<int, std::string> id);
IndexedMesh drawArrows(const vector<vec3> &points, const vector<vec3> &directions, float radius, float head_len, float head_radius, int radial, int straight, float eps, const std::variant<int, std::string> &id);
IndexedMesh drawVectorFieldArrows(const VectorField &field, const vector<vec3> &points, const HOM(float, float)& len, const HOM(float, float) &radius, const HOM(float, float) &head_len, const HOM(float, float)& head_radius, int radial, int straight, float eps, const std::variant<int, std::string> &id);


/**
 * \brief  Steady flows
 */

SteadyFlow PousevillePlanarFlow(float h, float nabla_p, float mu, float v0, float eps=.01);
SteadyFlow PousevillePipeFlow(float nabla_p, float mu, float c1, float c2, float eps=.01);


/**
 * \brief Unsteady flows
 */
SurfaceParametricPencil freeSurface(vector<float> a_m, vector<float> phi_m, vector<vec2> k_m, vector<float> omega_m, float h);


/**
 * \brief Implicit volumes
 */
ImplicitVolume implicitBall(float r, vec3 center=ORIGIN, float eps=.01);
ImplicitVolume implicitVolumeEllipsoid(float rx, float ry, float rz, vec3 center=ORIGIN, float eps=.01);




/**
 * \brief Deprecated
 *
 * But needed for backward compatibility
 */
SuperCurve circle(float r, std::function<float(float)> w, const std::function<MaterialPhong(float)> &mat, int n, vec3 center=ORIGIN, vec3 v1=e1, vec3 v2=e2, float eps=.01);

class Disk3D : public IndexedMesh {
	vec3 center;
	vec3 forward;
	vec3 down;
	vec3 normal;
	float radius;
	PolyGroupID id;
public:
	Disk3D(const std::vector<Vertex> &nodes, const std::vector<ivec3> &faceInds, vec3 center, vec3 forward, vec3 down, PolyGroupID id);
	Disk3D(const char* filename, vec3 center, vec3 forward, vec3 down, PolyGroupID id);
	Disk3D(float r, vec3 center, vec3 forward, vec3 down, int radial_res, int vertical_res, const PolyGroupID &id);
	void move(vec3 center, vec3 forward, vec3 down, bool scaleWidth);



	float moveRotate(vec3 center, vec3 forward, vec3 down);
	void rotate(float angle);
	static float angle(const BufferedVertex &v) { return v.getColor().x; }
	static float rParam(const BufferedVertex &v) { return v.getColor().y; }
	static float width(const BufferedVertex &v) { return v.getColor().z; }
	static float widthNormalised(const BufferedVertex &v) { return v.getColor().w; }
	float getR() const {return radius;}

	float rReal(const BufferedVertex &v);
	void scaleR(float r, bool scaleWidth);
	void setR(float r);;
	static void setAbsoluteWidth(BufferedVertex &v, float w) {v.setColor(w, 2);}
	static void setRelativeWidth(BufferedVertex &v, float w) {v.setColor(w, 3);}
	void setEmpiricalRadius();
	void setColorInfo();
};
