/*
 *  KeyMapNames.h
 *  Event Horizon
 *
 *  Created by user on Wed Jan 01 2003.
 *  Copyright (c) 2003 Paul Dorman. All rights reserved.
 *
 */

typedef struct{
	int index;
	unsigned int val;
} keyCode;

const char *keyValue( int index, int val );
const char *keyName( keyCode key );
keyCode fromName( const char *name );