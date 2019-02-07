//
//  Map.m
//  Event Horizon
//
//  Created by user on Sun Jan 26 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Map.h"
#import "Controller.h"
#import "aglString.h"
#import "main.h"
#import "Stack.h"
#import "Mission.h"
#import "EHButton.h"

float mapFactor = 3;			// map scaling
int mapX = 0, mapY = 0;			// map translation
extern int mX, mY;
Solarsystem *current;
BOOL shift = NO;

void setUpMap(){
	[Stack pushWithScene:@"Map"];
	current = sys->system;
	[EHButton storeSet:@"PreMap"];
	[EHButton clear];
	[EHButton newButton:@"Leave" atX:sys->screenWidth-164 y:50 width:128 callback:endMap withParam:NO active:YES isActive:nil];
}

void endMap(){
	if( [sys->itinerary count] > 0 && [sys->system->links containsObject:[sys->itinerary objectAtIndex:0]] )
		sys->systemIndex = [sys->system->links indexOfObject:[sys->itinerary objectAtIndex:0]];
	else {
		unsigned link = [sys->system->links indexOfObject:current];
		if( link != NSNotFound )
			sys->systemIndex = link;
	}	
	[Stack pop];
	mX = -1;
	mY = -1;
	[sys save];
	[EHButton recallSet:@"PreMap"];
}

void drawMap(){
	NSArray *systems;
	int i, index;
	Solarsystem *temp;
	NSMutableArray *flagSystems = [[NSMutableArray alloc] init];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport( 0, 128, sys->screenWidth-256, sys->screenHeight-128 );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, sys->screenWidth-256, 128, sys->screenHeight, 0, 1 );
	glMatrixMode( GL_MODELVIEW );
	systems = [Controller objectsOfType:@"system"];

	for( i = 0; i < [sys->missions count]; i++ ){
		Mission *m = [sys->missions objectAtIndex:i];
		[flagSystems addObject:m->endSystem];
	}

	for( index = 0; index < [systems count]; index++ ){
		const char *string;
		float dx, dy;
		GLint rasterPos[4];
		temp = [systems objectAtIndex:index];
		string = [temp->name cString];
		BOOL display = NO;

		dx = sys->screenWidth/2-128 + temp->x*mapFactor + mapX;
		dy = sys->screenHeight/2+64 + temp->z*mapFactor + mapY;
		
		if( mX >= dx-5 && mX <= dx+5 ){
			if( sys->screenHeight-mY >= dy-5 && sys->screenHeight-mY <= dy+5 ){
				current = temp;
				if( shift ){	// add temp to itinerary
					if( temp == [sys->itinerary lastObject] )
						[sys->itinerary removeLastObject];
					else {
						Solarsystem *dummy;
						if( [sys->itinerary count] == 0 )
							dummy = sys->system;
						else
							dummy = [sys->itinerary lastObject];
						if( [dummy->links indexOfObject:temp] != NSNotFound )
							[sys->itinerary addObject:temp];
					}
					shift = NO;
				}
			}
		}
	
		if( temp->displayOnMap ){
			glLoadIdentity();
			glColor3f( 1, 1, 1 );
			if( temp->gov == nil )
				glColor3f( .25, .25, .25 );
			else
				glColor3f( temp->gov->red, temp->gov->green, temp->gov->blue );
			if( temp == current )
				glColor3f( 0, 1, 0 );
			if( temp == sys->system )
				glColor3f( 0, 1, 1 );
			glTranslatef( dx, dy, 0 );
			glBindTexture( GL_TEXTURE_2D, sys->texture[NAVPLANET_TEXTURE] );
			glBegin( GL_QUADS );
				glTexCoord2f( 0, 0 );
				glVertex2f( -4, -4 );
				glTexCoord2f( 1, 0 );
				glVertex2f( 4, -4 );
				glTexCoord2f( 1, 1 );
				glVertex2f( 4, 4 );
				glTexCoord2f( 0, 1 );
				glVertex2f( -4, 4 );
			glEnd();
			glBindTexture( GL_TEXTURE_2D, 0 );
			glLoadIdentity();
			glColor3f( 1, 1, 1 );
			for( i = 0; i < [temp->links count]; i++ ){
				Solarsystem *s = [temp->links objectAtIndex:i];
				glBegin( GL_LINES );
				glVertex3f( dx, dy, -.5 );
				glVertex3f( sys->screenWidth/2-128 + s->x*mapFactor + mapX, sys->screenHeight/2+64 + s->z*mapFactor + mapY, -.5 );
				glEnd();
				if( !s->displayOnMap ){
					float x, y;
					
					x = sys->screenWidth/2-128 + s->x*mapFactor + mapX;
					y = sys->screenHeight/2+64 + s->z*mapFactor + mapY;
					
					glBindTexture( GL_TEXTURE_2D, sys->texture[NAVPLANET_TEXTURE] );
					glBegin( GL_QUADS );
						glTexCoord2f( 0, 0 );
						glVertex2f( x-4, y-4 );
						glTexCoord2f( 1, 0 );
						glVertex2f( x+4, y-4 );
						glTexCoord2f( 1, 1 );
						glVertex2f( x+4, y+4 );
						glTexCoord2f( 0, 1 );
						glVertex2f( x-4, y+4 );
					glEnd();
					glBindTexture( GL_TEXTURE_2D, 0 );					
				}
			}
		}
		if( [flagSystems containsObject:temp] ){
			glLoadIdentity();
			glTranslatef( dx, dy+6, 0 );
			glColor3f( 1, 0, 0 );
			glBegin( GL_TRIANGLES );
				glVertex3f( 0, 0, -.5 );
				glVertex3f( 3, 6, -.5 );
				glVertex3f( -3, 6, -.5 );
			glEnd();
		}
		if( !temp->displayOnMap ){
			for( i = 0; i < [temp->links count]; i++ ){
				if( ((Solarsystem*)[temp->links objectAtIndex:i])->displayOnMap ){
					display = YES;
					string = "<Unknown>";
					break;
				}
			}
		}
		if( temp->displayOnMap || display ){
			glColor3f( 1, 1, 1 );
			i = 0;
			glLoadIdentity();
			glTranslatef( 0, 0, .5 );
			glRasterPos3f( dx + 8, dy - 3, -1 );
			while( string[i] ){
				glGetIntegerv( GL_CURRENT_RASTER_POSITION, rasterPos );
				if( rasterPos[0] > sys->screenWidth-266 )
					break;
				DrawCStringGL( [[NSString stringWithFormat:@"%c", string[i++]] cString], font );
			}
		}
	}
	glLoadIdentity();
	glColor3f( 0, 1, 0 );
	glLineWidth( 3 );
	temp = sys->system;
	glBegin( GL_LINES );
	for( i = 0; i < [sys->itinerary count]; i++ ){
		Solarsystem *s = [sys->itinerary objectAtIndex:i];
		if( i == 0 && ![sys->system->links containsObject:s] )
			glColor3f( .25, .5, .25 );
		glVertex2f( sys->screenWidth/2-128 + temp->x*mapFactor + mapX, sys->screenHeight/2+64 + temp->z*mapFactor + mapY );
		glVertex2f( sys->screenWidth/2-128 + s->x*mapFactor + mapX, sys->screenHeight/2+64 + s->z*mapFactor + mapY );
		if( i == 0 && ![sys->system->links containsObject:s] )
			glColor3f( 0, 1, 0 );
		temp = s;
	}
	glEnd();
	glLineWidth( 1 );

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );

	glViewport( 0, 0, sys->screenWidth, sys->screenHeight );
	glColor3f( 1, 0, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBegin( GL_LINES );
		glVertex2f( 0, 127 );
		glVertex2f( sys->screenWidth-255, 127 );
		glVertex2f( sys->screenWidth-255, 127 );
		glVertex2f( sys->screenWidth-255, sys->screenHeight );
	glEnd();
	
	glColor3f( 0, 1, 0 );
	glRasterPos2f( sys->screenWidth-220, sys->screenHeight-35 );
	DrawCStringGL( "Itinerary", font );
	glColor3f( 1, 1, 0 );
	for( i = 0; i < [sys->itinerary count]; i++ ){
		glRasterPos2f( sys->screenWidth-220, sys->screenHeight-55-15*i );
		if( ((Solarsystem*)[sys->itinerary objectAtIndex:i])->displayOnMap )
			DrawCStringGL( [((Solarsystem*)[sys->itinerary objectAtIndex:i])->name cString], font );
		else
			DrawCStringGL( "<Unkown>", font );
	}
	
	[EHButton update];

	glColor3f( 1, 0, 0 );	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glRasterPos2f( 12, 100 );
	DrawCStringGL( "Name: ", font );
	DrawCStringGL( current->displayOnMap ? [current->name cString] : "<Unknown>", font );
	glRasterPos2f( 12, 88 );
	glColor3f( 1, 0, 0 );
	DrawCStringGL( "Ports: ", font );
	if( current->displayOnMap ){
		int i;
		NSString *string = @"";
		if( [current->planets count] >= 1 ){
			for( i = 0; i < ([current->planets count] - 1); i++ ){
				NSString *temp = [NSString stringWithFormat:@"%@, ", ((Planet*)[current->planets objectAtIndex:i])->name];
				string = [string stringByAppendingString:temp];
			}
			string = [string stringByAppendingString:((Planet*)[current->planets lastObject])->name];
		}
		else
			string = @"N/A";
		DrawCStringGL( [string cString], font );
	} else {
		DrawCStringGL( "<Unkown>", font );
	}
	glColor3f( 1, 0, 0 );
	glRasterPos2f( 12, 76 );
	DrawCStringGL( "Government: ", font );
	if( current->displayOnMap ){
		if( current->gov )
			DrawCStringGL( [current->gov->name cString], font );
		else
			DrawCStringGL( "N/A", font );		
	} else {
		DrawCStringGL( "<Unkown>", font );
	}
	
//	mX = -100;
//	mY = -100;
	[flagSystems release];
}

void mouseMap( int button, int state, int x, int y ){
	if( button == shiftKey || button == NSShiftKeyMask )
		shift = YES;
	if( state == 1 ){
		mapX += x - mX;
		mapY += mY - y;
	}
	mX = x;
	mY = y;
}

void keyMap( int key ){
	switch( key ){
		case '\r':
		case 3:
			endMap();
			break;
		case '+':
			mapFactor += .2;
			break;
		case '-':
			mapFactor -= .2;
			break;
		case 30:	// up arrow
		case NSUpArrowFunctionKey:
			mapY += 10;
			break;
		case 31:	// down arrow
		case NSDownArrowFunctionKey:
			mapY -= 10;
			break;
		case 28:	// key left
		case NSLeftArrowFunctionKey:
			mapX -= 10;
			break;
		case 29:	// right key
		case NSRightArrowFunctionKey:
			mapX += 10;
			break;
	}
}

void eventMap( EventRef event ){
	UInt32 class, kind;
	
	class = GetEventClass( event );
	kind = GetEventKind( event );
	
	switch( class ){
		case kEventClassKeyboard:
			if( kind == kEventRawKeyDown || kind == kEventRawKeyRepeat){
				if( ![sys functionKey] ){
					char theChar;
					GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &theChar );
					keyMap( theChar );
				}
			}
			break;
		case kEventClassMouse:
			if( kind == kEventMouseDown ){
				Point p;
				UInt32 mods;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				GetEventParameter( event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &mods );
				mouseMap( (mods & shiftKey), 0, p.h, p.v );
			} else if( kind == kEventMouseUp ){
				mouseMap( 0, 0, -1, -1 );
			} else if( kind == kEventMouseDragged ){
				Point p;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				mouseMap( 0, 1, p.h, p.v );
			}
			break;
	}
}

void nsEventMap( NSEvent *event ){
	switch( [event type] ){
		case NSLeftMouseDown:
			mouseMap( [event modifierFlags] & NSShiftKeyMask, 0, [event locationInWindow].x, sys->screenHeight-[event locationInWindow].y-22 );
			break;
		case NSLeftMouseUp:
			mouseMap( 0, 0, -1, -1 );
			break;
		case NSLeftMouseDragged:
			mouseMap( 0, 1, [event locationInWindow].x, sys->screenHeight-[event locationInWindow].y-22 );
			break;
		case NSKeyDown:
			if( ![sys functionKey] )
				keyMap( [[event characters] characterAtIndex:0] );
			break;
		default:
			break;
	}
}