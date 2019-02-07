//
//  Background.h
//  Event Horizon
//
//  Created by user on Mon Mar 17 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface Background : NSObject {
	int *x, *y, *z;
	int count;
}

- init;
- (void) generate:(float)density;
- (void) draw;

@end
