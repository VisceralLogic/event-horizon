#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>

#include "background.h"
#include "sphere.h"
#include "spaceobject.h"
#include "controller.h"

Sphere* Background::sphere;
int Background::count;
int* Background::x = nullptr, * Background::y = nullptr, * Background::z = nullptr;
float Background::distance = 350;

void Background::initialize() {
	sphere = new Sphere(8, 8);
}

void Background::generate(float density) {
	count = density * 1000;
	if (x != nullptr) {
		free(x);
		free(y);
		free(z);
	}
	x = (int*)malloc(count * sizeof(int));
	y = (int*)malloc(count * sizeof(int));
	z = (int*)malloc(count * sizeof(int));

	for (int i = 0; i < count; i++) {
		float temp = 2.0f * SpaceObject::pi * rand() / RAND_MAX;
		x[i] = distance * sin(temp);
		y[i] = distance * cos(temp);
		temp = SpaceObject::pi * rand() / RAND_MAX - SpaceObject::pi / 2;
		z[i] = distance * sin(temp);
		x[i] *= cos(temp);
		y[i] *= cos(temp);
	}
}

void Background::draw() {
	Controller::shader->use();
	Controller::shader->setUniformMat4("projection", GLShaderProgram::perspective);
	glm::mat4 view = glm::lookAt(sys->pos, sys->pos + sys->vForward, sys->vUp);
	Controller::shader->setUniformMat4("view", view);
	for (int i = 0; i < count; i++) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x[i], y[i], z[i]) + sys->pos);
		model = glm::scale(model, glm::vec3(0.5));
		Controller::shader->setUniformMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, 0);
		sphere->draw();
	}
}