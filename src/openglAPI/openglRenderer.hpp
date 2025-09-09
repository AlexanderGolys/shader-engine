#pragma once

#include "bufferUtils.hpp"
#include "logging.hpp"
#include "macroParsing.hpp"


namespace openglAPI {
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void openGL_API_init();

	GLuint bindVAO();
	void disableAttributeArrays(int how_many=4);
	mat4 generateMVP(vec3 camPosition, vec3 camLookAt, vec3 upVector, float fov, float aspectRatio, float clippingRangeMin, float clippingRangeMax, const mat4 &modelTransform);

	enum GLSLType {
		BOOL,
		FLOAT,
		INT,
		VEC2,
		VEC3,
		VEC4,
		MAT2,
		MAT3,
		MAT4,
		SAMPLER1D,
		SAMPLER2D,
		SAMPLER3D
	};

	enum ShaderType {
		CLASSIC,
		GEOMETRY1,
		COMPUTE
	};

	enum Resolution {
		FHD,
		HD2K,
		UHD,
		UNKNOWN
	};

	ivec2 getResolutionSize(Resolution resolution);
	size_t sizeOfGLSLType(GLSLType type);
	int lengthOfGLSLType(GLSLType type);
	GLenum primitiveGLSLType(GLSLType type);


	class Window {
	public:
		GLFWwindow* window;
		int width;
		int height;
		float aspectRatio;
		Window(int width, int height, const char* title);
		Window(Resolution resolution, const char* title);
		~Window();
		int destroy();

		void initViewport();
		void showCursor();
		void disableCursor();
		void hideCursorWithinWindow();
		void stickyKeys(bool sticky);
		void stickyMouseButtons(bool sticky);
		void setCallbacks(const GLFWkeyfun* keyCallback = nullptr, const GLFWcharfun* charCallback = nullptr, const GLFWmousebuttonfun* mouseButtonCallback = nullptr, GLFWcursorposfun* cursorPosCallback = nullptr, GLFWcursorenterfun* cursorEnterCallback = nullptr, GLFWscrollfun* scrollCallback = nullptr, GLFWdropfun* dropCallback = nullptr);
		bool isOpen();

		void renderFramebufferToScreen();
	};









	class Shader {
	protected:
		GLenum shaderType;
		GLuint shaderID;
		string code = "";
	public:
		explicit Shader(CodeFileDescriptor &file);
		explicit Shader(CodeFileDescriptor &&file);
		explicit Shader(const Path &file) : Shader(CodeFileDescriptor(file)) {}
		explicit Shader(const TemplateCodeFile &file);

		Shader(const string &code, GLenum shaderType);

		Shader(const Shader &other);
		Shader(Shader &&other) noexcept;
		Shader & operator=(const Shader &other);
		Shader & operator=(Shader &&other) noexcept;
		virtual ~Shader() = default;


		GLuint getID() const;
		void compile();
		string getCode() const;

		static GLenum getTypeFromExtension(const string &extension);
	};


	class ShaderProgram {
	protected:
		shared_ptr<Shader> vertexShader;
		shared_ptr<Shader> fragmentShader;
		shared_ptr<Shader> geometryShader;
		ShaderType shaderType;

	public:
		unordered_map<string, GLuint> uniformLocations;
		unordered_map<string, GLSLType> uniformTypes;
		GLuint programID;

		ShaderProgram(const shared_ptr<Shader> &vertexShader, const shared_ptr<Shader> &fragmentShader);
		ShaderProgram(const shared_ptr<Shader> &vertexShader, const shared_ptr<Shader> &fragmentShader, const shared_ptr<Shader> &geometryShader);
		ShaderProgram(const string &vertexPath, const string &fragPath);
		ShaderProgram() = default;


		void linkShaders();

		explicit ShaderProgram(const string &standard_file_path);
		~ShaderProgram();

		void use();
		void initUniforms(const unordered_map<string, GLSLType> &uniforms);

		void setUniforms(const unordered_map<string, const GLfloat*> &uniformValues);
		void setUniform(const string &uniformName, const GLfloat* uniformValue);
		void setUniform(const string &uniformName, float uniformValue);
		void setUniform(const string &uniformName, int uniformValue);
		void setUniform(const string &uniformName, vec2 uniformValue);
		void setUniform(const string &uniformName, vec3 uniformValue);
		void setUniform(const string &uniformName, vec4 uniformValue);
		void setUniform(const string &uniformName, mat2 uniformValue);
		void setUniform(const string &uniformName, mat3 uniformValue);
		void setUniform(const string& uniformName, mat4 uniformValue);
		void setUniform(const string& uniformName, float x, float y);
		void setUniform(const string &uniformName, float x, float y, float z);
		void setUniform(const string &uniformName, float x, float y, float z, float w);
	};


	class Attribute {
	public:
		string name;
		GLuint bufferAddress;
		size_t size;
		GLSLType type;
		int inputNumber;
		bool enabled;
		bool bufferInitialized;

		Attribute(const string &name, GLSLType type, int inputNumber);
		virtual ~Attribute();

		virtual void initBuffer();
		virtual void enable();
		virtual void disable();
		virtual void load(const void* firstElementAdress, int bufferLength);
		virtual void freeBuffer();
	};
	// }
}
