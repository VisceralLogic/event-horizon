//
//  EHMenu.m
//  Event Horizon
//
//  Created by Paul Dorman on 2/16/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "EHMenu.h"
#import "EHMenuItem.h"
#import <OpenGL/gl.h>

#define NEW_PRESS( key ) ((keys[key.index] & key.val) && !(menuKeys[key.index] & key.val))

NSMutableDictionary *menus;		// all existing menus
EHMenu *menu;					// currently active menu
KeyMap2 menuKeys;				// last key press

@implementation EHMenu

+ (void) newMenuWithTitle:(NSString *)title items:(NSArray *)items{
	EHMenu *menu = [[[EHMenu alloc] init] autorelease];
	EHMenuItem *titleItem = [[EHMenuItem alloc] initWithName:title hotkey:"ESC" menu:nil width:128 target:[EHMenu class] action:@selector(upOne)];
	int i, maxWidth = 0;
	
	menu->items = [[NSMutableArray arrayWithArray:items] retain];
	titleItem->state = SELECTED;
	[menu->items insertObject:titleItem atIndex:0];
	for( i = 0; i < [items count]; i++ ){
		EHMenuItem *item = [items objectAtIndex:i];
		if( item->width > maxWidth )
			maxWidth = item->width;
	}
	for( i = 0; i < [items count]; i++ ){
		EHMenuItem *item = [items objectAtIndex:i];
		item->width = maxWidth;
	}
	[menus setObject:menu forKey:title];
}

+ (void) displayMenu:(NSString *)title{
	[EHMenu displayMenu:title atX:400 y:400];
}

+ (void) displayMenu:(NSString *)title atX:(int)x y:(int)y{
	int i;
	EHMenu *old = menu;

	for( i = 0; i < 4; i ++ )
		menuKeys[i] = 0xFFFFFFFF;

	menu = [menus objectForKey:title];
	if( !menu )
		return;
	menu->parent = old;
	menu->x = x;
	menu->y = y;
}

+ (void) initialize{
	menus = [[NSMutableDictionary alloc] init];
}

+ (void) update:(KeyMap2)keys{
	int h, i;
	EHMenuItem *itemToDo = nil;
		
	if( !menu )		// no active menus
		return;

	h = menu->y;
	beginOrtho();
	for( i = 0; i < [menu->items count]; i++ ){
		EHMenuItem *item = [menu->items objectAtIndex:i];
		keyCode key = item->hotkey;
		if( NEW_PRESS( key ) )
			itemToDo = item;
		h -= [item drawAtX:menu->x y:h];
	}
	if( itemToDo ){
		if( itemToDo->menu )
			[EHMenu displayMenu:itemToDo->menu atX:menu->x y:menu->y];
		else {
			EHMenu *parent = menu;
			[itemToDo perform];
			if( parent == menu )
				menu = nil;
		}		
	}
	
	for( i = 0; i < 4; i++ )
		menuKeys[i] = keys[i];
	
	endOrtho();
	glBindTexture( GL_TEXTURE_2D, 0 );
	glColor3f( 1, 1, 1 );
}

+ (BOOL) active{
	return (menu != nil);
}

+ (void) upOne{
	menu = menu->parent;
}

@end
