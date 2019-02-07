//
//  EHMenuItem.m
//  Event Horizon
//
//  Created by Paul Dorman on 2/16/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "EHMenuItem.h"
#import "Texture.h"
#import "aglString.h"
#import "main.h"
#import "Controller.h"

GLuint menuItemTex;

@implementation EHMenuItem

- initWithName:(NSString *)name hotkey:(const char *)keyName menu:(EHMenu *)menu width:(int)width target:(id)target action:(SEL)action{
	self = [self initWithName:name hotkey:keyName menu:menu width:width target:target action:action object:nil];
	objectPresent = NO;
	return self;
}

- initWithName:(NSString *)name hotkey:(const char *)keyName menu:(EHMenu *)menu width:(int)width target:(id)target action:(SEL)action object:(id)object{
	self = [super init];
	self->name = [name retain];
	self->hotkey = fromName( keyName );
	self->menu = [menu retain];
	self->state = ACTIVE;
	self->width = width;
	self->target = target;
	self->action = action;
	self->object = object;
	objectPresent = YES;
	return self;
}

- (int) drawAtX:(int)x y:(int)y{
	float delta;
	int nameOffset = 32;
	
	if( state == ACTIVE )
		delta = 0;
	else if( state == SELECTED )
		delta = -.25;
	else
		delta = -.75;
	
	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	// draw name
	if( hotkey.val == 0x2000 && hotkey.index == 1 ){
		nameOffset = 0;
	} else {
		if( state == ACTIVE )
			glColor3f( 0, 1, 1 );
		else if( state == SELECTED )
			glColor3f( 0, 1, 1 );
		else
			glColor3f( .2, .3, .3 );
		glRasterPos2i( x+5, y+9 );
		DrawCStringGL( keyName( hotkey ), buttonFont );		
	}
	if( state == ACTIVE )
		glColor3f( 0, 1, 0 );
	else if( state == SELECTED )
		glColor3f( 0, 1, 1 );
	else
		glColor3f( .2, .3, .3 );
	glRasterPos2i( x+5+nameOffset, y+9 );
	DrawCStringGL( [name cString], buttonFont );
	
	// draw background
	glColor3f( 1, 1, 1 );
	glBindTexture( GL_TEXTURE_2D, menuItemTex );
	glBegin( GL_QUADS );
	if( width == 128 ){
		glTexCoord2f( 0, delta-.25 );
		glVertex2i( x, y );
		glTexCoord2f( 1, delta-.25 );
		glVertex2i( x+128, y );
		glTexCoord2f( 1, delta );
		glVertex2i( x+128, y+32 );
		glTexCoord2f( 0, delta );
		glVertex2i( x, y+32 );
	} else {
		// left segment
		glTexCoord2f( 0, delta-.25 );
		glVertex2i( x, y );
		glTexCoord2f( .25, delta-.25 );
		glVertex2i( x+32, y );
		glTexCoord2f( .25, delta );
		glVertex2i( x+32, y+32 );
		glTexCoord2f( 0, delta );
		glVertex2i( x, y+32 );
		
		//middle segment
		glTexCoord2f( .25, delta-.25 );
		glVertex2i( x+32, y );
		glTexCoord2f( .75, delta-.25 );
		glVertex2i( x+width-32, y );
		glTexCoord2f( .75, delta );
		glVertex2i( x+width-32, y+32 );
		glTexCoord2f( .25, delta );
		glVertex2i( x+32, y+32 );
		
		// right segment
		glTexCoord2f( .75, delta-.25 );
		glVertex2i( x+width-32, y );
		glTexCoord2f( 1, delta-.25 );
		glVertex2i( x+width, y );
		glTexCoord2f( 1, delta );
		glVertex2i( x+width, y+32 );
		glTexCoord2f( .75, delta );
		glVertex2i( x+width-32, y+32 );
	}
	glEnd();	
	
	return 32;
}

- (void) perform{
	if( target && [target respondsToSelector:action] ){
		if( objectPresent )
			[target performSelector:action withObject:object];
		else
			[target performSelector:action];
	}
}

@end
