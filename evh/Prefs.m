//
//  Prefs.m
//  Event Horizon
//
//  Created by user on Sun Jan 26 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Prefs.h"
#import "Controller.h"
#import "KeyMapNames.h"
#import "Stack.h"
#import "EHButton.h"
#import "aglString.h"

GLuint prefTex = 0;				// texture used to display prefs
int prefIndex = 0;				// which pref is selected?
int tempIndex[END_OF_KEYS];		// stores old values of sys->index
int tempVal[END_OF_KEYS];		// stores old values of sys->val
float tempFloat[END_OF_FLOATS];	// stores old values of sys->floatVal
BOOL edit;						// user wants to edit a pref?
BOOL new, newSet;				// a value was changed
char *prefKeyNames[END_OF_KEYS] = { "Forward:", "Stop:", "Right:", "Left:", "Orbit:", "Autpilot:", "Next Planet:", "Next Ship:", "Select Ship:", "Map:", "Next System:", "Hyperspace:", "Rear View:", "3rd Person View:", "Fire Primary:", "Next Secondary:", "Fire Secondary:", "Board:", "Recall:", "Land:", "Inventory:", "Preferences:", "Pause:", "Up:", "Down:", "Fullscreen:", "Throttle Up:", "Throttle Down:", "Console:", "Afterburner:", "Inertial Mode:" };
BOOL paused;					// use to handle game pause when entering prefs

void prefButton( NSString *button );

void drawPrefs(){
	int i;
	KeyMap2 keys;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f( 1, 1, 1 );
	glLoadIdentity();

	glBindTexture( GL_TEXTURE_2D, 0 );
	glColor3f( 0, 1, 0 );
	for( i = 0; i < END_OF_KEYS; i++ ){
		glRasterPos2f( sys->screenWidth/2-320+(i/24)*214, sys->screenHeight/2+260-i%24*20 );
		DrawCStringGL( prefKeyNames[i], font );
	}
	glColor3f( 1, 1, 0 );
	for( i = 0; i < END_OF_KEYS; i++ ){
		keyCode key;
		key.index = sys->index[i];
		key.val = sys->val[i];
		glRasterPos2f( sys->screenWidth/2-190+i/24*214, sys->screenHeight/2+260-i%24*20 );
		if( prefIndex != i || !edit || sin( 4*pi*sys->t ) > 0 )
			DrawCStringGL( keyName( key ), font );
	}
	glColor3f( 0, 1, 0 );
	glRasterPos2f( sys->screenWidth/2-320+214*2, sys->screenHeight/2+260 );
	DrawCStringGL( "Continuous Mouse:", font );
	glColor3f( 1, 1, 0 );
	glRasterPos2f( sys->screenWidth/2-190+214*2, sys->screenHeight/2+260 );
	if( prefIndex != -24 || !edit || sin( 4*pi*sys->t ) > 0 ){
		if( sys->floatVal[MOUSE] == 1.0 )
			DrawCStringGL( "*", font );
		else
			DrawCStringGL( "o", font );
	}
	glColor3f( 0, 1, 0 );
	glRasterPos2f( sys->screenWidth/2-320+214*2, sys->screenHeight/2+240 );
	DrawCStringGL( "Invert Y:", font );
	glColor3f( 1, 1, 0 );
	glRasterPos2f( sys->screenWidth/2-190+214*2, sys->screenHeight/2+240 );
	if( prefIndex != -23 || !edit || sin( 4*pi*sys->t ) > 0 ){
		if( sys->floatVal[INVERT_Y] == 1.0 )
			DrawCStringGL( "*", font );
		else
			DrawCStringGL( "o", font );
	}
	glColor3f( 0, 1, 0 );
	glRasterPos2f( sys->screenWidth/2-320+214*2, sys->screenHeight/2+220 );
	DrawCStringGL( "Sensitivity:", font );
	glColor3f( 1, 1, 0 );
	glRasterPos2f( sys->screenWidth/2-190+214*2, sys->screenHeight/2+220 );
	if( prefIndex != -22 || !edit || sin( 4*pi*sys->t ) > 0 ){
		if( sys->floatVal[INVERT_Y] == 1.0 )
			DrawCStringGL( "<COMING>", font );
		else
			DrawCStringGL( "<COMING>", font );
	}

	getKeys( keys );
	if( edit && !newSet ){
		BOOL change = NO;
		for( i = 0; i <= 4; i++ ){
			if( keys[i] != 0 ){
			if( edit )
				change = YES;
				break;
			}
		}
		if( change )
			newSet = YES;
	}
	if( edit && newSet && !(keys[1] & 0x10000000) && !(keys[2] & 0x100000) && !(keys[1] & 0x2000) ){
		for( i = 0; i <= 4; i++ ){
			if( keys[i] != 0 ){
				if( prefIndex >= 0 ){
					sys->index[prefIndex] = i;
					sys->val[prefIndex] = keys[i];
				} else {
					float f= sys->floatVal[24+prefIndex];
					if( f == 0.0 )
						sys->floatVal[24+prefIndex] = 1.0;
					else if( f == 1.0 )
						sys->floatVal[24+prefIndex] = 0.0;
				}
			}
		}
	}
	
	[EHButton update];

	glColor3f( 0, 0, 1 );
	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, 0 );
	glTranslatef( 0, 0, -.05 );
	if( prefIndex != END_OF_KEYS && prefIndex >= 0 ){
		glBegin( GL_QUADS );
			glVertex2f( sys->screenWidth/2-195+prefIndex/24*214, sys->screenHeight/2+258-prefIndex%24*20 );
			glVertex2f( sys->screenWidth/2-120+prefIndex/24*214, sys->screenHeight/2+258-prefIndex%24*20 );
			glVertex2f( sys->screenWidth/2-120+prefIndex/24*214, sys->screenHeight/2+274-prefIndex%24*20 );
			glVertex2f( sys->screenWidth/2-195+prefIndex/24*214, sys->screenHeight/2+274-prefIndex%24*20 );
		glEnd();
	} else if( prefIndex < 0 && prefIndex < END_OF_FLOATS-24 ){
		glBegin( GL_QUADS );
			glVertex2f( sys->screenWidth/2-195+2*214, sys->screenHeight/2+258-(24+prefIndex)%24*20 );
			glVertex2f( sys->screenWidth/2-120+2*214, sys->screenHeight/2+258-(24+prefIndex)%24*20 );
			glVertex2f( sys->screenWidth/2-120+2*214, sys->screenHeight/2+274-(24+prefIndex)%24*20 );
			glVertex2f( sys->screenWidth/2-195+2*214, sys->screenHeight/2+274-(24+prefIndex)%24*20 );
		glEnd();		
	}
	glLoadIdentity();
}

void mousePrefs( int button, int state, int x, int y ){
	if( x > sys->screenWidth/2-320 && x < sys->screenWidth/2+320 && y < sys->screenHeight/2+280 && y > sys->screenHeight/2-220 ){
		int tempIndex = prefIndex;
		x -= sys->screenWidth/2-320;
		x /= 214;
		y = (sys->screenHeight/2+272-y)/20;
		if( x < 2 ){
			prefIndex = y + x*24;
			if( prefIndex > END_OF_KEYS )
				prefIndex = END_OF_KEYS;
		} else {
			prefIndex = y-24;
			if( prefIndex > END_OF_FLOATS-24 )
				prefIndex = END_OF_KEYS;
		}
		if( prefIndex != tempIndex ){
			edit = NO;
			newSet = NO;
		}
	}
}

void keyPrefs( int key ){
	if( key == 13 || key == 3 ){
		edit = !edit;
		if( prefIndex == END_OF_KEYS )
			endPrefs();
	}
	if( edit )
		return;
	if( key == 30 || key == NSUpArrowFunctionKey ){ // up arrow
		prefIndex--;
		if( prefIndex == END_OF_KEYS-1 )
			prefIndex = END_OF_FLOATS-25;
		if( prefIndex == -1 )
			prefIndex = END_OF_KEYS;
		if( prefIndex == -25 )
			prefIndex = END_OF_KEYS-1;
		newSet = NO;
	} else if( key == 31 || key == NSDownArrowFunctionKey ){
		prefIndex++;
		if( prefIndex == END_OF_KEYS-1 )
			prefIndex = -24;
		if( prefIndex == END_OF_FLOATS-24 )
			prefIndex = END_OF_KEYS;
		if( prefIndex == END_OF_KEYS+1 )
			prefIndex = 0;
		newSet = NO;
	}
}

void endPrefs(){
	[sys save];
	[Stack pop];
	if( paused ){
		sys->pause = YES;
		paused = NO;
	}
	[EHButton clear];
}

void setUpPrefs(){
	int i;
	
	for( i = 0; i < END_OF_KEYS; i++ ){
		tempIndex[i] = sys->index[i];
		tempVal[i] = sys->val[i];
	}
	for( i = 0; i < END_OF_FLOATS; i++ ){
		tempFloat[i] = sys->floatVal[i];
	}
	prefIndex = 0;
	[Stack pushWithScene:@"Prefs"];
	edit = NO;
	new = NO;
	newSet = NO;
	if( sys->pause ){
		paused = YES;
		sys->pause = NO;
	}
	
	[EHButton newButton:@"Accept" atX:sys->screenWidth-164 y:50 width:128 callback:prefButton withParam:YES active:YES isActive:nil];
	[EHButton newButton:@"Cancel" atX:sys->screenWidth-528 y:50 width:128 callback:prefButton withParam:YES active:YES isActive:nil];
}

void prefButton( NSString *button ){
	int i;
	if( [button isEqualToString:@"Cancel"] ){
		for( i = 0; i < END_OF_KEYS; i++ ){
			sys->val[i] = tempVal[i];
			sys->index[i] = tempIndex[i];
		}
		for( i = 0; i < END_OF_FLOATS; i++ ){
			sys->floatVal[i] = tempFloat[i];
		}
	}
	endPrefs();
}

void eventPrefs( EventRef event ){
	UInt32 class, kind;
	
	class = GetEventClass( event );
	kind = GetEventKind( event );
	switch( class ){
		case kEventClassKeyboard:
			if( kind == kEventRawKeyDown || kind == kEventRawKeyRepeat ){
				char theChar;
				GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &theChar );
				keyPrefs( theChar );
			}
			break;
		case kEventClassMouse:
			if( kind == kEventMouseDown ){
				Point p;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				mousePrefs( 1, 1, p.h, sys->screenHeight-p.v );
			}
			break;
	}
}

void nsEventPrefs( NSEvent *event ){
	switch( [event type] ){
		case NSLeftMouseDown:
			mousePrefs( 1, 1, [event locationInWindow].x, [event locationInWindow].y+22 );
			break;
		case NSKeyDown:
			keyPrefs( [[event characters] characterAtIndex:0] );
			break;
		default:
			break;
	}
}