#pragma once
#include "openglRenderer.hpp"

namespace openglAPI {

	class ComputeShaderProgram : public ShaderProgram {
		shared_ptr<Shader> computeShader;
	public:
		ComputeShaderProgram(const shared_ptr<Shader> &computeShader);
		void linkComputeShader();

		void run(int numGroupsX, int numGroupsY=1, int numGroupsZ=1) {
			use();
			glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			glFinish();
		}

	};

}
