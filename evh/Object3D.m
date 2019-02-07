//
//  Object3D.m
//  Event Horizon
//
//  Created by user on Sat Mar 01 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Object3D.h"
#import "Texture.h"

void makeNormal( NSArray *array, int index );
void normalize( Coord *a, Coord *b, Coord *c );

@implementation Object3D

- init{
	self = [super init];
	texNum = 1;
	texture = calloc( texNum, sizeof( GLuint ) );
	return self;
}

- (void) dealloc{
	free( texture );
	
	[super dealloc];
}

- (void) loadOBJ:(NSString *)objfile{
	FILE *file;
	char *str = malloc( 256 );
	char *mtllib = malloc( 256 );
	OBJ *o = nil;
	int vCount = 0, nCount = 0, tCount = 0;
	int i;
	NSMutableDictionary *textures = [[NSMutableDictionary alloc] initWithCapacity:2];
	NSString *path, *texlib;
//	NSMutableArray *objects = [[NSMutableArray alloc] initWithCapacity:1];
	NSEnumerator *enumerator;
	NSString *key;

	obj = YES;
	objects = [[NSMutableArray alloc] initWithCapacity:1];
	file = fopen( [objfile cString], "r" );
	while( fscanf( file, "%s", str ), !feof( file ) ){
		if( str[0] == '#' )	// read a comment
			while( fgetc(file) != '\n' )
				;
		else if( strcmp( str, "mtllib" ) == 0 )
			fscanf( file, "%s", mtllib );
		else if( strcmp( str, "o" ) == 0 ){		// new object
			fscanf( file, "%s", str );
			if( o != nil ){
				[objects addObject:o];
				vCount += [o->vertices count];
				nCount += [o->normals count];
				tCount += [o->uvCoords count];
			}
			o = [[OBJ alloc] init];
			o->mtllib = [[NSString alloc] initWithCString:mtllib];
			o->name = [[NSString alloc] initWithCString:str];
		} else if( strcmp( str, "v" ) == 0 ){		// new vertex
			Coord *c = [[[Coord alloc] init] autorelease];
			float f;
			fscanf( file, "%f", &f );
			c->x = f;
			fscanf( file, "%f", &f );
			c->y = f;
			fscanf( file, "%f", &f );
			c->z = f;
			[o->vertices addObject:c];
		} else if( strcmp( str, "vn" ) == 0 ){		// vertex normal
			Coord *c = [[[Coord alloc] init] autorelease];
			float f;
			fscanf( file, "%f", &f );
			c->x = f;
			fscanf( file, "%f", &f );
			c->y = f;
			fscanf( file, "%f", &f );
			c->z = f;
			[o->normals addObject:c];
		} else if( strcmp( str, "vt" ) == 0 ){		// texture coord
			Coord *c = [[[Coord alloc] init] autorelease];
			float f;
			fscanf( file, "%f", &f );
			c->x = f;
			fscanf( file, "%f", &f );
			c->y = f;
			[o->uvCoords addObject:c];			
		} else if( strcmp( str, "f" ) == 0 ){		// face
			NSArray *comps;
			NSMutableArray *face = [NSMutableArray arrayWithCapacity:4];
			int i;
			str = fgets( str, 256, file );
			comps = [[NSString stringWithCString:str] componentsSeparatedByString:@" "];
			for( i = 1; i < [comps count]; i++ ){
				NSString *string = [comps objectAtIndex:i];
				NSArray *temp = [string rangeOfString:@"//"].location == NSNotFound ? [string componentsSeparatedByString:@"/"] : [string componentsSeparatedByString:@"//"];
				Coord *c = [[[Coord alloc] init] autorelease];
				c->x = [[temp objectAtIndex:0] intValue]-vCount;		// vertex number (1-based)
				if( [temp count] == 2 ){
					c->z = [[temp objectAtIndex:1] intValue]-nCount;	// normal number
				}else if( [temp count] == 3 ){
					c->z = [[temp objectAtIndex:2] intValue]-nCount;	// normal coords
					c->y = [[temp objectAtIndex:1] intValue]-tCount;	// texture coords
				}
				[face addObject:c];
			}
			[o->faces addObject:face];
		} else if( strcmp( str, "usemtl" ) == 0 ){
			fscanf( file, "%s", str );
			if( [o->mtl length] == 0 ){		// "default" sometimes comes along later...
				o->mtl = [[NSString alloc] initWithCString:str];
			}
		} else if( strcmp( str, "s" ) == 0 ){
			o->smooth = YES;
		}
	}
	if( o != nil )
		[objects addObject:[o retain]];

	path = [objfile substringToIndex:[objfile rangeOfString:[objfile lastPathComponent]].location];
	texlib = [NSString stringWithFormat:@"%@%s", path, mtllib];
	file = fopen( [texlib cString], "r" );
	while( !feof(file) ){
		NSString *mtlName;
		while( strcmp( str, "newmtl" ) != 0 && !feof(file) )
			fscanf( file, "%s", str );
		fscanf( file, "%s", str );
		mtlName = [NSString stringWithCString:str];
		while( strcmp( str, "newmtl" ) != 0 && strcmp( str, "map_Kd" ) != 0 && !feof(file) )
			fscanf( file, "%s", str );
		if( strcmp( str, "map_Kd" ) == 0 ){
			fscanf( file, "%s", str );
			[textures setObject:[NSString stringWithFormat:@"%@%s", path, str] forKey:mtlName];
		}
	}
	fclose( file );
	
	texNum = [objects count];
	texture = calloc( texNum, sizeof(GLuint) );
	list = calloc( texNum, sizeof(GLuint) );
	
	enumerator = [textures keyEnumerator];
	while( key = [enumerator nextObject] ){
		GLuint tex = 0;
		NSNumber *temp;
		[Texture loadTexture:[textures objectForKey:key] into:&tex withWidth:nil height:nil];
		temp = [NSNumber numberWithUnsignedInt:tex];
		[textures setObject:[NSNumber numberWithUnsignedInt:tex] forKey:key];
	}
	
	for( i = 0; i < [objects count]; i++ ){
		OBJ *o = [objects objectAtIndex:i];
		if( [textures objectForKey:o->mtl] )
			texture[i] = [[textures objectForKey:o->mtl] unsignedIntValue];
		list[i] = glGenLists( 1 );
		glNewList( list[i], GL_COMPILE );
		[self objListDraw:o];
		glEndList();
	}
}

- (void) objListDraw:(OBJ*)o{
	int j;

	glBegin( GL_TRIANGLES );
		for( j = 0; j < [o->faces count]; j++ ){	// for each face...
			NSArray *temp = [o->faces objectAtIndex:j];
			if( [temp count] == 3 ){			/// if it's a tri...
				Coord *a = [temp objectAtIndex:0];
				Coord *b = [temp objectAtIndex:1];
				Coord *c = [temp objectAtIndex:2];
				if( !o->smooth )
					normalize( [o->vertices objectAtIndex:a->x-1], [o->vertices objectAtIndex:b->x-1], [o->vertices objectAtIndex:c->x-1] );
				if( a->y ){
					[[o->uvCoords objectAtIndex:a->y-1] makeOBJTex];
				}
				else
					glTexCoord2f( 0.002, 0.002 );
				if( o->smooth )
					[[o->normals objectAtIndex:a->z-1] makeNormal];
				[[o->vertices objectAtIndex:a->x-1] makeVertex];
				if( b->y ){
					[[o->uvCoords objectAtIndex:b->y-1] makeOBJTex];
				} else
					glTexCoord2f( 0.002, 0.002 );
				if( o->smooth )
					[[o->normals objectAtIndex:b->z-1] makeNormal];
				[[o->vertices objectAtIndex:b->x-1] makeVertex];
				if( c->y ){
					[[o->uvCoords objectAtIndex:c->y-1] makeOBJTex];
				} else
					glTexCoord2f( 0.002, 0.002 );
				if( o->smooth )
					[[o->normals objectAtIndex:c->z-1] makeNormal];
				[[o->vertices objectAtIndex:c->x-1] makeVertex];
			}
		}
	glEnd();
	
	glBegin( GL_QUADS );
		for( j = 0; j < [o->faces count]; j++ ){	// for each face...
			NSArray *temp = [o->faces objectAtIndex:j];
			if( [temp count] == 4 ){			/// if it's a quad...
				Coord *a = [temp objectAtIndex:0];
				Coord *b = [temp objectAtIndex:1];
				Coord *c = [temp objectAtIndex:2];
				Coord *d = [temp objectAtIndex:3];
				if( !o->smooth )
					normalize( [o->vertices objectAtIndex:a->x-1], [o->vertices objectAtIndex:b->x-1], [o->vertices objectAtIndex:c->x-1] );
				if( a->y ){
					[[o->uvCoords objectAtIndex:a->y-1] makeOBJTex];
				} else
					glTexCoord2f( 0.002, 0.002 );
				if( o->smooth )
					[[o->normals objectAtIndex:a->z-1] makeNormal];
				[[o->vertices objectAtIndex:a->x-1] makeVertex];
				if( b->y ){
					[[o->uvCoords objectAtIndex:b->y-1] makeOBJTex];
				} else
					glTexCoord2f( 0.002, 0.002 );
				if( o->smooth )
					[[o->normals objectAtIndex:b->z-1] makeNormal];
				[[o->vertices objectAtIndex:b->x-1] makeVertex];
				if( c->y ){
					[[o->uvCoords objectAtIndex:c->y-1] makeOBJTex];
				} else
					glTexCoord2f( 0.002, 0.002 );
				if( o->smooth )
					[[o->normals objectAtIndex:c->z-1] makeNormal];
				[[o->vertices objectAtIndex:c->x-1] makeVertex];
				if( d->y ){
					[[o->uvCoords objectAtIndex:d->y-1] makeOBJTex];
				} else
					glTexCoord2f( 0.002, 0.002 );
				if( o->smooth )
					[[o->normals objectAtIndex:d->z-1] makeNormal];
				[[o->vertices objectAtIndex:d->x-1] makeVertex];
			}
		}
	glEnd();
			
	for( j = 0; j < [o->faces count]; j++ ){	// for each face...
		NSArray *temp = [o->faces objectAtIndex:j];
		int x;
		if( [temp count] > 4 ){			/// if it's a poly...
		glBegin( GL_POLYGON );{
			Coord *a = [temp objectAtIndex:0];
			Coord *b = [temp objectAtIndex:1];
			Coord *c = [temp objectAtIndex:2];
			if( !o->smooth )
				normalize( [o->vertices objectAtIndex:a->x-1], [o->vertices objectAtIndex:b->x-1], [o->vertices objectAtIndex:c->x-1] );
			if( a->y )
				[[o->uvCoords objectAtIndex:a->y-1] makeOBJTex];
			else
				glTexCoord2f( 0.002, 0.002 );
			if( o->smooth )
				[[o->normals objectAtIndex:a->z-1] makeNormal];
			[[o->vertices objectAtIndex:a->x-1] makeVertex];
			if( b->y )
				[[o->uvCoords objectAtIndex:b->y-1] makeOBJTex];
			else
				glTexCoord2f( 0.002, 0.002 );
			if( o->smooth )
				[[o->normals objectAtIndex:b->z-1] makeNormal];
			[[o->vertices objectAtIndex:b->x-1] makeVertex];
			if( c->y )
				[[o->uvCoords objectAtIndex:c->y-1] makeOBJTex];
			else
				glTexCoord2f( 0.002, 0.002 );
			if( o->smooth )
				[[o->normals objectAtIndex:c->z-1] makeNormal];
			[[o->vertices objectAtIndex:c->x-1] makeVertex];
			for( x = 3; x < [temp count]; x++ ){
				c = [temp objectAtIndex:x];
				if( c->y )
					[[o->uvCoords objectAtIndex:c->y-1] makeOBJTex];
				else
					glTexCoord2f( 0.002, 0.002 );
				if( o->smooth )
					[[o->normals objectAtIndex:c->z-1] makeNormal];
				[[o->vertices objectAtIndex:c->x-1] makeVertex];
			}
		}glEnd();
		}
	}
}

- (void) load:(NSData *)data withTexture:(NSString *)tex{
	Coord *point;
	const void *buffer = [data bytes];
	if( strncmp( buffer, "EVH3", 4 ) != 0 )	// valid data
		return;

	listNum = glGenLists( 1 );

	tri = [[NSMutableArray alloc] initWithCapacity:25];
	quad = [[NSMutableArray alloc] initWithCapacity:50];
	poly = [[NSMutableArray alloc] initWithCapacity:5];
	
	buffer = &buffer[4];
	while( strncmp( buffer, "\n----\n", 6 ) != 0 ){	// we've got a tri
		point = [[Coord alloc] init];
		memcpy( &point->x, &buffer[0], 4 );
		memcpy( &point->y, &buffer[4], 4 );
		memcpy( &point->z, &buffer[8], 4 );
		memcpy( &point->u, &buffer[12], 4 );
		memcpy( &point->v, &buffer[16], 4 );
		[tri addObject:point];
		buffer = &buffer[20];
	}
	buffer = &buffer[6];
	while( strncmp( buffer, "\n----\n", 6 ) != 0 ){	// we've got a quad
		point = [[Coord alloc] init];
		memcpy( &point->x, &buffer[0], 4 );
		memcpy( &point->y, &buffer[4], 4 );
		memcpy( &point->z, &buffer[8], 4 );
		memcpy( &point->u, &buffer[12], 4 );
		memcpy( &point->v, &buffer[16], 4 );
		[quad addObject:point];
		buffer = &buffer[20];
	}
	buffer = &buffer[6];
	while( strncmp( buffer, "\n----\n", 6 ) != 0 ){	// we've got a poly
		NSMutableArray *temp = [[NSMutableArray alloc] init];
		while( strncmp( buffer, "\n++++\n", 6 ) != 0 ){
			point = [[Coord alloc] init];
			memcpy( &point->x, &buffer[0], 4 );
			memcpy( &point->y, &buffer[4], 4 );
			memcpy( &point->z, &buffer[8], 4 );
			memcpy( &point->u, &buffer[12], 4 );
			memcpy( &point->v, &buffer[16], 4 );
			[temp addObject:point];
			buffer = &buffer[20];
		}
		buffer = &buffer[6];
		[poly addObject:temp];
	}
	[self loadTextures:&tex];
	
	glNewList( listNum, GL_COMPILE );
	[self listDraw];
	glEndList();
}

- (void) listDraw{
	int i;
	
	glBegin( GL_TRIANGLES );
		for( i = 0; i < [tri count]; i++ ){
			if( i%3 == 0 )
				makeNormal( tri, i );
			[[tri objectAtIndex:i] makeTexCoord];
			[[tri objectAtIndex:i] makeVertex];
		}
	glEnd();
	
	glBegin( GL_QUADS );
		for( i = 0; i < [quad count]; i++ ){
			if( i %4 == 0 )
				makeNormal( quad, i );
			[[quad objectAtIndex:i] makeTexCoord];
			[[quad objectAtIndex:i] makeVertex];
		}
	glEnd();
	
	for( i = 0; i < [poly count]; i++ ){
		int j;
		NSMutableArray *temp = [poly objectAtIndex:i];
		glBegin( GL_POLYGON );
		makeNormal( temp, i );
		for( j = 0; j < [temp count]; j++ ){
			[[temp objectAtIndex:j] makeTexCoord];
			[[temp objectAtIndex:j] makeVertex];
		}
		glEnd();
	}
}

- (void) drawObject{
	if( !obj )
		glCallList( listNum );
	else{
		int i;
		for( i = 0; i < texNum; i++ ){
			glCallList( list[i] );
		}
	}
}

@end

void makeNormal( NSArray *array, int index ){
	/*
	(u2v3-u3v2, u3v1-u1v3, u1v2-u2v1)
	*/
	float u1, u2, u3, v1, v2, v3;
	
	if( [array count] <= index+2 )
		return;

	u1 = ((Coord*)[array objectAtIndex:index+2])->x - ((Coord*)[array objectAtIndex:index+1])->x;
	u2 = ((Coord*)[array objectAtIndex:index+2])->y - ((Coord*)[array objectAtIndex:index+1])->y;
	u3 = ((Coord*)[array objectAtIndex:index+2])->z - ((Coord*)[array objectAtIndex:index+1])->z;
	v1 = ((Coord*)[array objectAtIndex:index])->x - ((Coord*)[array objectAtIndex:index+1])->x;
	v2 = ((Coord*)[array objectAtIndex:index])->y - ((Coord*)[array objectAtIndex:index+1])->y;
	v3 = ((Coord*)[array objectAtIndex:index])->z - ((Coord*)[array objectAtIndex:index+1])->z;
	
	glNormal3f( u2*v3 - u3*v2, u3*v1 - u1*v3, u1*v2 - u2*v1 );
}

void normalize( Coord *a, Coord *b, Coord *c ){
	float u1, u2, u3, v1, v2, v3;

	u1 = b->x - a->x;
	u2 = b->y - a->y;
	u3 = b->z - a->z;
	v1 = c->x - a->x;
	v2 = c->y - a->y;
	v3 = c->z - a->z;
	
	glNormal3f( u2*v3 - u3*v2, u3*v1 - u1*v3, u1*v2 - u2*v1 );	
}