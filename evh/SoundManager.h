//
//  SoundManager.h
//  Event Horizon
//
//  Created by Paul Dorman on Fri Jan 28 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SpaceObject.h"

@interface SoundManager : NSObject {

}

+ (void) initialize;
+ (unsigned int) registerSound:(NSString *)file;
+ (void) attachSound:(unsigned int)soundNum toSource:(SpaceObject *)source;
+ (void) update;
+ (BOOL) openAL;

@end
