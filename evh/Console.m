//
//  Console.m
//  Event Horizon
//
//  Created by Paul Dorman on Sat Feb 26 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import "Console.h"
#import "Controller.h"
#import "main.h"
#import "aglString.h"
#import "ControllerThreading.h"

NSMutableArray *text;
NSMutableArray *size;
NSMutableString *input;

extern GLuint buttonFont;

@implementation Console

+ (void) initialize{
	text = [[NSMutableArray alloc] initWithCapacity:5];
	size = [[NSMutableArray alloc] initWithCapacity:5];
	input = [[NSMutableString alloc] init];
}

+ (void) draw{
	int i;

	beginOrtho();
	
	glColor3f( 1, 1, 1 );
	glBindTexture( GL_TEXTURE_2D, sys->texture[INFOTAB_TEXTURE] );
	glBegin( GL_QUADS );
		glTexCoord2i( 0, 1 );
		glVertex2f( 0.175*sys->screenWidth, sys->screenHeight );
		glTexCoord2i( 0, 0 );
		glVertex2f( 0.175*sys->screenWidth, .778*sys->screenHeight );
		glTexCoord2i( 1, 0 );
		glVertex2f( 0.825*sys->screenWidth, .778*sys->screenHeight );
		glTexCoord2i( 1, 1 );
		glVertex2f( 0.825*sys->screenWidth, sys->screenHeight );
	glEnd();

	glBindTexture( GL_TEXTURE_2D, 0 );
	glColor3f( 0, 1, 0 );
	
	glRasterPos2i( 0.175*sys->screenWidth+32, 0.778*sys->screenHeight+16 );
	DrawCStringGL( [input cString], buttonFont );
	for( i = 0; i < [text count]; i++ ){
		glRasterPos2i( 0.175*sys->screenWidth+32, 0.778*sys->screenHeight+16*(i+2) );
		DrawCStringGL( [[text objectAtIndex:i] cString], buttonFont );
	}

	endOrtho();
}

+ (void) appendChar:(char)c{
	if( c == 27 ){	// escape
		if( [input length] > 0 ){
			[input release];
			input = [[NSMutableString alloc] init];
		} else {
			[Console end];
		}
	} else if( c == 13 || c == 3 ){	// return or enter
		[text insertObject:input atIndex:0];
		[sys->interpreter execute:input];
		[input release];
		input = [[NSMutableString alloc] init];
		if( [text count] > 10 )
			[text removeLastObject];
	} else if( c == 8 || c == 127 ){	// backspace
		if( [input length] > 0 )
			[input deleteCharactersInRange:NSMakeRange([input length]-1,1)];
	} else {
		[input appendFormat:@"%c",c];
	}
}

+ (void) print:(NSString *)str{
	if( ![str isKindOfClass:[NSString class]] )
		[text insertObject:[str description] atIndex:0];
	else
		[text insertObject:str atIndex:0];
	if( [text count] > 10 )
		[text removeLastObject];
}

+ (void) dump:(id) data{
	NSLog( [data description] );
}

+ (void) setup{
	eventScene = eventConsole;
	nsEventScene = nsEventConsole;
	sys->console = YES;
	sys->pause = YES;
	input = [[NSMutableString alloc] init];
	[pauseLock lock];
}

+ (void) end{
	eventScene = NULL;
	nsEventScene = NULL;
	sys->console = NO;	
	sys->pause = NO;
	[input release];
	[pauseLock unlock];
}

@end

void eventConsole( EventRef event ){
	char c;

	if( GetEventClass( event ) != kEventClassKeyboard )
		return;

	switch( GetEventKind( event ) ){
		case kEventRawKeyDown:
		case kEventRawKeyRepeat:
			GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &c );
			[Console appendChar:c];
		default:
			break;
	}
}

void nsEventConsole( NSEvent *event ){
	switch( [event type] ){
		case NSKeyDown:
			[Console appendChar:[[event characters] characterAtIndex:0]];
			break;
		default:
			break;
	}
}
