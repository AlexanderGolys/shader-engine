#pragma once
#include "solidMeshes.hpp"

class RigidBody;

class Force {
    R3 force;
    betterInFamily(void) updateOnSource;
    END(void) noLongerValidInterface; 
public:
    Force(R3 force, const procrastinateIn(float) &updateOnSource, const END(void) &noLongerValidInterface) : force(force), updateOnSource(updateOnSource), noLongerValidInterface(noLongerValidInterface) {}
    virtual ~Force() {noLongerValidInterface();}
    Force(R3 force, const procrastinateIn(float) &updateOnSource) : force(force), updateOnSource(updateOnSource), noLongerValidInterface([] {}) {}
    explicit Force(R3 force) : force(force), updateOnSource([](float) {}), noLongerValidInterface([] {}) {}

    virtual void apply(float t) {updateOnSource(t);}
    virtual void noLongerValid() {noLongerValidInterface();}
    virtual void setForce(R3 force) {this->force = force;}
    virtual void setUpdateCallback (const procrastinateIn(float) &updateOnSource);
	virtual void setDestructionCallback (const END(void) &noLongerValidInterface);
	virtual R3 getForceVector() {return force;}
};

enum class ForceType {
    COLLISION, CONST_FIELD, STATIC_VF, DYNAMIC_VF, CUSTOM
};

class RigidForce : public Force {
    AffineLine* actionAxis = nullptr;
    R3* pointOfAction = nullptr;
    int* indexOfClosestPointInBody = nullptr;
    ForceType type;
    vector<END(void)> destroyCallbacksRegistered;
public:
    explicit RigidForce(ForceType type) : Force(R3(0)), type(type) {}
    virtual void addForceToBody(RigidBody &body, const END(void) &bodyChangeCallback, const END(void) &forceChangeCallback);
};


class RigidCollisionForce : public RigidForce {
    R3 normal;
    float magnitude;
    std::weak_ptr<RigidBody> body1, body2;
    std::weak_ptr<END(void)> notifierBody1, notifierBody2;
public:
    RigidCollisionForce(const R3 &normal, float magnitude, const std::weak_ptr<RigidBody> &body1, const std::weak_ptr<RigidBody> &body2);
    virtual void movementEvent(int notifierBodyIndex);
    void setForce(vec3 force) override {normal = force; magnitude = length(force);}
    vec3 getForceVector() override {return normal*magnitude;}
};

class RigidForceFieldConst : public RigidForce {
    R3 force;
public:
    explicit RigidForceFieldConst(const R3 &field) : RigidForce(ForceType::CONST_FIELD), force(field) {};
    void setForce(vec3 force) override {throw std::logic_error("Constant field cannot be modified by definition");}
    vec3 getForceVector() override {return force;}
};


BigMatrix_t dMdt (const BigMatrix_t &M, float delta);

inline BigMatrix_t operator* (const BigMatrix_t &M, float f) { return [M, f](float t) {return M(t)*f;}; };
inline BigMatrix_t operator+ (const BigMatrix_t & M, const BigMatrix_t & M2) { return [M, M2](float t) {return M(t)+M2(t);}; };

inline vec3 cast_vec3(const vector<float> &v) {return vec3(v[0], v[1], v[2]);};


class RigidBody {
    vector<RigidForce> activeForces;
    FloatMatrix _positions; // positions of the points relative to the center of world
    OPT(FloatMatrix) _dPos; // first derivative of positions
    OPT(FloatMatrix) _ddPos; // second derivative of positions
    OPT(FloatMatrix) _V; // linear velocities
    vec69 _masses; // linear accelerations
    OPT(FloatMatrix) _P; // linear momenta
    OPT(vec69) _L;
    OPT(mat3) _I; // innertia tensor
    OPT(R3) _angularVelocity;
    R3 _centerOfWorld;
    R3 _centerOfMass;
    float_hm _potentialEnergy;
    float_hm _totalEnergy;
	float_hm _kineticEnergy;
    float time;
    float eps=.01;

public:
    RigidBody(const FloatMatrix& positions, const vec69& masses, const FloatMatrix& linearVelocities, R3 centerOfWorld, R3 angularVelocity, float time=0);;
	RigidBody(const FloatMatrix& positions, const vec69& masses, const FloatMatrix& linearVelocities, R3 angularVelocity, float time=0);;

    END(void) addForceField(const RigidForce &field);
    R3 accumulateForces(R3 pos);
    R3 calculateCenterOfMass();
    float  calculateKineticEnergy();
    float potentialEnergy() const;
    void setTotalEnergy(float totalEnergy);
    vec3 position(float t, int i);
    vec3 positionRelCm(float t, int i);
    void changeFrame(HOM(float, HOM(const IndexedTriangle&, mat3)) M_t);
    R3 linearVelocity(float t, int i);
    R3 linearMomentum(float t, int i);
    R3 totalLinearMomentum(float t);
    R3 angularMomentum(float t);
    R3 angularVelocity(float t);

    void calculateInertiaTensor();
    void calculateMomenta();
};

class PhysicalEnvironment {
    std::vector<RigidBody> bodies;
    public:
    void addRigidBody(const RigidBody &body) {
        bodies.push_back(body);
    };
};


class RigidBodyTriangulated2D {
	std::shared_ptr<IndexedMesh> mesh;
	vec3 centerOfMass = vec3(0);
	mat3 I = mat3(0);
	vec3 angularVelocity;
	vec3 linearVelocityCM;
	vec3 angularAcceleration;
	vec3 linearAccelerationCM;
public:
	RigidBodyTriangulated2D(std::shared_ptr<IndexedMesh> mesh, vec3 angularVelocity,  vec3 linearVelocity,  vec3 angularAcceleration,  vec3 linearAcceleration);

	void update(float dt);

	void setAngularVelocity(const vec3 &omega);
	void setLinearVelocity(const vec3 &v);
	void setAngularAcceleration(const vec3 &alpha);
	void setLinearAcceleration(const vec3 &a);
	void addAngularVelocity(const vec3 &omega);
	void addLinearVelocity(const vec3 &v);
	void addAngularAcceleration(const vec3 &alpha);
	void addLinearAcceleration(const vec3 &a);

	void approximateInertiaTensor(vec3 p);
	void approximateInertiaTensorCM();
	void calculateCenterOfMass();

	vec3 getCm() const {return centerOfMass;}
	mat3 getI() const {return I;}
};

class RollingBody {
	SmoothParametricCurve boundary; // polar b(phi)
	SmoothParametricCurve floor; // arclen _f(s)
	vec3 gravity;
	vec3 centerOfMass;
	float angularVelocity = 0;
	float distanceTravelled=0;
	float polarParam=0;
	float I_center;

	PolyGroupID boundaryID;
	PolyGroupID floorID;
	PolyGroupID centerID;

public:
	std::shared_ptr<IndexedMesh> mesh;
	std::shared_ptr<IndexedMesh> floormesh;
	std::shared_ptr<IndexedMesh> centermesh;

	RollingBody(std::shared_ptr<IndexedMesh> mesh, SmoothParametricCurve boundary, SmoothParametricCurve floor, vec3 gravity);
	RollingBody(SmoothParametricCurve boundary, SmoothParametricCurve floor, vec3 polarConeCenter, vec3 gravity, int n, int m, float pipe_r);

	float rollingPathLen(float phi);
	void roll(float dt);
	float calculate_angularAcc();
	void update_omega(float dt);
	float I_zz(vec3 p);
	float I_0();
	vec3 contactPoint() {return floor(distanceTravelled);}
	vec3 r_contact() {return centerOfMass-contactPoint();}
	void shift(vec3 v);
	void rotate(float angle);
	void step(float dt);
	float rotationAngle(float d1, float d2);

	void shift(vec3 v, IndexedMesh &mesh, IndexedMesh &centermesh);
	void rotate(float angle, IndexedMesh &mesh, IndexedMesh &centermesh);
	void step(float dt, IndexedMesh &mesh, IndexedMesh &centermesh);
	void roll(float dt, IndexedMesh &mesh, IndexedMesh &centermesh);


	vec3 getCM() {return centerOfMass;}
	float getAngularVelocity();
	std::shared_ptr<IndexedMesh> getMesh();
	std::shared_ptr<IndexedMesh> getCenterMesh();
	std::shared_ptr<IndexedMesh> getFloorMesh();

};


class RigidBody3D {
	vec3 centerOfMass = vec3(0);
	mat3 I = mat3(0);
	vec3 angularVelocity;
	vec3 linearVelocityCM;
	vec3 angularAcceleration;
	vec3 linearAccelerationCM;
	float mass;
	mat3 R;

public:
	RigidBody3D(vec3 cm, mat3 I, vec3 angularVelocity,
		vec3 linearVelocity,  vec3 angularAcceleration,  vec3 linearAcceleration, float mass, mat3 R);

	void rotateAroundCM(mat3 M, IndexedMesh &mesh);
	void rotate(mat3 M, vec3 p, IndexedMesh &mesh);
	vec3 angularMomentum(vec3 rotationCenter);
	float  kineticEnergy(vec3 rotationCenter);
	mat3 I_p(vec3 p);
	mat3 spin();
	mat3 dRdt() {return spin()*R;}
	mat3 I_rotated(const mat3 &R) {return R*I*transpose(R);}
	vec3 alpha();
	void freeMotion(float dt, IndexedMesh &mesh);
	void shift(vec3 v, IndexedMesh &mesh);
	vec3 angularMomentum() {return I*angularVelocity;}

	vec3 getCM() const {return centerOfMass;}
};

std::pair<RigidBody3D, IndexedMesh> boxRigid(vec3 size, vec3 center, float mass, vec3 velocity, vec3 angularVelocity, vec3 angularAcceleration, vec3 linearAcceleration, const mat3 &R);
