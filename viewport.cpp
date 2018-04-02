#include "stdafx.h"
#include "viewport.h"

static uint32 canvas_dim[2] = { 1, 1 };
static uint32 n_viewports = 1;
static bool viewport_states[4] = { true, false, false, false };
static uint32 viewport_list[4][4] = {
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 }
};

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

 // full-view
 if(n_viewports == 1) {
    uint32* vp = &viewport_list[0][0];
    vp[0] = 0;
    vp[1] = 0;
    vp[2] = canvas_dim[0];
    vp[3] = canvas_dim[1];
   }
 // half-view
 else if(n_viewports == 2) {
    // upper-half
    uint32* vp1 = &viewport_list[0][0];
    vp1[0] = 0;
    vp1[1] = 0;
    vp1[2] = canvas_dim[0];
    vp1[3] = canvas_dim[1]/2;
    // lower-half
    uint32* vp2 = &viewport_list[1][0];
    vp2[0] = 0;
    vp2[1] = vp1[3];
    vp2[2] = canvas_dim[0];
    vp2[3] = canvas_dim[1] - vp1[3];
   }
 // quad-view
 else if(n_viewports == 3 || n_viewports == 4) {
    // upper-half (L)
    uint32* vp1 = &viewport_list[0][0];
    vp1[0] = 0;
    vp1[1] = 0;
    vp1[2] = canvas_dim[0]/2;
    vp1[3] = canvas_dim[1]/2;
    // upper-half (R)
    uint32* vp2 = &viewport_list[1][0];
    vp2[0] = vp1[2];
    vp2[1] = 0;
    vp2[2] = canvas_dim[0] - vp1[2];
    vp2[3] = canvas_dim[1]/2;
    // lower-half (L)
    uint32* vp3 = &viewport_list[2][0];
    vp3[0] = 0;
    vp3[1] = vp1[3];
    vp3[2] = canvas_dim[0];
    vp3[3] = canvas_dim[1] - vp1[3];
    // lower-half (R)
    uint32* vp4 = &viewport_list[3][0];
    vp4[0] = vp1[2];
    vp4[1] = vp1[3];
    vp4[2] = canvas_dim[0] - vp1[2];
    vp4[3] = canvas_dim[1] - vp1[3];
   }
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