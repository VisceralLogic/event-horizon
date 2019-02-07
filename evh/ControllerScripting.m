//
//  ControllerScripting.m
//  Event Horizon
//
//  Created by Paul Dorman on 3/12/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "ControllerScripting.h"


@implementation Controller (ControllerScripting)

- (NSMutableArray *) planets{
	return planets;
}

- (Solarsystem *) system{
	return system;
}

- (NSMutableArray *) ships{
	return ships;
}

- (NSMutableArray *) weaps{
	return weaps;
}

- (NSMutableArray *) missions{
	return missions;
}

- (NSMutableDictionary *) plugins{
	return plugins;
}

- (float) time{
	return t;
}

- (NSDate *) date{
	return date;
}

- (void) setDate:(NSDate *)newDate{
	[date release];
	date = [newDate retain];
}

- (NSMutableDictionary *) govRecord{
	return govRecord;
}

- (int) combatRating{
	return combatRating;
}

- (void) setCombatRating:(int)rating{
	combatRating = rating;
}

- (void) newMessage:(NSString *)newMessage{
	if( ![newMessage isKindOfClass:[NSString class]] )
		return;
	if( message )
		[message release];
	message = [newMessage retain];
	messageTime = 0.;
}

- (int) valForFlag:(NSString *)flag{
	if( [flag rangeOfString:@"$GOV_RECORD"].location == 0 ){
		NSRange range;
		range.location = 12;
		range.length = [flag length]-13;
		NSString *gummint = [flag substringWithRange:range];
		return [[govRecord objectForKey:gummint] intValue];
	}
	else if( [flag isEqualToString:@"COMBAT_RATING"] )
		return combatRating;
	return 0;
}

@end
