#pragma once
#include "smoothImplicit.hpp"
#include "../engine/specific.hpp"

class SmoothingKernel {
	float d;
	BIHOM(vec3, float, float) F;
	BIHOM(vec3, float, vec3) DF;
public:
	SmoothingKernel(float radius_of_influence, BIHOM(vec3, float, float) F, BIHOM(vec3, float, vec3) DF);
	virtual ~SmoothingKernel() = default;
	float radius_of_influence() const { return d; }

	virtual float operator()(vec3 p) const { return F(p, d); }
	virtual vec3 grad(vec3 p) const { return DF(p, d); }
};


class Poly6Kernel : public SmoothingKernel {
public:
	explicit Poly6Kernel(float radius_of_influence);
};


class FluidParticle {
	vec3 position;
	vec3 velocity;
	vec3 acceleration;
	vec3 forces = vec3(0.0f);
	vector<float> attributes;
	float mass;
	RP1 density = UNDEFINED;
	RP1 pressure = UNDEFINED;
	float viscosity;

public:
	FluidParticle() = default;
	FluidParticle(vec3 position, float mass, float viscosity, vector<float> attributes = {});
	FluidParticle(vec3 position, vec3 velocity, float mass, float viscosity, vector<float> attributes = {});
	FluidParticle(vec3 position, vec3 velocity, vec3 acceleration, float mass, float viscosity, vector<float> attributes = {});

	vec3 pos() const { return position; }
	vec3 v() const { return velocity; }
	vec3 a() const { return acceleration; }
	vec3 f() const { return forces; }

	float get_attribute(int i) const { return attributes[i]; }
	float m() const { return mass; }
	float rho() const { return density.value_or(0.0f); }
	float p() const { return pressure.value_or(0.0f); }
	float mu() const { return viscosity; }

	void set_forces(vec3 f) { forces = f; }
	void add_force(vec3 f) { forces += f; }
	void set_density(float rho) { density = rho; }
	void set_pressure(float p) { pressure = p; }
	void set_attribute(int i, float value) { attributes[i] = value; }
	void set_velocity(vec3 v) { velocity = v; }
	void set_position(vec3 p) { position = p; }
	void set_acceleration(vec3 a) { acceleration = a; }

	void update(float dt);
};


struct SPH_SETTINGS {;
	float rest_density;
	float stiffness;
	float compressibility;
	float viscosity;
};


class FluidParticleSystem {
	vector<FluidParticle> particles;
	HOM(vec3, vec3) gravity_field;
	ImplicitVolume bounding_volume;
	vec3 bound_min, bound_max;
	float d;
	SmoothingKernel smoothing_kernel;
	ivec3 grid_size;
	int no_particles;
	SPH_SETTINGS params;

	vector<FluidParticle> fluid_particles;
	std::map<string, std::set<int>> particle_chunks;

	static string ivec3key(const ivec3 &v) {
		return std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z);
	}

public:
	FluidParticleSystem(const vector<FluidParticle> &particles, const HOM(vec3, vec3) &gravity_field, const ImplicitVolume &boundary, SmoothingKernel smoothing_kernel, SPH_SETTINGS params);
	ivec3 chunk(vec3 p) const { return ivec3((p - bound_min) / d); }
	vector<ivec3> relevant_chunks(vec3 p) const;

	float density(vec3 x) const;
	void calculate_densities();
	void calculate_pressures();
	void calculate_forces();

	void update(float dt);

	SmoothImplicitSurface density_surface(float level) const;
	IndexedMesh particle_mesh(float r, int icosphere_res) const;
	MarchingCubeChunk boundary_surface_march(const ivec3 &res) const;
	MarchingCubeChunk free_surface_march(const ivec3 &res, float level) const;

	FluidParticle &operator[](int i) { return particles[i]; }
};
