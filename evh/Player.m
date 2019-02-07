//
//  Player.m
//  Event Horizon
//
//  Created by user on Sun Mar 09 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "ControllerGraphics.h"
#import "Player.h"
#import "Prefs.h"
#import "EHButton.h"
#import "aglString.h"
#import "Texture.h"

extern void DrawGLScene();

void loadPlayerNames();
void drawButton( char *title, int x, int y, int width, int height, BOOL active );
int validPlayer();
void playerDelete();
void playerEnter();
void playerQuit();
void playerNew();
void playerName( NSString *name );
void playerCancel();

int playerIndex = 0;
GLuint playerTex, splitLogoTex;
int numPlayers = 0;
BOOL selectPlayer = YES;
float t = 0;
int gScreenWidth, gScreenHeight;
NSString *name;
UnsignedWide myT;
UInt64 u;
NSMutableArray *players;
float pluginLoading = 0;

void beginPlayer(){
	drawScene = drawPlayerScreen;
	eventScene = eventPlayer;
	nsEventScene = nsEventPlayer;
	[Texture loadTexture:@"LogoLarge.png" into:&playerTex withWidth:nil height:nil];
	[Texture loadTexture:@"Split.png" into:&splitLogoTex withWidth:nil height:nil];
	Microseconds( &myT );
	u = UnsignedWideToUInt64( myT );
	players = [[NSMutableArray alloc] initWithCapacity:5];
	ShowCursor();
	[EHButton newButton:@"Enter" atX:gScreenWidth-164 y:50 width:128 callback:nil withParam:NO active:NO isActive:validPlayer];
	[EHButton newButton:@"New Player..." atX:gScreenWidth/2-200 y:gScreenHeight-288 width:200 callback:playerNew withParam:NO active:YES isActive:nil];
	[EHButton newButton:@"Quit" atX:164 y:50 width:128 callback:playerQuit withParam:NO active:YES isActive:nil];
	[EHButton newButton:@"Delete" atX:gScreenWidth-528 y:50 width:128 callback:playerDelete withParam:NO active:NO isActive:validPlayer];
	loadPlayerNames();
}

void endPlayer( BOOL newPlayer ){
	[EHButton clearSet:@"Player"];
	[EHButton clear];
	HideCursor();
	if( !sys ){
		sys = [[Controller alloc] initWithName:name new:newPlayer];
		sys->screenWidth = gScreenWidth;
		sys->screenHeight = gScreenHeight;
	} else {
		sys->name = name;
		[sys load];
	}
	if( sys->floatVal[MOUSE] == 1.0f )
		CGAssociateMouseAndMouseCursorPosition( NO );
	drawScene = DrawGLScene;
	eventScene = NULL;
	nsEventScene = NULL;
}

void drawPlayerScreen(){
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, gScreenWidth, 0, gScreenHeight, 0, 1 );
	glMatrixMode( GL_MODELVIEW );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	CGContextSelectFont(cgContext, "Helvetica", 12, kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(cgContext, kCGTextFill);
	CGContextSetRGBFillColor(cgContext, 1, 1, 1, 0);
	CGContextShowTextAtPoint(cgContext, 0, 0, "Testing Quartz Text", 19);

	Microseconds( &myT );
	if( t == 0 )
		t = 0.01;
	else
		t += (UnsignedWideToUInt64(myT) - u)/1E6;
	u = UnsignedWideToUInt64(myT);

	if( pluginLoading != 0 || t >= 10 ){
		glBindTexture( GL_TEXTURE_2D, splitLogoTex );
		glColor3f( 1, 1, 1 );
		glBegin( GL_QUADS );
		glTexCoord2f( 0, 0.5 );
		glVertex2i( (gScreenWidth-733)/2, gScreenHeight-256 );
		glTexCoord2f( 1, 0.5 );
		glVertex2i( gScreenWidth/2+145, gScreenHeight-256 );
		glTexCoord2f( 1, 1 );
		glVertex2i( gScreenWidth/2+145, gScreenHeight );
		glTexCoord2f( 0, 1 );
		glVertex2i( (gScreenWidth-733)/2, gScreenHeight );
		
		glTexCoord2f( 0, 0 );
		glVertex2i( gScreenWidth/2+145, gScreenHeight-256 );
		glTexCoord2f( 0.43164, 0 );
		glVertex2i( (gScreenWidth+733)/2, gScreenHeight-256 );
		glTexCoord2f( 0.43164, .5 );
		glVertex2i( (gScreenWidth+733)/2, gScreenHeight );
		glTexCoord2f( 0, .5 );
		glVertex2i( gScreenWidth/2+145, gScreenHeight );
		glEnd();
	} 
	if( pluginLoading != 0 ){
		glLoadIdentity();
		glBindTexture( GL_TEXTURE_2D, 0 );
		glColor3f( 0, 1, 0 );
		glRasterPos2i( gScreenWidth/2-128, 250 );
		DrawCStringGL( "Loading...", font );
		glColor3f( 1, 0, 0 );
		glBegin( GL_LINE_STRIP );
			glVertex2i( gScreenWidth/2-129, 199 );
			glVertex2i( gScreenWidth/2+129, 199 );
			glVertex2i( gScreenWidth/2+129, 217 );
			glVertex2i( gScreenWidth/2-129, 217 );
			glVertex2i( gScreenWidth/2-129, 199 );
		glEnd();
		glBegin( GL_QUADS );
		glColor3f( 0, 0, 1 );
		glVertex2i( gScreenWidth/2-128, 216 );
		glVertex2i( gScreenWidth/2-128, 200 );
		glColor3f( 0, 0, 0 );
		glVertex2f( gScreenWidth/2-128+256*pluginLoading, 200 );
		glVertex2f( gScreenWidth/2-128+256*pluginLoading, 216 );
		glEnd();
	} else if( t < 10 ){
		glColor3f( t/5, t/5, t/5 );
		glBindTexture( GL_TEXTURE_2D, playerTex );
		glLoadIdentity();
		glBegin( GL_QUADS );
			glTexCoord2f( 0, 0 );
			glVertex2f( gScreenWidth/2-256, gScreenHeight/2-256 );
			glTexCoord2f( 1, 0 );
			glVertex2f( gScreenWidth/2+256, gScreenHeight/2-256 );
			glTexCoord2f( 1, 1 );
			glVertex2f( gScreenWidth/2+256, gScreenHeight/2+256 );
			glTexCoord2f( 0, 1 );
			glVertex2f( gScreenWidth/2-256, gScreenHeight/2+256 );
		glEnd();
	} else {
		[EHButton update];
		
		if( selectPlayer ){		// draw selection screen
			int i;

			glBindTexture( GL_TEXTURE_2D, 0 );
			for( i = 0; i < numPlayers; i++ ){
				if( playerIndex == i+1 ){
					glColor3f( .5+.5*sin(6*t), 1, 0 );
					glRasterPos2i( gScreenWidth/2-196, gScreenHeight-278-40*(i+1) );
					DrawCStringGL( "È", buttonFont );
				}
			}
		} else {	// creating new player
			glBindTexture( GL_TEXTURE_2D, 0 );
			glColor3f( 1, 1, 0 );
			glRasterPos2i( gScreenWidth/2-200, gScreenHeight-288 );
			DrawCStringGL( "Enter your name:", font );
			glColor3f( 0, 1, 0 );
			glRasterPos2i( gScreenWidth/2-76, gScreenHeight-288 );
			DrawCStringGL( [name cString], font );
	# pragma mark ___ADD CODE HERE!
	// TO DO: get raster pos, if too long, stop entry
			if( sin(8*t) > 0.5 )
				DrawCStringGL( "|", font );
		}
	}

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

void loadPlayerNames(){
	NSArray *files;
	int i;

	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] bundlePath]];
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:@"../Players"];
	files = [[NSFileManager defaultManager] directoryContentsAtPath:@"."];
	numPlayers = 0;
	for( i = 0; i < [files count]; i++ ){
		if( [[[files objectAtIndex:i] pathExtension] isEqualToString:@"evh"] ){
			NSString *text = [[[files objectAtIndex:i] componentsSeparatedByString:@"."] objectAtIndex:0];
			[players addObject:text];
			numPlayers++;
			[EHButton newButton:text atX:gScreenWidth/2-184 y:gScreenHeight-288-40*numPlayers width:184 callback:playerName withParam:YES active:YES isActive:nil];
		}
	}
}

void drawButton( char *title, int x, int y, int width, int height, BOOL active ){
	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, 0 );

	glColor3f( 0, 1, 0 );
	if( !active )
		glColor3f( 0, 0.5, 0 );
	glRasterPos2i( x+10, y+9 );
	DrawCStringGL( title, buttonFont );
	glBegin( GL_QUADS );
		if( active )
			glColor3f( .243, .243, .243 );
		else
			glColor3f( .122, .122, .122 );
		glVertex2i( x+4, y+4 );
		glVertex2i( x+width-4, y+4 );
		glVertex2i( x+width-4, y+height-4 );
		glVertex2i( x+4, y+height-4 );

		if( active )
			glColor3f( .502, .502, .502 );
		else
			glColor3f( .25, .25, .25 );
		glVertex2i( x+3, y+3 );
		glVertex2i( x+width-3, y+3 );
		glVertex2i( x+width-3, y+height-3 );
		glVertex2i( x+3, y+height-3 );

		if( active )
			glColor3f( .275, .275, .275 );
		else
			glColor3f( .138, .138, .138 );
		glVertex2i( x+2, y+2 );
		glVertex2i( x+width-2, y+2 );
		glVertex2i( x+width-2, y+height-2 );
		glVertex2i( x+2, y+height-2 );

		if( active )
			glColor3f( .11, .11, .11 );
		else
			glColor3f( .055, .055, .055 );
		glVertex2i( x+1, y+1 );
		glVertex2i( x+width-1, y+1 );
		glVertex2i( x+width-1, y+height-1 );
		glVertex2i( x+1, y+height-1 );

		if( active )
			glColor3f( .084, .084, .084 );
		else
			glColor3f( .042, .042, .042 );
		glVertex2i( x, y );
		glVertex2i( x+width, y );
		glVertex2i( x+width, y+height );
		glVertex2i( x, y+height );
	glEnd();
}

void eventPlayer( EventRef event ){
	UInt32 class, kind;
	
	class = GetEventClass( event );
	kind = GetEventKind( event );
	
	switch( class ){
		case kEventClassKeyboard:
			if( kind == kEventRawKeyDown || kind == kEventRawKeyRepeat){
				if( ![sys functionKey] ){
					char theChar;
					GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &theChar );
					playerKeyFunc( theChar );
				}
			}
			break;
		case kEventClassMouse:
			if( kind == kEventMouseDown ){
				Point p;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				playerMouse( p.h, gScreenHeight-p.v );
			}
			break;
	}
}

void nsEventPlayer( NSEvent *event ){
	switch( [event type] ){
		case NSKeyDown:
			playerKeyFunc( [[event characters] characterAtIndex:0] );
			break;
		case NSLeftMouseDownMask:
			playerMouse( [event locationInWindow].x, [event locationInWindow].y+22 );
			break;
		default:
			break;
	}
}

void playerKeyFunc( unsigned char key ){
	if( t < 10 ){
		t = 10;
		return;
	}
	if( key == 27 ){ // 'esc'
		if( selectPlayer )
			exit( 0 );
		else {
			selectPlayer = YES;
			[EHButton clear];
			[EHButton recallSet:@"Player"];
			name = @"";
		}
	} else if( key == '\r' || key == 3 ){
		playerEnter();
	} else if( !selectPlayer ){
		[name autorelease];
		if( key == 8 ){	// backspace key
			if( [name length] > 1 )
				name = [[NSString alloc] initWithCString:[name cString] length:[name length]-1];
			else
				name = @"";
		} else
			name = [[NSString stringWithFormat:@"%@%c", name, key] retain];
	}
}

void playerMouse( int x, int y ){
	if( t < 10 )
		t = 10;
	else if( x > gScreenWidth-164 && x < gScreenWidth-36 && y > 50 && y < 82 ){
		playerEnter();	// "Enter" button
	}
}

int validPlayer(){
	if( (selectPlayer && playerIndex > 0 && playerIndex <= numPlayers) || [name length] > 0 )
		return 1;
	return 0;
}

void playerCancel(){
	playerKeyFunc( 27 );
}

void playerName( NSString *s ){
	name = s;
	playerIndex = [players indexOfObject:s]+1;
}

void playerNew(){
	[EHButton storeSet:@"Player"];
	[EHButton clear];
	[EHButton newButton:@"Enter" atX:gScreenWidth-164 y:50 width:128 callback:nil withParam:NO active:NO isActive:validPlayer];
	[EHButton newButton:@"Cancel" atX:gScreenWidth-528 y:50 width:128 callback:playerCancel withParam:NO active:YES isActive:nil];
	selectPlayer = NO;
	if( name != nil )
		[name autorelease];
	name = @"";
}

void playerDelete(){
	int i;
	numPlayers--;
	[players removeObjectAtIndex:playerIndex-1];
	playerIndex = 0;
	[[NSFileManager defaultManager] removeFileAtPath:[NSString stringWithFormat:@"%@/%@.evh",[[NSFileManager defaultManager] currentDirectoryPath], name] handler:nil];
	[EHButton removeByName:name];
	for( i = 0; i < numPlayers; i++ ){
		if( [[players objectAtIndex:i] compare:name] == NSOrderedDescending )
			[EHButton buttonWithName:[players objectAtIndex:i]]->y += 40;
	}
}

void playerEnter(){
	if( selectPlayer ){
		if( playerIndex > 0 )	// enter existing player
			endPlayer( NO );
	} else {	// new player
		if( [name length] <= 0 )
			return;
		endPlayer( YES );
		setUpPrefs();
	}
}

void playerQuit(){
	exit(0);
}