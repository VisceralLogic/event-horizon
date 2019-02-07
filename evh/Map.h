//
//  Map.h
//  Event Horizon
//
//  Created by user on Sun Jan 26 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>

void setUpMap();
void drawMap();
void mouseMap( int button, int state, int x, int y );
void keyMap( int key );
void eventMap( EventRef event );
//void eventMap( EventRecord *event );
void nsEventMap( NSEvent *event );
void endMap();