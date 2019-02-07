//
//  Government.m
//  Event Horizon
//
//  Created by user on Sun May 25 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Government.h"
#import "Controller.h"

@implementation Government

+ (void) registerFromDictionary:(NSDictionary *)dictionary{
	NSMutableDictionary *plugin = [Controller ensurePlugin:[dictionary objectForKey:@"PluginName"] path:@"governments"];
	Government *gov = [[Government alloc] init];
	NSData *color = [dictionary objectForKey:@"Color"];

	gov->name = [[dictionary objectForKey:@"Name"] copy];
	gov->ID = [[NSString alloc] initWithFormat:@"%@.%@.%@", [dictionary objectForKey:@"PluginName"], @"governments", [dictionary objectForKey:@"ID"]];
	gov->red = ((unsigned char*)[color bytes])[0]/255.;
	gov->green = ((unsigned char*)[color bytes])[1]/255.;
	gov->blue = ((unsigned char*)[color bytes])[2]/255.;
	gov->enemies = [[dictionary objectForKey:@"Enemies"] mutableCopyWithZone:nil];
	gov->allies = [[dictionary objectForKey:@"Allies"] mutableCopyWithZone:nil];
	gov->initData = [dictionary objectForKey:@"InitialRecord"];
	gov->shootPenalty = [dictionary objectForKey:@"ShootPenalty"] ? [[dictionary objectForKey:@"ShootPenalty"] intValue] : 20;
	gov->disablePenalty = [dictionary objectForKey:@"DisablePenalty"] ? [[dictionary objectForKey:@"DisablePenalty"] intValue] : 50;
	gov->boardPenalty = [dictionary objectForKey:@"BoardPenalty"] ? [[dictionary objectForKey:@"BoardPenalty"] intValue] : 50;
	gov->destroyPenalty = [dictionary objectForKey:@"DestroyPenalty"] ? [[dictionary objectForKey:@"DestroyPenalty"] intValue] : 75;
	if( [dictionary objectForKey:@"Replace"] ){
		NSArray *temp = [[dictionary objectForKey:@"Replace"] componentsSeparatedByString:@"."];
		NSMutableDictionary *d = [Controller ensurePlugin:[temp objectAtIndex:0] path:@"governments"];
		[d setObject:gov forKey:[temp objectAtIndex:2]];
		gov->ID = [dictionary objectForKey:@"Replace"];
	} else
		[plugin setObject:gov forKey:[dictionary objectForKey:@"ID"]];
}

- (void) finalize{
	if( enemies ){
		int i;
		for( i = 0; i < [enemies count]; i++ ){
			Government *gov = [Controller componentNamed:[enemies objectAtIndex:i]];
			if( ![gov->enemies containsObject:self->ID] )
				[gov->enemies addObject:self->ID];
		}
	}
	if( !initData )
		initData = [NSNumber numberWithInt:0];
	[sys->govRecord setObject:initData forKey:self->ID];
}

- (NSString *) description{
	return [NSString stringWithFormat:@"<Government: %@>", self->ID];
}

- (NSMutableArray *) enemies{
	return enemies;
}

- (NSMutableArray *) allies{
	return allies;
}

@end
