#ifndef agl_String_h#define agl_String_h#include <AGL/agl.h>#ifdef __cplusplusextern "C" {#endifvoid DrawCStringGL (const char * cstrOut, GLuint fontList);GLuint BuildFontGL (AGLContext ctx, GLint fontID, Style face, GLint size);void DeleteFontGL (GLuint fontList);#ifdef __cplusplus}#endif#endif // agl_String_h