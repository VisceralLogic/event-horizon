//
//  Player.h
//  Event Horizon
//
//  Created by user on Sun Mar 09 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Carbon/carbon.h>
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "Controller.h"

void drawPlayerScreen();
void setUpPlayerTex();
void beginPlayer();
void endPlayer( BOOL newPlayer );
void eventPlayer( EventRef event );
void nsEventPlayer( NSEvent *event );
void playerKeyFunc( unsigned char key );
void playerMouse();