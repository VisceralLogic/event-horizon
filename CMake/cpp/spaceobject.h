#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "sphere.h"

#define sqr(x) ((x)*(x))

using namespace std;

class Controller;
class Planet;

extern Controller* sys;

class SpaceObject {
public:
	static shared_ptr<SpaceObject> SPACEOBJECT_ORIGIN;
	static Sphere* sphereObj;
	static constexpr double pi = 3.14159265358979323846;

	double MAX_ANGULAR_VELOCITY = 1;		// maximum rate of rotation
	double ANGULAR_ACCELERATION = 1;		// rate of increase of rotation
	double ACCELERATION = 1;
	double MAX_VELOCITY = 1;

	glm::vec3 velocity = glm::vec3(0);		// velocity
	glm::vec3 pos, vUp, vForward, vRight;		// position, up, forward, right vectors
	double deltaRot = 0, deltaPitch = 0, deltaRoll = 0;					// angular velocities
	double angle = 0, pitch = 0;

	vector<GLuint> texture;			// array of gl textures
	int texNum = 1;						// number of textures

	float size = 1;						// radius from center
	int mass = 1;						// mass of object

	shared_ptr<SpaceObject> centerOfRotation = nullptr;	// SpaceObject to orbit
	double distance = 0;					// distance from centerOfRotation
	double w = 0;						// angular velocity about centerOfRotation
	double theta = 0;					// angle about centerOfRotation

	string name = "<SpaceObject>";
	string description = "";			// information about this
	string flagRequirements = "";		// flags planet must have for this to be sold there

	static void initialize();
	SpaceObject();
	void loadTextures(const vector<string>& files);
	virtual void bracket();
	virtual void draw() {}
	virtual void update() {}
	void setOrbit(shared_ptr<SpaceObject> center);
	
	template<typename SO>
	void collide(vector<shared_ptr<SO>> array) {
		for (int i = 0; i < array.size(); i++) {
			shared_ptr<SpaceObject> a = static_pointer_cast<SpaceObject>(array[i]);
			if (a.get() != this)
				bounce(a);
		}
	}

	void bounce(shared_ptr<SpaceObject> sphere);
	void position();
	double getDistance(shared_ptr<SpaceObject> other);
	bool visible();
	glm::vec3 globalToLocal(const glm::vec3& global);

	// useful for scripting
	double getAngularAcceleration();
	void setAngularAcceleration(double a);
	double getMaxAngularVelocity();
	void setMaxAngularVelocity(double w);
	double getMaxVelocity();
	void setMaxVelocity(double v);
	int getMass();
	void setMass(int mass);
	string getName();
	void setName(string name);
};