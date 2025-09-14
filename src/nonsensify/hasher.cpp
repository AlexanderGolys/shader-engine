#include "hasher.hpp"

Hasher::Hasher(size_t max_buffer_size, shared_ptr<openglAPI::ComputeShaderProgram> computeShaderProgram)
: max_buffer_size(max_buffer_size), shader(std::move(computeShaderProgram)) {}

void Hasher::hashFile(FileDescriptor &file) {
	START_TIMER("hashing");

	int groups_nb = 32;
	size_t old_size = file.getSize();
	size_t size_left = old_size;
	size_t padding_bytes = groups_nb * sizeof(GLint) / sizeof(char);

	if (size_left % padding_bytes != 0){
		size_left += padding_bytes - size_left % padding_bytes;
		file.resize(size_left);
		LOG(format("File size increased from {} to {} bytes to be multiple of {}", old_size, size_left, padding_bytes));
	}
	file.mapFile();
	void* address = file.getAddress();
	size_t bytes_shift = 0;
	shader->use();

	while (size_left > 0) {
		size_t chunk_size = std::min(max_buffer_size, size_left);
		void* src = (char*)address + bytes_shift;
		openglAPI::ShaderStorageBufferObject ssbo(chunk_size, src);
		ssbo.bind();
		shader->run(chunk_size/padding_bytes);
		ssbo.readData(src);
		ssbo.unbind();

		bytes_shift += chunk_size;
		size_left -= chunk_size;
	}
	file.flush();
	file.closeFile();

	if (old_size != file.getSize()) {
		file.resize(old_size);
		LOG(format("File size restored to original value of {} bytes", old_size));
	}
	auto time = STOP_TIMER("hashing");
	LOG(format("File hashed in {} ms", time));
}
