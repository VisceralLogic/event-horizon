//
//  SoundManager.m
//  Event Horizon
//
//  Created by Paul Dorman on Wed Jan 26 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import "SoundManager.h"
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import "Controller.h"

BOOL al;					// use OpenAL, or NSMoviewViews?

typedef struct {
	ALuint alSource;
	SpaceObject *location;
} ALSoundSource;

ALSoundSource *sources;		// hold source info
int MAX_CHANNELS = 32;
NSMovieView **channel;
NSMutableArray *buffer;		// NSMovieView buffers
int chanIndex;				// current channel
NSMutableDictionary *nums;	// source data nums

@implementation SoundManager

+ (void) initialize{
	int i;

	if( alGetError == NULL )
		al = NO;
	else
		al = YES;
	
	if( !al ){
		channel = calloc( MAX_CHANNELS, sizeof(NSMovieView *) );
		for( i = 0; i < MAX_CHANNELS; i++ )
			channel[i] = [[NSMovieView alloc] init];
		buffer = [[NSMutableArray alloc] init];
	} else {
		ALfloat sysOri[6] = { 1, 0, 0, 0, 1, 0 };
		alutInit( &i, nil );
		sources = calloc( MAX_CHANNELS, sizeof(ALSoundSource) );
		for( i = 0; i < MAX_CHANNELS; i++ ){
			alGenSources( 1, &sources[i].alSource );
		}
		alListener3f( AL_POSITION, 0, 0, 0 );
		alListenerfv( AL_ORIENTATION, sysOri );
	}
	nums = [[NSMutableDictionary alloc] init];	
	
	i = 0;
}

+ (unsigned int) registerSound:(NSString *)file{
	if( [nums objectForKey:file] )		// already registered
		return [[nums objectForKey:file] unsignedIntValue];
	if( al ){
		ALenum format;
		ALsizei size;
		ALvoid* data;
		ALsizei freq;
		ALuint buffer;

		alGenBuffers(1, &buffer);
		alutLoadWAVFile([file cString], &format, &data, &size, &freq);
		alBufferData(buffer, format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);
		[nums setObject:[NSNumber numberWithUnsignedInt:buffer] forKey:file];
		return buffer;
	} else {
		NSMovie *movie = [[NSMovie alloc] initWithURL:[NSURL fileURLWithPath:file] byReference:YES];
		[buffer addObject:movie];
		[nums setObject:[NSNumber numberWithUnsignedInt:[buffer count]-1] forKey:file];
		return [buffer count]-1;
	}
}

+ (void) attachSound:(unsigned int)soundNum toSource:(SpaceObject *)source{
	if( soundNum == -1 )	// AL failure
		return;
	if( al ){
		ALSoundSource *s = &sources[chanIndex++];
		alDeleteSources( 1, &s->alSource );
		alGenSources( 1, &s->alSource );
		alSourcei( s->alSource, AL_BUFFER, soundNum );
		alSourcef( s->alSource, AL_MAX_DISTANCE, 150.0f );
		if( chanIndex == MAX_CHANNELS )
			chanIndex = 0;
		alSourcePlay( s->alSource );
		s->location = source;
	} else {
		float dist = sqrt(sqr(source->x-sys->x)+sqr(source->y-sys->y)+sqr(source->z-sys->z));
		float vol = -dist/75+.5;
		if( vol > 0 ){
			[channel[chanIndex] setMovie:[buffer objectAtIndex:soundNum]];
			[channel[chanIndex] setVolume:vol];
			[channel[chanIndex] gotoBeginning:nil];
			[channel[chanIndex++] start:nil];
			if( chanIndex == MAX_CHANNELS )
				chanIndex = 0;
		}
	}
}

+ (void) update{
	int i;

	if( !al )
		return;

	for( i = 0; i < MAX_CHANNELS; i++ ){
		ALSoundSource sound = sources[i];
		if( !sound.location )
			continue;
		sys->_x = sound.location->x;
		sys->_y = sound.location->y;
		sys->_z = sound.location->z;
		[sys globalToLocal];
		alSource3f( sound.alSource, AL_POSITION, sys->_x, sys->_y, sys->_z );		
	}
}

+ (BOOL) openAL{
	return al;
}

@end
