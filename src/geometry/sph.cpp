//
// Created by PC on 12.06.2025.
//

#include "sph.hpp"

SmoothingKernel::SmoothingKernel(float radius_of_influence, std::function<float(vec3, float)> F, std::function<vec3(vec3, float)> DF): d(radius_of_influence), F(F), DF(DF) {}

Poly6Kernel::Poly6Kernel(float radius_of_influence): SmoothingKernel(radius_of_influence, [](vec3 p, float d) {
																		 if (dot(p, p) > d*d) return 0.0f;
																		 float a = 315.0f / (64.0f * PI * std::pow(d, 9));
																		 return a * pow3(d*d - dot(p, p));
																	 }, [](vec3 p, float d) {
																		 if (dot(p, p) > d*d) return vec3(0.0f);
																		 float a = 315.0f / (64.0f * PI * std::pow(d, 9));
																		 return p * -6.f * a * pow2(d*d - dot(p, p));
																	 }) {}

FluidParticle::FluidParticle(vec3 position, float mass, float viscosity, vector<float> attributes)
: FluidParticle(position, vec3(0.0f), mass, viscosity, attributes) {}

FluidParticle::FluidParticle(vec3 position, vec3 velocity, float mass, float viscosity, vector<float> attributes)
: FluidParticle(position, velocity, vec3(0.0f), mass, viscosity, attributes) {}


FluidParticle::FluidParticle(vec3 position, vec3 velocity, vec3 acceleration, float mass, float viscosity, vector<float> attributes) {
	this->position = position;
	this->velocity = velocity;
	this->acceleration = acceleration;
	this->mass = mass;
	this->viscosity = viscosity;
	this->attributes = attributes;

}

void FluidParticle::update(float dt) {
	acceleration = forces / density.value_or(0.f);
	velocity += acceleration * dt;
	position += velocity * dt;
}

FluidParticleSystem::FluidParticleSystem(const vector<FluidParticle> &particles, const std::function<vec3(vec3)> &gravity_field, const ImplicitVolume &boundary, SmoothingKernel smoothing_kernel, SPH_SETTINGS params)
: particles(particles), gravity_field(gravity_field), bounding_volume(boundary), bound_min(bounding_volume.bounding_box().first), bound_max(bounding_volume.bounding_box().second),
	d(smoothing_kernel.radius_of_influence()), smoothing_kernel(smoothing_kernel), no_particles(particles.size()), params(params)
{
	grid_size = ivec3(ceil((bound_max - bound_min).x) / d,
					  ceil((bound_max - bound_min).y) / d,
					  ceil((bound_max - bound_min).z) / d);

	for (int i = 0; i < no_particles; i++) {
		vec3 p = particles[i].pos();
		ivec3 chunk = ivec3((p - bound_min) / d);

		if (particle_chunks.contains(ivec3key(chunk)))
			particle_chunks[ivec3key(chunk)].insert(i);
		else
			particle_chunks[ivec3key(chunk)] = {i};
	}
}

vector<ivec3> FluidParticleSystem::relevant_chunks(vec3 p) const {
	ivec3 current_chunk = chunk(p);
	vector<ivec3> res;
	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
			for (int z = -1; z <= 1; z++)
				if (particle_chunks.contains(ivec3key(current_chunk + ivec3(x, y, z))))
					res.push_back(current_chunk + ivec3(x, y, z));
	return res;
}

float FluidParticleSystem::density(vec3 x) const{
	float rho = 0.0f;
	for (ivec3 c : relevant_chunks(x))
		for (int i : particle_chunks.at(ivec3key(c)))
			rho += particles[i].m() * smoothing_kernel(x - particles[i].pos());

	return rho;
}

void FluidParticleSystem::calculate_densities() {
	for (int i = 0; i < no_particles; i++) {
		vec3 p = particles[i].pos();
		particles[i].set_density(density(p));
	}
}

void FluidParticleSystem::calculate_pressures() {
	for (int i = 0; i < no_particles; i++)
		particles[i].set_pressure(params.stiffness * (std::pow(particles[i].rho()/params.rest_density, params.compressibility) - 1.f));
}

void FluidParticleSystem::calculate_forces() {
	for (int i = 0; i < no_particles; i++) {
		vec3 x = particles[i].pos();
		vec3 f = gravity_field(x) * particles[i].m();
		float m = particles[i].m();
		vec3 pressure_f = vec3(0.0f);
		vec3 viscosity_f = vec3(0.0f);
		float p_over_rho2 = particles[i].p()/pow2(particles[i].rho());
		for (ivec3 c : relevant_chunks(x)) {
			for (int j : particle_chunks[ivec3key(c)]) {
				if (i == j) continue;
				auto y = particles[j].pos();
				auto m2 = particles[j].m();
				auto rho2 = particles[j].rho();
				pressure_f -= smoothing_kernel.grad(x - y) * m2 * m * (p_over_rho2 + particles[j].p()/pow2(particles[j].rho()));
				viscosity_f += smoothing_kernel.grad(x - y) * params.viscosity * 10.f * m2/rho2 * dot(x-y, particles[i].v() - particles[j].v()) / (dot(x-y, x-y) + .01f*d*d) * m;
			}
		}
		f += pressure_f + viscosity_f;
		particles[i].set_forces(f);
	}
}

void FluidParticleSystem::update(float dt) {
	calculate_densities();
	calculate_pressures();
	calculate_forces();
	for (int i = 0; i < no_particles; i++) {
		auto old_x = particles[i].pos();
		ivec3 c = chunk(old_x);
		particles[i].update(dt);
		auto new_x = particles[i].pos();
		if (!bounding_volume.contains(new_x)) {
			auto n = bounding_volume.inside_normal(old_x);
			particles[i].set_velocity(glm::reflect(particles[i].v(), n));
			particles[i].set_position(old_x + particles[i].v() * dt);
			new_x = particles[i].pos();
		}

		ivec3 new_chunk = chunk(new_x);
		if (c != new_chunk) {
			particle_chunks[ivec3key(c)].erase(i);
			if (particle_chunks[ivec3key(c)].empty())
				particle_chunks.erase(ivec3key(c));
			if (!particle_chunks.contains(ivec3key(new_chunk)))
				particle_chunks[ivec3key(new_chunk)] = {};
			particle_chunks[ivec3key(new_chunk)].insert(i);
		}
	}
}

SmoothImplicitSurface FluidParticleSystem::density_surface(float level) const {
	return SmoothImplicitSurface([this, level](vec3 p) { return density(p) - level; });
}

IndexedMesh FluidParticleSystem::particle_mesh(float r, int icosphere_res) const {
	IndexedMesh mesh = IndexedMesh();
	for (int i = 0; i < no_particles; i++) {
		vec3 x = particles[i].pos();

		// color = (pressure, density, total forces, speed)
		vec4 color = vec4(particles[i].p(), x.x, x.y, x.z);

		mesh.mergeAndKeepID(icosphere(r, icosphere_res, x, i, color));
	}
	return mesh;
}

MarchingCubeChunk FluidParticleSystem::boundary_surface_march(const ivec3 &res) const {
	return MarchingCubeChunk(bound_min, bound_max, res, make_shared<SmoothImplicitSurface>(bounding_volume.boundary_surface()));
}

MarchingCubeChunk FluidParticleSystem::free_surface_march(const ivec3 &res, float level) const {
	return MarchingCubeChunk(bound_min, bound_max, res, make_shared<SmoothImplicitSurface>(density_surface(level)));
}
