//
//  Mod.m
//  Event Horizon
//
//  Created by user on Tue Apr 08 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Mod.h"
#import "Controller.h"
#import "Texture.h"

@implementation Mod

+ (void) registerFromDictionary:(NSDictionary *)dictionary{
	NSMutableDictionary *plugin = [Controller ensurePlugin:[dictionary objectForKey:@"PluginName"] path:@"mods"];
	Mod *mod = [[Mod alloc] init];

	mod->name = [[dictionary objectForKey:@"Name"] copy];
	mod->ID = [[NSString alloc] initWithFormat:@"%@.%@.%@", [dictionary objectForKey:@"PluginName"], @"mods", [dictionary objectForKey:@"ID"]];
	mod->mass = [[dictionary objectForKey:@"Mass"] intValue];
	mod->flags = [[dictionary objectForKey:@"Flags"] mutableCopyWithZone:nil];
	mod->description = [[dictionary objectForKey:@"Description"] copy];
	mod->price = [[dictionary objectForKey:@"Price"] intValue];
	if( [dictionary objectForKey:@"Max"] )
		mod->max = [[dictionary objectForKey:@"Max"] intValue];
	else
		mod->max = -1;
	mod->initData = [dictionary objectForKey:@"Ammo"];
	glGenTextures( 1, &mod->modPic );
	[Texture loadTexture:[dictionary objectForKey:@"ModPicture"] into:&mod->modPic withWidth:nil height:nil];
	mod->flagRequirements = [[dictionary objectForKey:@"FlagRequirements"] copy];
	mod->value = [[dictionary objectForKey:@"Value"] floatValue];
	if( [[dictionary objectForKey:@"ModType"] isEqualToString:@"LAUNCHER"] )
		mod->modType = LAUNCHER;
	else if( [[dictionary objectForKey:@"ModType"] isEqualToString:@"COMPENSATOR"] )
		mod->modType = COMPENSATOR;
	else if( [[dictionary objectForKey:@"ModType"] isEqualToString:@"FUELTANK"] )
		mod->modType = FUELTANK;
	else if( [[dictionary objectForKey:@"ModType"] isEqualToString:@"AFTERBURNER"] )
		mod->modType = AFTERBURNER;
	mod->value2 = [[dictionary objectForKey:@"Value2"] floatValue];
	mod->value3 = [[dictionary objectForKey:@"Value3"] floatValue];
	
	if( [dictionary objectForKey:@"Replace"] )
		[plugin setObject:mod forKey:[dictionary objectForKey:@"Replace"]];
	else
		[plugin setObject:mod forKey:[dictionary objectForKey:@"ID"]];
}

- shipInstance{
	Mod *mod = [[Mod alloc] init];
	mod->name = name;
	mod->ID = self->ID;
	mod->mass = mass;
	mod->flags = flags;
	mod->max = max;
	mod->count = 1;
	mod->ammo = ammo;
	mod->value = value;
	mod->value2 = value2;
	mod->value3 = value3;
	mod->modType = modType;
	return mod;
}

- (void) finalize{
	if( initData ){
		ammo = [Controller componentNamed:initData];
	}
}

- (NSString *) description{
	return [NSString stringWithFormat:@"<Mod: %@>", self->ID];
}

- (int) price{
	return price;
}

- (int) mass{
	return mass;
}

- (NSString *) ID{
	return self->ID;
}

- (void) incrementCount{
	count++;
}

- (int) count{
	return count;
}

- (int) max{
	return max;
}

- (NSMutableDictionary *) flags{
	return flags;
}

- (NSString *) desc{
	return description;
}

- (GLuint) texture{
	return modPic;
}

- (NSString *) name{
	return name;
}

- (NSString *) flagRequirements{
	return flagRequirements;
}

@end
