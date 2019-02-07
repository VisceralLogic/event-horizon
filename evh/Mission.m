//
//  Mission.m
//  Event Horizon
//
//  Created by user on Wed May 21 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Mission.h"
#import "SpaceObject.h"
#import "Controller.h"

NSString *replace( NSString *start, NSString *what, NSString *new );

@implementation Mission

+ (void) registerFromDictionary:(NSDictionary *)dictionary{
	NSMutableDictionary *plugin = [Controller ensurePlugin:[dictionary objectForKey:@"PluginName"] path:@"missions"];
	Mission *mission = [[Mission alloc] init];

	mission->availType = [[dictionary objectForKey:@"AvailableType"] copy];
	mission->avail = [[dictionary objectForKey:@"Available"] copy];
	mission->destType = [[dictionary objectForKey:@"DestType"] copy];
	mission->dest = [[dictionary objectForKey:@"Destination"] copy];
	mission->size = [[dictionary objectForKey:@"Size"] intValue];
	mission->payment = [[dictionary objectForKey:@"Payment"] intValue];
	mission->title = [[dictionary objectForKey:@"Title"] copy];
	mission->description = [[dictionary objectForKey:@"Description"] copy];
	mission->ID = [[NSString alloc] initWithFormat:@"%@.%@.%@", [dictionary objectForKey:@"PluginName"], @"missions", [dictionary objectForKey:@"ID"]];
	mission->freq = [[dictionary objectForKey:@"Frequency"] floatValue];
	mission->flags = [[dictionary objectForKey:@"Flags"] mutableCopyWithZone:nil];
	mission->endText = [[dictionary objectForKey:@"EndText"] copy];
	mission->availFlags = [[dictionary objectForKey:@"AvailFlags"] copy];
	mission->startFlags = [[dictionary objectForKey:@"StartFlags"] copy];
	mission->setFlags = [[dictionary objectForKey:@"SetFlags"] copy];
	mission->endTime = [[dictionary objectForKey:@"EndTime"] intValue]*86400.0;
	mission->loadScript = [[dictionary objectForKey:@"LoadScript"] copy];
	mission->startScript = [[dictionary objectForKey:@"StartScript"] copy];
	mission->endScript = [[dictionary objectForKey:@"EndScript"] copy];
	if( [dictionary objectForKey:@"Replace"] ){
		NSArray *temp = [[dictionary objectForKey:@"Replace"] componentsSeparatedByString:@"."];
		NSMutableDictionary *d = [Controller ensurePlugin:[temp objectAtIndex:0] path:@"missions"];
		[d setObject:mission forKey:[temp objectAtIndex:2]];
		mission->ID = [dictionary objectForKey:@"Replace"];
	} else
		[plugin setObject:mission forKey:[dictionary objectForKey:@"ID"]];
}

- (void) finalize{
}

- (NSString *) description{
	return [NSString stringWithFormat:@"<Mission: %@>", self->ID];
}

- (void) initialScript{
	if( loadScript )
		[sys->interpreter execute:loadScript];
}

- (NSDictionary *) flags{
	return flags;
}

- (Mission*) newInstanceIn:(Solarsystem*)system on:(Planet*)planet{
	Mission *mission = [[Mission alloc] init];
	mission->ID = self->ID;
	mission->startSystem = system;
	mission->startPlanet = planet;
	mission->size = size;
	mission->payment = payment;
	mission->title = title;
	mission->description = description;
	if( [destType isEqualToString:@"government"] ){
		NSMutableArray *systems = [Controller objectsOfType:@"system"];
		int i;
		for( i = 0; i < [systems count]; i++ ){
			Solarsystem *system = [systems objectAtIndex:i];
			if( !system->gov || (![dest isEqualToString:@"any"] && ![system->gov->ID isEqualToString:dest]) ){
				[systems removeObjectAtIndex:i];
				i--;
			}
		}
		mission->endSystem = [systems objectAtIndex:random()%[systems count]];
		mission->endPlanet = [mission->endSystem->planets objectAtIndex:random()%[mission->endSystem->planets count]];
	} else if( [destType isEqualToString:@"system"] ){
		mission->endSystem = [Controller componentNamed:dest];
		mission->endPlanet = [mission->endSystem->planets objectAtIndex:random()%[mission->endSystem->planets count]];
	} else if( [destType isEqualToString:@"planet"] ){
		NSArray *systems = [Controller objectsOfType:@"system"];
		int i = 0;
		BOOL found = NO;
		while( i < [systems count] && !found ){
			int j;
			Solarsystem *system = [systems objectAtIndex:i];
			for( j = 0; j < [system->planets count]; j++ ){
				Planet *planet = [system->planets objectAtIndex:j];
				if( [planet->ID isEqualToString:dest] ){
					mission->endSystem = system;
					mission->endPlanet = planet;
					found = YES;
					break;
				}
			}
			i++;
		}
	}
	if( endTime != 0 )
		mission->expires = [[sys->date addTimeInterval:endTime] retain];
	mission->title = replace( title, @"<PLANET>", mission->endPlanet->name );
	mission->title = replace( mission->title, @"<SYSTEM>", mission->endSystem->name );
	mission->title = replace( mission->title, @"<DATE>", [mission->expires descriptionWithCalendarFormat:@"%m/%d/%Y" timeZone:nil locale:nil] );
	mission->description = replace( description, @"<PLANET>", mission->endPlanet->name );
	mission->description = replace( mission->description, @"<SYSTEM>", mission->endSystem->name );
	mission->description = replace( mission->description, @"<DATE>", [mission->expires descriptionWithCalendarFormat:@"%m/%d/%Y" timeZone:nil locale:nil] );
	mission->endText = replace( endText, @"<PLANET>", mission->endPlanet->name );
	mission->endText = replace( mission->endText, @"<SYSTEM>", mission->endSystem->name );
	mission->endText = replace( mission->endText, @"<DATE>", [mission->expires descriptionWithCalendarFormat:@"%m/%d/%Y" timeZone:nil locale:nil] );
	mission->startFlags = startFlags;
	mission->setFlags = setFlags;
	mission->endScript = endScript;
	mission->startScript = startScript;
	[mission->title retain];
	[mission->description retain];
	[mission->endText retain];

	return mission;
}

- (void) accept{
	int i;
	NSArray *temp;

	sys->cargoSpace -= size;
	sys->mass += size;
	sys->ACCELERATION = sys->thrust/(sys->mass*sys->compensation);
	[sys->missions addObject:self];
	temp = [startFlags componentsSeparatedByString:@" "];
	for( i = 0; i < [temp count]; i++ ){
		if( i < [temp count]-1 && [[temp objectAtIndex:i+1] isEqualToString:@"="] ){
			NSString *s = [temp objectAtIndex:i+2];
			if( [s rangeOfString:@"!"].location == 0 ){
				if( [[sys->flags objectForKey:[s substringFromIndex:1]] intValue] == 0 )
					[sys->flags setObject:[NSNumber numberWithInt:1] forKey:[temp objectAtIndex:i]];
				else
					[sys->flags removeObjectForKey:[temp objectAtIndex:i]];
			} else if( [s rangeOfString:@"$"].location == 0 ){
				[sys->flags setObject:[NSNumber numberWithInt:[sys valForFlag:[temp objectAtIndex:i+2]]] forKey:[temp objectAtIndex:i]];
			} else {
				if( [sys->flags objectForKey:s] )
					[sys->flags setObject:[sys->flags objectForKey:s] forKey:[temp objectAtIndex:i]];
				else
					[sys->flags setObject:[NSNumber numberWithInt:[s intValue]] forKey:[temp objectAtIndex:i]];
			}
			i += 2;
		} else if( [[temp objectAtIndex:i] rangeOfString:@"!"].location == 0 ){
			[sys->flags removeObjectForKey:[[temp objectAtIndex:i] substringFromIndex:1]];
		} else {
			[sys->flags setObject:[NSNumber numberWithInt:1] forKey:[temp objectAtIndex:i]];
		}
	}
	if( startScript )
		[sys->interpreter execute:startScript];
}

- (void) accomplish{
	int i;
	NSArray *temp;
	
	sys->money += payment;
	sys->mass -= size;
	sys->cargoSpace += size;
	sys->ACCELERATION = sys->thrust/(sys->mass*sys->compensation);
	temp = [setFlags componentsSeparatedByString:@" "];	
	for( i = 0; i < [temp count]; i++ ){
		if( i < [temp count]-1 && [[temp objectAtIndex:i+1] isEqualToString:@"="] ){
			NSString *s = [temp objectAtIndex:i+2];
			if( [s rangeOfString:@"!"].location == 0 ){
				if( [[sys->flags objectForKey:[s substringFromIndex:1]] intValue] == 0 )
					[sys->flags setObject:[NSNumber numberWithInt:1] forKey:[temp objectAtIndex:i]];
				else
					[sys->flags removeObjectForKey:[temp objectAtIndex:i]];
			} else if( [s rangeOfString:@"$"].location == 0 ){
				[sys->flags setObject:[NSNumber numberWithInt:[sys valForFlag:[temp objectAtIndex:i+2]]] forKey:[temp objectAtIndex:i]];
			} else {
				if( [sys->flags objectForKey:s] )
					[sys->flags setObject:[sys->flags objectForKey:s] forKey:[temp objectAtIndex:i]];
				else
					[sys->flags setObject:[NSNumber numberWithInt:[s intValue]] forKey:[temp objectAtIndex:i]];
			}
			i += 2;
		} else if( [[temp objectAtIndex:i] rangeOfString:@"!"].location == 0 ){
			[sys->flags removeObjectForKey:[[temp objectAtIndex:i] substringFromIndex:1]];
		} else {
			[sys->flags setObject:[NSNumber numberWithInt:1] forKey:[temp objectAtIndex:i]];
		}
	}
	if( endScript )
		[sys->interpreter execute:endScript];
}

- (Mission*) suitableSystem:(Solarsystem*)system andPlanet:(Planet*)planet{
	NSArray *array = [NSArray arrayWithObject:sys];
	if( [[Controller objects:array withFlags:availFlags] count] == 0 )	// make sure availFlags are set
		return nil;
	if( ((float)random())/RAND_MAX > freq )
		return nil;
	if( [availType isEqualToString:@"government"] ){
		if( [system->gov->ID isEqualToString:avail] || [avail isEqualToString:@"any"] )
			return [self newInstanceIn:system on:planet];
	} else if( [availType isEqualToString:@"system"] ){
		if( [system->ID isEqualToString:avail] || [avail isEqualToString:@"any"] )
			return [self newInstanceIn:system on:planet];
	} else if( [availType isEqualToString:@"planet"] ){
		if( [planet->ID isEqualToString:avail] || [avail isEqualToString:@"any"] )
			return [self newInstanceIn:system on:planet];
	}
	return nil;
}

- (void) update{
	if( expires != nil && [[sys->date laterDate:expires] isEqualToDate:sys->date] && ![sys->date isEqualToDate:expires] ){	// mission expired
		sys->mass -= size;
		sys->cargoSpace += size;
		sys->ACCELERATION = sys->thrust/(sys->mass*sys->compensation);
		[sys newMessage:@"Time limit expired - mission failed"];
		[sys->missions removeObject:self];
	}
}

@end

NSString *replace( NSString *initial, NSString *what, NSString *new ){
	NSString *replace = [NSString stringWithString:initial];
	NSRange range = [replace rangeOfString:what];
	NSRange temp;
	NSString *start, *end;

	while( range.location != NSNotFound ){
		temp.location = 0;
		temp.length = range.location;
		start = [replace substringWithRange:temp];
		end = [replace substringFromIndex:range.location+[what length]];
		replace = [NSString stringWithFormat:@"%@%@%@", start, new, end];
		range = [replace rangeOfString:@"%S"];
	}

	return replace;
}
