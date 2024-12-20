#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

using namespace std;

class GLShaderProgram {
	GLuint program;

	public:
		GLShaderProgram(const string& vertexShader, const string& fragmentShader);
		~GLShaderProgram();

		void use();
		void setUniformf(const string& name, float value);
		void setUniformMat4(const string& name, glm::mat4& mat);
		void setUniform3f(const string& name, float v0, float v1, float v2);
		void setUniform3fv(const string& name, const float* value);
		void setUniformb(const string& name, bool value);

		static glm::mat4 orthoTransform;
		static glm::mat4 perspective;
		static void initialize();
};