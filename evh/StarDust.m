//
//  StarDust.m
//  Event Horizon
//
//  Created by Paul Dorman on Sat Jul 30 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import "StarDust.h"
#import "Controller.h"

NSMutableArray *dust;

@implementation StarDust

+ (void) initialize{
	dust = [[NSMutableArray alloc] initWithCapacity:75];
}

+ (void) update{
	int i;
	for(i = 0; i < [dust count]; i++ ){
		StarDust *d = [dust objectAtIndex:i];
		if( sqr(d->x-sys->x) + sqr(d->y-sys->y) + sqr(d->z-sys->z) > 64 )
			[dust removeObjectAtIndex:i--];
	}
	while( [dust count] < 75 ){
		StarDust *d = [[[StarDust alloc] init] autorelease];
		[dust addObject:d];
	}
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	for( i = 0; i < 75; i++ )
		[[dust objectAtIndex:i] update];
	
	glColor4f( 1, 1, 1, 1 );
}

- init{
	self = [super init];

	_x = random();
	_y = random();
	
	x = 8*sin(_x)*cos(_y);
	y = 8*cos(_x)*cos(_y);
	z = 8*sin(_y);
	
	x += sys->x;
	y += sys->y;
	z += sys->z;
		
	return self;
}

- (void) update{
	distance = sqrt(sqr(x-sys->x) + sqr(y-sys->y) + sqr(z-sys->z));
	distance = 1-distance/8;
	glColor4f( .8, .8, .5, distance );
	_x = sys->speedx/10;
	_y = sys->speedy/10;
	_z = sys->speedz/10;
	[self position];
	glBegin( GL_LINES );
		glVertex3f( 0, 0, 0 );
		glColor4f( .8, .8, .5, 0 );
		glVertex3f( _x, _y, -_z );
	glEnd();
}

@end
