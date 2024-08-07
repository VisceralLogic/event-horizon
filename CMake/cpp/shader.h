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
		void setUniformMat4(const string& name, const float* value);

		static glm::mat4 orthoTransform;
		static void initialize();
};