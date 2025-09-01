#include "stardust.h"
#include "controller.h"

vector<shared_ptr<StarDust>> StarDust::dust;

GLuint StarDust::vao, StarDust::vbo;

void StarDust::initialize() {
	for (int i = 0; i < 75; i++) {
		shared_ptr<StarDust> d = make_shared<StarDust>();
		d->init();
		dust.push_back(d);
	}

	// initialize vao and vbo
	float vertices[] = {
		// x, y, z, r, g, b, a, 0
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0
	};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void StarDust::update() {
	for (int i = 0; i < dust.size(); i++) {
		shared_ptr<StarDust> d = dust[i];
		if (sqr(d->pos.x - sys->pos.x) + sqr(d->pos.y - sys->pos.y) + sqr(d->pos.z - sys->pos.z) > 64) {
			d->init();
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	for (shared_ptr<StarDust>& d : dust) {
		d->draw();
	}
}

void StarDust::init() {
	float a = (rand() % 360) * pi / 180;
	float b = (rand() % 360) * pi / 180;
	pos.x = sys->pos.x + 8 * sin(a) * cos(b);
	pos.y = sys->pos.y + 8 * cos(a) * cos(b);
	pos.z = sys->pos.z + 8 * sin(b);
}

void StarDust::draw() {
	float distance = sqrt(sqr(pos.x - sys->pos.x) + sqr(pos.y - sys->pos.y) + sqr(pos.z - sys->pos.z));
	distance = 1 - distance / 8;
	glm::vec3 vertex = glm::vec3(sys->velocity.x/10, sys->velocity.y/10, -sys->velocity.z/10);
	position();
	// draw line
	sys->shader->setUniformb("useColor", true);
	
	sys->shader->setUniformb("useColor", false);
}