#include "planet.h"
#include "controller.h"
#include "plugin.h"
#include <glm/ext/matrix_transform.hpp>

Sphere* Planet::sphere;
GLuint Planet::ringVAO;
GLuint Planet::ringVBO;

void Planet::initialize() {
	sphere = new Sphere(20, 20);

	float vertices[] = {
		-1, 0, -1, 0, 1, 0, 0, 0,
		-1, 0, 1, 0, 1, 0, 0, 1,
		1, 0, 1, 0, 1, 0, 1, 1,
		-1, 0, -1, 0, 1, 0, 0, 0,
		1, 0, 1, 0, 1, 0, 1, 1,
		1, 0, -1, 0, 1, 0, 1, 0
	};

	glGenVertexArrays(1, &ringVAO);
	glGenBuffers(1, &ringVBO);

	glBindVertexArray(ringVAO);
	glBindBuffer(GL_ARRAY_BUFFER, ringVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);	
	// load sounds
}

void Planet::registerFromDictionary(const json& dictionary) {
	shared_ptr<Planet> planet = make_shared<Planet>();
	json empty;
	if( dictionary.contains("Coords") && dictionary["Coords"].is_array() ) {
		planet->pos.x = dictionary["Coords"][0];
		planet->pos.z = dictionary["Coords"][1];
	}
	planet->name = dictionary.contains("Name") ? (string)dictionary["Name"] : (string)dictionary["ID"];
	planet->ID = (string)dictionary["PluginName"] + ".planets." + (string)dictionary["ID"];
	planet->size = dictionary.contains("Size") ? (float)dictionary["Size"] : 1;
	planet->deltaRot = dictionary.contains("Speed") ? (float)dictionary["Speed"] : 0;
	planet->mass = dictionary.contains("Mass") ? (int)dictionary["Mass"] : 1;
	planet->initData = dictionary.contains("Orbit") ? dictionary["Orbit"] : empty;
	planet->description = dictionary.contains("Description") ? (string)dictionary["Description"] : "";
	// goods
	if( dictionary.contains("PicturePath") ) {
		planet->texFile = (string)dictionary["Directory"] + (string)dictionary["PicturePath"];
		planet->texNum = 1;
		planet->texture.push_back(0);
	} else if( dictionary.contains("Model") ) {
		planet->model = make_shared<Object3D>();
		planet->model->loadOBJ((string)dictionary["Directory"] + (string)dictionary["Model"]);
	}
	planet->flags = dictionary.contains("Flags") ? dictionary["Flags"] : empty;
	if( dictionary.contains("ShipFlags") ) {
		planet->shipFlags = dictionary["ShipFlags"];
		planet->shipyard = true;
	}
	if( dictionary.contains("ModFlags") ) {
		planet->modFlags = dictionary["ModFlags"];
		planet->mod = true;
	}
	if( dictionary.contains("MissionFlags") ) {
		planet->missionFlags = dictionary["MissionFlags"];
		planet->mission = true;
	}
	planet->pitch = dictionary.contains("Tilt") ? (float)dictionary["Tilt"] : 0;
	planet->inclination = dictionary.contains("Inclination") ? (float)dictionary["Inclination"]*pi/180 : 0;
	if( dictionary.contains("Ring") ) {
		planet->ringTex = (string)dictionary["Directory"] + (string)dictionary["Ring"]["Path"];
		planet->ringSize = dictionary["Ring"]["Size"];
	}
	if (dictionary.contains("Atmosphere")) {
		planet->atmosTex = (string)dictionary["Directory"] + (string)dictionary["Atmosphere"]["Path"];
		planet->atmosSize = dictionary["Atmosphere"]["Size"];
		planet->atmosSpeed = dictionary["Atmosphere"]["Speed"];
	}
	if( dictionary.contains("Replace") )
		planet->ID = dictionary["Replace"];
	vector<string> temp = split(planet->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->planets[temp[2]] = planet;
}

void Planet::finalize() {
	if (initData.is_null())
		return;
	// set up orbit
	if( initData.is_string() )
		setOrbit(static_pointer_cast<Planet>(Controller::componentNamed(initData)));
	else if (initData.is_number()) {
		SPACEOBJECT_ORIGIN->mass = initData;
		setOrbit(SPACEOBJECT_ORIGIN);
	}
	initData.clear();
	vUp.x = 0;
	vUp.y = cos(pitch * pi / 180);
	vUp.z = sin(pitch * pi / 180);
	vForward.x = 1;
	vForward.y = 0;
	vForward.z = 0;
	vRight = glm::normalize(glm::cross(vUp, vForward));
}

Planet::Planet() {
	texture.resize(3);
	texNum = 3;
}

Planet::~Planet() {
	for( int i = 0; i < texNum; i++ ) {
		glDeleteTextures(1, &texture[i]);
	}
}

void Planet::draw() {
	if (!visible())
		return;
	position();
	if (this->model) {
		this->model->draw();
	}
	else {
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		sphere->draw();
	}
}

void Planet::drawRing() {
	if (ringSize == 0)
		return;

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::rotate(model, float(pitch * pi / 180), glm::vec3(1, 0, 0));
	model = glm::rotate(model, float(angle * pi / 180), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(ringSize));
	Controller::shader->setUniformMat4("model", model);
	glBindVertexArray(ringVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Planet::update() {
	if (centerOfRotation) {
		orbit();
	}
	angle += deltaRot*sys->FACTOR;
	atmosRot += atmosSpeed * sys->FACTOR;
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), float(angle*pi/180), vUp);
	vForward = glm::vec3(rot * glm::vec4(1, 0, 0, 1));
	vRight = glm::normalize(glm::cross(vUp, vForward));
}

void Planet::orbit() {
	theta += w * sys->FACTOR;
	if (theta > 360)
		theta -= 360;
	else if (theta < 0)
		theta += 360;
	pos.x = centerOfRotation->pos.x + distance*cos(theta*pi/180);
	pos.y = centerOfRotation->pos.y + distance*sin(theta*pi/180)*sin(inclination*pi/180);
	pos.z = centerOfRotation->pos.z + distance*sin(theta*pi/180)*cos(inclination*pi/180);
}