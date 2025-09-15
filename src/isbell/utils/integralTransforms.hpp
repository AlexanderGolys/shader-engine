#pragma once

#include "func.hpp"


RealFunction dirichlet_kernel(int n, float L=TAU);

RealFunction fejer_kernel(int n, float L=TAU);

CompactlySupportedRealFunction sine_component(int n, float L=TAU);

CompactlySupportedRealFunction cosine_component(int n, float L=TAU);

class FourierSeriesR1 {
	float target_approx_error;
	int max_terms;
	float norm_f;
	float realised_norm = 0;
	float x_min, x_max;
	vec2 dom;
	bool even, odd;
	int prec;
	vector<float> a_n = {};
	vector<float> b_n = {0};
	RealFunction _f;

	float compute_a_n(const RealFunction &f, int n) const;
	float compute_b_n(const RealFunction &f, int n) const;
	Complex compute_c_n(const RealFunction &f, int n) const;
	void add_term();
	float explained_norm() const;
	void compute();

public:
	float L;
	explicit FourierSeriesR1(const RealFunction &f, vec2 domain=vec2(-PI, PI), float target_approx_error=.01f, int max_terms=100, int prec=30, bool even=false, bool odd=false);
	Complex c(int n) const;
	float a(int n) const;
	float b(int n) const;
	float realised_norm_ratio();
	float computed_terms();
	float A(int n) const;
	float phi(int n) const {return c(n).arg();}
};




class IntegralTransform {
protected:
	vec2 integration_domain;
	int prec;
public:
	IntegralTransform():integration_domain(0, 1), prec(100){}
	IntegralTransform(float a, float b, int prec) :integration_domain(a, b), prec(prec){}
	IntegralTransform(vec2 dom, int prec) :integration_domain(dom), prec(prec){}
	IntegralTransform(float L, int prec) :integration_domain(-L, L), prec(prec){}
	virtual ~IntegralTransform() = default;


	virtual ComplexValuedFunction operator()(RealFunction f) const = 0;
	virtual RealFunction inv(ComplexValuedFunction f) const = 0;
	virtual RealFunction inv(RealFunction f) const = 0;
	// virtual RealFunctionR2 operator()(RealFunctionR2 _f, int var);
	// virtual RealFunctionR2 inv(RealFunctionR2 _f, int var);
};

class STFT {
	RealFunction kernel;
	float L;
public:
	explicit STFT(const RealFunction &kernel, float L=2, int prec=100): kernel(kernel), L(L) {}
	virtual ~STFT() = default;

	RealFunctionR2 phase(RealFunction f) const;
	RealFunctionR2 spectrogram(RealFunction f) const;
	RealFunctionR2 magnitude(const RealFunction &f) const { return spectrogram(f); }
	RealFunctionR2 spectral_power_density(const RealFunction &f) const { return spectrogram(f); }
	RealFunction inverse(RealFunctionR2 spectral_power, RealFunctionR2 phase) const;
};



class FourierTransform : public IntegralTransform {
public:
	FourierTransform(float L, int prec):IntegralTransform(-L, L, prec){}
	FourierTransform():FourierTransform(20, 100){}

	ComplexValuedFunction operator()(RealFunction f) const override;
	RealFunction inv(ComplexValuedFunction f) const override;
	RealFunction inv(RealFunction f) const override;
	// RealFunctionR2 operator()(RealFunctionR2 _f, int var) override;
	// RealFunctionR2 inv(RealFunctionR2 _f, int var) override;
};




class DiscreteTransform {
protected:
	int n_min, n_max, prec, N;
	float dt;

public:
	DiscreteTransform(int n_min, int n_max, int prec, float dt);
	DiscreteTransform();
	DiscreteTransform(int n, int prec, float dt, bool negatives=false);
	virtual ~DiscreteTransform() = default;

	virtual FiniteSequence<Complex> operator()(FiniteSequence<Complex> fn) const = 0;
	virtual FiniteSequence<Complex> inv(FiniteSequence<Complex> fn) const = 0;
	virtual FiniteSequence<Complex> operator()(RealFunction fn) const = 0;
	virtual RealFunction inv_cnt(FiniteSequence<Complex> fn) const = 0;

	FiniteSequence<Complex> operator()(const FiniteSequence<float> &fn) const;
};


class DFT : public DiscreteTransform {
public:
	explicit DFT(int N=1, float dt=1) : DiscreteTransform(-(N-N%2)/2, (N-1-N%2)/2, 100, dt) {}
	FiniteSequence<Complex> operator()(FiniteSequence<Complex> fn) const override;
	FiniteSequence<Complex> inv(FiniteSequence<Complex> fn) const override;
	FiniteSequence<Complex> operator()(RealFunction fn) const override;
	RealFunction inv_cnt(FiniteSequence<Complex> fn) const override;
	FiniteSequence<float> frequencies() const;
};


class DiscreteGaborTransform {
	DiscreteRealFunction shroom_kernel;
	int step;
public:
	explicit DiscreteGaborTransform(const DiscreteRealFunction &shroom_kernel, int step=1) : shroom_kernel(shroom_kernel), step(step) {}
	explicit DiscreteGaborTransform(int k, int step=1) : DiscreteGaborTransform(DiscreteRealFunction(EXP_R&(-X_R*X_R*PI), vec2(-1.91, 1.91), k), step) {}

	int window_size() const { return shroom_kernel.samples(); }

	DiscreteComplexFunctionR2 transform(const DiscreteRealFunction &f) const {
		vector<DiscreteComplexFunction> res;
		int win_size = window_size();
		auto padded = f.two_sided_zero_padding(f.samples() + win_size);
		for (int i = 0; i < padded.samples()-win_size; i+=step) {
			auto piece = (padded.slice(i, i+win_size)*shroom_kernel).fft();
			piece.setDom(-1, 1);
			// for (int j = 0; j < step; j++)
			res.push_back(piece);
		}
		return DiscreteComplexFunctionR2(res, f.getDomain());
	}

	DiscreteComplexFunctionR2 transform_print(const DiscreteRealFunction &f) const {
		vector<DiscreteComplexFunction> res;
		int win_size = window_size();
		auto padded = f.two_sided_zero_padding(f.samples() + win_size);
		for (int i = 0; i < padded.samples()-win_size; i+=step) {
			auto piece = (padded.slice(i, i+win_size)*shroom_kernel).fft();
			piece.setDom(-1, 1);
			res.push_back(piece);
			if (i % ((padded.samples()-win_size)/20) < step)
				std::cout << "progress: " << (int)100.f*(i+1)/(padded.samples()-win_size) << "%" << std::endl;

		}
		return DiscreteComplexFunctionR2(res, f.getDomain());
	}
};


class MultiplierOperator {
	virtual DiscreteComplexFunction kernel(int n) const = 0;
public:
	virtual ~MultiplierOperator() = default;
	DiscreteComplexFunction operator()(const DiscreteComplexFunction &f) const {
		return (f.fft() * kernel(f.samples())).ifft();
	}
	DiscreteRealFunction operator()(const DiscreteRealFunction &f) const {
		return (f.fft() * kernel(f.samples())).ifft().re();
	}
	// DiscreteComplexFunctionR2 operator()(const DiscreteComplexFunctionR2 &_f, int var) const {
	// 	return (_f.fft(var) * kernel(
	// }
	// TODO: All operations that mix two-variable discrete functions with one variable need some systematic implementation. In particular convolution on one component, multiplication on one component.
};


class higherDerivativeMultiplier : public MultiplierOperator {
	int ord;
public:
	explicit higherDerivativeMultiplier(int ord) : ord(ord) {}
	DiscreteComplexFunction kernel(int n) const override {
		return DiscreteComplexFunction(Vector<Complex>(n, [n=n, ord=ord](int k) {
		return pow(1.0i*(TAU/n)*k, ord) ;
		}), vec2(-1, 1));
	}
};



class MeanValueMultiplier : public MultiplierOperator {
public:
	MeanValueMultiplier(){}

	DiscreteComplexFunction kernel(int n) const override {
		return DiscreteComplexFunction(Vector<Complex>(n, [](int k) {
			return Complex(k==0);
		}), vec2(-1, 1));
	}
};
