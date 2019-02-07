//
//  Board.h
//  Event Horizon
//
//  Created by Paul Dorman on Fri Sep 26 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>

void setUpBoard();
void drawBoard();
void mouseBoard( int button, int state, int x, int y );
void keyBoard( int key );
void eventBoard( EventRef event );
//void eventBoard( EventRecord *event );
void nsEventBoard( NSEvent *event );
void endBoard();