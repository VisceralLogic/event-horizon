#pragma once
#include <string>
#include <map>
#include <vector>
#include <Windows.h>
#include <gl/glu.h>

using namespace std;

class Controller;
class Planet;

class SpaceObject {
public:
	static SpaceObject* SPACEOBJECT_ORIGIN;
	static GLUquadricObj* sphereObj;
	static constexpr double pi = 3.14159265358979323846;
	static Controller* sys;

	double MAX_ANGULAR_VELOCITY;		// maximum rate of rotation
	double ANGULAR_ACCELERATION;		// rate of increase of rotation
	double ACCELERATION;
	double MAX_VELOCITY;

	double angle, roll, pitch;		// angle in xz plane, about local z, about local x
	double deltaPitch, deltaRoll;	// pitch and roll angular velocity
	double x = 0, y = 0, z = 0;		// cartesian coordinates
	double speedx, speedy, speedz;	// velocity vector
	double deltaRot;					// angular velocity

	vector<GLuint> texture;			// array of gl textures
	int texNum;						// number of textures

	double size;						// radius from center
	int mass;						// mass of object

	SpaceObject* centerOfRotation;	// SpaceObject to orbit
	double distance;					// distance from centerOfRotation
	double w;						// angular velocity about centerOfRotation
	double theta;					// angle about centerOfRotation

	string name;
	string ID;					// used for storing player data
	void* initData;					// used to store data between init and finalize
	string description;			// information about this
	string flagRequirements;		// flags planet must have for this to be sold there
	map<string, void*>* flags;	// each flag has its name as the key, and its value as the object
	double _x, _y, _z;				// used for rotating vectors from local to global
	double fX, fY, fZ;				// forward vector in global
	double rX, rY, rZ;				// right vector in global
	double uX, uY, uZ;				// up vector in global

	static void initialize();
	void loadTextures(const vector<string>& files);
	virtual void bracket();
	virtual void draw() {}
	void setOrbit(SpaceObject* center);
	void collide(SpaceObject **array, int count);
	void bounce(SpaceObject* sphere);
	void position();
	double getDistance(SpaceObject* other);
	void localToGlobal();
	void globalToLocal();
	void calcAngles();
	bool visible();

	// useful for scripting
	double getX();
	void setX(double x);
	double getY();
	void setY(double y);
	double getZ();
	void setZ(double z);
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
	map<string, void*>* getFlags();
};

extern SpaceObject* SPACEOBJECT_ORIGIN;
extern Controller* sys;
extern GLUquadricObj* sphereObj;