#include <map>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "object3D.h"
#include "texture.h"

void Object3D::makeNormal(vector<Coord*>& array, int index){
	/*
	(u2v3-u3v2, u3v1-u1v3, u1v2-u2v1)
	*/
	double u1, u2, u3, v1, v2, v3;

	if (array.size() <= index + 2)
		return;

	u1 = array[index + 2]->x - array[index + 1]->x;
	u2 = array[index + 2]->y - array[index + 1]->y;
	u3 = array[index + 2]->z - array[index + 1]->z;
	v1 = array[index]->x - array[index + 1]->x;
	v2 = array[index]->y - array[index + 1]->y;
	v3 = array[index]->z - array[index + 1]->z;

	glNormal3d(u2 * v3 - u3 * v2, u3 * v1 - u1 * v3, u1 * v2 - u2 * v1);
}

void Object3D::normalize(Coord* a, Coord* b, Coord* c) {
	double u1, u2, u3, v1, v2, v3;

	u1 = b->x - a->x;
	u2 = b->y - a->y;
	u3 = b->z - a->z;
	v1 = c->x - a->x;
	v2 = c->y - a->y;
	v3 = c->z - a->z;

	glNormal3d(u2 * v3 - u3 * v2, u3 * v1 - u1 * v3, u1 * v2 - u2 * v1);
}

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
}

Object3D::~Object3D() {
	for (int i = 0; i < objects.size(); i++) {
		OBJ* o = objects[i];
		delete o;
	}
}

void Object3D::loadOBJ(string objfile) {
	ifstream file;
	string mtllib;
	OBJ* o = NULL;
	int vCount = 0, nCount = 0, tCount = 0;
	int i;
	map<string, string> texturePaths;
	string path, texlib;

	obj = true;
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
			if (o != NULL) {
				objects.push_back(o);
				vCount += o->vertices.size();
				nCount += o->normals.size();
				tCount += o->uvCoords.size();
			}
			o = new OBJ();
			o->mtllib = mtllib;
			o->name = str;
		}
		else if ( str == "v" ) {		// new vertex
			Coord* c = new Coord();// [[[Coord alloc]init] autorelease];
			float f;
			file >> f;
			c->x = f;
			file >> f;
			c->y = f;
			file >> f;
			c->z = f;
			o->vertices.push_back(c);
		}
		else if ( str == "vn" ) {		// vertex normal
			Coord* c = new Coord();//[[[Coord alloc]init] autorelease];
			float f;
			file >> f;
			c->x = f;
			file >> f;
			c->y = f;
			file >> f;
			c->z = f;
			o->normals.push_back(c);
		}
		else if ( str == "vt" ) {		// texture coord
			Coord* c = new Coord(); // [[[Coord alloc]init] autorelease];
			float f;
			file >> f;
			c->x = f;
			file >> f;
			c->y = f;
			o->uvCoords.push_back(c);
		}
		else if ( str == "f" ) {		// face
			vector<string> comps;
			vector<Coord*> face;
			int i;
			getline(file, str);
			comps = split(str, ' ');
			for (i = 1; i < comps.size(); i++) {
				string comp = comps[i];
				vector<string> temp = comp.find("//") == string::npos ? split(comp, '/') : split(comp, "//");
				Coord* c = new Coord();// [[[Coord alloc]init] autorelease];
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
		else if ( str == "ustmtl" ) {
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

	path = objfile.substr(0, objfile.find_last_of(filesystem::path::preferred_separator));
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
		OBJ* o = objects[i];
		if (textures.find(o->mtl) != textures.end())
			texture[i] = textures[o->mtl];
		list.push_back(glGenLists(1));
		glNewList(list[i], GL_COMPILE);
		objListDraw(o);
		glEndList();
	}
}

void Object3D::objListDraw(OBJ* o) {
	glBegin(GL_TRIANGLES);
	for (int j = 0; j < o->faces.size(); j++) {
		vector<Coord*> face = o->faces[j];
		if (face.size() == 3) {
			Coord* a = face[0];
			Coord* b = face[1];
			Coord* c = face[2];
			if (!o->smooth)
				normalize(o->vertices[a->x - 1], o->vertices[b->x - 1], o->vertices[c->x - 1]);
			if (a->y)
				o->uvCoords[a->y - 1]->makeOBJTex();
			else
				glTexCoord2d(0.002, 0.002);
			if (o->smooth)
				o->normals[a->z - 1]->makeNormal();
			o->vertices[a->x - 1]->makeVertex();
			if (b->y)
				o->uvCoords[b->y - 1]->makeOBJTex();
			else
				glTexCoord2d(0.002, 0.002);
			if (o->smooth)
				o->normals[b->z - 1]->makeNormal();
			o->vertices[b->x - 1]->makeVertex();
			if (c->y)
				o->uvCoords[c->y - 1]->makeOBJTex();
			else
				glTexCoord2d(0.002, 0.002);
			if (o->smooth)
				o->normals[c->z - 1]->makeNormal();
			o->vertices[c->x - 1]->makeVertex();
		}
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int j = 0; j < o->faces.size(); j++) {
		vector<Coord*> face = o->faces[j];
		if (face.size() == 4) {
			Coord* a = face[0];
			Coord* b = face[1];
			Coord* c = face[2];
			Coord* d = face[3];
			if( !o->smooth )
				normalize( o->vertices[a->x-1], o->vertices[b->x-1], o->vertices[c->x-1] );
			if( a->y )
				o->uvCoords[a->y - 1]->makeOBJTex();
			else
				glTexCoord2d(0.002, 0.002);
			if( o->smooth )
				o->normals[a->z - 1]->makeNormal();
			o->vertices[a->x - 1]->makeVertex();
			if( b->y )
				o->uvCoords[b->y - 1]->makeOBJTex();
			else
				glTexCoord2d(0.002, 0.002);
			if( o->smooth )
				o->normals[b->z - 1]->makeNormal();
			o->vertices[b->x - 1]->makeVertex();
			if( c->y )
				o->uvCoords[c->y - 1]->makeOBJTex();
			else
				glTexCoord2d(0.002, 0.002);
			if( o->smooth )
				o->normals[c->z - 1]->makeNormal();
			o->vertices[c->x - 1]->makeVertex();
			if( d->y )
				o->uvCoords[d->y - 1]->makeOBJTex();
			else
				glTexCoord2d(0.002, 0.002);
			if( o->smooth )
				o->normals[d->z - 1]->makeNormal();
			o->vertices[d->x - 1]->makeVertex();
		}
	}
	glEnd();

	for (int j = 0; j < o->faces.size(); j++) {
		vector<Coord*> face = o->faces[j];
		if (face.size() > 4) {
			glBegin(GL_POLYGON);
				Coord* a = face[0];
				Coord* b = face[1];
				Coord* c = face[2];
				if (!o->smooth)
					normalize(o->vertices[a->x - 1], o->vertices[b->x - 1], o->vertices[c->x - 1]);
				if (a->y)
					o->uvCoords[a->y - 1]->makeOBJTex();
				else
					glTexCoord2d(0.002, 0.002);
				if (o->smooth)
					o->normals[a->z - 1]->makeNormal();
				o->vertices[a->x - 1]->makeVertex();
				if (b->y)
					o->uvCoords[b->y - 1]->makeOBJTex();
				else
					glTexCoord2d(0.002, 0.002);
				if (o->smooth)
					o->normals[b->z - 1]->makeNormal();
				o->vertices[b->x - 1]->makeVertex();
				if (c->y)
					o->uvCoords[c->y - 1]->makeOBJTex();
				else
					glTexCoord2d(0.002, 0.002);
				if (o->smooth)
					o->normals[c->z - 1]->makeNormal();
				o->vertices[c->x - 1]->makeVertex();
				for (int k = 3; k < face.size(); k++) {
					Coord* d = face[k];
					if (d->y)
						o->uvCoords[d->y - 1]->makeOBJTex();
					else
						glTexCoord2d(0.002, 0.002);
					if (o->smooth)
						o->normals[d->z - 1]->makeNormal();
					o->vertices[d->x - 1]->makeVertex();
				}
			glEnd();
		}
	}
}

void Object3D::load(const char  *data, string tex) {
	Coord* point;
	const char *buffer = data;
	if( strncmp( buffer, "EVH3", 4 ) != 0 ) {
		return;
	}

	listNum = glGenLists(1);

	buffer = &buffer[4];
	while( strncmp( buffer, "\n----\n", 6 ) != 0 ) {	// read triangles
		point = new Coord();
		memcpy(&point->x, &buffer[0], 4);
		memcpy(&point->y, &buffer[4], 4);
		memcpy(&point->z, &buffer[8], 4);
		memcpy(&point->u, &buffer[12], 4);
		memcpy(&point->v, &buffer[16], 4);
		tri.push_back(point);
		buffer = &buffer[20];
	}
	buffer = &buffer[6];
	while( strncmp( buffer, "\n----\n", 6 ) != 0 ) {	// read quads
		point = new Coord();
		memcpy(&point->x, &buffer[0], 4);
		memcpy(&point->y, &buffer[4], 4);
		memcpy(&point->z, &buffer[8], 4);
		memcpy(&point->u, &buffer[12], 4);
		memcpy(&point->v, &buffer[16], 4);
		quad.push_back(point);
		buffer = &buffer[20];
	}
	buffer = &buffer[6];
	while( strncmp( buffer, "\n----\n", 6 ) != 0 ) {	// read polygons
		vector<Coord*> polyFace;
		while( strncmp( buffer, "\n++++\n", 6 ) != 0 ) {
			point = new Coord();
			memcpy(&point->x, &buffer[0], 4);
			memcpy(&point->y, &buffer[4], 4);
			memcpy(&point->z, &buffer[8], 4);
			memcpy(&point->u, &buffer[12], 4);
			memcpy(&point->v, &buffer[16], 4);
			polyFace.push_back(point);
			buffer = &buffer[20];
		}
		buffer = &buffer[6];
		poly.push_back(polyFace);
	}
	vector<string> texFile;
	texFile.push_back(tex);
	loadTextures(texFile);

	glNewList(listNum, GL_COMPILE);
	listDraw();
	glEndList();
}

void Object3D::listDraw	() {
	int i;
	glBegin(GL_TRIANGLES);
	for (i = 0; i < tri.size(); i++) {
		if( i%3 == 0 )
			makeNormal(tri, i);
		tri[i]->makeTexCoord();
		tri[i]->makeVertex();
	}
	glEnd();

	glBegin(GL_QUADS);
	for (i = 0; i < quad.size(); i++) {
		if( i%4 == 0 )
			makeNormal(quad, i);
		quad[i]->makeTexCoord();
		quad[i]->makeVertex();
	}
	glEnd();

	for (i = 0; i < poly.size(); i++) {
		vector<Coord*> face = poly[i];
		glBegin(GL_POLYGON);
		makeNormal(face, i);
		for( int j = 0; j < face.size(); j++ ) {
			face[j]->makeTexCoord();
			face[j]->makeVertex();
		}
		glEnd();
	}
}

void Object3D::drawObject() {
	for (int i = 0; i < texNum; i++) {
		glCallList(list[i]);
	}
}