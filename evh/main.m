#define kUseFades

#import <Carbon/Carbon.h>
#import <DrawSprocket/DrawSprocket.h>
#import <AGL/agl.h>
#import <AGL/aglRenderers.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <Quartz/Quartz.h>

#import <math.h>
#import <stdio.h>
#import <string.h>
 
#import "Player.h"
#import "aglString.h"
#import "Interface.h"
#import "MyWindow.h"
#import "EHButton.h"
#import "BackgroundMusic.h"
#import "main.h"

extern void (* drawScene)(void);
extern void DrawGLScene();
extern int gScreenWidth;
extern int gScreenHeight;

// functions (internal/private) ---------------------------------------------

static UInt32 CheckMacOSX (void);
void CToPStr (StringPtr outString, const char *inString);
void ReportError (char * strError);
OSStatus DSpDebugStr (OSStatus error);
GLenum aglDebugStr (void);
GLenum glDebugStr (void);

CGrafPtr SetupDSpFullScreen (GDHandle hGD);
void ShutdownDSp (CGrafPtr pDSpPort);

AGLContext SetupAGL (GDHandle hGD, AGLDrawable win);
void CleanupAGL (AGLContext ctx);
void initGL();
void init();

Boolean SetUp (void);
void DoUpdate (void);

pascal void IdleTimer (EventLoopTimerRef inTimer, void* userData);
EventLoopTimerUPP GetTimerUPP (void);

pascal OSStatus HandleEvent( EventHandlerCallRef nextHandler, EventRef event, void *userData );
void CleanUp (void);

void translateToLocal( int *x, int *y );

void (* eventScene)( EventRef event );
//void (* eventScene)( EventRecord *event );
void (* nsEventScene)( NSEvent *event );

// statics/globals (internal only) ------------------------------------------

enum {
	kForegroundSleep = 0,
	kBackgroundSleep = 10000
};
EventLoopTimerRef gTimer = NULL;

enum {
	kBitsPerPixel = 32,
	kContextWidth = 1024,
	kContextHeight = 768
};

const RGBColor rgbBlack = { 0x0000, 0x0000, 0x0000 };

NumVersion gVersionDSp;
DSpContextAttributes gContextAttributes;
DSpContextReference gContext = 0;
//AGLContext gaglContext = 0;
CGContextRef cgContext = 0;
GLuint font;
GLuint buttonFont;
char gcstrMode [256] = "";

CGLContextObj fullScreenContextObj;

AGLDrawable gpDSpPort = NULL; // will be NULL for full screen under X
Rect gRectPort = {0, 0, 0, 0};

UInt32 gSleepTime = kForegroundSleep;
Boolean gfFrontProcess = true;

UnsignedWide t1;
UInt64 u1;

NSWindow *window;
BOOL full;
BOOL toggleFull;
GDHandle hTargetDevice;
Interface *interface;

long keyMap5;

BOOL mouseMoved;

#pragma mark -

void debugString( NSString *out ){
	
}

//-----------------------------------------------------------------------------------------------------------------------

// are we running on Mac OS X
// returns 0 if < Mac OS X or version number of Mac OS X (10.0 for GM)

static UInt32 CheckMacOSX(void){
	UInt32 response;
    
	if( (Gestalt(gestaltSystemVersion, (SInt32 *) &response) == noErr) && (response >= 0x01000) )
		return response;
	else
		return 0;
}

//-----------------------------------------------------------------------------------------------------------------------

// Copy C string to Pascal string

void CToPStr( StringPtr outString, const char *inString ){	
	unsigned char x = 0;
	do
		*(((char*)outString) + x + 1) = *(inString + x++);
	while ((*(inString + x) != 0)  && (x < 256));
	*((char*)outString) = (char) x;									
}

// --------------------------------------------------------------------------

void ReportError( char * strError ){
	char errMsgCStr [256];
	Str255 strErr;

	sprintf (errMsgCStr, "%s", strError); 

	// out as debug string
	CToPStr (strErr, errMsgCStr);
	DebugStr (strErr);
}

//-----------------------------------------------------------------------------------------------------------------------

OSStatus DSpDebugStr ( OSStatus error ){
	switch ( error ){
		case noErr:
			break;
		case kDSpNotInitializedErr:
			ReportError ("DSp Error: Not initialized");
			break;
		case kDSpSystemSWTooOldErr:
			ReportError ("DSp Error: system Software too old");
			break;
		case kDSpInvalidContextErr:
			ReportError ("DSp Error: Invalid context");
			break;
		case kDSpInvalidAttributesErr:
			ReportError ("DSp Error: Invalid attributes");
			break;
		case kDSpContextAlreadyReservedErr:
			ReportError ("DSp Error: Context already reserved");
			break;
		case kDSpContextNotReservedErr:
			ReportError ("DSp Error: Context not reserved");
			break;
		case kDSpContextNotFoundErr:
			ReportError ("DSp Error: Context not found");
			break;
		case kDSpFrameRateNotReadyErr:
			ReportError ("DSp Error: Frame rate not ready");
			break;
		case kDSpConfirmSwitchWarning:
//			ReportError ("DSp Warning: Must confirm switch"); // removed since it is just a warning, add back for debugging
			return 0; // don't want to fail on this warning
			break;
		case kDSpInternalErr:
			ReportError ("DSp Error: Internal error");
			break;
		case kDSpStereoContextErr:
			ReportError ("DSp Error: Stereo context");
			break;
	}
	return error;
}

//-----------------------------------------------------------------------------------------------------------------------

// if error dump agl errors to debugger string, return error

GLenum aglDebugStr (void){
	GLenum err = aglGetError();
	if (AGL_NO_ERROR != err)
		ReportError ((char *)aglErrorString(err));
	return err;
}

//-----------------------------------------------------------------------------------------------------------------------

// if error dump agl errors to debugger string, return error

GLenum glDebugStr (void){
	GLenum err = glGetError();
	if (GL_NO_ERROR != err)
		ReportError ((char *)gluErrorString(err));
	return err;
}

#pragma mark -
//-----------------------------------------------------------------------------------------------------------------------

// Set up DSp screen on graphics device requested
// side effect: sets both gpDSpWindow and gpPort

CGrafPtr SetupDSpFullScreen (GDHandle hGD){
	DSpContextAttributes foundAttributes;
	DisplayIDType displayID = 0;
	CGrafPtr port;
	GDHandle temp;
	Rect bounds;
	
	GetGWorld( &port, &temp );
	GetPortBounds( port, &bounds );

	gVersionDSp = DSpGetVersion ();

	// Note: DSp < 1.7.3 REQUIRES the back buffer attributes even if only one buffer is required
	memset(&gContextAttributes, 0, sizeof (DSpContextAttributes));
	gContextAttributes.displayWidth			= bounds.right - bounds.left;
	gContextAttributes.displayHeight		= bounds.bottom - bounds.top;
	gContextAttributes.colorNeeds			= kDSpColorNeeds_Require;
	gContextAttributes.displayBestDepth		= kBitsPerPixel;
	gContextAttributes.backBufferBestDepth	= kBitsPerPixel;
	gContextAttributes.displayDepthMask		= kDSpDepthMask_All;
	gContextAttributes.backBufferDepthMask	= kDSpDepthMask_All;
	gContextAttributes.pageCount			= 1;								// only the front buffer is needed
	
	DMGetDisplayIDByGDevice( hGD, &displayID, true );	
	
	if (noErr != DSpDebugStr (DSpFindBestContextOnDisplayID (&gContextAttributes, &gContext, displayID))){
		ReportError ("DSpFindBestContext() had an error.");
		return NULL;
	}

	if (noErr != DSpDebugStr (DSpContext_GetAttributes (gContext, &foundAttributes))){ // see what we actually found
		ReportError ("DSpContext_GetAttributes() had an error.");
		return NULL;
	}

	// reset width and height to full screen and handle our own centering
	// HWA will not correctly center less than full screen size contexts
	gContextAttributes.displayWidth 	= foundAttributes.displayWidth;
	gContextAttributes.displayHeight 	= foundAttributes.displayHeight;
	gContextAttributes.pageCount		= 1;									// only the front buffer is needed
	gContextAttributes.contextOptions	= kDSpContextOption_DontSyncVBL;	// no page flipping and no VBL sync needed

	DSpSetBlankingColor(&rgbBlack);

	if (noErr !=  DSpDebugStr (DSpContext_Reserve ( gContext, &gContextAttributes))){ // reserve our context
		ReportError ("DSpContext_Reserve() had an error.");
		return NULL;
	}

	if (noErr != DSpDebugStr (DSpContext_SetState (gContext, kDSpContextState_Active))){ // activate our context
		ReportError ("DSpContext_SetState() had an error.");
		return NULL;
	}


	if ((CheckMacOSX ()) && !((gVersionDSp.majorRev > 0x01) || ((gVersionDSp.majorRev == 0x01) && (gVersionDSp.minorAndBugRev >= 0x99)))){// DSp should be supported in version after 1.98
		ReportError ("Mac OS X with DSp < 1.99 does not support DrawSprocket for OpenGL full screen");
		return NULL;
	}
	else if (CheckMacOSX ()){ // DSp should be supported in versions 1.99 and later
		CGrafPtr pPort;
		// use DSp's front buffer on Mac OS X
		if (noErr != DSpDebugStr (DSpContext_GetFrontBuffer (gContext, &pPort))){
			ReportError ("DSpContext_GetFrontBuffer() had an error.");
			return NULL;
		}
		return pPort;
	}
	return NULL;
}

//-----------------------------------------------------------------------------------------------------------------------

// clean up DSp

void ShutdownDSp (CGrafPtr pDSpPort){
	if ((NULL != pDSpPort) && !CheckMacOSX ())
		DisposeWindow (GetWindowFromPort (pDSpPort));
	DSpContext_SetState( gContext, kDSpContextState_Inactive);
	DSpContext_Release (gContext);
}

#pragma mark -
//-----------------------------------------------------------------------------------------------------------------------

// OpenGL Setup

AGLContext SetupAGL (GDHandle hGD, AGLDrawable drawable){
	GLint			attrib[64];
	AGLPixelFormat 	fmt;
	AGLContext     	ctx;
	GLint			sync = 0;			// use vertical sync?

// different possible pixel format choices for different renderers 
// basics requirements are RGBA and double buffer
// OpenGL will select acclerated context if available

	short i = 0;
	attrib [i++] = AGL_RGBA; // red green blue and alpha
	attrib [i++] = AGL_DOUBLEBUFFER; // double buffered
	attrib [i++] = AGL_ACCELERATED; // HWA pixel format only
	attrib [i++] = AGL_DEPTH_SIZE;	// need a depth buffer
	attrib [i++] = 32;
	attrib [i++] = AGL_NONE;

	if ((Ptr) kUnresolvedCFragSymbolAddress == (Ptr) aglChoosePixelFormat){ // check for existance of OpenGL
		ReportError ("OpenGL not installed");
		return NULL;
	}	

	if (hGD)
		fmt = aglChoosePixelFormat (&hGD, 1, attrib); // get an appropriate pixel format
	else
		fmt = aglChoosePixelFormat(NULL, 0, attrib); // get an appropriate pixel format
	aglDebugStr ();
	if (NULL == fmt) {
		ReportError("Could not find valid pixel format");
		return NULL;
	}

	ctx = aglCreateContext (fmt, NULL); // Create an AGL context
	aglDebugStr ();
	if (NULL == ctx){
		ReportError ("Could not create context");
		return NULL;
	}
	
	// aglSetHIViewRef

	if (!aglSetDrawable (ctx, drawable)){ // attach the window to the context
		ReportError ("SetDrawable failed");
		aglDebugStr ();
		return NULL;
	}


	if (!aglSetCurrentContext (ctx)){ // make the context the current context
		aglDebugStr ();
		aglSetDrawable (ctx, NULL);
		return NULL;
	}

	aglDestroyPixelFormat(fmt); // pixel format is no longer needed
	
	aglSetInteger( ctx, AGL_SWAP_INTERVAL, &sync );

	initGL();
	
	return ctx;
}

//-----------------------------------------------------------------------------------------------------------------------

// OpenGL Cleanup

void CleanupAGL(AGLContext ctx){
	aglSetDrawable (ctx, NULL);
	aglSetCurrentContext (NULL);
	aglDestroyContext (ctx);
}

//-----------------------------------------------------------------------------------------------------------------------

// OpenGL Drawing


#pragma mark -


// --------------------------------------------------------------------------

Boolean SetUp (void){
	short fNum = 0;
	EventTypeSpec eventTypes[] = { {kEventClassMouse, kEventMouseDown},
								{kEventClassMouse, kEventMouseUp},
								{kEventClassMouse, kEventMouseWheelMoved},
								{kEventClassMouse, kEventMouseMoved},
								{kEventClassMouse, kEventMouseDragged},
								{kEventClassKeyboard, kEventRawKeyDown},
								{kEventClassKeyboard, kEventRawKeyRepeat},
								{kEventClassCommand, kEventProcessCommand} };

	InitCursor();
	
	gpDSpPort = NULL;
//	gaglContext = 0;
	
	// check for DSp
	if ((Ptr) kUnresolvedCFragSymbolAddress == (Ptr) DSpStartup) 
		ReportError ("DSp not installed");

	if (noErr != DSpDebugStr (DSpStartup()))
		return false;
//	DSpSetDebugMode (true);
	
	//hTargetDevice = DMGetFirstScreenDevice (true); // check number of screens

#if defined kUseFades
	//DSpDebugStr (DSpContext_FadeGammaOut (NULL, NULL)); // fade display, remove for debug
#endif

	NSApplicationLoad();
	window = [[MyWindow alloc] initWithContentRect:NSMakeRect(CGDisplayPixelsWide(kCGDirectMainDisplay)/2-400, CGDisplayPixelsHigh(kCGDirectMainDisplay)/2-300, 800, 600) styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO];
	[window setTitle:@"Event Horizon"];
	[window setAcceptsMouseMovedEvents:YES];
	[window makeKeyAndOrderFront:nil];
	full = NO;
	
	/*if (NULL != (gpDSpPort = SetupDSpFullScreen (hTargetDevice))){ // Setup DSp for OpenGL sets hTargetDeviceto device actually used 
		GetPortBounds (gpDSpPort, &gRectPort);
		sprintf (gcstrMode, "DrawSprocket Full Screen: %d x %d x %d", gRectPort.right - gRectPort.left, gRectPort.bottom - gRectPort.top, (**(**hTargetDevice).gdPMap).pixelSize);
		gaglContext = SetupAGL (hTargetDevice, gpDSpPort);
		cgContext = CGDisplayGetDrawingContext(kCGDirectMainDisplay);
	}*/
	
	gRectPort.left = 0;
	gRectPort.top = 0;
	gRectPort.right = CGDisplayPixelsWide(kCGDirectMainDisplay);
	gRectPort.bottom = CGDisplayPixelsHigh(kCGDirectMainDisplay);
	
	CGLPixelFormatObj pixelFormatObj;
	GLint numPixelFormats;
	CGDisplayCapture(kCGDirectMainDisplay);
	
	CGLPixelFormatAttribute attribs[] = {
		
		// The full-screen attribute
		kCGLPFAFullScreen,
		
		// The display mask associated with the captured display
		// We may be on a multi-display system (and each screen may be driven by a different renderer), so we need to specify which screen we want to take over. For this demo, we'll specify the main screen.
		kCGLPFADisplayMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
		
		// Attributes common to full-screen and non-fullscreen
		kCGLPFAAccelerated,
		kCGLPFANoRecovery,
		kCGLPFADoubleBuffer,
		kCGLPFAColorSize, 24,
		kCGLPFADepthSize, 32,
        0
    };
	
	CGLChoosePixelFormat(attribs, &pixelFormatObj, &numPixelFormats);
	CGLCreateContext(pixelFormatObj, NULL, &fullScreenContextObj);
	CGLDestroyPixelFormat(pixelFormatObj);
	
	if (!fullScreenContextObj) {
        NSLog(@"Failed to create full-screen context");
		CGReleaseAllDisplays();
        return false;
    }
	
	// Set the current context to the one to use for full-screen drawing
	CGLSetCurrentContext(fullScreenContextObj);
	
	// Attach a full-screen drawable object to the current context
	CGLSetFullScreen(fullScreenContextObj);

	cgContext = CGDisplayGetDrawingContext(kCGDirectMainDisplay);
	initGL();
	
    // Lock us to the display's refresh rate
    GLint newSwapInterval = 1;
    CGLSetParameter(fullScreenContextObj, kCGLCPSwapInterval, &newSwapInterval);
	
	
#if defined kUseFades
	DSpDebugStr (DSpContext_FadeGammaIn (NULL, NULL));
#endif

	/*if (gaglContext){
		fNum = FMGetFontFamilyFromName("\pChicago");	// build font
		if( fNum == kInvalidFontFamily )
			fNum = 0;
		font = BuildFontGL (gaglContext, fNum, normal, 12 );
		buttonFont = BuildFontGL( gaglContext, fNum, bold, 14 );
		InstallEventLoopTimer (GetCurrentEventLoop(), 0.01, 0.0001, GetTimerUPP (), 0, &gTimer);
	}*/
	
	InstallApplicationEventHandler( NewEventHandlerUPP(HandleEvent), 8, eventTypes, NULL, NULL );

	init();

	/*if (gaglContext)
		return true;
	else
		return false;*/
	
	return true;
}

// --------------------------------------------------------------------------

void DoUpdate (void){
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	//aglSetCurrentContext (gaglContext); // ensure our context is current prior to drawing
	CGLSetCurrentContext(fullScreenContextObj);
	drawScene();
	//aglSwapBuffers(gaglContext); // send swap command
	CGLFlushDrawable(fullScreenContextObj);

	/*if( toggleFull ){
		full = !full;
		toggleFull = NO;
		if( !full ){
			void *ref = [window windowRef];
			ShutdownDSp (gpDSpPort);
			GetWindowPortBounds( ref, &gRectPort );
			aglSetDrawable( gaglContext, GetWindowPort(ref) );
			initGL();
			sys->screenWidth = gRectPort.right - gRectPort.left;
			sys->screenHeight = gRectPort.bottom - gRectPort.top;
			if( ![NSApp isRunning] )
				[NSApp run];
			[window makeKeyAndOrderFront:NSApp];
			NSLog( @"<%d, %d>", (gRectPort.right+gRectPort.left)/2, (gRectPort.bottom+gRectPort.top)/2 );
			NSLog( @"CGWarpMouseCursorPosition Error: %d", CGWarpMouseCursorPosition( CGPointMake( (gRectPort.right+gRectPort.left)/2, (gRectPort.bottom+gRectPort.top)/2 ) ) );
		} else {
			gpDSpPort = SetupDSpFullScreen( hTargetDevice );
			GetPortBounds( gpDSpPort, &gRectPort );
			aglSetDrawable( gaglContext, gpDSpPort );
			initGL();
			sys->screenWidth = gRectPort.right - gRectPort.left;
			sys->screenHeight = gRectPort.bottom - gRectPort.top;
			[NSApp stop:sys];
		}
		[sys setUpFrame];
	}*/
	
	if( sys ){
		Microseconds( &t1 );
		sys->FACTOR = (UnsignedWideToUInt64( t1 ) - u1)/1E6;
		u1 = UnsignedWideToUInt64( t1 );
		if( sys->pause )
			sys->FACTOR = 0;
		if( sys->t == 0. ){
			sys->t = 0.01;
			sys->FACTOR = 0;
		} else
			sys->t += sys->FACTOR;
	}
	
	[pool release];
}

// --------------------------------------------------------------------------

pascal void IdleTimer (EventLoopTimerRef inTimer, void* userData){
	#pragma unused (inTimer, userData)
	UnsignedWide t;
	UInt64 lastRefresh = 0;
	
	Microseconds(&t);
	if( (UnsignedWideToUInt64(t) - lastRefresh)*1000000 > refreshTime ){
		DoUpdate();
		lastRefresh = UnsignedWideToUInt64(t);
	}
}

// --------------------------------------------------------------------------

EventLoopTimerUPP GetTimerUPP (void){
	static EventLoopTimerUPP	sTimerUPP = NULL;
	
	if (sTimerUPP == NULL)
		sTimerUPP = NewEventLoopTimerUPP (IdleTimer);
	
	return sTimerUPP;
}

// --------------------------------------------------------------------------

pascal OSStatus HandleEvent( EventHandlerCallRef nextHandler, EventRef event, void *userData ){
	UInt32 kind = GetEventKind(event);
	
	if( eventScene )
		eventScene( event );
	[EHButton handleEvent:event];

	switch( GetEventClass(event) ){
		case kEventClassKeyboard:
			// reaches here in 10.2-10.3, not 10.4
			break;
		case kEventClassMouse:
			switch( kind ){
				UInt16 button;
				SInt32 delta;
				case kEventMouseDown:
					GetEventParameter( event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(UInt16), NULL, &button );
					if( button == kEventMouseButtonPrimary )
						keyMap5 |= 0x1;
					else if( button == kEventMouseButtonSecondary )
						keyMap5 |= 0x2;
					break;
				case kEventMouseUp:
					GetEventParameter( event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(UInt16), NULL, &button );
					if( button == kEventMouseButtonPrimary )
						keyMap5 &= 0xFFFFFFFE;
					else if( button == kEventMouseButtonSecondary )
						keyMap5 &= 0xFFFFFFFD;
					break;
				case kEventMouseWheelMoved:
					GetEventParameter( event, kEventParamMouseWheelDelta, typeSInt32, NULL, sizeof(SInt32), NULL, &delta );
					if( delta > 0 )
						keyMap5 |= 0x4;
					else if( delta < 0 )
						keyMap5 |= 0x8;
					break;
				case kEventMouseMoved:
				case kEventMouseDragged:
					mouseMoved = YES;
					break;
			}
			break;
		case kEventClassCommand:
			if( kind == kEventProcessCommand ){
				HICommand c;
				GetEventParameter( event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &c );
				if( c.commandID == kHICommandQuit || c.commandID == 0 )
					QuitApplicationEventLoop();		// we get 0 if back from windowed mode
			}
			break;
	}
	
	return noErr;
}

// --------------------------------------------------------------------------

void CleanUp (void){
	RemoveEventLoopTimer(gTimer);
	gTimer = NULL;

	DeleteFontGL( font );
	DeleteFontGL( buttonFont );
	[window orderOut:nil];
	//CleanupAGL( gaglContext) ; // Cleanup the OpenGL context
	if (full){
	#if defined kUseFades
		DSpDebugStr (DSpContext_FadeGammaOut (NULL, NULL)); // fade display, remove for debug
	#endif

		ShutdownDSp (gpDSpPort); // DSp shutdown
		gpDSpPort = NULL;
		
		// Set the current context to NULL
		CGLSetCurrentContext(NULL);
		// Clear the drawable object
		CGLClearDrawable(fullScreenContextObj);
		// Destroy the rendering context
		CGLDestroyContext(fullScreenContextObj);
		// Release the displays
		CGReleaseAllDisplays();
	
	#if defined kUseFades
		DSpDebugStr (DSpContext_FadeGammaIn (NULL, NULL)); // fade display, remove for debug
	#endif
	}
	DSpShutdown ();
}

void initGL(){
	glViewport (0, 0, gRectPort.right - gRectPort.left, gRectPort.bottom - gRectPort.top );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0, 1.0*(gRectPort.right-gRectPort.left)/(gRectPort.bottom-gRectPort.top), 0.01, 500 );
	glMatrixMode( GL_MODELVIEW );

	glEnable(GL_DEPTH_TEST);			// Enables Depth Testing 
	glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
	glEnable( GL_CULL_FACE );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void init(){
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];
	Microseconds( &t1 );
	u1 = UnsignedWideToUInt64( t1 );
	random();
	[BackgroundMusic startMusic];
	gScreenWidth = gRectPort.right - gRectPort.left;
	gScreenHeight = gRectPort.bottom - gRectPort.top;
	interface = [[Interface alloc] init];
	[NSApp setDelegate:interface];
	[window makeKeyAndOrderFront:nil];
	if( [NSApp isRunning] )
		[NSApp stop:nil];
	beginPlayer();
}

void getKeys( KeyMap2 map ){
	GetKeys( map );
	map[4] = keyMap5;
	keyMap5 &= 0x3;		// clear scroll wheels
}

// --------------------------------------------------------------------------

int main (void){
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (SetUp ())	
		RunApplicationEventLoop();
	CleanUp ();
	[pool release];
	return 0;
}