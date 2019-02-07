//
//  Cargo.h
//  Event Horizon
//
//  Created by user on Tue Feb 11 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface Cargo : NSObject {
@public
	NSString *name;
	int size;
	BOOL mission;
}

@end