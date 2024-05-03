#pragma once
#ifndef SCREENGRAB_H
#define SCREENGRAB_H

#include "types.h"
#include "MMBitmap.h"
#include <stdbool.h>

#if defined(USE_X11)
#include "xdisplay.h"
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

/* Returns a raw bitmap of screengrab of the display (to be destroyed()'d by
 * caller), or NULL on error. */
MMBitmapRef copyMMBitmapFromDisplayInRect(MMRect rect);

bool captureScreenAreaToJPEG(MMRect rect, const char *outputPath, float compressionQuality);
bool captureAndResizeScreenAreaToJPEG(MMRect rect, const char *outputPath, float compressionQuality);


#ifdef __cplusplus
}
#endif

#endif /* SCREENGRAB_H */
