#include "integralTransforms.hpp"

RealFunction dirichlet_kernel(int n, float L)  {
	return RealFunction([n, L](float x) {
		if (x == 0) return 2.f*n+1;
		return std::sin((2*n+1)*PI*x/L)/std::sin(PI*x/L);	}, 0.0001, Regularity::SMOOTH);
}

RealFunction fejer_kernel(int n, float L) {
	return RealFunction([n, L](float x) {
		if (x == 0) return 1.f*n;
		return (1.f-(float)std::cos(x*n*TAU/L))/((1.f-std::cos(TAU*x/L))*n);	}, 0.0001f, Regularity::SMOOTH);
}

CompactlySupportedRealFunction sine_component(int n, float L) {
	auto s = RealFunction(
		[n, L](float x) { return std::sin(PI*n*x/L);	},
		[n, L](float x) { return PI*n/L*std::cos(PI*n*x/L);	},
		[n, L](float x) { return -sq(PI*n/L)*std::sin(PI*n*x/L);	},
		0.0001, Regularity::SMOOTH);
	return CompactlySupportedRealFunction(s, vec2(0, L));
}

CompactlySupportedRealFunction cosine_component(int n, float L) {
	auto s = RealFunction(
		[n, L](float x) { return std::cos(PI*n*x/L);	},
		[n, L](float x) { return -PI*n/L*std::sin(PI*n*x/L);	},
		[n, L](float x) { return -sq(PI*n/L)*std::cos(PI*n*x/L);	},
		0.0001, Regularity::SMOOTH);
	return CompactlySupportedRealFunction(s, vec2(-L/2, L/2));
}

float FourierSeriesR1::compute_a_n(const RealFunction &f, int n) const {
	if (n<0) throw std::invalid_argument("n must be non-negative");
	if (odd) return 0;
	if (n == 0) return f.integral(x_min, x_max, prec)/L;
	return f.L2_product(cosine_component(n, L), dom, prec)*2.f/L;
}

float FourierSeriesR1::compute_b_n(const RealFunction &f, int n) const {
	if (n<=0) throw std::invalid_argument("n must be positive.");
	if (even) return 0;
	return f.L2_product(sine_component(n, L), dom, prec)*2.f/L;
}

Complex FourierSeriesR1::compute_c_n(const RealFunction &f, int n) const {
	if (n == 0) return Complex(f.integral(x_min, x_max, prec)/L, 0);
	if (n<0) return Complex(compute_a_n(f, -n), compute_b_n(f, -n))/2.f;
	return Complex(compute_a_n(f, n), -compute_b_n(f, n))/2.f;
}

void FourierSeriesR1::add_term() {
	int n = a_n.size();
	a_n.push_back(compute_a_n(_f, n));
	if (n>0) b_n.push_back(compute_b_n(_f, n));
}

float FourierSeriesR1::explained_norm() const {
	return realised_norm/norm_f;
}

void FourierSeriesR1::compute() {
	while (a_n.size() < max_terms) {
		add_term();
	}
}

FourierSeriesR1::FourierSeriesR1(const RealFunction &f, vec2 domain, float target_approx_error, int max_terms, int prec, bool even, bool odd)
: _f(f), dom(domain), target_approx_error(target_approx_error), max_terms(max_terms), even(even), odd(odd), prec(prec) {
	x_min = domain[0];
	x_max = domain[1];
	L = x_max - x_min;
	norm_f = f.L2_norm(domain, prec);
	compute();
}

Complex FourierSeriesR1::c(int n) const {
	return Complex(a(n), b(n));
}

float FourierSeriesR1::a(int n) const {
	return a_n[n];
}

float FourierSeriesR1::b(int n) const {
	return b_n[n];
}

float FourierSeriesR1::A(int n) const {
	return norm(vec2(a(n), b(n)));
}




RealFunctionR2 STFT::phase(RealFunction f) const {
	return RealFunctionR2([this, f](float tau, float omega) {
		return ((kernel & (X_R - tau)) * f * EXP_it & (-omega)).integral(-L, L, 100).arg();
	}, 0.0001f, Regularity::SMOOTH);
}

RealFunctionR2 STFT::spectrogram(RealFunction f) const {
	return RealFunctionR2([this, f](float tau, float omega) {
		return abs(((kernel & (X_R - tau)) * f * EXP_it & (-omega)).integral(-L, L, 100));
	}, 0.0001f, Regularity::SMOOTH);
}

RealFunction STFT::inverse(RealFunctionR2 spectral_power, RealFunctionR2 phase) const {

	return RealFunction([this, spectral_power, phase](float t) {
		return abs(ComplexValuedFunction( [this, t, spectral_power, phase](float tau) {
			ComplexValuedFunction f = ComplexValuedFunction(spectral_power.partially_evaulate(0, tau), phase.partially_evaulate(0, tau), false);
			return (f * kernel(t - tau) * EXP_it(t) ).integral(-L, L, 100);
		}).integral(-L, L, 100));
	}, 0.0001f, Regularity::SMOOTH);
}

ComplexValuedFunction FourierTransform::operator()(RealFunction f) const {
	return ComplexValuedFunction([f=f, L=integration_domain.x, p=prec](float omega) {
		ComplexValuedFunction G = ComplexValuedFunction([omega, ff=f](float t) {
			return exp(Complex(0, -omega*t))*ff(t);
		});
		return G.integral(-L, L, p)/TAU;
	});
}

RealFunction FourierTransform::inv(ComplexValuedFunction f) const {
	return ComplexValuedFunction([f=f, L=integration_domain.x, p=prec](float omega) {
		ComplexValuedFunction G = ComplexValuedFunction([omega, ff=f](float t) {
			return exp(Complex(0, omega*t))*ff(t);
		});
		return G.integral(-L, L, p);
	}).re();
}

RealFunction FourierTransform::inv(RealFunction f) const {
	return inv(ComplexValuedFunction(f));
}

DiscreteTransform::DiscreteTransform(int n_min, int n_max, int prec, float dt)
: n_min(n_min), n_max(n_max), prec(prec), dt(dt) {
	N = n_max - n_min + 1;
}

DiscreteTransform::DiscreteTransform()
: DiscreteTransform(0, 0, 100, 1) {}

DiscreteTransform::DiscreteTransform(int n, int prec, float dt, bool negatives)
: DiscreteTransform(negatives ? -n : 0, n, prec, dt) {}

FiniteSequence<Complex> DiscreteTransform::operator()(const FiniteSequence<float> &fn) const {
	return operator()(fn.base_change<Complex>());
}

FiniteSequence<Complex> DFT::operator()(FiniteSequence<Complex> fn) const {
	FiniteSequence<Complex> res = FiniteSequence<Complex>();
	for (int k = n_min; k <= n_max; ++k) {
		Complex sum = Complex(0);
		for (int n = n_min; n <= n_max; ++n)
			sum += fn[n] * exp(-TAU*1.0i*k*n/N);
		res.set(k, sum);
	}
	return res;
}

FiniteSequence<Complex> DFT::inv(FiniteSequence<Complex> fn) const {
	FiniteSequence<Complex> res = FiniteSequence<Complex>();
	for (int k = n_min; k <= n_max; ++k) {
		Complex sum = Complex(0);
		for (int n = n_min; n <= n_max; ++n)
			sum += fn[n] * exp(TAU*1.0i*k*n/N);
		res.set(k, sum);
	}
	return res/Complex(N);
}

FiniteSequence<Complex> DFT::operator()(RealFunction f) const {
	FiniteSequence<Complex> res = FiniteSequence<Complex>();
	for (int k = n_min; k <= n_max; ++k) {
		Complex sum = Complex(0);
		for (int n = n_min; n <= n_max; ++n)
			sum +=  ((EXP_it & -TAU*k*n/N)*f).integral(n_min*dt, n_max*dt, prec);
		res.set(k, sum);
	}
	return res;
}

RealFunction DFT::inv_cnt(FiniteSequence<Complex> fn) const {
	ComplexValuedFunction res = ComplexValuedFunction(0.f);
		auto freq = frequencies();
		for (int k = n_min; k <= n_max; ++k)
			res = res + EXP_it(TAU*freq[k])*fn[k];
		return res.re()/N;
}

FiniteSequence<float> DFT::frequencies() const {
	auto fr = FiniteSequence<float>();
	for (int k = n_min; k <= n_max; ++k) {
		fr.set(k, k/N/dt);
	}
	return fr;
}



// RealFunctionR2 FourierTransform::operator()(RealFunctionR2 _f, int var) {
// 	throw std::logic_error("Not implemented");
// }
//
// RealFunctionR2 FourierTransform::inv(RealFunctionR2 _f, int var) {
// 	throw std::logic_error("Not implemented");
// }
