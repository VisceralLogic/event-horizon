//
//  EHButton.m
//  Event Horizon
//
//  Created by Paul Dorman on Fri Jun 17 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import "ControllerGraphics.h"
#import "SpaceObject.h"
#import "EHButton.h"
#import "aglString.h"
#import "main.h"
#import "Texture.h"

GLuint buttonTex;
NSMutableArray *buttons;
EHButton *mouseButton;
NSMutableDictionary *sets;
NSMutableArray *toDelete;

@implementation EHButton

+ (void) initialize{
	int tempWidth, tempHeight;
	[Texture loadTexture:@"Button.png" into:&buttonTex withWidth:&tempWidth height:&tempHeight];
	buttons = [[NSMutableArray alloc] init];
	sets = [[NSMutableDictionary alloc] init];
	toDelete = [[NSMutableArray alloc] init];
}

+ (void) update{
	int i;
	for( i = 0; i < [buttons count]; i++ ){
		[(EHButton *)[buttons objectAtIndex:i] draw];
	}
	[toDelete removeAllObjects];
}

+ (void) removeByName:(NSString *)name{
	int i;
	for( i = 0; i < [buttons count]; i++ ){
		EHButton *b = [buttons objectAtIndex:i];
		if( [b->text isEqualToString:name] ){
			[toDelete addObject:b];
			[buttons removeObjectAtIndex:i];
		}
	}
}

+ (void) remove:(EHButton *)button{
	if( button ){
		[toDelete addObject:button];
		[buttons removeObject:button];
	}
}

+ (EHButton *) buttonWithName:(NSString *)name{
	int i;
	for( i = 0; i < [buttons count]; i++ ){
		EHButton *b = [buttons objectAtIndex:i];
		if( [b->text isEqualToString:name] )
			return b;
	}
	return nil;
}

+ (void) setName:(NSString *)newName forName:(NSString *)oldName{
	[EHButton buttonWithName:oldName]->text = newName;
}

+ (void) clear{
	while( [buttons count] ){
		[toDelete addObject:[buttons lastObject]];
		[buttons removeLastObject];
	}
}

+ (void) storeSet:(NSString *)name{
	[sets setObject:[NSArray arrayWithArray:buttons] forKey:name];
}

+ (void) recallSet:(NSString *)name{
	[self clear];
	[buttons autorelease];
	buttons = [[NSMutableArray arrayWithArray:[sets objectForKey:name]] retain];
}

+ (void) clearSet:(NSString *)name{
	[sets removeObjectForKey:name];
}

+ newButton:(NSString *)text atX:(int)x y:(int)y width:(int)width callback:(void *)callback withParam:(BOOL)param active:(BOOL)active isActive:(int *)isActive{
	EHButton *b = [[[EHButton alloc] init] autorelease];
	b->text = text;
	b->x = x;
	b->y = y;
	if( !param )
		b->call = callback;
	else
		b->callParam = callback;
	b->active = active;
	b->width = width;
	b->isActive = isActive;
	
	[buttons insertObject:b atIndex:0];
	return b;
}

+ (void) handleEvent:(EventRef)event{
	UInt32 kind = GetEventKind(event);
	
	if( ![buttons count] )
		return;
	
	switch( GetEventClass(event) ){
		case kEventClassMouse:
			if( kind == kEventMouseDown ){
				UInt16 button;
				Point p;
				GetEventParameter( event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(UInt16), NULL, &button );
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				p.v = gScreenHeight-p.v;
				if( button == kEventMouseButtonPrimary ){
					int i;
					for( i = 0; i < [buttons count]; i++ ){
						EHButton *b = [buttons objectAtIndex:i];
						if( [b inBoundsX:p.h y:p.v] ){
							b->pressed = YES;
							b->mouseIn = YES;
							mouseButton = b;
							break;
						}
					}
				}
			} else if( kind == kEventMouseUp ){
				UInt16 button;
				Point p;
				GetEventParameter( event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(UInt16), NULL, &button );
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				p.v = gScreenHeight-p.v;
				if( button == kEventMouseButtonPrimary && mouseButton ){
					if( [mouseButton inBoundsX:p.h y:p.v] ){
						if( mouseButton->pressed )
							mouseButton->clicked = YES;
					} else {
						mouseButton->pressed = NO;
					}
					mouseButton = nil;
				}
			} else if( kind == kEventMouseMoved || kind == kEventMouseDragged ){
				Point p;
				int i;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				p.v = gScreenHeight-p.v;
				for( i = 0; i < [buttons count]; i++ ){
					EHButton *b = [buttons objectAtIndex:i];
					if( [b inBoundsX:p.h y:p.v] )
						b->mouseIn = YES;
					else
						b->mouseIn = NO;
				}
			}
			break;
		default:
			break;
	}
}

+ (void) handleNSEvent:(NSEvent *)event{
	if( ![buttons count] )
		return;
	
	switch( [event type] ){
			int i;
			
		case NSLeftMouseDown:
			for( i = 0; i < [buttons count]; i++ ){
				EHButton *b = [buttons objectAtIndex:i];
				if( [b inBoundsX:[event locationInWindow].x y:[event locationInWindow].y+22] ){
					b->pressed = YES;
					b->mouseIn = YES;
					mouseButton = b;
					break;
				}				
			}
			break;
		case NSLeftMouseUp:
			if( mouseButton ){
				if( [mouseButton inBoundsX:[event locationInWindow].x y:[event locationInWindow].y+22] ){
					if( mouseButton->pressed )
						mouseButton->clicked = YES;
				} else {
					mouseButton->pressed = NO;
				}
				mouseButton = nil;
			}
			break;
		case NSMouseMoved:
		case NSLeftMouseDragged:
			for( i = 0; i < [buttons count]; i++ ){
				EHButton *b = [buttons objectAtIndex:i];
				if( [b inBoundsX:(int)[event locationInWindow].x y:(int)[event locationInWindow].y+22] )
					b->mouseIn = YES;
				else
					b->mouseIn = NO;
			}
			break;
		default:
			break;
	}
}

- (void) draw{
	float delta;
	
	if( isActive )
		active = isActive();
	
	if( active ){
		if( pressed && mouseIn )
			delta = -.5;
		else if( mouseIn )
			delta = -.25;
		else
			delta = 0;
	} else
		delta = -.75;
	
	glDisable( GL_DEPTH_TEST );
	
	glColor3f( 1, 1, 1 );
	glBindTexture( GL_TEXTURE_2D, buttonTex );
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
	
	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	if( active ){
		if( pressed && mouseIn )
			glColor3f( .75, .88, 1 );
		else if( mouseIn )
			glColor3f( 0, 1, 1 );
		else
			glColor3f( 0, 1, 0 );
	} else
		glColor3f( .2, .3, .3 );
	glRasterPos2i( x+20, y+9 );
	DrawCStringGL( [text cString], buttonFont );
	
	glEnable( GL_DEPTH_TEST );
	
	if( pressed && clicked ){   // fresh click
		pressed = NO;
		if( call ){
			call();
		}
		if( callParam )
			callParam( text );
	} else if( clicked ){
		clicked = NO;
	}
}

- (BOOL) clicked{
	return clicked;
}

- (BOOL) inBoundsX:(int)_x y:(int)_y{
	return (_x >= x && _x <= x+width && _y >= y && _y <= y+32);
}

@end