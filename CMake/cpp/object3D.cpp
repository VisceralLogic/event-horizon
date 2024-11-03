#include <map>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "object3D.h"
#include "texture.h"
#include "controller.h"
#include <glm/ext/matrix_transform.hpp>


vector<string> Object3D::split(string str, char delimiter) {
	vector<string> result;
	stringstream ss(str);
	string item;

	while (getline(ss, item, delimiter)) {
		result.push_back(item);
	}

	return result;
}

// split a string on another string
vector<string> Object3D::split(string str, string delimiter) {
	vector<string> result;
	size_t pos = 0;
	string token;

	while ((pos = str.find(delimiter)) != string::npos) {
		token = str.substr(0, pos);
		result.push_back(token);
		str.erase(0, pos + delimiter.length());
	}

	result.push_back(str);

	return result;
}

Object3D::Object3D() {
	texNum = 1;
	texture.resize(1);
	price = 0;
}

void Object3D::loadOBJ(string objfile) {
	ifstream file;
	string mtllib;
	shared_ptr<OBJ> o;
	int vCount = 0, nCount = 0, tCount = 0;
	int i;
	map<string, string> texturePaths;
	string path, texlib;

	file.open(objfile);
	if( !file.is_open()) {
		cerr << "Could not open file: " << objfile << endl;
		return;
	}
	string str;
	while ( file >> str && !file.eof() ) {
		if (str[0] == '#')	// read a comment
			// read from the file until the end of the line
			while (file.get() != '\n' && !file.eof())
				;
		else if (str == "mtllib" )
			file >> mtllib;
		else if ( str == "o" ) {		// new object
			file >> str;
			if (o != nullptr) {
				objects.push_back(o);
				vCount += o->vertices.size();
				nCount += o->normals.size();
				tCount += o->uvCoords.size();
			}
			o = shared_ptr<OBJ>(new OBJ());
			o->mtllib = mtllib;
			o->name = str;
		}
		else if ( str == "v" ) {		// new vertex
			shared_ptr<Coord> c(new Coord());
			file >> c->x;
			file >> c->y;
			file >> c->z;
			o->vertices.push_back(c);
		}
		else if ( str == "vn" ) {		// vertex normal
			shared_ptr<Coord> c(new Coord());
			file >> c->x;
			file >> c->y;
			file >> c->z;
			o->normals.push_back(c);
		}
		else if ( str == "vt" ) {		// texture coord
			shared_ptr<Coord> c(new Coord());
			file >> c->x;
			file >> c->y;
			c->y = 1.0f - c->y;
			o->uvCoords.push_back(c);
		}
		else if ( str == "f" ) {		// face
			vector<string> comps;
			vector<shared_ptr<Coord>> face;
			int i;
			getline(file, str);
			comps = split(str, ' ');
			for (i = 1; i < comps.size(); i++) {
				string comp = comps[i];
				if (comp == "")
					continue;
				vector<string> temp = comp.find("//") == string::npos ? split(comp, '/') : split(comp, "//");
				shared_ptr<Coord> c(new Coord());
				c->x = stoi(temp[0]) -vCount;		// vertex number (1-based)
				if (temp.size() == 2) {
					c->z = stoi(temp[1]) - nCount;	// normal number
				}
				else if (temp.size() == 3) {
					c->z = stoi(temp[2]) - nCount;	// normal coords
					c->y = stoi(temp[1]) - tCount;	// texture coords
				}
				face.push_back(c);
			}
			o->faces.push_back(face);
		}
		else if ( str == "usemtl" ) {
			file >> str;
			if (o->mtl.size() == 0) {		// "default" sometimes comes along later...
				o->mtl = str;
			}
		}
		else if ( str == "s" ) {
			o->smooth = true;
		}
	}
	if (o != NULL)
		objects.push_back(o); // add last object [o retain];

	path = objfile.substr(0, objfile.find_last_of(filesystem::path::preferred_separator)) + '/';
	texlib = path + mtllib;
	file.close();
	file.open(texlib);
	while (!file.eof()) {
		string mtlName;
		while ( str != "newmtl" && !file.eof() )
			file >> str;
		file >> str;
		mtlName = str;
		while (str != "newmtl" && str != "map_Kd" && !file.eof())
			file >> str;
		if ( str == "map_Kd" ) {
			file >> str;
			texturePaths[mtlName] = path + str;
		}
	}
	file.close();

	texNum = objects.size();

	map<string, GLuint> textures;
	for( auto it : texturePaths ) {
		GLuint tex = 0;
		Texture::loadTexture(it.second.c_str(), &tex, NULL, NULL);
		textures[it.first] = tex;
	}

	for (i = 0; i < objects.size(); i++) {
		shared_ptr<OBJ> o = objects[i];
		if (textures.find(o->mtl) != textures.end())
			texture[i] = textures[o->mtl];
		o->generateBuffers();
	}
}

void Object3D::draw() {
	for( int i = 0; i < objects.size(); i++ ) {
		shared_ptr<OBJ> o = objects[i];
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glBindVertexArray(o->VAO);
		glDrawArrays(GL_TRIANGLES, 0, o->triangles*3);
	}
}