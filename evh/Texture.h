//
//  Texture.h
//  Event Horizon
//
//  Created by Paul Dorman on 2/16/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <Carbon/Carbon.h>

@interface Texture : NSObject {

}

+ (void) loadTexture:(NSString *)file into:(GLuint *)tex withWidth:(int *)width height:(int *)height;
+ (void) loadTextureFromGWorld:(GWorldPtr)offScreen into:(GLuint *)tex;

@end
