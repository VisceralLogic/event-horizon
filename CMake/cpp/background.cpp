#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>

#include "background.h"
#include "sphere.h"
#include "spaceobject.h"
#include "controller.h"

Sphere* Background::sphere;
int Background::count;
int* Background::x = nullptr, * Background::y = nullptr, * Background::z = nullptr;
float* Background::size = nullptr;
float* Background::brightness = nullptr;
float Background::distance = 450;
GLShaderProgram* Background::shader;

void Background::initialize() {
	sphere = new Sphere(8, 8);

	string shaderVertSource = "#version 330 core\n"
		"layout(location = 0) in vec3 vertex;\n"
		"uniform mat4 projection;\n"
		"uniform mat4 view;\n"
		"uniform mat4 model;\n"
		"void main() {\n"
		"	gl_Position = projection * view * model * vec4(vertex, 1.0);\n"
		"}\n";
	string shaderFragSource = "#version 330 core\n"
		"out vec4 color;\n"
		"uniform float brightness;\n"
		"void main() {\n"
		"	color = vec4(brightness, brightness, brightness, 1.0);\n"
		"}\n";
	shader = new GLShaderProgram(shaderVertSource, shaderFragSource);
}

void Background::generate(float density, size_t seed) {
	count = density * 1000;
	if (x != nullptr) {
		free(x);
		free(y);
		free(z);
		free(size);
		free(brightness);
	}
	x = (int*)malloc(count * sizeof(int));
	y = (int*)malloc(count * sizeof(int));
	z = (int*)malloc(count * sizeof(int));
	size = (float*)malloc(count * sizeof(float));
	brightness = (float*)malloc(count * sizeof(float));

	srand(seed);
	for (int i = 0; i < count; i++) {
		float temp = 2.0f * SpaceObject::pi * rand() / RAND_MAX;
		x[i] = distance * sin(temp);
		y[i] = distance * cos(temp);
		temp = SpaceObject::pi * rand() / RAND_MAX - SpaceObject::pi / 2;
		z[i] = distance * sin(temp);
		x[i] *= cos(temp);
		y[i] *= cos(temp);
		size[i] = 0.1f + 0.5f * rand() / RAND_MAX;
		brightness[i] = 0.5f + 0.5f * rand() / RAND_MAX;
	}

	srand(time(NULL));
}

void Background::draw() {
	shader->use();
	shader->setUniformMat4("projection", GLShaderProgram::perspective);
	glm::mat4 view = sys->viewMatrix();
	shader->setUniformMat4("view", view);
	for (int i = 0; i < count; i++) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x[i], y[i], z[i]) + sys->pos);
		model = glm::scale(model, glm::vec3(size[i]));
		shader->setUniformMat4("model", model);
		shader->setUniformf("brightness", brightness[i]);
		glBindTexture(GL_TEXTURE_2D, 0);
		sphere->draw();
	}
}