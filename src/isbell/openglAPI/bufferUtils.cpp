#include "bufferUtils.hpp"

using namespace openglAPI;


ShaderStorageBufferObject::ShaderStorageBufferObject(size_t bufferSize, const void *data) {
	this->bufferSize = bufferSize;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, data, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
}

void ShaderStorageBufferObject::bind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
}

void ShaderStorageBufferObject::unbind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBufferObject::readData(void *destination) const {
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (GLsizeiptr)bufferSize, destination);
}
