//
//  Inventory.h
//  Event Horizon
//
//  Created by user on Sat Jul 12 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>

void setUpInventory();
void drawInventory();
void mouseInventory( int button, int state, int x, int y );
void keyInventory( int key );
void eventInventory( EventRef event );
//void eventInventory( EventRecord *event );
void nsEventInventory( NSEvent *event );
void endInventory();