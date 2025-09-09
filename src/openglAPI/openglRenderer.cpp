
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ranges>
#include <sstream>

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include "src/utils/exceptions.hpp"
#include "openglRenderer.hpp"

using namespace glm;

namespace openglAPI {
	ivec2 getResolutionSize(Resolution resolution) {
		switch (resolution) {
			case FHD:
				return ivec2(1920, 1080);
			case HD2K:
				return ivec2(2560, 1440);
			case UHD:
				return ivec2(3840, 2160);
			case UNKNOWN:
				throw UnknownVariantError("Cannot get size of UNKNOWN resolution", __FILE__, __LINE__);
			default:
				throw UnknownVariantError("Unknown resolution enum value", __FILE__, __LINE__);
		}
	}
	void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);
	}

	void openGL_API_init() {
		logging::Logger::init();
		if (!glfwInit()) {
			LOG_ERROR("Failed to initialize GLFW");
			throw SystemError("Failed to initialize GLFW", __FILE__, __LINE__);
		}
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		int width = 100, height = 100;
		auto w = glfwCreateWindow(width, height, "dupa", nullptr, nullptr);

		glfwMakeContextCurrent(w);
		glfwGetFramebufferSize(w, &width, &height);


		glfwSetErrorCallback([](int error, const char* description) {
			std::cerr << "GLFW Error " << error << ": " << description << std::endl;
		});

		glewExperimental = true;
		if (glewInit() != GLEW_OK) {
			LOG_ERROR("Failed to initialize GLEW");
			throw SystemError("Failed to initialize GLEW", __FILE__, __LINE__);
		}
		LOG("OpenGL renderer initialized: " + string((const char *)glGetString(GL_RENDERER)));

	}

	size_t sizeOfGLSLType(GLSLType type) {
		switch (type) {
			case BOOL:
				return sizeof(bool);
			case FLOAT:
				return sizeof(float);
			case INT:
				return sizeof(int);
			case VEC2:
				return sizeof(vec2);
			case VEC3:
				return sizeof(vec3);
			case VEC4:
				return sizeof(vec4);
			case MAT2:
				return sizeof(mat2);
			case MAT3:
				return sizeof(mat3);
			case MAT4:
				return sizeof(mat4);
			case SAMPLER1D:
			case SAMPLER2D:
			case SAMPLER3D:
				return sizeof(GLuint);
			default:
				throw UnknownVariantError("GLSL type not recognized", __FILE__, __LINE__);
		}
	}


	int lengthOfGLSLType(GLSLType type)
	{
		switch (type)
		{
			case FLOAT:
			case INT:
			case SAMPLER1D:
			case SAMPLER2D:
			case SAMPLER3D:
				return 1;
			case VEC2:
				return 2;
			case VEC3:
				return 3;
			case VEC4:
			case MAT2:
				return 4;
			case MAT3:
				return 9;
			case MAT4:
				return 16;
		}
		std::cout << "Error: unknown GLSLType" << std::endl;
		return -1;
	}

	GLenum primitiveGLSLType(GLSLType type)
	{
		switch (type)
		{
			case FLOAT:
			case VEC2:
			case VEC3:
			case VEC4:
			case MAT2:
			case MAT3:
			case MAT4:
				return GL_FLOAT;
			case INT:
			case SAMPLER1D:
			case SAMPLER2D:
			case SAMPLER3D:
				return GL_INT;
			case BOOL:
				return GL_BOOL;
			default:
				throw UnknownVariantError("GLSL type not recognized", __FILE__, __LINE__);

		}
		std::cout << "Error: unknown GLSLType" << std::endl;
		return -1;
	}


	GLenum Shader::getTypeFromExtension(const string &extension) {
		string nodot = extension;
		if (nodot[0] == '.')
			nodot = nodot.substr(1);

		if (nodot == "vert")
			return GL_VERTEX_SHADER;
		if (nodot == "frag")
			return GL_FRAGMENT_SHADER;
		if (nodot == "geom" || nodot == "geo")
			return GL_GEOMETRY_SHADER;
		if (nodot == "comp")
			return GL_COMPUTE_SHADER;
		throw SystemError("Unknown shader extension: ." + nodot, __FILE__, __LINE__);
	}


	GLuint Shader::getID() const {
		return shaderID;
	}

	void Shader::compile() {
		shaderID = glCreateShader(shaderType);
		GLint Result = GL_FALSE;
		int InfoLogLength;

		char const *sourcePtr = code.c_str();

		glShaderSource(shaderID, 1, &sourcePtr, nullptr);
		glCompileShader(shaderID);

		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			vector<char> error(InfoLogLength + 1);
			glGetShaderInfoLog(shaderID, InfoLogLength, nullptr, &error[0]);
			throw SystemError(&error[0], __FILE__, __LINE__);
		}
	}

	string Shader::getCode() const { return code; }

	Shader::Shader(const TemplateCodeFile &file): Shader(file.generatedCodeFile()) {}

	Shader::Shader(const string &code, GLenum shaderType)
	: shaderType(shaderType), code(code)
	{
		shaderID = glCreateShader(shaderType);
	}


	Shader::Shader(CodeFileDescriptor &&file) {
		shaderType = getTypeFromExtension(file.extension());
		code = file.getCode();
		shaderID = glCreateShader(shaderType);
	}

	Shader::Shader(CodeFileDescriptor &file) : Shader(file.getCode(), getTypeFromExtension(file.extension())) {}

	Shader::Shader(const Shader &other): shaderType(other.shaderType),
										 shaderID(other.shaderID),
										 code(other.code) {}

	Shader::Shader(Shader &&other) noexcept: shaderType(other.shaderType),
											 shaderID(other.shaderID),
											 code(std::move(other.code)) {}

	Shader & Shader::operator=(const Shader &other) {
		if (this == &other)
			return *this;
		shaderType = other.shaderType;
		shaderID   = other.shaderID;
		code       = other.code;
		return *this;
	}
	Shader & Shader::operator=(Shader &&other) noexcept {
		if (this == &other)
			return *this;
		shaderType = other.shaderType;
		shaderID   = other.shaderID;
		code       = std::move(other.code);
		return *this;
	}



	GLuint bindVAO()
	{
		GLuint VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
		return VertexArrayID;
	}

	void disableAttributeArrays(int how_many)
	{
		for (int i = 0; i < how_many; i++)
			glDisableVertexAttribArray(i);
	}



	mat4 generateMVP(vec3 camPosition, vec3 camLookAt, vec3 upVector, float fov, float aspectRatio, float clippingRangeMin, float clippingRangeMax, const mat4 &modelTransform)
	{
		mat4 ViewMatrix = lookAt(camPosition, camLookAt, upVector);
		mat4 ProjectionMatrix = perspective(fov, aspectRatio, clippingRangeMin, clippingRangeMax);
		return ProjectionMatrix * ViewMatrix * modelTransform;
	}




	Window::Window(int width, int height, const char *title)
	{
		if (!glfwInit())
			throw SystemError("GLFW initialization failed", __FILE__, __LINE__);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		this->width = width;
		this->height = height;
		this->aspectRatio = (float)width / (float)height;
		this->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!this->window)
		{
			glfwTerminate();
			exit(2136);
		}
		glfwMakeContextCurrent(this->window);
		glfwGetFramebufferSize(window, &width, &height);
	}

	Window::Window(Resolution resolution, const char *title)
	{
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		width = getResolutionSize(resolution).x;
		height = getResolutionSize(resolution).y;
		aspectRatio = (float)width / height;
		window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			throw SystemError("GLFW window creation failed", __FILE__, __LINE__);
		}
		glfwMakeContextCurrent(window);
	}

	Window::~Window()
	{
		destroy();
	}



	void Window::renderFramebufferToScreen()
	{
		glfwSwapBuffers(this->window);
		glfwPollEvents();
	}

	void Window::showCursor()
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void Window::disableCursor()
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void Window::hideCursorWithinWindow()
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	void Window::stickyKeys(bool sticky)
	{
		if (sticky)
			glfwSetInputMode(this->window, GLFW_STICKY_KEYS, GL_TRUE);
		else
			glfwSetInputMode(this->window, GLFW_STICKY_KEYS, GL_FALSE);
	}

	void Window::stickyMouseButtons(bool sticky)
	{
		if (sticky)
			glfwSetInputMode(this->window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
		else
			glfwSetInputMode(this->window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE);
	}

	void Window::setCallbacks(const GLFWkeyfun *keyCallback, const GLFWcharfun *charCallback, const GLFWmousebuttonfun *mouseButtonCallback, GLFWcursorposfun *cursorPosCallback, GLFWcursorenterfun *cursorEnterCallback, GLFWscrollfun *scrollCallback, GLFWdropfun *dropCallback)
	{
		if (keyCallback != nullptr)
			glfwSetKeyCallback(this->window, *keyCallback);
		if (charCallback != nullptr)
			glfwSetCharCallback(this->window, *charCallback);
		if (mouseButtonCallback != nullptr)
			glfwSetMouseButtonCallback(this->window, *mouseButtonCallback);
		if (cursorPosCallback != nullptr)
			glfwSetCursorPosCallback(this->window, *cursorPosCallback);
		if (cursorEnterCallback != nullptr)
			glfwSetCursorEnterCallback(this->window, *cursorEnterCallback);
		if (scrollCallback != nullptr)
			glfwSetScrollCallback(this->window, *scrollCallback);
		if (dropCallback != nullptr)
			glfwSetDropCallback(this->window, *dropCallback);
	}

	bool Window::isOpen()
	{
		return !glfwWindowShouldClose(this->window);
	}

	int Window::destroy() {
		glfwDestroyWindow(this->window);
		glfwTerminate();
		return 0;
	}
	void Window::initViewport() {
		glViewport(0, 0, this->width, this->height);
	}


	void ShaderProgram::linkShaders() {
		GLint Result = GL_FALSE;
		int InfoLogLength;

		vertexShader->compile();
		fragmentShader->compile();
		if (geometryShader)
			geometryShader->compile();


		GLuint VertexShaderID =  vertexShader->getID();
		GLuint FragmentShaderID =  fragmentShader->getID();
		GLuint GeometryShaderID = geometryShader ? geometryShader->getID() : 0;

		programID = glCreateProgram();

		glAttachShader(programID, VertexShaderID);
		glAttachShader(programID, FragmentShaderID);
		if (geometryShader)
			glAttachShader(programID, GeometryShaderID);

		glLinkProgram(programID);
		glGetProgramiv(programID, GL_LINK_STATUS, &Result);
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0)
		{
			vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			throw SystemError(&ProgramErrorMessage[0], __FILE__, __LINE__);
		}

		glDetachShader(programID, VertexShaderID);
		if (geometryShader) glDetachShader(programID, GeometryShaderID);
		glDetachShader(programID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
		if (geometryShader) glDeleteShader(GeometryShaderID);
		glDeleteShader(FragmentShaderID);

		shaderType = geometryShader ? GEOMETRY1 : CLASSIC;
	}


	ShaderProgram::ShaderProgram(const shared_ptr<Shader> &vertexShader, const shared_ptr<Shader> &fragmentShader, const shared_ptr<Shader> &geometryShader) :
	vertexShader(nullptr),
	fragmentShader(nullptr),
	geometryShader(nullptr)
	{
		this->vertexShader = make_shared<Shader>(*vertexShader);
		this->fragmentShader = make_shared<Shader>(*fragmentShader);
		this->geometryShader = make_shared<Shader>(*geometryShader);
		linkShaders();
	}

	ShaderProgram::ShaderProgram(const string &vertexPath, const string &fragPath) :
	vertexShader(nullptr),
	fragmentShader(nullptr),
	geometryShader(nullptr)
	{
		vertexShader = make_shared<Shader>(CodeFileDescriptor(vertexPath));
		fragmentShader = make_shared<Shader>(CodeFileDescriptor(fragPath));
		linkShaders();
	}

	ShaderProgram::ShaderProgram(const shared_ptr<Shader> &vertexShader, const shared_ptr<Shader> &fragmentShader) :
	vertexShader(nullptr),
	fragmentShader(nullptr),
	geometryShader(nullptr)
	{
		this->vertexShader = make_shared<Shader>(*vertexShader);
		this->fragmentShader = make_shared<Shader>(*fragmentShader);
		linkShaders();
	}


	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(this->programID);
	}

	void ShaderProgram::use()
	{
		glUseProgram(this->programID);
	}

	void ShaderProgram::initUniforms(const std::unordered_map<string, GLSLType> &uniforms)
	{
		for (auto uni : uniforms)
			this->uniformTypes[uni.first] = uni.second;

		for (const auto &key: uniforms | std::views::keys)
		{
			GLuint uniformLocation = glGetUniformLocation(this->programID, key.c_str());
			this->uniformLocations[key] = uniformLocation;
		}
	}

	// void ShaderProgram::initTextureSampler(const Texture *texture) {
	// 	string name = texture->samplerName;
	// 	uniformTypes[name] = SAMPLER2D;
	// 	GLuint uniformLocation = glGetUniformLocation(this->programID, texture->samplerName);
	// 	uniformLocations[name] = uniformLocation;
	// }
	//
	// void ShaderProgram::setTextureSampler(const Texture *texture) const
	// {
	// 	string name = texture->samplerName;
	// 	GLuint uniformLocation = this->uniformLocations.at(name);
	// 	glUniform1i(uniformLocation, texture->abs_slot);
	// }

	void ShaderProgram::setUniforms(const std::unordered_map<string, const GLfloat *> &uniformValues)
	{
		for (auto const &uniform : uniformValues)
			setUniform(uniform.first, uniform.second);

	}

	void ShaderProgram::setUniform(const string &uniformName, const GLfloat *uniformValue)
	{
		GLSLType uniformType = this->uniformTypes[uniformName];
		GLuint uniformLocation = this->uniformLocations[uniformName];

		switch (uniformType)
		{
			case FLOAT:
				glUniform1fv(uniformLocation, 1, uniformValue);
				break;
			case INT:
				glUniform1iv(uniformLocation, 1, (GLint *)uniformValue);
				break;
			case VEC2:
				glUniform2fv(uniformLocation, 1, uniformValue);
				break;
			case VEC3:
				glUniform3fv(uniformLocation, 1, uniformValue);
				break;
			case VEC4:
				glUniform4fv(uniformLocation, 1, uniformValue);
				break;
			case MAT2:
				glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, uniformValue);
				break;
			case MAT3:
				glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, uniformValue);
				break;
			case MAT4:
				glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, uniformValue);
				break;

			default:
				throw std::invalid_argument("Uniform type not recognized");
		}
	}

	void ShaderProgram::setUniform(const string &uniformName, float uniformValue)
	{
		if (this->uniformTypes[uniformName] != FLOAT)
			throw std::invalid_argument("Uniform type must be FLOAT");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniform1f(uniformLocation, uniformValue);
	}

	void ShaderProgram::setUniform(const string &uniformName, int uniformValue)
	{
		if (this->uniformTypes[uniformName] != INT)
			throw std::invalid_argument("Uniform type must be INT");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniform1i(uniformLocation, uniformValue);
	}

	void ShaderProgram::setUniform(const string &uniformName, vec2 uniformValue)
	{
		if (this->uniformTypes[uniformName] != VEC2)
			throw std::invalid_argument("Uniform type must be VEC2");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniform2f(uniformLocation, uniformValue.x, uniformValue.y);
	}

	void ShaderProgram::setUniform(const string &uniformName, vec3 uniformValue)
	{
		if (this->uniformTypes[uniformName] != VEC3)
			throw std::invalid_argument("Uniform type must be VEC3");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniform3f(uniformLocation, uniformValue.x, uniformValue.y, uniformValue.z);
	}

	void ShaderProgram::setUniform(const string &uniformName, vec4 uniformValue)
	{
		if (this->uniformTypes[uniformName] != VEC4)
			throw std::invalid_argument("Uniform type must be VEC4");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniform4f(uniformLocation, uniformValue.x, uniformValue.y, uniformValue.z, uniformValue.w);
	}

	void ShaderProgram::setUniform(const string &uniformName, mat2 uniformValue)
	{
		if (this->uniformTypes[uniformName] != MAT2)
			throw std::invalid_argument("Uniform type must be MAT2");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, &uniformValue[0][0]);
	}

	void ShaderProgram::setUniform(const string &uniformName, mat3 uniformValue)
	{
		if (this->uniformTypes[uniformName] != MAT3)
			throw std::invalid_argument("Uniform type must be MAT3");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &uniformValue[0][0]);
	}

	void ShaderProgram::setUniform(const string& uniformName, mat4 uniformValue)
	{
		if (this->uniformTypes[uniformName] != MAT4)
			throw std::invalid_argument("Uniform type must be MAT4");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &uniformValue[0][0]);
	}

	void ShaderProgram::setUniform(const string& uniformName, float x, float y)
	{
		if (this->uniformTypes[uniformName] != VEC2)
			throw std::invalid_argument("Uniform type must be VEC2");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniform2f(uniformLocation, x, y);
	}

	void ShaderProgram::setUniform(const string &uniformName, float x, float y, float z)
	{
		if (this->uniformTypes[uniformName] != VEC3)
			throw std::invalid_argument("Uniform type must be VEC3");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniform3f(uniformLocation, x, y, z);
	}

	void ShaderProgram::setUniform(const string &uniformName, float x, float y, float z, float w)
	{
		if (this->uniformTypes[uniformName] != VEC4)
			throw std::invalid_argument("Uniform type must be VEC4");
		GLuint uniformLocation = this->uniformLocations[uniformName];
		glUniform4f(uniformLocation, x, y, z, w);
	}





	Attribute::Attribute(const string &name, GLSLType type, int inputNumber)
	{
		this->name = name;
		this->type = type;
		this->bufferAddress = 0;
		this->size = sizeOfGLSLType(type);
		this->enabled = false;
		this->bufferInitialized = false;
		this->inputNumber = inputNumber;
		// this->bufferType = bufferType;
	}

	Attribute::~Attribute()
	{
		if (enabled) Attribute::disable();
		if (bufferInitialized) Attribute::freeBuffer();
	}

	void Attribute::initBuffer()
	{
		this->bufferInitialized = true;
		GLuint buffer;
		glGenBuffers(1, &buffer);
		this->bufferAddress = buffer;
	}

	void Attribute::enable()
	{
		glEnableVertexAttribArray(this->inputNumber);
		glBindBuffer(GL_ARRAY_BUFFER, this->bufferAddress);
		this->enabled = true;
		glVertexAttribPointer(this->inputNumber, lengthOfGLSLType(this->type), GL_FLOAT, GL_FALSE, 0, (void *)0);
	}

	void Attribute::disable()
	{
		glDisableVertexAttribArray(this->inputNumber);
		this->enabled = false;
	}

	void Attribute::load(const void *firstElementAdress, int bufferLength)
	{
		if (!bufferInitialized) {
			initBuffer();
			glBindBuffer(GL_ARRAY_BUFFER, this->bufferAddress);
			glBufferData(GL_ARRAY_BUFFER, bufferLength * this->size, firstElementAdress, GL_STATIC_DRAW);
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, this->bufferAddress);
		glBufferData(GL_ARRAY_BUFFER, bufferLength * this->size, firstElementAdress, GL_STATIC_DRAW);
		// void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		// memcpy(ptr, firstElementAdress, bufferLength * this->size);
		// glUnmapBuffer(GL_ARRAY_BUFFER);

	}

	void Attribute::freeBuffer() {
		glDeleteBuffers(1, &this->bufferAddress);
		this->bufferAddress = -1;
		this->bufferInitialized = false;
		this->enabled = false;
	}
}
