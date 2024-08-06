#include "shieldspot.h"
#include "spaceobject.h"
#include "controller.h"

GLuint shieldSpotTexture;

ShieldSpot::ShieldSpot(float x, float y, float z, float time, float size) {
	float mag;

	this->x = x;
	this->y = y;
	this->z = z;
	this->start = time;
	this->size = size;
	this->done = false;

	// find a normal unit vector
	a = -y;
	b = x;
	c = 0;
	mag = sqrt(a * a + b * b);
	a /= mag;
	b /= mag;

	u = -z * b;
	v = z * a;
	w = x * b - y * a;
	mag = sqrt(u * u + v * v + w * w);
	u /= mag;
	v /= mag;
	w /= mag;
}

void ShieldSpot::update(float percent) {
	float scale = size * (sys->t - start);
	if (sys->t - start - 1.0f > 0) {
		done = true;
		return;
	}
	glColor4f(0.5 - 0.5 * percent, 0.5 + 0.5 * percent, 0.5 + 0.5 * percent, 1 + start - sys->t);
	glBindTexture(GL_TEXTURE_2D, shieldSpotTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(x + a * scale, y + b * scale, z + c * scale);
	glTexCoord2f(0, 1);
	glVertex3f(x + u * scale, y + v * scale, z + w * scale);
	glTexCoord2f(1, 1);
	glVertex3f(x - a * scale, y - b * scale, z - c * scale);
	glTexCoord2f(1, 0);
	glVertex3f(x - u * scale, y - v * scale, z - w * scale);
	glEnd();
}