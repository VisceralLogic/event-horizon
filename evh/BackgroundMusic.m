//
//  BackgroundMusic.m
//  Event Horizon
//
//  Created by Paul Dorman on 11/12/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "BackgroundMusic.h"


@implementation BackgroundMusic

+ (void) startMusic{
	//	BackgroundMusic *music = [[BackgroundMusic alloc] init];
	//	[NSThread detachNewThreadSelector:@selector(musicThread) toTarget:music withObject:nil];
	NSURL *url = [NSURL fileURLWithPath:@"intro.mp3"];
	NSMovie *movie = [[NSMovie alloc] initWithURL:url byReference:TRUE];
	NSMovieView *view = [[NSMovieView alloc] init];
	[view setMovie:movie];
	[view setVolume:1];
	[view gotoBeginning:nil];
	[view start:nil];
}

- (void) musicThread{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSURL *url = [NSURL fileURLWithPath:@"intro.mp3"];
	NSMovie *movie = [[NSMovie alloc] initWithURL:url byReference:TRUE];
	NSMovieView *view = [[NSMovieView alloc] init];
	[view setMovie:movie];
	[view setVolume:1];
	[view gotoBeginning:nil];
	[view start:nil];
	
	while( [view isPlaying] )
		;
	
	[pool release];
}

@end
