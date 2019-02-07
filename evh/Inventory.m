//
//  Inventory.m
//  Event Horizon
//
//  Created by user on Sat Jul 12 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Inventory.h"
#import "Controller.h"
#import "Stack.h"
#import "aglString.h"
#import "Cargo.h"
#import "Mod.h"
#import "Weapon.h"
#import "Mission.h"
#import "EHButton.h"

extern int mX, mY;

void setUpInventory(){
	[Stack pushWithScene:@"Inventory"];
	[EHButton storeSet:@"PreInv"];
	[EHButton clear];
	[EHButton newButton:@"Leave" atX:sys->screenWidth-164 y:50 width:128 callback:endInventory withParam:NO active:YES isActive:nil];
}

void endInventory(){
	[Stack pop];
	mX = -1;
	mY = -1;
	[EHButton recallSet:@"PreInv"];
}

void drawInventory(){
	int i, j;
	int total = 0;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, 0 );
	glColor3f( 0, 1, 0 );
	glRasterPos2f( 50, sys->screenHeight-50 );
	DrawCStringGL( "Cargo", font );
	glColor3f( 1, 1, 0 );
	for( i = 0; i < [sys->cargo count]; i++ ){
		Cargo *cargo = [sys->cargo objectAtIndex:i];
		glRasterPos2f( 50, sys->screenHeight-70-15*i );
		DrawCStringGL( [cargo->name cString], font );
		glRasterPos2f( 200, sys->screenHeight-70-15*i );
		DrawCStringGL( [[NSString stringWithFormat:@"%d ton", cargo->size] cString], font );
		if( cargo->size > 1 )
			DrawCStringGL( "s", font );
		total += cargo->size;
	}
	glColor3f( 0, 1, 0 );
	glRasterPos2f( 50, sys->screenHeight-75-15*i );
	DrawCStringGL( "Used", font );
	glRasterPos2f( 200, sys->screenHeight-75-15*i );
	DrawCStringGL( [[NSString stringWithFormat:@"%d ton", total] cString], font );
	if( total > 1 )
		DrawCStringGL( "s", font );
	glRasterPos2f( 50, sys->screenHeight-90-15*i );
	DrawCStringGL( "Free", font );
	glRasterPos2f( 200, sys->screenHeight-90-15*i );
	DrawCStringGL( [[NSString stringWithFormat:@"%d ton", sys->cargoSpace] cString], font );
	if( sys->cargoSpace > 1 )
		DrawCStringGL( "s", font );
	
	glRasterPos2f( 350, sys->screenHeight-50 );
	DrawCStringGL( "Mods", font );
	glColor3f( 1, 1, 0 );
	total = 0;
	for( i = 0; i < [sys->mods count]; i++ ){
		Mod *mod = [sys->mods objectAtIndex:i];
		glRasterPos2f( 350, sys->screenHeight-70-15*i );
		DrawCStringGL( [mod->name cString], font );
		glRasterPos2f( 500, sys->screenHeight-70-15*i );
		DrawCStringGL( [[NSString stringWithFormat:@"%dx", mod->count] cString], font );
		total += mod->mass*mod->count;
	}
	for( j = 0; j < [sys->weapons count]; j++ ){
		Weapon *weap = [sys->weapons objectAtIndex:j];
		glRasterPos2f( 350, sys->screenHeight-70-15*i-15*j );
		DrawCStringGL( [weap->name cString], font );
		glRasterPos2f( 500, sys->screenHeight-70-15*i-15*j );
		DrawCStringGL( [[NSString stringWithFormat:@"%dx", weap->count] cString], font );
		total += weap->mass*weap->count;
	}
	glColor3f( 0, 1, 0 );
	glRasterPos2f( 350, sys->screenHeight-75-15*i-15*j );
	DrawCStringGL( "Used", font );
	glRasterPos2f( 500, sys->screenHeight-75-15*i-15*j );
	DrawCStringGL( [[NSString stringWithFormat:@"%d ton", total] cString], font );
	if( total > 1 )
		DrawCStringGL( "s", font );
	glRasterPos2f( 350, sys->screenHeight-90-15*i-15*j );
	DrawCStringGL( "Free", font );
	glRasterPos2f( 500, sys->screenHeight-90-15*i-15*j );
	DrawCStringGL( [[NSString stringWithFormat:@"%d ton", sys->modSpace] cString], font );
	if( sys->cargoSpace > 1 )
		DrawCStringGL( "s", font );
	
	glColor3f( 0, 1, 0 );
	glRasterPos2f( 650, sys->screenHeight-50 );
	DrawCStringGL( "Missions", font );
	glColor3f( 1, 1, 0 );
	for( i = 0; i < [sys->missions count]; i++ ){
		Mission *misn = [sys->missions objectAtIndex:i];
		glRasterPos2f( 650, sys->screenHeight-70-15*i );
		DrawCStringGL( [misn->title cString], font );
	}
	
	glRasterPos2f( 50, 95 );
	DrawCStringGL( [[NSString stringWithFormat:@"Shields: %d%% (%d)", 100*(int)sys->shields/sys->maxShield, (int)sys->shields] cString], font );
	glRasterPos2f( 50, 80 );
	DrawCStringGL( [[NSString stringWithFormat:@"Armor: %d%% (%d)", 100*sys->armor/sys->maxArmor, sys->armor] cString], font );
	{
		NSString *string = [NSString stringWithFormat:@"Fuel: %f", sys->fuel];
		NSRange range = [string rangeOfString:@"."];
		string = [string substringToIndex:range.location+3];
		glRasterPos2f( 50, 65 );
		DrawCStringGL( [string cString], font );
	}
	glRasterPos2f( 50, 50 );
	DrawCStringGL( [[NSString stringWithFormat:@"Money: %d", sys->money] cString], font );
	glRasterPos2f( 350, 95 );
	DrawCStringGL( [[NSString stringWithFormat:@"Max speed: %d", (int)(sys->MAX_VELOCITY*50)] cString], font );
	glRasterPos2f( 350, 80 );
	DrawCStringGL( [[NSString stringWithFormat:@"Acceleration: %d", (int)(sys->ACCELERATION*50)] cString], font );
	glRasterPos2f( 350, 65 );
	DrawCStringGL( [[NSString stringWithFormat:@"Max rotation: %d", (int)(sys->MAX_ANGULAR_VELOCITY)] cString], font );
	glRasterPos2f( 350, 50 );
	DrawCStringGL( [[NSString stringWithFormat:@"Angular accel: %d", (int)(sys->ANGULAR_ACCELERATION)] cString], font );
	
	glRasterPos2f( 600, 100 );
	DrawCStringGL( [[sys->date descriptionWithCalendarFormat:@"%a, %b %e, %Y" timeZone:nil locale:nil] cString], font );
	
	[EHButton update];
}

void mouseInventory( int button, int state, int x, int y ){
	mX = x;
	mY = y;
}

void keyInventory( int key ){
	switch( key ){
		case '\r':
		case 3:
			endInventory();
			break;
	}
}

void eventInventory( EventRef event ){
	UInt32 class, kind;
	
	class = GetEventClass( event );
	kind = GetEventKind( event );
	
	switch( class ){
		case kEventClassKeyboard:
			if( kind == kEventRawKeyDown || kind == kEventRawKeyRepeat){
				if( ![sys functionKey] ){
					char theChar;
					GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &theChar );
					keyInventory( theChar );
				}
			}
			break;
		case kEventClassMouse:
			if( kind == kEventMouseDown ){
				Point p;
				UInt32 mods;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				GetEventParameter( event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &mods );
				mouseInventory( (mods & shiftKey), 0, p.h, p.v );
			} else if( kind == kEventMouseUp ){
				mouseInventory( 0, 0, -1, -1 );
			}
			break;
	}
}

void nsEventInventory( NSEvent *event ){
	switch( [event type] ){
		case NSLeftMouseDown:
			mouseInventory( [event modifierFlags] & NSShiftKeyMask, 0, [event locationInWindow].x, sys->screenHeight-[event locationInWindow].y-22 );
			break;
		case NSLeftMouseUp:
			mouseInventory( 0, 0, -1, -1 );
			break;
		case NSKeyDown:
			if( ![sys functionKey] )
				keyInventory( [[event characters] characterAtIndex:0] );
			break;
		default:
			break;
	}
}