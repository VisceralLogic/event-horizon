//
//  ControllerScripting.h
//  Event Horizon
//
//  Created by Paul Dorman on 3/12/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "Controller.h"


@interface Controller (ControllerScripting)

- (NSMutableArray *) planets;
- (Solarsystem *) system;
- (void) newMessage:(NSString *)message;
- (NSMutableArray *) ships;
- (NSMutableArray *) weaps;
- (NSMutableArray *) missions;
- (NSMutableDictionary *) plugins;
- (float) time;
- (NSDate *) date;
- (void) setDate:(NSDate *)date;
- (NSMutableDictionary *) govRecord;
- (int) combatRating;
- (void) setCombatRating:(int)rating;
- (int) valForFlag:(NSString *)flag;		// returns integer value for flag requirement evaluation

@end
