//
//  Prefs.h
//  Event Horizon
//
//  Created by user on Sun Jan 26 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>

void drawPrefs();
void mousePrefs( int button, int state, int x, int y );
void keyPrefs( int key );
void setUpPrefs();
void endPrefs();
void eventPrefs( EventRef event );
//void eventPrefs( EventRecord *event );
void nsEventPrefs( NSEvent *event );
