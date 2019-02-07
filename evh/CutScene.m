//
//  CutScene.m
//  Event Horizon
//
//  Created by Paul Dorman on Fri Dec 10 2004.
//  Copyright (c) 2004 Paul Dorman. All rights reserved.
//

#import "ControllerGraphics.h"
#import "CutScene.h"
#import "Controller.h"

int whichScene;

void drawCut();
void eventCut( EventRef event );
void nsEventCut( NSEvent *event );

extern GLuint planetList;

float localT;

@implementation CutScene

+ (void) setup:(int)which{
	localT = sys->t;
	whichScene = which;
	drawScene = drawCut;
	eventScene = eventCut;
	nsEventScene = nsEventCut;
}

@end

void drawCut(){
	float lightPos[3];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glColor3f( 1, 1, 1 );
	
	glEnable( GL_LIGHTING );
	if( whichScene == LAND_1 ){
		float scale = .2 - .02*(sys->t-localT);
		lightPos[0] = -50;
		lightPos[1] = 10;
		lightPos[2] = 100;
		glLightfv( GL_LIGHT1, GL_POSITION, lightPos );
		glTranslatef( 1.4-2.43*cos(.1*pi*(sys->t-localT)), 0, -1.4-2.43*sin(.1*pi*(sys->t-localT)) );
		glRotatef( 90, 0, 1, 0 );
		glRotatef( -9.*(sys->t-localT), 0, 1, 0 );
		glRotatef( 12, 1, 0, 0 );
		glScalef( scale, scale, scale );
		glBindTexture( GL_TEXTURE_2D, sys->escortee->texture[0] );
		[sys->escortee drawObject];
		glLoadIdentity();
		glTranslatef( 1.4, 0, -1.4 );
		glRotatef( -90, 1, 0, 0 );
		glRotatef( (localT-sys->t)*18, 0, 0, 1 );
		glBindTexture(GL_TEXTURE_2D, sys->curPlanet->texture[0]);
		glCallList( planetList );
		if( (sys->t-localT) >= 5 ){
			whichScene = LAND_2;
			localT = sys->t;
		}
	}
	else if( whichScene == LAND_2 ){
		float scale = 0.04;
		glTranslatef( 0, -1+1.25*sin(.1*pi*(sys->t-localT)+0.25*pi), -1-1.25*cos(.1*pi*(sys->t-localT)+0.25*pi)  );
		glRotatef( -90, 0, 1, 0 );
		glRotatef( 45-18.*(sys->t-localT), 0, 0, 1 );
		glScalef( scale, scale, scale );
		glBindTexture( GL_TEXTURE_2D, sys->escortee->texture[0] );
		[sys->escortee drawObject];
		glLoadIdentity();
		glTranslatef( 0, -1, -1 );
		glRotatef( 90, 0, 1, 0 );
		glRotatef( (sys->t-localT)*18, 0, 0, 1 );
		glBindTexture( GL_TEXTURE_2D, sys->curPlanet->texture[0] );
		glCallList( planetList );
		if( (sys->t-localT) >= 5 ){
			whichScene = LAND_3;
			localT = sys->t;
		}
	}
	else if( whichScene == LAND_3 ){
		float scale = .2-.04*(sys->t-localT);
		float r = 1.2-.04*(sys->t-localT);
		glTranslatef( r*sin(.05*pi*(sys->t-localT)), 0, -3+r*cos(.05*pi*(sys->t-localT)) );
		glRotatef( 90, 1, 0, 0 );
		glRotatef( -9*(sys->t-localT), 0, 0, 1 );
		glScalef( scale, scale, scale );
		glBindTexture( GL_TEXTURE_2D, sys->escortee->texture[0] );
		[sys->escortee drawObject];
		glLoadIdentity();
		glTranslatef( 0, 0, -3 );
		glBindTexture( GL_TEXTURE_2D, sys->curPlanet->texture[0] );
		glRotatef( -90, 1, 0, 0 );
		glRotatef( (sys->t-localT)*9, 0, 0, 1 );
		glCallList( planetList );
		if( (sys->t-localT) >= 5 )
			setUpPlanet();
	} else if( whichScene == LAND_STATION ){
		float Xi = -5, Xf = 3, Zf = -1.5;
		float X = Xf+(Xi-Xf)*exp(-0.4*(sys->t-localT));
		float Z = Zf*(X-Xi)/(Xf-Xi);
		float R = sqrt(X*X+Z*Z);
		float A = 90.-atan2(-Z,X)*180./pi;
		float L = atan2( -Zf, Xf-Xi )*180./pi;
		float T = atan2( -Z, X)*180./pi-90.-L;

		lightPos[0] = -50;
		lightPos[1] = 0;
		lightPos[2] = 0;
		glLightfv( GL_LIGHT1, GL_POSITION, lightPos );		
		
		glTranslatef( 0, 0, -R );
		glRotatef( -T, 0, 1, 0 );
		glScalef( .3, .3, .3 );
		glBindTexture( GL_TEXTURE_2D, sys->escortee->texture[0] );
		[sys->escortee drawObject];
		
		glLoadIdentity();
		glRotatef( A, 0, 1, 0 );
		glTranslatef( Xf+.21*(1.+1./sys->size), 0, Zf-.21*(1.+1./sys->size) );
		glScalef( .3/sys->size, .3/sys->size, .3/sys->size );
		glBindTexture( GL_TEXTURE_2D, sys->curPlanet->model->texture[0] );
		[sys->curPlanet->model drawObject];
		
		if( (sys->t-localT) >= 10 )
			setUpPlanet();
	}
}

void eventCut( EventRef event ){
	UInt32 what = GetEventClass( event );
	if( (what == kEventClassMouse && GetEventClass(event) != kEventMouseMoved) || what == kEventClassKeyboard ){
		switch( whichScene ){
			case LAND_1:
			case LAND_2:
			case LAND_3:
			case LAND_STATION:
				setUpPlanet();
				break;
		}
	}
}

void nsEventCut( NSEvent *event ){
	switch( [event type] ){
		case NSLeftMouseDown:
		case NSRightMouseDown:
		case NSOtherMouseDown:
		case NSKeyDown:
			break;
		default:
			return;
	}
	switch( whichScene ){
		case LAND_1:
		case LAND_2:
		case LAND_3:
		case LAND_STATION:
			setUpPlanet();
			break;
	}
}
