//
//  Forklift.h
//  Event Horizon
//
//  Created by Paul Dorman on 6/19/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "Weapon.h"

@interface Forklift : Weapon {

}

+ (void) install;
- (void) newInstanceFrom:(Spaceship *)source target:(Spaceship *)target;
- shipInstance;

@end
