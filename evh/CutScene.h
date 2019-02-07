//
//  CutScene.h
//  Event Horizon
//
//  Created by Paul Dorman on Fri Dec 10 2004.
//  Copyright (c) 2004 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SpaceObject.h"

enum{
	LAND_1,
	LAND_2,
	LAND_3,
	LAND_STATION
};

@interface CutScene : NSObject {
@public
}

+ (void) setup:(int)which;

@end