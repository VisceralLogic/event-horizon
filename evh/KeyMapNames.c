/*
 *  KeyMapNames.c
 *  Event Horizon
 *
 *  Created by user on Wed Jan 01 2003.
 *  Copyright (c) 2003 Paul Dorman. All rights reserved.
 *
 */

#include "KeyMapNames.h"

const char *keyName( keyCode key ){
	return keyValue( key.index, key.val );
}

const char *keyValue( int index, int val ){
	int keys[5];
	keys[0] = 0;
	keys[1] = 0;
	keys[2] = 0;
	keys[3] = 0;
	keys[4] = 0;
	keys[index] = val;
	
#ifdef __ppc__

	if( keys[0] & 0x1 )
		return "=";
	else if( keys[0] & 0x2 )
		return "9";
	else if( keys[0] & 0x4 )
		return "7";
	else if( keys[0] & 0x8 )
		return "-";
	else if( keys[0] & 0x10 )
		return "8";
	else if( keys[0] & 0x20 )
		return "0";
	else if( keys[0] & 0x40 )
		return "]";
	else if( keys[0] & 0x80 )
		return "O";
	else if( keys[0] & 0x100 )
		return "Y";
	else if( keys[0] & 0x200 )
		return "T";
	else if( keys[0] & 0x400 )
		return "1";
	else if( keys[0] & 0x800 )
		return "2";
	else if( keys[0] & 0x1000 )
		return "3";
	else if( keys[0] & 0x2000 )
		return "4";
	else if( keys[0] & 0x4000 )
		return "6";
	else if( keys[0] & 0x8000 )
		return "5";
	else if( keys[0] & 0x10000 )
		return "C";
	else if( keys[0] & 0x20000 )	// room 0x40000
		return "V";
	else if( keys[0] & 0x80000 )
		return "B";
	else if( keys[0] & 0x100000 )
		return "Q";
	else if( keys[0] & 0x200000 )
		return "W";
	else if( keys[0] & 0x400000 )
		return "E";
	else if( keys[0] & 0x800000 )
		return "R";
	else if( keys[0] & 0x1000000 )
		return "A";
	else if( keys[0] & 0x2000000 )
		return "S";
	else if( keys[0] & 0x4000000 )
		return "D";
	else if( keys[0] & 0x8000000 )
		return "F";
	else if( keys[0] & 0x10000000 )
		return "H";
	else if( keys[0] & 0x20000000 )
		return "G";
	else if( keys[0] & 0x40000000 )
		return "Z";
	else if( keys[0] & 0x80000000 )
		return "X";
	else if( keys[1] & 0x1 )
		return "SHIFT";
	else if( keys[1] & 0x2 )
		return "CAPS";
	else if( keys[1] & 0x4 )
		return "OPT";
	else if( keys[1] & 0x8 )		// room 0x10, 0x20, 0x40, 0x80
		return "CTRL";
	else if( keys[1] & 0x100 )
		return "TAB";
	else if( keys[1] & 0x200 )
		return "SPACE";
	else if( keys[1] & 0x400 )
		return "~";
	else if( keys[1] & 0x800 )		// room 0x1000
		return "DELETE";
	else if( keys[1] & 0x2000 )		// room 0x4000
		return "ESC";
	else if( keys[1] & 0x8000 )
		return "CMND";
	else if( keys[1] & 0x10000 )
		return "K";
	else if( keys[1] & 0x20000 )
		return ";";
	else if( keys[1] & 0x40000 )
		return "\\";
	else if( keys[1] & 0x80000 )
		return ",";
	else if( keys[1] & 0x100000 )
		return "/";
	else if( keys[1] & 0x200000 )
		return "N";
	else if( keys[1] & 0x400000 )
		return "M";
	else if( keys[1] & 0x800000 )
		return ".";
	else if( keys[1] & 0x1000000 )
		return "U";
	else if( keys[1] & 0x2000000 )
		return "[";
	else if( keys[1] & 0x4000000 )
		return "I";
	else if( keys[1] & 0x8000000 )
		return "P";
	else if( keys[1] & 0x10000000 )
		return "RETURN";
	else if( keys[1] & 0x20000000 )
		return "L";
	else if( keys[1] & 0x40000000 )
		return "J";
	else if( keys[1] & 0x80000000 )
		return "'";
	else if( keys[2] & 0x1 )
		return "PAD 6";
	else if( keys[2] & 0x2 )			// room 0x4
		return "PAD 7";
	else if( keys[2] & 0x8 )
		return "PAD 8";
	else if( keys[2] & 0x10 )			// room 0x20, 0x40, 0x80, 0x100
		return "PAD 9";
	else if( keys[2] & 0x200 )
		return "PAD =";
	else if( keys[2] & 0x400 )
		return "PAD 0";
	else if( keys[2] & 0x800 )
		return "PAD 1";
	else if( keys[2] & 0x1000 )
		return "PAD 2";
	else if( keys[2] & 0x2000 )
		return "PAD 3";
	else if( keys[2] & 0x4000 )
		return "PAD 4";
	else if( keys[2] & 0x8000 )			// room 0x10000, 0x20000, 0x40000
		return "PAD 5";
	else if( keys[2] & 0x80000 )
		return "PAD /";
	else if( keys[2] & 0x100000 )		// room 0x200000
		return "ENTER";
	else if( keys[2] & 0x400000 )		// room 0x800000, 0x1000000
		return "PAD -";
	else if( keys[2] & 0x2000000 )		// roomo 0x4000000
		return "PAD .";
	else if( keys[2] & 0x8000000 )		// room 0x10000000
		return "PAD *";
	else if( keys[2] & 0x20000000 )		// room 0x40000000
		return "PAD +";
	else if( keys[2] & 0x80000000 )
		return "CLEAR";
	else if( keys[3] & 0x1 )
		return "F2";
	else if( keys[3] & 0x2 )
		return "PG DN";
	else if( keys[3] & 0x4 )
		return "F1";
	else if( keys[3] & 0x8 )
		return "LEFT";
	else if( keys[3] & 0x10 )
		return "RIGHT";
	else if( keys[3] & 0x20 )
		return "DOWN";
	else if( keys[3] & 0x40 )			// room 0x80, 0x100
		return "UP";
	else if( keys[3] & 0x200 )
		return "F15";
	else if( keys[3] & 0x400 )
		return "HELP";
	else if( keys[3] & 0x800 )
		return "HOME";
	else if( keys[3] & 0x1000 )
		return "PG UP";
	else if( keys[3] & 0x2000 )
		return "DEL";
	else if( keys[3] & 0x4000 )
		return "F4";
	else if( keys[3] & 0x8000 )			// room 0x10000
		return "END";
	else if( keys[3] & 0x20000 )		// room 0x40000
		return "F13";
	else if( keys[3] & 0x80000 )		// room 0x100000
		return "F14";
	else if( keys[3] & 0x200000 )		// room 0x400000
		return "F10";
	else if( keys[3] & 0x800000 )
		return "F12";
	else if( keys[3] & 0x1000000 )
		return "F5";
	else if( keys[3] & 0x2000000 )
		return "F6";
	else if( keys[3] & 0x4000000 )
		return "F7";
	else if( keys[3] & 0x8000000 )
		return "F3";
	else if( keys[3] & 0x10000000 )
		return "F8";
	else if( keys[3] & 0x20000000 )		// room 0x40000000
		return "F9";
	else if( keys[3] & 0x80000000 )		// END DEFAULT KEYBOARD EVENTS
		return "F11";
	
#else
	if( keys[1] & 0x200000 )
		return "ESC";
	else if( keys[3] & 0x4000000 )
		return "F1";
	else if( keys[3] & 0x1000000 )
		return "F2";
	else if( keys[3] & 0x8 )
		return "F3";
	else if( keys[3] & 0x400000 )
		return "F4";
	else if( keys[3] & 0x1 )
		return "F5";
	else if( keys[3] & 0x2 )
		return "F6";
	else if( keys[3] & 0x4 )
		return "F7";
	else if( keys[3] & 0x10 )
		return "F8";
	else if( keys[3] & 0x20 )
		return "F9";
	else if( keys[3] & 0x2000 )
		return "F10";
	else if( keys[3] & 0x80 )
		return "F11";
	else if( keys[3] & 0x8000 )
		return "F12";
	else if( keys[3] & 0x200 )
		return "F13";
	else if( keys[3] & 0x800 )
		return "F14";
	else if( keys[3] & 0x20000 )
		return "F15";
	else if( keys[3] & 0x400 )
		return "F16";
	else if( keys[1] & 0x40000 )
		return "~";
	else if( keys[0] & 0x40000 )
		return "1";
	else if( keys[0] & 0x80000 )
		return "2";
	else if( keys[0] & 0x100000 )
		return "3";
	else if( keys[0] & 0x200000 )
		return "4";
	else if( keys[0] & 0x800000 )
		return "5";
	else if( keys[0] & 0x400000 )
		return "6";
	else if( keys[0] & 0x4000000 )
		return "7";
	else if( keys[0] & 0x10000000 )
		return "8";
	else if( keys[0] & 0x2000000 )
		return "9";
	else if( keys[0] & 0x20000000 )
		return "0";
	else if( keys[0] & 0x8000000 )
		return "-";
	else if( keys[0] & 0x1000000 )
		return "+";
	else if( keys[1] & 0x80000 )
		return "DELETE";
	else if(keys[1] & 0x10000 )
		return "TAB";
	else if( keys[0] & 0x1000 )
		return "Q";
	else if( keys[0] & 0x2000 )
		return "W";
	else if( keys[0] & 0x4000 )
		return "E";
	else if( keys[0] & 0x8000 )
		return "R";
	else if( keys[0] & 0x20000 )
		return "T";
	else if( keys[0] & 0x10000 )
		return "Y";
	else if( keys[1] & 0x1 )
		return "U";
	else if( keys[1] & 0x4 )
		return "I";
	else if( keys[0] & 0x80000000 )
		return "O";
	else if( keys[1] & 0x8 )
		return "P";
	else if( keys[1] & 0x2 )
		return "[";
	else if( keys[0] & 0x40000000 )
		return "]";
	else if( keys[1] & 0x400 )
		return "\\";
	else if( keys[1] & 0x2000000 )
		return "CAPS";
	else if( keys[0] & 0x1 )
		return "A";
	else if( keys[0] & 0x2 )
		return "S";
	else if( keys[0] & 0x4 )
		return "D";
	else if( keys[0] & 0x8 )
		return "F";
	else if( keys[0] & 0x20 )
		return "G";
	else if( keys[0] & 0x10 )
		return "H";
	else if( keys[1] & 0x40 )
		return "J";
	else if( keys[1] & 0x100 )
		return "K";
	else if( keys[1] & 0x20 )
		return "L";
	else if( keys[1] & 0x200 )
		return ";";
	else if( keys[1] & 0x80 )
		return "'";
	else if( keys[1] & 0x10 )
		return "RETURN";
	else if( keys[1] & 0x1000000 )
		return "SHIFT";
	else if( keys[0] & 0x40 )
		return "Z";
	else if( keys[0] & 0x80 )
		return "X";
	else if( keys[0] & 0x100 )
		return "C";
	else if( keys[0] & 0x200 )
		return "V";
	else if( keys[0] & 0x800 )
		return "B";
	else if( keys[1] & 0x2000 )
		return "N";
	else if( keys[1] & 0x4000 )
		return "M";
	else if( keys[1] & 0x800 )
		return ",";
	else if( keys[1] & 0x8000 )
		return ".";
	else if( keys[1] & 0x1000 )
		return "/";
	else if( keys[1] & 0x8000000 )
		return "CTRL";
	else if( keys[1] & 0x4000000 )
		return "OPT";
	else if( keys[1] & 0x800000 )
		return "CMND";
	else if( keys[1] & 0x20000 )
		return "SPACE";
	else if( keys[3] & 0x40000 )
		return "HELP";
	else if( keys[3] & 0x80000 )
		return "HOME";
	else if( keys[3] & 0x100000 )
		return "PG UP";
	else if( keys[3] & 0x200000 )
		return "DEL";
	else if( keys[3] & 0x800000 )
		return "END";
	else if( keys[3] & 0x2000000 )
		return "PG DN";
	else if( keys[3] & 0x40000000 )
		return "UP";
	else if( keys[3] & 0x8000000 )
		return "LEFT";
	else if( keys[3] & 0x20000000 )
		return "DOWN";
	else if( keys[3] & 0x10000000 )
		return "RIGHT";
	else if( keys[2] & 0x80 )
		return "CLEAR";
	else if( keys[2] & 0x20000 )
		return "PAD =";
	else if( keys[2] & 0x800 )
		return "PAD /";
	else if( keys[2] & 0x8 )
		return "PAD *";
	else if( keys[2] & 0x2000000 )
		return "PAD 7";
	else if( keys[2] & 0x8000000 )
		return "PAD 8";
	else if( keys[2] & 0x10000000 )
		return "PAD 9";
	else if( keys[2] & 0x4000 )
		return "PAD -";
	else if( keys[2] & 0x400000 )
		return "PAD 4";
	else if( keys[2] & 0x800000 )
		return "PAD 5";
	else if( keys[2] & 0x1000000 )
		return "PAD 6";
	else if( keys[2] & 0x20 )
		return "PAD +";
	else if( keys[2] & 0x80000 )
		return "PAD 1";
	else if( keys[2] & 0x100000 )
		return "PAD 2";
	else if( keys[2] & 0x200000 )
		return "PAD 3";
	else if( keys[2] & 0x40000 )
		return "PAD 0";
	else if( keys[2] & 0x2 )
		return "PAD .";
	else if( keys[2] & 0x1000 )
		return "ENTER";
	else if( keys[1] & 0x80000000 )
		return "FN";
#endif
	
	else if( keys[4] & 0x1 )
		return "MOUSE";
	else if( keys[4] & 0x2 )
		return "RT MOUSE";
	else if( keys[4] & 0x4 )
		return "WHL UP";
	else if( keys[4] & 0x8 )
		return "WHL DOWN";
	else
		return "N/A";
}

keyCode fromName( const char *name ){
	keyCode key;

#ifdef __ppc__
	if( strcmp( name, "=" ) == 0 ){
		key.val = 0x1;
		key.index = 0;
	} else if( strcmp( name, "9" ) == 0 ){
		key.val = 0x2;
		key.index = 0;
	} else if( strcmp( name, "7" ) == 0 ){
		key.val = 0x4;
		key.index = 0;
	} else if( strcmp( name, "-" ) == 0 ){
		key.val = 0x8;
		key.index = 0;
	} else if( strcmp( name, "8" ) == 0 ){
		key.val = 0x10;
		key.index = 0;
	} else if( strcmp( name, "0" ) == 0 ){
		key.val = 0x20;
		key.index = 0;
	} else if( strcmp( name, "]" ) == 0 ){
		key.val = 0x40;
		key.index = 0;
	} else if( strcmp( name, "O" ) == 0 ){
		key.val = 0x80;
		key.index = 0;
	} else if( strcmp( name, "Y" ) == 0 ){
		key.val = 0x100;
		key.index = 0;
	} else if( strcmp( name, "T" ) == 0 ){
		key.val = 0x200;
		key.index = 0;
	} else if( strcmp( name, "1" ) == 0 ){
		key.val = 0x400;
		key.index = 0;
	} else if( strcmp( name, "2" ) == 0 ){
		key.val = 0x800;
		key.index = 0;
	} else if( strcmp( name, "3" ) == 0 ){
		key.val = 0x1000;
		key.index = 0;
	} else if( strcmp( name, "4" ) == 0 ){
		key.val = 0x2000;
		key.index = 0;
	} else if( strcmp( name, "6" ) == 0 ){
		key.val = 0x4000;
		key.index = 0;
	} else if( strcmp( name, "5" ) == 0 ){
		key.val = 0x8000;
		key.index = 0;
	} else if( strcmp( name, "C" ) == 0 ){
		key.val = 0x10000;
		key.index = 0;
	} else if( strcmp( name, "V" ) == 0 ){
		key.val = 0x20000;
		key.index = 0;
	} else if( strcmp( name, "B" ) == 0 ){
		key.val = 0x80000;
		key.index = 0;
	} else if( strcmp( name, "Q" ) == 0 ){
		key.val = 0x100000;
		key.index = 0;
	} else if( strcmp( name, "W" ) == 0 ){
		key.val = 0x200000;
		key.index = 0;
	} else if( strcmp( name, "E" ) == 0 ){
		key.val = 0x400000;
		key.index = 0;
	} else if( strcmp( name, "R" ) == 0 ){
		key.val = 0x800000;
		key.index = 0;
	} else if( strcmp( name, "A" ) == 0 ){
		key.val = 0x1000000;
		key.index = 0;
	} else if( strcmp( name, "S" ) == 0 ){
		key.val = 0x2000000;
		key.index = 0;
	} else if( strcmp( name, "D" ) == 0 ){
		key.val = 0x4000000;
		key.index = 0;
	} else if( strcmp( name, "F" ) == 0 ){
		key.val = 0x8000000;
		key.index = 0;
	} else if( strcmp( name, "H" ) == 0 ){
		key.val = 0x10000000;
		key.index = 0;
	} else if( strcmp( name, "G" ) == 0 ){
		key.val = 0x20000000;
		key.index = 0;
	} else if( strcmp( name, "Z" ) == 0 ){
		key.val = 0x40000000;
		key.index = 0;
	} else if( strcmp( name, "X" ) == 0 ){
		key.val = 0x80000000;
		key.index = 0;
	} else if( strcmp( name, "SHIFT" ) == 0 ){
		key.val = 0x1;
		key.index = 1;
	} else if( strcmp( name, "CAPS" ) == 0 ){
		key.val = 0x2;
		key.index = 1;
	} else if( strcmp( name, "OPT" ) == 0 ){
		key.val = 0x4;
		key.index = 1;
	} else if( strcmp( name, "CTRL" ) == 0 ){
		key.val = 0x8;
		key.index = 1;
	} else if( strcmp( name, "TAB" ) == 0 ){
		key.val = 0x100;
		key.index = 1;
	} else if( strcmp( name, "SPACE" ) == 0 ){
		key.val = 0x200;
		key.index = 1;
	} else if( strcmp( name, "~" ) == 0 ){
		key.val = 0x400;
		key.index = 1;
	} else if( strcmp( name, "DELETE" ) == 0 ){
		key.val = 0x800;
		key.index = 1;
	} else if( strcmp( name, "ESC" ) == 0 ){
		key.val = 0x2000;
		key.index = 1;
	} else if( strcmp( name, "CMND" ) == 0 ){
		key.val = 0x8000;
		key.index = 1;
	} else if( strcmp( name, "K" ) == 0 ){
		key.val = 0x10000;
		key.index = 1;
	} else if( strcmp( name, ";" ) == 0 ){
		key.val = 0x20000;
		key.index = 1;
	} else if( strcmp( name, "\\" ) == 0 ){
		key.val = 0x40000;
		key.index = 1;
	} else if( strcmp( name, "," ) == 0 ){
		key.val = 0x80000;
		key.index = 1;
	} else if( strcmp( name, "/" ) == 0 ){
		key.val = 0x100000;
		key.index = 1;
	} else if( strcmp( name, "N" ) == 0 ){
		key.val = 0x200000;
		key.index = 1;
	} else if( strcmp( name, "M" ) == 0 ){
		key.val = 0x400000;
		key.index = 1;
	} else if( strcmp( name, "." ) == 0 ){
		key.val = 0x800000;
		key.index = 1;
	} else if( strcmp( name, "U" ) == 0 ){
		key.val = 0x1000000;
		key.index = 1;
	} else if( strcmp( name, "[" ) == 0 ){
		key.val = 0x2000000;
		key.index = 1;
	} else if( strcmp( name, "I" ) == 0 ){
		key.val = 0x4000000;
		key.index = 1;
	} else if( strcmp( name, "P" ) == 0 ){
		key.val = 0x8000000;
		key.index = 1;
	} else if( strcmp( name, "RETURN" ) == 0 ){
		key.val = 0x10000000;
		key.index = 1;
	} else if( strcmp( name, "L" ) == 0 ){
		key.val = 0x20000000;
		key.index = 1;
	} else if( strcmp( name, "J" ) == 0 ){
		key.val = 0x40000000;
		key.index = 1;
	} else if( strcmp( name, "'" ) == 0 ){
		key.val = 0x80000000;
		key.index = 1;
	} else if( strcmp( name, "PAD 6" ) == 0 ){
		key.val = 0x1;
		key.index = 2;
	} else if( strcmp( name, "PAD 7" ) == 0 ){
		key.val = 0x2;
		key.index = 2;
	} else if( strcmp( name, "PAD 8" ) == 0 ){
		key.val = 0x8;
		key.index = 2;
	} else if( strcmp( name, "PAD 9" ) == 0 ){
		key.val = 0x10;
		key.index = 2;
	} else if( strcmp( name, "PAD =" ) == 0 ){
		key.val = 0x200;
		key.index = 2;
	} else if( strcmp( name, "PAD 0" ) == 0 ){
		key.val = 0x400;
		key.index = 2;
	} else if( strcmp( name, "PAD 1" ) == 0 ){
		key.val = 0x800;
		key.index = 2;
	} else if( strcmp( name, "PAD 2" ) == 0 ){
		key.val = 0x1000;
		key.index = 2;
	} else if( strcmp( name, "PAD 3" ) == 0 ){
		key.val = 0x2000;
		key.index = 2;
	} else if( strcmp( name, "PAD 4" ) == 0 ){
		key.val = 0x4000;
		key.index = 2;
	} else if( strcmp( name, "PAD 5" ) == 0 ){
		key.val = 0x8000;
		key.index = 2;
	} else if( strcmp( name, "PAD /" ) == 0 ){
		key.val = 0x80000;
		key.index = 2;
	} else if( strcmp( name, "ENTER" ) == 0 ){
		key.val = 0x100000;
		key.index = 2;
	} else if( strcmp( name, "PAD -" ) == 0 ){
		key.val = 0x400000;
		key.index = 2;
	} else if( strcmp( name, "PAD ." ) == 0 ){
		key.val = 0x2000000;
		key.index = 2;
	} else if( strcmp( name, "PAD *" ) == 0 ){
		key.val = 0x8000000;
		key.index = 2;
	} else if( strcmp( name, "PAD +" ) == 0 ){
		key.val = 0x20000000;
		key.index = 2;
	} else if( strcmp( name, "CLEAR" ) == 0 ){
		key.val = 0x80000000;
		key.index = 2;
	} else if( strcmp( name, "F2" ) == 0 ){
		key.val = 0x1;
		key.index = 3;
	} else if( strcmp( name, "PG DN" ) == 0 ){
		key.val = 0x2;
		key.index = 3;
	} else if( strcmp( name, "F1" ) == 0 ){
		key.val = 0x4;
		key.index = 3;
	} else if( strcmp( name, "LEFT" ) == 0 ){
		key.val = 0x8;
		key.index = 3;
	} else if( strcmp( name, "RIGHT" ) == 0 ){
		key.val = 0x10;
		key.index = 3;
	} else if( strcmp( name, "DOWN" ) == 0 ){
		key.val = 0x20;
		key.index = 3;
	} else if( strcmp( name, "UP" ) == 0 ){
		key.val = 0x40;
		key.index = 3;
	} else if( strcmp( name, "F15" ) == 0 ){
		key.val = 0x200;
		key.index = 3;
	} else if( strcmp( name, "HELP" ) == 0 ){
		key.val = 0x400;
		key.index = 3;
	} else if( strcmp( name, "HOME" ) == 0 ){
		key.val = 0x800;
		key.index = 3;
	} else if( strcmp( name, "PG UP" ) == 0 ){
		key.val = 0x1000;
		key.index = 3;
	} else if( strcmp( name, "DEL" ) == 0 ){
		key.val = 0x2000;
		key.index = 3;
	} else if( strcmp( name, "F4" ) == 0 ){
		key.val = 0x4000;
		key.index = 3;
	} else if( strcmp( name, "END" ) == 0 ){
		key.val = 0x8000;
		key.index = 3;
	} else if( strcmp( name, "F13" ) == 0 ){
		key.val = 0x20000;
		key.index = 3;
	} else if( strcmp( name, "F14" ) == 0 ){
		key.val = 0x80000;
		key.index = 3;
	} else if( strcmp( name, "F10" ) == 0 ){
		key.val = 0x200000;
		key.index = 3;
	} else if( strcmp( name, "F12" ) == 0 ){
		key.val = 0x800000;
		key.index = 3;
	} else if( strcmp( name, "F5" ) == 0 ){
		key.val = 0x1000000;
		key.index = 3;
	} else if( strcmp( name, "F6" ) == 0 ){
		key.val = 0x2000000;
		key.index = 3;
	} else if( strcmp( name, "F7" ) == 0 ){
		key.val = 0x4000000;
		key.index = 3;
	} else if( strcmp( name, "F3" ) == 0 ){
		key.val = 0x8000000;
		key.index = 3;
	} else if( strcmp( name, "F8" ) == 0 ){
		key.val = 0x10000000;
		key.index = 3;
	} else if( strcmp( name, "F9" ) == 0 ){
		key.val = 0x20000000;
		key.index = 3;
	} else if( strcmp( name, "F11" ) == 0 ){
		key.val = 0x80000000;
		key.index = 3;
	}
	
#else
	if( strcmp( name, "ESC" ) == 0 ){
		key.val = 0x200000;
		key.index = 1;
	}else if( strcmp( name, "F1" ) == 0 ){
		key.val = 0x4000000;
		key.index = 3;
	} else if( strcmp( name, "F2" ) == 0 ){
		key.val = 0x1000000;
		key.index = 3;
	} else if( strcmp( name, "F3" ) == 0 ){
		key.val = 0x8;
		key.index = 3;
	} else if( strcmp( name, "F4" ) == 0 ){
		key.val = 0x400000;
		key.index = 3;
	} else if( strcmp( name, "F5" ) == 0 ){
		key.val = 0x1;
		key.index = 3;
	} else if( strcmp( name, "F6" ) == 0 ){
		key.val = 0x2;
		key.index = 3;
	} else if( strcmp( name, "F7" ) == 0 ){
		key.val = 0x4;
		key.index = 3;
	} else if( strcmp( name, "F8" ) == 0 ){
		key.val = 0x10;
		key.index = 3;
	} else if( strcmp( name, "F9" ) == 0 ){
		key.val = 0x20;
		key.index = 3;
	} else if( strcmp( name, "F10" ) == 0 ){
		key.val = 0x2000;
		key.index = 3;
	} else if( strcmp( name, "F11" ) == 0 ){
		key.val = 0x80;
		key.index = 3;
	} else if( strcmp( name, "F12" ) == 0 ){
		key.val = 0x8000;
		key.index = 3;
	} else if( strcmp( name, "F13" ) == 0 ){
		key.val = 0x200;
		key.index = 3;
	} else if( strcmp( name, "F14" ) == 0 ){
		key.val = 0x800;
		key.index = 3;
	} else if( strcmp( name, "F15" ) == 0 ){
		key.val = 0x20000;
		key.index = 3;
	} else if( strcmp( name, "F16" ) == 0 ){
		key.val = 0x400;
		key.index = 3;
	} else if( strcmp( name, "~" ) == 0 ){
		key.val = 0x40000;
		key.index = 1;
	} else if( strcmp( name, "1" ) == 0 ){
		key.val = 0x40000;
		key.index = 0;
	} else if( strcmp( name, "2" ) == 0 ){
		key.val = 0x80000;
		key.index = 0;
	} else if( strcmp( name, "3" ) == 0 ){
		key.val = 0x100000;
		key.index = 0;
	} else if( strcmp( name, "4" ) == 0 ){
		key.val = 0x200000;
		key.index = 0;
	} else if( strcmp( name, "5" ) == 0 ){
		key.val = 0x800000;
		key.index = 0;
	} else if( strcmp( name, "6" ) == 0 ){
		key.val = 0x400000;
		key.index = 0;
	} else if( strcmp( name, "7" ) == 0 ){
		key.val = 0x4000000;
		key.index = 0;
	} else if( strcmp( name, "8" ) == 0 ){
		key.val = 0x10000000;
		key.index = 0;
	} else if( strcmp( name, "9" ) == 0 ){
		key.val = 0x2000000;
		key.index = 0;
	} else if( strcmp( name, "0" ) == 0 ){
		key.val = 0x20000000;
		key.index = 0;
	} else if( strcmp( name, "-" ) == 0 ){
		key.val = 0x8000000;
		key.index = 0;
	} else if( strcmp( name, "+" ) == 0 ){
		key.val = 0x1000000;
		key.index = 0;
	} else if( strcmp( name, "DELETE" ) == 0 ){
		key.val = 0x80000;
		key.index = 1;
	} else if( strcmp( name, "TAB" ) == 0 ){
		key.val = 0x10000;
		key.index = 1;
	} else if( strcmp( name, "Q" ) == 0 ){
		key.val = 0x1000;
		key.index = 0;
	} else if( strcmp( name, "W" ) == 0 ){
		key.val = 0x2000;
		key.index = 0;
	} else if( strcmp( name, "E" ) == 0 ){
		key.val = 0x4000;
		key.index = 0;
	} else if( strcmp( name, "R" ) == 0 ){
		key.val = 0x8000;
		key.index = 0;
	} else if( strcmp( name, "T" ) == 0 ){
		key.val = 0x20000;
		key.index = 0;
	} else if( strcmp( name, "Y" ) == 0 ){
		key.val = 0x10000;
		key.index = 0;
	} else if( strcmp( name, "U" ) == 0 ){
		key.val = 0x1;
		key.index = 1;
	} else if( strcmp( name, "I" ) == 0 ){
		key.val = 0x4;
		key.index = 1;
	} else if( strcmp( name, "O" ) == 0 ){
		key.val = 0x80000000;
		key.index = 0;
	} else if( strcmp( name, "P" ) == 0 ){
		key.val = 0x8;
		key.index = 1;
	} else if( strcmp( name, "[" ) == 0 ){
		key.val = 0x2;
		key.index = 1;
	} else if( strcmp( name, "]" ) == 0 ){
		key.val = 0x40000000;
		key.index = 0;
	} else if( strcmp( name, "\\" ) == 0 ){
		key.val = 0x400;
		key.index = 1;
	} else if( strcmp( name, "CAPS" ) == 0 ){
		key.val = 0x2000000;
		key.index = 1;
	} else if( strcmp( name, "A" ) == 0 ){
		key.val = 0x1;
		key.index = 0;
	} else if( strcmp( name, "S" ) == 0 ){
		key.val = 0x2;
		key.index = 0;
	} else if( strcmp( name, "D" ) == 0 ){
		key.val = 0x4;
		key.index = 0;
	} else if( strcmp( name, "F" ) == 0 ){
		key.val = 0x8;
		key.index = 0;
	} else if( strcmp( name, "G" ) == 0 ){
		key.val = 0x20;
		key.index = 0;
	} else if( strcmp( name, "H" ) == 0 ){
		key.val = 0x10;
		key.index = 0;
	} else if( strcmp( name, "J" ) == 0 ){
		key.val = 0x40;
		key.index = 1;
	} else if( strcmp( name, "K" ) == 0 ){
		key.val = 0x100;
		key.index = 1;
	} else if( strcmp( name, "L" ) == 0 ){
		key.val = 0x20;
		key.index = 1;
	} else if( strcmp( name, ";" ) == 0 ){
		key.val = 0x200;
		key.index = 1;
	} else if( strcmp( name, "'" ) == 0 ){
		key.val = 0x80;
		key.index = 1;
	} else if( strcmp( name, "RETURN" ) == 0 ){
		key.val = 0x10;
		key.index = 1;
	} else if( strcmp( name, "SHIFT" ) == 0 ){
		key.val = 0x1000000;
		key.index = 1;
	} else if( strcmp( name, "Z" ) == 0 ){
		key.val = 0x40;
		key.index = 0;
	} else if( strcmp( name, "X" ) == 0 ){
		key.val = 0x80;
		key.index = 0;
	} else if( strcmp( name, "C" ) == 0 ){
		key.val = 0x100;
		key.index = 0;
	} else if( strcmp( name, "V" ) == 0 ){
		key.val = 0x200;
		key.index = 0;
	} else if( strcmp( name, "B" ) == 0 ){
		key.val = 0x800;
		key.index = 0;
	} else if( strcmp( name, "N" ) == 0 ){
		key.val = 0x2000;
		key.index = 1;
	} else if( strcmp( name, "M" ) == 0 ){
		key.val = 0x4000;
		key.index = 1;
	} else if( strcmp( name, "," ) == 0 ){
		key.val = 0x800;
		key.index = 1;
	} else if( strcmp( name, "." ) == 0 ){
		key.val = 0x8000;
		key.index = 1;
	} else if( strcmp( name, "/" ) == 0 ){
		key.val = 0x1000;
		key.index = 1;
	} else if( strcmp( name, "CTRL" ) == 0 ){
		key.val = 0x8000000;
		key.index = 1;
	} else if( strcmp( name, "OPT" ) == 0 ){
		key.val = 0x4000000;
		key.index = 1;
	} else if( strcmp( name, "CMND" ) == 0 ){
		key.val = 0x800000;
		key.index = 1;
	} else if( strcmp( name, "SPACE" ) == 0 ){
		key.val = 0x20000;
		key.index = 1;
	} else if( strcmp( name, "HELP" ) == 0 ){
		key.val = 0x40000;
		key.index = 3;
	} else if( strcmp( name, "HOME" ) == 0 ){
		key.val = 0x80000;
		key.index = 3;
	} else if( strcmp( name, "PG UP" ) == 0 ){
		key.val = 0x100000;
		key.index = 3;
	} else if( strcmp( name, "DEL" ) == 0 ){
		key.val = 0x200000;
		key.index = 3;
	} else if( strcmp( name, "END" ) == 0 ){
		key.val = 0x800000;
		key.index = 3;
	} else if( strcmp( name, "PG DN" ) == 0 ){
		key.val = 0x2000000;
		key.index = 3;
	} else if( strcmp( name, "UP" ) == 0 ){
		key.val = 0x40000000;
		key.index = 3;
	} else if( strcmp( name, "LEFT" ) == 0 ){
		key.val = 0x8000000;
		key.index = 3;
	} else if( strcmp( name, "DOWN" ) == 0 ){
		key.val = 0x20000000;
		key.index = 3;
	} else if( strcmp( name, "RIGHT" ) == 0 ){
		key.val = 0x10000000;
		key.index = 3;
	} else if( strcmp( name, "CLEAR" ) == 0 ){
		key.val = 0x80;
		key.index = 2;
	} else if( strcmp( name, "PAD =" ) == 0 ){
		key.val = 0x20000;
		key.index = 2;
	} else if( strcmp( name, "PAD /" ) == 0 ){
		key.val = 0x800;
		key.index = 2;
	} else if( strcmp( name, "PAD *" ) == 0 ){
		key.val = 0x8;
		key.index = 2;
	} else if( strcmp( name, "PAD 7" ) == 0 ){
		key.val = 0x2000000;
		key.index = 2;
	} else if( strcmp( name, "PAD 8" ) == 0 ){
		key.val = 0x8000000;
		key.index = 2;
	} else if( strcmp( name, "PAD 9" ) == 0 ){
		key.val = 0x10000000;
		key.index = 2;
	} else if( strcmp( name, "PAD -" ) == 0 ){
		key.val = 0x4000;
		key.index = 2;
	} else if( strcmp( name, "PAD 4" ) == 0 ){
		key.val = 0x400000;
		key.index = 2;
	} else if( strcmp( name, "PAD 5" ) == 0 ){
		key.val = 0x800000;
		key.index = 2;
	} else if( strcmp( name, "PAD 6" ) == 0 ){
		key.val = 0x1000000;
		key.index = 2;
	} else if( strcmp( name, "PAD +" ) == 0 ){
		key.val = 0x20;
		key.index = 2;
	} else if( strcmp( name, "PAD 1" ) == 0 ){
		key.val = 0x80000;
		key.index = 2;
	} else if( strcmp( name, "PAD 2" ) == 0 ){
		key.val = 0x100000;
		key.index = 2;
	} else if( strcmp( name, "PAD 3" ) == 0 ){
		key.val = 0x200000;
		key.index = 2;
	} else if( strcmp( name, "PAD 0" ) == 0 ){
		key.val = 0x40000;
		key.index = 2;
	} else if( strcmp( name, "PAD ." ) == 0 ){
		key.val = 0x2;
		key.index = 2;
	} else if( strcmp( name, "ENTER" ) == 0 ){
		key.val = 0x1000;
		key.index = 2;
	} else if( strcmp( name, "FN" ) == 0 ){
		key.val = 0x80000000;
		key.index = 1;
	} 

#endif
	
	else if( strcmp( name, "MOUSE" ) == 0 ){
		key.val = 0x1;
		key.index = 4;
	} else if( strcmp( name, "RT MOUSE" ) == 0 ){
		key.val = 0x2;
		key.index = 4;
	} else if( strcmp( name, "WHL UP" ) == 0 ){
		key.val = 0x4;
		key.index = 4;
	} else if( strcmp( name, "WHL DOWN" ) == 0 ){
		key.val = 0x8;
		key.index = 4;
	} else {
		key.val = 0;
		key.index = 0;
	}
	
	return key;
}