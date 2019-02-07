
#include <Carbon/Carbon.h>

#include <AGL/agl.h>

#include <stdio.h>

#include "aglString.h"

//-----------------------------------------------------------------------------------------------------------------------

void DrawCStringGL (const char * cstrOut, GLuint fontList)
{
	GLint i = 0;
	while (cstrOut [i])
		glCallList (fontList + cstrOut[i++]);
}

//-----------------------------------------------------------------------------------------------------------------------

GLuint BuildFontGL (AGLContext ctx, GLint fontID, Style face, GLint size)
{
	GLuint listBase = glGenLists (256);
	if (aglUseFont (ctx, fontID , face, size, 0, 256, (long) listBase))
	{
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		return listBase;
	}
	else
	{
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glDeleteLists (listBase, 256);
		return 0;
	}
}

//-----------------------------------------------------------------------------------------------------------------------

void DeleteFontGL (GLuint fontList)
{
	if (fontList)
		glDeleteLists (fontList, 256);
}