//
//  Texture.m
//  Event Horizon
//
//  Created by Paul Dorman on 2/16/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "Texture.h"


@implementation Texture

+ (void) loadTexture:(NSString *)file into:(GLuint *)tex withWidth:(int *)width height:(int *)height{
    NSBitmapImageRep *bitmap;
    long bitmapWidth, bitmapHeight, bitmapBytes, bitmapBPP;
    long row, col, srcRow, destRow, bytes;
    char *flippedBitmap, *bitmapSource;
	int format;
	//	NSLog( @"loadTexture:%@ into:%d", file, *tex );
	if( *tex == 0 )
		glGenTextures( 1, tex );
	
	bitmap = [NSBitmapImageRep imageRepWithContentsOfFile:file];
	bitmapWidth = [bitmap pixelsWide];
	bitmapHeight = [bitmap pixelsHigh];
	bitmapBPP = [bitmap bitsPerPixel];
	
	switch (bitmapBPP){			// no alpha channel
		case 24:
			bytes = 3;
			format = GL_RGB;
			break;
		case 32:			// alpha channel
			bytes = 4;
			format = GL_RGBA;
			break;
		default:
			break;
	}
	
	bitmapBytes = bitmapWidth * bitmapHeight * bytes;
	flippedBitmap = malloc( bitmapBytes );
	bitmapSource = [bitmap bitmapData];
	
	for( row = 0; row < bitmapHeight; row++ ){
		destRow = row * bitmapWidth * bytes;
		srcRow = ( bitmapHeight - row - 1 ) * bitmapWidth * bytes;
		for( col = 0; col < bitmapWidth * bytes; col += bytes ){
			switch( bitmapBPP ){
				case 24:
					flippedBitmap[destRow + col] = bitmapSource[srcRow + col];
					flippedBitmap[destRow + col + 1] = bitmapSource[srcRow + col + 1];
					flippedBitmap[destRow + col + 2] = bitmapSource[srcRow + col + 2];
					break;
				case 32:
					flippedBitmap[destRow + col] = bitmapSource[srcRow + col];
					flippedBitmap[destRow + col + 1] = bitmapSource[srcRow + col + 1];
					flippedBitmap[destRow + col + 2] = bitmapSource[srcRow + col + 2];
					flippedBitmap[destRow + col + 3] = bitmapSource[srcRow + col + 3];
					break;
			}
		}
	}
	
	glBindTexture( GL_TEXTURE_2D, *tex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, format, bitmapWidth, bitmapHeight, 0, format, GL_UNSIGNED_BYTE,	flippedBitmap );
	free( flippedBitmap );
	
	if( width )
		*width = bitmapWidth;
	if( height )
		*height = bitmapHeight;
}

+ (void) loadTextureFromGWorld:(GWorldPtr)offScreen into:(GLuint *)tex{
	Rect rect;
	char *map;
	int x, y, width, height;
	unsigned char *data = GetPixBaseAddr(GetGWorldPixMap(offScreen));
	int index = 0;
	int rowBytes = GetPixRowBytes(GetGWorldPixMap(offScreen));
	
	GetPortBounds( offScreen, &rect );
	width = rect.right-rect.left;
	height = rect.bottom-rect.top;
	map = malloc( width*height*4 );
	
	for (y = 0; y < height; y++){
		for (x = 0; x < width; x++){
#ifdef __ppc__
			map[index++] = *(++data);
			map[index++] = *(++data);
			map[index++] = *(++data);
			map[index++] = 0;
			data++;
#else
			map[index++] = *(data+2);
			map[index++] = *(data+1);
			map[index++] = *(data+0);
			map[index++] = 0;
			data += 4;
#endif
		}
		data += rowBytes-4*width;
	}
	
	if( *tex == 0 )
		glGenTextures( 1, tex );
	glBindTexture( GL_TEXTURE_2D, *tex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, map );
	free( map );
}

@end
