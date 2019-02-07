//
//  ControllerGraphics.h
//  Event Horizon
//
//  Created by Paul Dorman on 3/12/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "Controller.h"


@interface Controller (ControllerGraphics) 

- (void) setUpFrame:(NSString *)fileName;
- (void) drawNavTab;
- (void) drawFrame;
- (void) drawString:(char *)string atX:(float)x y:(float)y;
- (void) drawSelectionTab:(int)pos;
- (void) drawInfoTab;

@end

void DrawGLScene();
void beginOrtho();
void endOrtho();
extern void (* drawScene)(void);
extern int gScreenWidth;
extern int gScreenHeight;
extern GLuint defaultFrameList;
