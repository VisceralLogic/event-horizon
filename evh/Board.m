//
//  Board.m
//  Event Horizon
//
//  Created by Paul Dorman on Fri Sep 26 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Board.h"
#import "Stack.h"
#import "Spaceship.h"
#import "Controller.h"
#import "aglString.h"
#import "Weapon.h"
#import "Mod.h"
#import "Cargo.h"
#import "EHButton.h"

Spaceship *ship;
extern int mX, mY;
NSString *status;
NSMutableDictionary *ammo;
BOOL ammoFound = NO;
int smallestAmmo;
void money();
int boardHasMoney();
void fuel();
int boardHasFuel();
void takeAmmo();
int boardHasAmmo();
void cargo();
int boardHasCargo();

void setUpBoard(){
	[Stack pushWithScene:@"Board"];
	ship = sys->selection;
	status = [[NSString alloc] init];
	if( !ammoFound ){
		int i;

		smallestAmmo = -1;
		ammo = [[NSMutableDictionary alloc] initWithCapacity:3];
		for( i = 0; i < [ship->weapons count]; i++ ){
			Weapon *w = [ship->weapons objectAtIndex:i];
			if( !w->isPrimary && [sys hasMod:[Controller componentNamed:w->launcher->ID]] ){
				[ammo setObject:[NSNumber numberWithInt:w->count] forKey:w->ID];
				if( w->mass < smallestAmmo || smallestAmmo == -1 )
					smallestAmmo = w->mass;
			}
		}
		ammoFound = YES;
	}
	[EHButton clear];
	[EHButton newButton:@"Leave" atX:sys->screenWidth-164 y:50 width:128 callback:endBoard withParam:NO active:YES isActive:nil];
	[EHButton newButton:@"Money" atX:sys->screenWidth-164 y:220 width:128 callback:money withParam:NO active:NO isActive:boardHasMoney];
	[EHButton newButton:@"Fuel" atX:sys->screenWidth-164 y:260 width:128 callback:fuel withParam:NO active:NO isActive:boardHasFuel];
	[EHButton newButton:@"Ammo" atX:sys->screenWidth-164 y:300 width:128 callback:takeAmmo withParam:NO active:NO isActive:boardHasAmmo];
	[EHButton newButton:@"Cargo" atX:sys->screenWidth-164 y:340 width:128 callback:cargo withParam:NO active:NO isActive:boardHasCargo];
}

void endBoard(){
	[ammo release];
	ammoFound = NO;
	[Stack pop];
	mX = -1;
	mY = -1;
	[EHButton clear];
}

void drawBoard(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glColor3f( 0.5, 0.5, 0.5 );
	glLoadIdentity();
	glTranslatef( sys->screenWidth/2, sys->screenHeight/2, -100 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glScalef( 300, 300, 40 );
	glRotatef( 90, 0, 1, 0 );
	glRotatef( 15, 0, 0, 1 );
	glRotatef( 15*sys->t, 0, 1, 0 );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glDisable( GL_CULL_FACE );
	[ship drawObject];
	glEnable( GL_CULL_FACE );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	
	glColor3f( 0, 1, 0 );
	glLoadIdentity();
	glRasterPos2f( 20, sys->screenHeight-20 );
	DrawCStringGL( [[NSString stringWithFormat:@"%@", ship->name] cString], font );
	glRasterPos2f( 20, 20 );
	DrawCStringGL( [status cString], font );
	
	glLoadIdentity();

	[EHButton update];
}

void mouseBoard( int button, int state, int x, int y ){
	mX = x;
	mY = y;
}

void keyBoard( int key ){
	switch( key ){
		case '\r':
		case 3:
			endBoard();
			break;
		case 'C':
		case 'c':
			if( [ship->cargo count] > 0 && sys->cargoSpace > 0 )
				cargo();
			break;
		case 'a':
		case 'A':
			if( [ammo count] > 0 && sys->modSpace >= smallestAmmo )
				takeAmmo();
			break;
		case 'f':
		case 'F':
			if( sys->fuel < sys->maxFuel && ship->fuel > 0 )
				fuel();
			break;
		case 'm':
		case 'M':
			if( ship->money > 0 )
				money();
			break;
	}
}

void eventBoard( EventRef event ){
	UInt32 class, kind;
	
	class = GetEventClass( event );
	kind = GetEventKind( event );
	
	switch( class ){
		case kEventClassKeyboard:
			if( kind == kEventRawKeyDown || kind == kEventRawKeyRepeat){
				if( ![sys functionKey] ){
					char theChar;
					GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &theChar );
					keyBoard( theChar );
				}
			}
			break;
		case kEventClassMouse:
			if( kind == kEventMouseDown ){
				Point p;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				mouseBoard( 0, 0, p.h, p.v );
			} else if( kind == kEventMouseUp ){
				mouseBoard( 0, 0, -1, -1 );
			}
			break;
	}
}

void nsEventBoard( NSEvent *event ){
	switch( [event type] ){
		case NSLeftMouseDownMask:
			mouseBoard( 0, 0, [event locationInWindow].x, sys->screenHeight-[event locationInWindow].y-22 );
			break;
		case NSLeftMouseUpMask:
			mouseBoard( 0, 0, -1, -1 );
			break;
		case NSKeyDownMask:
			if( ![sys functionKey] )
				keyBoard( [[event characters] characterAtIndex:0] );
			break;
		default:
			break;
	}
}

int boardHasCargo(){
	if([ship->cargo count] > 0 && sys->cargoSpace > 0)
		return 1;
	return 0;
}

void cargo(){
	NSMutableString *temp = [[NSMutableString alloc] initWithString:@"You took "];
	int index = 0;
	while( sys->cargoSpace > 0 && [ship->cargo count] > 0 ){
		Cargo *c, *c2;
		int amount, i;
		BOOL found = NO;
		
		c = [ship->cargo objectAtIndex:0];
		amount = sys->cargoSpace > c->size ? c->size : sys->cargoSpace;
		for( i = 0; i < [sys->cargo count]; i++ ){
			c2 = [sys->cargo objectAtIndex:i];
			if( [c->name isEqualToString:c2->name] ){
				found = YES;
				break;
			}
		}
		if( !found ){
			c2 = [[[Cargo alloc] init] autorelease];
			c2->name = c->name;
		}
		c2->size += amount;
		c->size -= amount;
		if( !found )
			[sys->cargo addObject:c2];
		sys->cargoSpace -= amount;
		sys->mass += amount;
		sys->ACCELERATION = sys->thrust/(sys->mass*sys->compensation);
		if( c->size == 0 )
			[ship->cargo removeObject:c];
		// update status string
		if( index++ > 0 )
			[temp appendString:@", "];
		[temp appendFormat:@"%d ton", amount];
		if( amount > 1 )
			[temp appendString:@"s"];
		[temp appendFormat:@" of %@", c2->name];
	}
	[status release];
	status = temp;
}

int boardHasAmmo(){
	if([ammo count] > 0 && sys->modSpace >= smallestAmmo)
		return 1;
	return 0;
}

void takeAmmo(){
	NSEnumerator *enumerator = [ammo keyEnumerator];
	id key;
	int number, transfer, i;
	Weapon *weap;
	
	while( (key = [enumerator nextObject]) ){		// for each ammo...
		number = [[ammo objectForKey:key] intValue];
		weap = [Controller componentNamed:key];
		transfer = sys->modSpace/weap->mass;		// add as many as possible
		for( i = 0; i < transfer; i++ ){
			[sys addMod:weap];
		}
		if( transfer < number )
			[ammo setObject:[NSNumber numberWithInt:(number-transfer)] forKey:weap->ID];
		else
			[ammo removeObjectForKey:weap->ID];
	}
	[status release];
	status = [[NSString alloc] initWithString:@"You took all the ammo you could fit aboard your ship"];
}

int boardHasFuel(){
	if(ship->fuel > 0 && sys->fuel < sys->maxFuel)
		return 1;
	return 0;
}

void fuel(){
	if( ship->fuel + sys->fuel >= sys->maxFuel ){
		sys->fuel = sys->maxFuel;
		[status release];
		status = [[NSString alloc] initWithString:@"Your fuel is now at capacity"];
	} else {
		sys->fuel += ship->fuel;
		[status release];
		status = [[NSString alloc] initWithFormat:@"You got %f units of fuel", ship->fuel];
	}
}

int boardHasMoney(){
	return ship->money;
}

void money(){
	sys->money += ship->money;
	[status release];
	status = [[NSString alloc] initWithFormat:@"You took %d credits", ship->money];
	ship->money = 0;
}