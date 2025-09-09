#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../utils/file-management/filesUtils.hpp"

namespace openglAPI {

	class ShaderStorageBufferObject {
		GLuint ssbo = 0;
		size_t bufferSize;

	public:
		ShaderStorageBufferObject(size_t bufferSize, const void* data);
		void bind();
		void unbind();
		void readData(void* destination) const;
	};
}
