#include "../nonsensify/hasher.hpp"

#include "../utils/logging.hpp"

int main() {
	openglAPI::openGL_API_init();
	FileDescriptor file = FileDescriptor("C:\\Users\\PC\\Downloads\\nazi.jpg");
	LOG(format("File {} (size: {} kB, path {})", file.getFilename(), file.getSize()/1024, file.getPath()));

	auto shader = make_shared<openglAPI::Shader>(CodeFileDescriptor("C:\\Users\\PC\\Desktop\\ogl-master\\src\\shaders\\compute\\base.comp", false));
	LOG(format("Shader loaded ({} chars)", shader->getCode().size()));

	auto program = make_shared<openglAPI::ComputeShaderProgram>(shader);
	LOG("Shader compiled");

	auto hasher = Hasher(1024*1024, program);
	LOG("Hasher initialised");

	hasher.hashFile(file);

}
