//
// Created by PC on 22.03.2025.
//

#include "dynamicalSystems.hpp"

AreaUnderWave::AreaUnderWave(RealFunctionR2 f, vec2 dom, int sampling_res, float h): f(std::move(f)), id(randomID()) {
	vector<Vertex> vertices = {};
	vector<ivec3> faceIndices = {};
	float x0 = dom.x;
	float x1 = dom.y;
	for (int i = 0; i < sampling_res; i++) {
		float x = lerp(x0, x1, 1.f * i / (sampling_res - 1));
		float y = this->f(x, 0);
		vertices.emplace_back(vec3(x, 0, h), vec2(0, 0), vec3(0, 0, 1));
		vertices.emplace_back(vec3(x, y, h), vec2(1, 1), vec3(0, 0, 1));
		if (i > 0) {
			int last_ind = vertices.size() - 1;
			faceIndices.emplace_back(last_ind, last_ind - 3, last_ind - 2);
			faceIndices.emplace_back(last_ind, last_ind - 1, last_ind - 3);
		}
	}
	addNewPolygroup(vertices, faceIndices, id);
}

void AreaUnderWave::update(float t) {
	this->t = t;
	auto deformation = [&, T=t](BufferedVertex &v) {
		if (v.getUV().x > .5) {
			float x = v.getPosition().x;
			float y = f(x, T);
			v.setPosition(vec3(x, y, v.getPosition().z));
		}
	};
	deformPerVertex(id, deformation);
}

MovingCurve::MovingCurve(RealFunctionR2 f, vec2 dom, int sampling_res, float h, float r, int radial_res)
: f(std::move(f)), id(randomID()), r(r), h(h), radial_res(radial_res), sampling_res(sampling_res)
{
	vector<Vertex> vertices = {};
	vector<ivec3> faceIndices = {};
	float x0 = dom.x;
	float x1 = dom.y;
	for (int i = 0; i < sampling_res; i++) {
		float x = lerp(x0, x1, 1.f * i / sampling_res );
		float y = this->f(x, 0);
		vec3 n = vec3(this->f.dx(x, 0), -1, 0)/norm(vec3(this->f.dx(x, 0), -1, 0));
		vec3 x_center = vec3(x, y, h);
		for (int j = 0; j < radial_res; j++) {
			float theta = (TAU * j) / (radial_res);
			vec3 v =  n * std::sin(theta) + e3 * std::cos(theta);
			vertices.emplace_back(x_center + v * r, vec2(x, theta), v, vec4(r, h, 0, 0));

			if (i > 0) {
				int i_now = i*radial_res + j;
				int i_next = i*radial_res + (j + 1 + radial_res)%radial_res;
				faceIndices.emplace_back(i_next, i_now - radial_res, i_next-radial_res);
				faceIndices.emplace_back(i_next, i_now, i_now - radial_res );
			}
		}
	}
	addNewPolygroup(vertices, faceIndices, id);
}

void MovingCurve::update(float T) {
	this->t = time_reparam(T);
	auto deformation = [&, t=t](BufferedVertex &v) {
		vec4 color = v.getColor();
		float R = color.x;
		float H = color.y;
		float x = v.getUV().x;
		float theta = v.getUV().y;
		float y = this->f(x, t);
		vec3 n_c = vec3(this->f.dx(x, t), -1, 0)/norm(vec3(this->f.dx(x, t), -1, 0));
		vec3 x_center = vec3(x, y, H);
		vec3 n = normalise(n_c * std::sin(theta) + e3 * std::cos(theta));

		v.setPosition(x_center + n * R);
		v.setNormal(n);
	};
	deformPerVertex(id, deformation);
}


String::String(float density, float tension, float r, float l, vec2 shift,
	const RealFunction& f_x0, const RealFunction &ft_x0,
	int n_max, int prec, int radial_res, int sampling_res)
:
density(density),
tension(tension),
l(l),
shift(shift),
f_x0(f_x0),
ft_x0(ft_x0),
eq(c(), l, f_x0, ft_x0, n_max, prec),
prec(prec),
MovingCurve(RealFunctionR2(0), vec2(0, l), sampling_res, 0, r, radial_res)
{
	f = eq.solution();
}

void String::set_l(float l1) {
	r *= std::sqrt(l/l1);
	l = l1;
	eq.set_l(l1);
	eq.set_c(c());
	f = eq.solution();
}

void String::set_tension(float T) {
	tension = T;
	eq.set_c(c());
	f = eq.solution();
}

void String::update(float T) {
	this->t = time_reparam(T);
	vec3 sh = vec3(this->shift.x, this->shift.y, 0);
	auto deformation = [&, t=t, sh](BufferedVertex &v) {
		vec4 color = v.getColor();
		float R = color.x;
		float H = color.y;
		float x = v.getUV().x;
		float theta = v.getUV().y;
		float y = this->f(x, t);
		vec3 n_c = vec3(this->f.dx(x, t), -1, 0)/norm(vec3(this->f.dx(x, t), -1, 0));
		vec3 x_center = vec3(x, y, H);
		vec3 n = normalise(n_c * std::sin(theta) + e3 * std::cos(theta));

		v.setPosition(x_center + n * R + sh);
		v.setNormal(n);
		v.setColor(get_params());
	};
	deformPerVertex(id, deformation);
}

DevelopingSurface::DevelopingSurface(RealFunctionR2 F, vec2 dom, int sampling_res, float h, float r, int radial_res, float T_max)
: MovingCurve(F, dom, sampling_res, h, r, radial_res), T_max(T_max),
s([f=f](float x, float t) {
	return vec3(x, t, f(x, t)); }, dom, vec2(0, T_max))
{
	addUniformSurface(s, sampling_res, sampling_res, surfaceID);
}
