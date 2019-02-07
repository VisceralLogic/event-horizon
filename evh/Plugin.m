//
//  Plugin.m
//  Event Horizon
//
//  Created by user on Mon Dec 09 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "Plugin.h"


@implementation Plugin

- initWithController:(Controller *)controller{
	sys = controller;
	
	[self initData];
	
	return self;
}

- (void) initData{
	NSFileManager *manager = [NSFileManager defaultManager];
	NSString *oldDirPath = [manager currentDirectoryPath];
	NSDictionary *info = [[NSBundle bundleForClass:[self class]] infoDictionary];
	NSString *name = [[info objectForKey:@"CFBundleExecutable"] lowercaseString];
	NSString *file;
	NSDirectoryEnumerator *enumerator;

	[manager changeCurrentDirectoryPath:[info objectForKey:@"NSBundleResourcePath"]];
	
	enumerator = [manager enumeratorAtPath:@"."];
	while( file = [enumerator nextObject] ){
		if( [[file pathExtension] isEqualToString:@"evh"] ){
			[self loadFile:file forPlugin:name];
		}
	}
	
	[manager changeCurrentDirectoryPath:oldDirPath];
}

- (void) loadFile:(NSString *)file forPlugin:(NSString *)name{
	CFStringRef errorString;
	NSMutableDictionary *dictionary = CFPropertyListCreateFromXMLData( NULL, (CFDataRef)[[NSFileManager defaultManager] contentsAtPath:file], kCFPropertyListImmutable, &errorString ); 
	if( dictionary ){
		NSString *type = [dictionary objectForKey:@"Type"];
		Class MyClass = NSClassFromString( type );
		[dictionary setObject:name forKey:@"PluginName"];
		[MyClass registerFromDictionary:dictionary];
	}
}

@end
