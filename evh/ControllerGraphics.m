//
//  ControllerGraphics.m
//  Event Horizon
//
//  Created by Paul Dorman on 3/12/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "ControllerGraphics.h"
#import "ControllerThreading.h"

void (* drawScene)(void);

@implementation Controller (ControllerGraphics)

- (void) setUpFrame{
	if( frameFront != 0 ){
		glDeleteLists( frameFront, 2 );
	}
	frameFront = glGenLists( 2 );
	frameRear = frameFront + 1;
	if( !frameFile ){
		glNewList( frameFront, GL_COMPILE );
		glBegin( GL_QUADS );
		glColor3f( .5, .5, .5 );
		glVertex2f( -.83, 1 );
		glVertex2f( -.83, .555 );
		glColor3f( 0, 0, 0 );
		glVertex2f( -.67, .555 );
		glVertex2f( -.67, 1 );
		
		glVertex2f( -1, -1 );
		glVertex2f( -.67, .555 );
		glColor3f( .5, .5, .5 );
		glVertex2f( -.83, .555 );
		glVertex2f( -1.15, -1 );
		
		glVertex2f( 1.15, -1 );
		glVertex2f( .83, .567 );
		glColor3f( 0, 0, 0 );
		glVertex2f( .67, .567 );
		glVertex2f( 1, -1 );
		
		glVertex2f( .67, 1 );
		glVertex2f( .67, .567 );
		glColor3f( .5, .5, .5 );
		glVertex2f( .83, .567 );
		glVertex2f( .83, 1 );
		glEnd();
		glEndList();
		frameRear = frameFront;
	} else {
		FILE *file = fopen( [frameFile cString], "r" );
		char *str = malloc( 256 );
		
		while( fscanf( file, "%s", str ), !feof( file ) ){
			if( strcmp( str, "<FRONT>" ) == 0 )
				glNewList( frameFront, GL_COMPILE );
			else if( strcmp( str, "<REAR>" ) == 0 )
				glNewList( frameRear, GL_COMPILE );
			else if( strcmp( str, "<REARSAME>" ) == 0 )
				frameRear = frameFront;
			else if( strcmp( str, "</FRONT>" ) == 0 || strcmp( str, "</REAR>" ) == 0 )
				glEndList();

			else if( strcmp( str, "<QUADS>" ) == 0 )
				glBegin( GL_QUADS );
			else if( strcmp( str, "<POINTS>" ) == 0 )
				glBegin( GL_POINTS );
			else if( strcmp( str, "<LINES>" ) == 0 )
				glBegin( GL_LINES );
			else if( strcmp( str, "<LINESTRIP>" ) == 0 )
				glBegin( GL_LINE_STRIP );
			else if( strcmp( str, "<LINELOOP>" ) == 0 )
				glBegin( GL_LINE_LOOP );
			else if( strcmp( str, "<TRIANGLES>" ) == 0 )
				glBegin( GL_TRIANGLES );
			else if( strcmp( str, "<TRIANGLESTRIP>" ) == 0 )
				glBegin( GL_TRIANGLE_STRIP );
			else if( strcmp( str, "<TRIANGLEFAN>" ) == 0 )
				glBegin( GL_TRIANGLE_FAN );
			else if( strcmp( str, "<QUADSTRIP>" ) == 0 )
				glBegin( GL_QUAD_STRIP );
			else if( strcmp( str, "<POLYGON>" ) == 0 )
				glBegin( GL_POLYGON );
			else if( strcmp( str, "</POINTS>" ) == 0 || strcmp( str, "</LINES>" ) == 0 || strcmp( str, "</LINESTRIP>" ) == 0 || strcmp( str, "</LINELOOP>" ) == 0 || strcmp( str, "</TRIANGLES>" ) == 0 || strcmp( str, "</QUADS>" ) == 0 || strcmp( str, "</TRIANGLESTRIP>" ) == 0 || strcmp( str, "</TRIANGLEFAN>" ) == 0 || strcmp( str, "</QUADSTRIP>" ) == 0 || strcmp( str, "</POLYGON>" ) == 0 )
				glEnd();
			
			else if( strcmp( str, "Color" ) == 0 ){
				float f1, f2, f3;
				fscanf( file, "%f,%f,%f", &f1, &f2, &f3 );
				glColor3f( f1, f2, f3 );
			}
			else if( strcmp( str, "Vertex" ) == 0 ){
				float f1, f2;
				fscanf( file, "%f,%f", &f1, &f2 );
				glVertex2f( f1, f2 );
			}
		}
		
		fclose( file );		
	}
}

- (void) drawNavTab{
	int i;
	NSArray *temp;

	glMatrixMode( GL_PROJECTION );		// don't modify our matrix
	glPushMatrix();
	glViewport( 5*screenWidth/6, screenHeight - screenWidth/6, screenWidth/6 + 1, screenWidth/6 );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glBindTexture(GL_TEXTURE_2D, texture[NAVPANEL_TEXTURE]);
	glColor3f( 1, 1, 1 );
			
	glBegin( GL_QUADS );		// the actual tab
		glTexCoord2f( 0, 0 );
		glVertex2f( -1, -1 );
		glTexCoord2f( 1, 0 );
		glVertex2f( 1, -1 );
		glTexCoord2f( 1, 1 );
		glVertex2f( 1, 1 );
		glTexCoord2f( 0, 1 );
		glVertex2f( -1, 1 );
	glEnd();
	
	glColor3f( 1, 1, 1 );
	
	glBindTexture(GL_TEXTURE_2D, texture[NAVPLANET_TEXTURE]);
	
	for( i = 0; i < [planets count]; i++ ){		// draw each planet
		Planet *planet = [planets objectAtIndex:i];
		float _size = .02*planet->size;
		glColor3f( 0, 0, .5 );
		
		_x = planet->x;
		_y = planet->y;
		_z = planet->z;
		[self globalToLocal];
		if( _x < 0 )
			continue;

		if( _size > .03 )
			_size = .03;
		else if( _size < .015 )
			_size = .015;
		if( planet == curPlanet ){
			_size *= 1.5;
			glColor3f( 1, 1, .5 );
		}
		glLoadIdentity();
		glTranslatef( .9*sin(atan(_z/_x))*cos(atan(_y/sqrt(_x*_x+_z*_z))), .9*sin(atan(_y/sqrt(_x*_x+_z*_z))), 0 );
		glBegin( GL_QUADS );
			glTexCoord2f( 0, 0 );
			glVertex2f( -_size, -_size );
			glTexCoord2f( 1, 0 );
			glVertex2f( _size, -_size );
			glTexCoord2f( 1, 1 );
			glVertex2f( _size, _size );
			glTexCoord2f( 0, 1 );
			glVertex2f( -_size, _size );
		glEnd();
	}
	
	[shipLock lock];
	temp = [NSArray arrayWithArray:ships];
	[shipLock unlock];
	for( i = 0; i < [temp count]; i++ ){
		AI *ai = [temp objectAtIndex:i];
		BOOL enemy = [enemies containsObject:ai];
		float _size = .02*ai->size;
		glColor3f( 0, .5, 0 );
		if( enemy )
			glColor3f( 0.5, 0, 0 );
		if( ai->escortee == sys )
			glColor3f( 0.5, 0.5, 0 );
		if( ai->state == DISABLED )
			glColor3f( 0.5, 0.5, 0.5 );
		
		_x = ai->x;
		_y = ai->y;
		_z = ai->z;
		[self globalToLocal];
		if( _x < 0 )
			continue;
		
		if( _size > .02 )
			_size = .02;
		if( _size < .01 )
			_size = .01;
		
		if( ai == selection ){
			_size *= 1.5;
			glColor3f( (1.+cos(10*t))/2, 1, (1.+cos(10*t))/2 );
			if( enemy )
				glColor3f( 1, (1.+cos(10*t))/2, (1.+cos(10*t))/2 );
			if( ai->escortee == sys )
				glColor3f( 1, 1, (1.+cos(10*t))/2 );
			if( ai->state == DISABLED )
				glColor3f( (1+cos(10*t))/2, (1+cos(10*t))/2, (1+cos(10*t))/2 );
		}
		glLoadIdentity();
		glTranslatef( .9*sin(atan(_z/_x))*cos(atan(_y/_x)), .9*sin(atan(_y/_x)), 0 );
		glBegin( GL_QUADS );
			glTexCoord2f( 0, 0 );
			glVertex2f( -_size, -_size );
			glTexCoord2f( 1, 0 );
			glVertex2f( _size, -_size );
			glTexCoord2f( 1, 1 );
			glVertex2f( _size, _size );
			glTexCoord2f( 0, 1 );
			glVertex2f( -_size, _size );
		glEnd();
	}
	
	glLoadIdentity();
	glColor3f( 0, 1, 1 );		// draw us
	glBegin( GL_QUADS );
		glTexCoord2f( 0, 0 );
		glVertex2f( -.02, -.02 );
		glTexCoord2f( 1, 0 );
		glVertex2f( .02, -.02 );
		glTexCoord2f( 1, 1 );
		glVertex2f( .02, .02 );
		glTexCoord2f( 0, 1 );
		glVertex2f( -.02, .02 );
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, 0);	
	glViewport( 0, 0, screenWidth, screenHeight );		// get our old matrix back
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}


- (void) drawFrame{
	glMatrixMode( GL_PROJECTION );		// don't modify our matrix
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	if( viewStyle == 0 )
		glCallList( frameFront );
	else if( viewStyle == 2 )
		glCallList( frameRear );
	
	if( viewStyle != 2 ){
		if( inertial )
			glColor3f( 0, 1, 0 );
		else
			glColor3f( 0, .1, 0 );
		glLoadIdentity();
		glTranslatef( -.75, .95, 0 );
		gluSphere( sphereObj, .01, 6, 6 );
		if( autopilot )
			glColor3f( 1, 1, 0 );
		else
			glColor3f( .1, .1, 0 );
		glLoadIdentity();
		glTranslatef( -.75, .9, 0 );
		gluSphere( sphereObj, .01, 6, 6 );
		if( hyper )
			glColor3f( 1, 0, 0 );
		else
			glColor3f( .1, 0, 0 );
		glLoadIdentity();
		glTranslatef( -.75, .85, 0 );
		gluSphere( sphereObj, .01, 6, 6 );
		
		glLoadIdentity();
		glBegin( GL_QUADS );
			glColor3f( .5, .5, .5 );
			glVertex2f( -.78, .6 );
			glVertex2f( -.78, .96 );
			glColor3f( 0, 0, 0 );
			glVertex2f( -.8, .96 );
			glVertex2f( -.8, .6 );
		
			glColor3f( 1, 0, 0 );
			glVertex2f( -.78, .6 );
			glVertex2f( -.78, .36*(armor/(float)maxArmor)+.6 );
			glColor3f( 0, 0, 0 );
			glVertex2f( -.8, .36*(armor/(float)maxArmor)+.6 );	
			glVertex2f( -.8, .6 );
	
			glColor3f( 0, 1, 0 );
			glVertex2f( -.78, .6 );
			glVertex2f( -.78, .36*(shields/(float)maxShield)+.6 );
			glColor3f( 0, 0, 0 );
			glVertex2f( -.8, .36*(shields/(float)maxShield)+.6 );
			glVertex2f( -.8, .6 );
			
			glColor3f( 0.5, 0.5, 0.5 );
			glVertex2f( -.7, .6 );
			glVertex2f( -.7, .96 );
			glColor3f( 0, 0, 0 );
			glVertex2f( -.72, .96 );
			glVertex2f( -.72, .6 );

			glColor3f( 0, 0, 0 );
			glVertex2f( -.7, .6 );
			glVertex2f( -.7, .36*(fuel/maxFuel)+.6 );
			glColor3f( 0, 0, 1 );
			glVertex2f( -.72, .36*(fuel/maxFuel)+.6 );
			glVertex2f( -.72, .6 );
			
			glColor3f( .5, .5, .5 );
			glVertex2f( -.7, .985 );
			glVertex2f( -.8, .985 );
			glColor3f( .25, .25, .25 );
			glVertex2f( -.8, .975 );
			glColor3f( .1, .1, .1 );
			glVertex2f( -.7, .975 );
			
			float fract = sqrt(sqr(speedx)+sqr(speedy)+sqr(speedz))/MAX_VELOCITY;
			float dx = -.8+0.1*fract;
			if( fract < 1 ){
				glColor3f( .5, 0, .5 );
				glVertex2f( -.8, .975 );
				glVertex2f( dx, .975 );
				glColor3f( .25, 0, .25 );
				glVertex2f( dx, .985 );
				glVertex2f( -.8, .985 );								
			} else {
				float temp = tanh( fract - 1 );
				glColor3f( .5, 0, .5 );
				glVertex2f( -.8, .975 );
				glVertex2f( -.7, .975 );
				glColor3f( .25, 0, .25 );
				glVertex2f( -.7, .985 );
				glVertex2f( -.8, .985 );								

				glColor3f( 1, 1, 1 );
				glVertex2f( -.8, .975 );
				glColor3f( .5, 0, .5 );
				glVertex2f( -.8+temp/10, .975 );
				glColor3f( .25, 0, .25 );
				glVertex2f( -.8+temp/10, .985 );
				glColor3f( .5, .5, .5 );
				glVertex2f( -.8, .985 );				
			}
		glEnd();
		
		glBegin( GL_LINES );
			glColor3f( 1, 0, 1 );				// throttle indicator
			dx = -.8+0.1*throttledSpeed/MAX_VELOCITY;
			glVertex2f( dx, .975 );
			glVertex2f( dx, .985 );

			glColor3f( 0, 1, 0 );
            glVertex2f( -0.1*screenHeight/screenWidth, 0 );		// cross hairs
            glVertex2f( -0.05*screenHeight/screenWidth, 0 );
            glVertex2f( 0.05*screenHeight/screenWidth, 0 );
            glVertex2f( 0.1*screenHeight/screenWidth, 0 );
            glVertex2f( 0, -0.1 );
            glVertex2f( 0, -0.05 );
            glVertex2f( 0, 0.05 );
            glVertex2f( 0, 0.1 );
			
			if( autopilot ){
				glVertex2f( -.05*screenHeight/screenWidth, 0 );
				glVertex2f( 0, .05 );
				glVertex2f( 0, .05 );
				glVertex2f( .05*screenHeight/screenWidth, 0 );
				glVertex2f( .05*screenHeight/screenWidth, 0 );
				glVertex2f( 0, -.05 );
				glVertex2f( 0, -.05 );
				glVertex2f( -.05*screenHeight/screenWidth, 0 );
			}
		glEnd();
	}

	glMatrixMode( GL_PROJECTION );		// get old matrix back
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

- (void) drawSelectionTab:(int)pos{
	glMatrixMode( GL_PROJECTION );		// don't modify our matrix
	glPushMatrix();
	glViewport( 5*screenWidth/6, screenHeight - screenWidth/6 - pos*(screenWidth/30), screenWidth/6 + 1, screenWidth/5. - screenWidth/6. );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glBindTexture(GL_TEXTURE_2D, texture[WIDGET_TEXTURE]);
	glColor3f( 1, 1, 1 );
			
	glBegin( GL_QUADS );		// the actual tab
		glTexCoord2f( 0, .75 );
		glVertex2f( -1, -1 );
		glTexCoord2f( 1, .75 );
		glVertex2f( 1, -1 );
		glTexCoord2f( 1, 1 );
		glVertex2f( 1, 1 );
		glTexCoord2f( 0, 1 );
		glVertex2f( -1, 1 );
	glEnd();
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	glViewport( 0, 0, screenWidth, screenHeight );
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

- (void) drawInfoTab{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glViewport( 5*screenWidth/6, screenHeight - 13*screenWidth/30, screenWidth/6+1, screenWidth/6+1 );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glBindTexture( GL_TEXTURE_2D, texture[INFOTAB_TEXTURE] );
	glColor3f( 1, 1, 1 );
	
	glBegin( GL_QUADS );
		glTexCoord2f( 0, 0 );
		glVertex2f( -1, -1 );
		glTexCoord2f( 1, 0 );
		glVertex2f( 1, -1 );
		glTexCoord2f( 1, 1 );
		glVertex2f( 1, 1 );
		glTexCoord2f( 0, 1 );
		glVertex2f( -1, 1 );
	glEnd();
	
	glBindTexture( GL_TEXTURE_2D, 0 );

	if( selection ){		// draw spaceship outline
		glScalef( 1/sqrt(2), 1/sqrt(2), 1/sqrt(2) );
		glRotatef( 5*t, 0, 1, 0 );
		glRotatef( 15, 0, 0, 1 );
		if( selection->state == ALIVE ){
			glColor3f( 0, 1, 0 );
			if( [enemies containsObject:selection] )
				glColor3f( .5, 0, 0 );
		} else {
			glColor3f( .5, .5, .5 );
		}
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glDisable( GL_CULL_FACE );
		[selection drawObject];
		glEnable( GL_CULL_FACE );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glLoadIdentity();
	}
	
	glViewport( 0, 0, screenWidth, screenHeight );
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

- (void) drawString:(char *)string atX:(float)x y:(float)y{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, screenWidth, 0, screenHeight, 0, 1 );
	glMatrixMode( GL_MODELVIEW );
		
	glRasterPos2f( screenWidth*x, screenHeight*y );

	DrawCStringGL( string, font );
	
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

@end

void DrawGLScene(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	[sys update];
}

void beginOrtho(){
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, sys->screenWidth, 0, sys->screenHeight, 0, 250 );
	glMatrixMode( GL_MODELVIEW );
}

void endOrtho(){
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}
