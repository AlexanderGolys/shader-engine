#include "computeShaders.hpp"

using namespace openglAPI;


ComputeShaderProgram::ComputeShaderProgram(const shared_ptr<Shader> &computeShader): ShaderProgram(), computeShader(computeShader) {
	shaderType = COMPUTE;
	computeShader->compile();
	linkComputeShader();
}

void ComputeShaderProgram::linkComputeShader() {
	programID = glCreateProgram();
	glAttachShader(programID, computeShader->getID());
	glLinkProgram(programID);
}
