#import <AGL/agl.h>
#import <OpenGL/glext.h>
#import <Cocoa/Cocoa.h>

typedef long KeyMap2[5];

//extern void (* eventScene)( EventRecord *event );
extern void (* nsEventScene)( NSEvent *event );
extern void (* eventScene)( EventRef event );
extern GLuint font;
extern GLuint buttonFont;
//extern AGLContext gaglContext;
extern CGContextRef cgContext;
extern BOOL full;
extern BOOL toggleFull;
void getKeys( KeyMap2 map );
void debugString( NSString *out );

//extern CGLContextObj fullScreenContextObj;