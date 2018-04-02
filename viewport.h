#ifndef __CS489_VIEWPORT_H
#define __CS489_VIEWPORT_H

/** \details When the game first starts, there is usually only one viewport that consumes the whole
 *  window client area. For splitscreen and overlay ports, we must divide the screenspace into mul-
 *  tiple viewports. We are going to support 1-, 2-, 3-, and 4-player splitscreen along with a sin-
 *  gle overlay port unique to each viewport. The overlay port is used for testing rendering algor-
 *  ithms, such as portal rendering, lighting, shadows, etc., where it is important to render some-
 *  thing from a different perspective than that of the camera. The "canvas" represents
 */

#include "errors.h"
#include "camera.h"

// Canvas Functions
ErrorCode InitCanvas(uint32 dx, uint32 dy);
void FreeCanvas(void);
ErrorCode SetCanvasDimensions(uint32 dx, uint32 dy);
const uint32* GetCanvasDimensions(void);
void SetCanvasViewportNumber(uint32 n);
uint32 GetCanvasViewportNumber(void);
void LayoutCanvas(void);
ErrorCode UpdateCanvas(void);

// Viewport Functions
void EnableViewport(uint32 index, bool state = true);
bool IsViewportEnabled(uint32 index);
const uint32* GetViewport(uint32 index);

// Camera Functions
OrbitCamera* GetViewportCamera(uint32 index);
OrbitCamera* GetOverlayCamera(uint32 index);
ErrorCode UpdateViewportCamera(uint32 index);
ErrorCode UpdateOverlayCamera(uint32 index);
ID3D11Buffer* GetViewportCameraBuffer(uint32 index);
ID3D11Buffer* GetOverlayCameraBuffer(uint32 index);
ErrorCode RenderViewportOrbitBox(uint32 index);
ErrorCode RenderOverlayOrbitBox(uint32 index);

// Mouse Functions
uint32 GetViewportIndexFromPosition(int x, int y);
OrbitCamera* GetViewportCameraFromPosition(int x, int y);


#endif
