//
//  Type.m
//  Event Horizon
//
//  Created by user on Wed Jun 25 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Type.h"

@implementation Type

+ (void) registerFromDictionary:(NSDictionary *)dictionary{
	NSMutableDictionary *plugin = [Controller ensurePlugin:[dictionary objectForKey:@"PluginName"] path:@"types"];
	Type *type = [[Type alloc] init];
	
	type->data = [[dictionary objectForKey:@"Data"] mutableCopyWithZone:nil];
	type->ID = [[NSString alloc] initWithFormat:@"%@.%@.%@", [dictionary objectForKey:@"PluginName"], @"types", [dictionary objectForKey:@"ID"]];
	if( [dictionary objectForKey:@"Replace"] ){
		NSArray *temp = [[dictionary objectForKey:@"Replace"] componentsSeparatedByString:@"."];
		NSMutableDictionary *d = [Controller ensurePlugin:[temp objectAtIndex:0] path:@"types"];
		[d setObject:type forKey:[temp objectAtIndex:2]];
		type->ID = [dictionary objectForKey:@"Replace"];
	} else
		[plugin setObject:type forKey:[dictionary objectForKey:@"ID"]];
	if( [dictionary objectForKey:@"AddTo"] )
		type->ID = [NSString stringWithFormat:@"ADDTO%@",[dictionary objectForKey:@"AddTo"]];
}

- initWithArray:(NSArray *)array{
	int i;

	self = [super init];
	data = [[NSMutableArray alloc] initWithCapacity:[array count]];
	for( i = 0; i < [array count]; i++ ){
		if( [[array objectAtIndex:i] count] == 2 )
			[data addObject:[[[TypeNode alloc] initWithDictionary:[array objectAtIndex:i] array:YES] autorelease]];
		else
			[data addObject:[[[TypeNode alloc] initWithDictionary:[array objectAtIndex:i] array:NO] autorelease]];
	}

	return self;
}

- (void) finalize{
	NSMutableArray *temp = [[NSMutableArray alloc] initWithCapacity:[data count]];
	int i;

	for( i = 0; i < [data count]; i++ ){
		if( [[data objectAtIndex:i] count] == 2 )
			[temp addObject:[[[TypeNode alloc] initWithDictionary:[data objectAtIndex:i] array:YES] autorelease]];
		else
			[temp addObject:[[[TypeNode alloc] initWithDictionary:[data objectAtIndex:i] array:NO] autorelease]];
	}
	data = temp;
	if( [self->ID rangeOfString:@"ADDTO"].location != NSNotFound ){
		NSString *addToID = [self->ID substringFromIndex:5];
		Type *t = [Controller componentNamed:addToID];
		TypeNode *d;
		int i;
		float f = 0;

		for( i = 0; i < [data count]; i++ ){
			d = [data objectAtIndex:i];
			f += d->probability;
		}
		for( i = 0; i < [t->data count]; i++ ){
			d = [t->data objectAtIndex:i];
			d->probability *= (1.-f);
		}
		for( i = 0; i < [data count]; i++ ){
			[t->data addObject:[data objectAtIndex:i]];
		}
	}
}

- (AI *) newInstance{
	float prob = ((float)random())/RAND_MAX;
	int i;

	for( i = 0; i < [data count]; i++ ){
		TypeNode *node = [data objectAtIndex:i];
		prob -= node->probability;
		if( prob <= 0 ){	// instantiate here
			if( [node->data isKindOfClass:[Type class]] ){
				return [node->data newInstance];
			} else {	// it's a real node, return our spaceship
				Class AIClass = node->ai ? NSClassFromString(node->ai) : NSClassFromString(((Spaceship*)node->data)->defaultAI);
				AI *ai = [[AIClass alloc] initWithShip:node->data];
				ai->gov = node->government;
				return ai;
			}
		}
	}
	
	return nil;
}

- (NSString *) description{
	return [NSString stringWithFormat:@"<Type: %@>", self->ID];
}

@end


@implementation TypeNode

- initWithDictionary:(NSDictionary *)dict array:(BOOL)array{
	self = [super init];		

	if( !array ){
		data = [Controller componentNamed:[dict objectForKey:@"ShipID"]];
		government = [Controller componentNamed:[dict objectForKey:@"GovernmentID"]];
		ai = [dict objectForKey:@"AI"];
	}
	else {
		data = [[Type alloc] initWithArray:[dict objectForKey:@"Data"]];
	}
	probability = [[dict objectForKey:@"Probability"] floatValue];

	return self;
}

- (NSString *) description{
	return [NSString stringWithFormat:@"%f: %@", probability, [data description]];
}

@end