#ifndef __CS489_VIEWPORT_H
#define __CS489_VIEWPORT_H

/** \details When the game first starts, there is usually only one viewport that consumes the whole
 *  window client area. For splitscreen and overlay ports, we must divide the screenspace into mul-
 *  tiple viewports. We are going to support 1-, 2-, 3-, and 4-player splitscreen along with a sin-
 *  gle overlay port unique to each viewport. The overlay port is used for testing rendering algor-
 *  ithms, such as portal rendering, lighting, shadows, etc., where it is important to render some-
 *  thing from a different perspective than that of the camera. The "canvas" represents
 */

// Canvas Functions
void SetCanvasDimensions(uint32 dx, uint32 dy);
const uint32* GetCanvasDimensions(void);

// Viewport Functions
void EnableViewport(int n);
void DisableViewport(int n);


#endif
