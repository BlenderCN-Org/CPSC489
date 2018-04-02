#include "stdafx.h"
#include "viewport.h"

static uint32 canvas_dim[2] = { 1, 1 };
static uint32 n_viewports = 1;
static bool viewport_states[4] = { true, false, false, false };

void SetCanvasDimensions(uint32 dx, uint32 dy)
{
 // nothing to do
 if(canvas_dim[0] == dx && canvas_dim[1] == dy)
    return;

 /* If there is a change to the canvas, the any viewports and overlays must be resized as well. If
  * the window gets too small, we always have to have an area to draw to so don't let any viewport
  * dimension go to zero.
  */

 // single viewport
 if(n_viewports == 1) {
    if(dx < 1) dx = 1;
    if(dy < 1) dy = 1;
   }
 // splitscreen horizontal stack
 else if(n_viewports == 2) {
    if(dx < 1) dx = 1;
    if(dy < 2) dy = 2;
   }
 // splitscreen four-port with one blank port
 else if(n_viewports == 3) {
    if(dx < 2) dx = 2;
    if(dy < 2) dy = 2;
   }
 // splitscreen four-port
 else if(n_viewports == 4) {
    if(dx < 2) dx = 2;
    if(dy < 2) dy = 2;
   }

 // set canvas dimensions
 canvas_dim[0] = dx;
 canvas_dim[1] = dy;
}

const uint32* GetCanvasDimensions(void)
{
 return &canvas_dim[0];
}

void EnableViewport(int n)
{
}

void DisableViewport(int n)
{
}