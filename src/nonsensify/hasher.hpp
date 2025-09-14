#pragma once
#include "../openglAPI/computeShaders.hpp"
#include "../utils/file-management/filesUtils.hpp"
#include "fileExtension.hpp"





class Hasher {
	size_t max_buffer_size;
	shared_ptr<openglAPI::ComputeShaderProgram> shader;

	public:
		Hasher(size_t max_buffer_size, shared_ptr<openglAPI::ComputeShaderProgram> computeShaderProgram);
		void hashFile(FileDescriptor &file);

};
