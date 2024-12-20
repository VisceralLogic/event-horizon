#include "shader.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

extern int gScreenWidth, gScreenHeight;

glm::mat4 GLShaderProgram::orthoTransform;
glm::mat4 GLShaderProgram::perspective;

void GLShaderProgram::initialize() {
	orthoTransform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / gScreenWidth, 2.0f / gScreenHeight, 1.0f));
	orthoTransform = glm::translate(orthoTransform, glm::vec3(-gScreenWidth / 2, -gScreenHeight / 2, 0.0f));
	perspective = glm::perspective(glm::radians(45.0f), (float)gScreenWidth / (float)gScreenHeight, 0.1f, 500.0f);
}

GLShaderProgram::GLShaderProgram(const string& vertexShader, const string& fragmentShader) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint success, infoLength;
	char *infoLog;

	const char* vertexShaderCStr = vertexShader.c_str();
	const char* fragmentShaderCStr = fragmentShader.c_str();

	glShaderSource(vertexShaderID, 1, &vertexShaderCStr, NULL);
	glCompileShader(vertexShaderID);

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLength);
		infoLog = new char[infoLength];
		glGetShaderInfoLog(vertexShaderID, infoLength, NULL, infoLog);
		cerr << "Vertex shader compilation failed: " << infoLog << endl;
		delete [] infoLog;
		throw "Vertex shader compilation failed";
	}

	glShaderSource(fragmentShaderID, 1, &fragmentShaderCStr, NULL);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLength);
		infoLog = new char[infoLength];
		glGetShaderInfoLog(fragmentShaderID, infoLength, NULL, infoLog);
		cerr << "Fragment shader compilation failed: " << infoLog << endl;
		delete [] infoLog;
		throw "Fragment shader compilation failed";
	}

	program = glCreateProgram();
	glAttachShader(program, vertexShaderID);
	glAttachShader(program, fragmentShaderID);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLength);
		infoLog = new char[infoLength];
		glGetProgramInfoLog(program, infoLength, NULL, infoLog);
		cerr << "Shader program linking failed: " << infoLog << endl;
		delete [] infoLog;
		throw "Shader program linking failed";
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

GLShaderProgram::~GLShaderProgram() {
	glDeleteProgram(program);
}

void GLShaderProgram::use() {
	glUseProgram(program);
}

void GLShaderProgram::setUniformf(const string& name, float value) {
	GLint location = glGetUniformLocation(program, name.c_str());
	if (location == -1) {
		cerr << "Uniform " << name << " not found" << endl;
		return;
	}
	glUniform1f(location, value);
}

void GLShaderProgram::setUniformMat4(const string& name, glm::mat4& mat) {
	GLint location = glGetUniformLocation(program, name.c_str());
	if (location == -1) {
		cerr << "Uniform " << name << " not found" << endl;
		return;
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void GLShaderProgram::setUniform3f(const string& name, float v0, float v1, float v2) {
	GLint location = glGetUniformLocation(program, name.c_str());
	if (location == -1) {
		cerr << "Uniform " << name << " not found" << endl;
		return;
	}
	glUniform3f(location, v0, v1, v2);
}

void GLShaderProgram::setUniform3fv(const string& name, const float* value) {
	GLint location = glGetUniformLocation(program, name.c_str());
	if (location == -1) {
		cerr << "Uniform " << name << " not found" << endl;
		return;
	}
	glUniform3fv(location, 1, value);
}

void GLShaderProgram::setUniformb(const string& name, bool value) {
	GLint location = glGetUniformLocation(program, name.c_str());
	if (location == -1) {
		cerr << "Uniform " << name << " not found" << endl;
		return;
	}
	glUniform1i(location, value);
}