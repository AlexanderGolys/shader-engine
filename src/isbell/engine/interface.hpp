#pragma once
#include "dynamicalSystems.hpp"
#include "glslUtils.hpp"
#include "indexedRendering.hpp"
#include "specific.hpp"



enum SIM_STAGE {
	INITIALIZING,
	PRECOMPUTING,
	PLAYING,
	PAUSED
};






class BackStageInterface : public Renderer {
public:
	using Renderer::Renderer;
	shared_ptr<DynamicalInterface> dynamicalSystem = nullptr;
	SIM_STAGE stage = INITIALIZING;

	void connect_system(const ShaderProgram& shader, const shared_ptr<DynamicalInterface> &system, const shared_ptr<MaterialPhong> &material);

	void pause();
	void play();
	void reset();
};
