//
//  Government.h
//  Event Horizon
//
//  Created by user on Sun May 25 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Government : NSObject {
@public
	NSString *name;
	NSString *ID;
	NSMutableArray *enemies;
	NSMutableArray *allies;
	float red, green, blue;
	id initData;
	int shootPenalty, disablePenalty, destroyPenalty, boardPenalty;
}

+ (void) registerFromDictionary:(NSDictionary *)dictionary;
- (void) finalize;

- (NSMutableArray *) enemies;
- (NSMutableArray *) allies;

@end
