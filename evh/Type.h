//
//  Type.h
//  Event Horizon
//
//  Created by user on Wed Jun 25 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AI.h"
#import "Spaceship.h"
#import "Government.h"

@interface Type : NSObject {
@public
	NSMutableArray *data;
	NSString *ID;
}

- initWithArray:(NSArray *)array;
+ (void) registerFromDictionary:(NSDictionary *)dictionary;
- (void) finalize;
- (AI *) newInstance;

@end

@interface TypeNode : NSObject {
@public
	id data;			// either Spaceship or Type, allowing meta-Types
	id government;
	float probability;
	id ai;
}

- initWithDictionary:(NSDictionary *)dict array:(BOOL)array;

@end