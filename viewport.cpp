#include "stdafx.h"
#include "gfx.h"
#include "orbit.h"
#include "viewport.h"

// Viewport Variables
static uint32 canvas_dim[2] = { 1, 1 };
static uint32 n_viewports = 0;
static bool viewport_states[4] = { false, false, false, false };
static uint32 viewport_list[4][4] = {
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 }
};

// Overlay Variables
static bool overlay_states[4] = { false, false, false, false };
static uint32 overlay_list[4][4] = {
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 }
};

// Camera Variables (Viewport)
static OrbitCamera cameras[4];
static ID3D11Buffer* buffers[4] = { nullptr, nullptr, nullptr, nullptr };
static OrbitBox orbit_boxes[4];

// Camera Variables (Overlay)
static OrbitCamera overlay_cameras[4];
static ID3D11Buffer* overlay_buffers[4] = { nullptr, nullptr, nullptr, nullptr };
static OrbitBox overlay_orbit_boxes[4];

ErrorCode InitCanvas(uint32 dx, uint32 dy)
{
 // free previous
 FreeCanvas();

 // start with only one viewport (change it if you want)
 n_viewports = 4;

 // initialize viewports
 for(uint32 i = 0; i < n_viewports; i++)
    {
     // enable viewport by default
     viewport_states[i] = true;

     // viewport camera and orbit box
     cameras[i].Reset();
     ErrorCode code = orbit_boxes[i].InitBox(cameras[i]);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // overlay camera and orbit box
     overlay_cameras[i].Reset();
     code = overlay_orbit_boxes[i].InitBox(overlay_cameras[i]);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // const viewport buffer
     code = CreateDynamicMatrixConstBuffer(&buffers[i]);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // const overlay buffer
     code = CreateDynamicMatrixConstBuffer(&overlay_buffers[i]);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
    }

 // set canvas dimensions
 return SetCanvasDimensions(dx, dy);
}

void FreeCanvas(void)
{
 // clear viewport and overlay buffers
 for(uint32 i = 0; i < 4; i++)
    {
     // viewport buffers
     cameras[i].Reset();
     if(buffers[i]) buffers[i]->Release();
     buffers[i] = nullptr;

     // overlay buffers
     overlay_cameras[i].Reset();
     if(overlay_buffers[i]) overlay_buffers[i]->Release();
     overlay_buffers[i] = nullptr;
    }

 // clear viewport and overlay states
 for(int i = 0; i < 4; i++)
    {
     // viewport states
     viewport_states[i] = false;
     uint32* vp = &viewport_list[i][0];
     vp[0] = vp[1] = vp[2] = vp[3] = 0;

     // overlay states
     overlay_states[i] = false;
     vp = &overlay_list[i][0];
     vp[0] = vp[1] = vp[2] = vp[3] = 0;
    }

 // clear canvas properties
 n_viewports = 0;
 canvas_dim[0] = 0;
 canvas_dim[1] = 0;
}

ErrorCode SetCanvasDimensions(uint32 dx, uint32 dy)
{
 // nothing to do
 if(canvas_dim[0] == dx && canvas_dim[1] == dy)
    return EC_SUCCESS;

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

 // layout and update canvas
 LayoutCanvas();
 return UpdateCanvas();
}

const uint32* GetCanvasDimensions(void)
{
 return &canvas_dim[0];
}

void SetCanvasViewportNumber(uint32 n)
{
 if(n == 0 || n > 4) return;
 n_viewports = n;
 LayoutCanvas();
}

uint32 GetCanvasViewportNumber(void)
{
 return n_viewports;
}

void LayoutCanvas(void)
{
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
    vp3[2] = canvas_dim[0]/2;
    vp3[3] = canvas_dim[1] - vp1[3];
    // lower-half (R)
    uint32* vp4 = &viewport_list[3][0];
    vp4[0] = vp1[2];
    vp4[1] = vp1[3];
    vp4[2] = canvas_dim[0] - vp1[2];
    vp4[3] = canvas_dim[1] - vp1[3];
   }
}

ErrorCode UpdateCanvas(void)
{
 // set camera parameters
 for(uint32 i = 0; i < n_viewports; i++)
    {
     // viewport cameras
     const uint32* vp1 = &viewport_list[i][0];
     cameras[i].SetViewport((int)vp1[0], (int)vp1[1], (int)vp1[2], (int)vp1[3]);
     ErrorCode code = orbit_boxes[i].UpdateBox(cameras[i]);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // overlay cameras
     const uint32* vp2 = &overlay_list[i][0];
     overlay_cameras[i].SetViewport((int)vp2[0], (int)vp2[1], (int)vp2[2], (int)vp2[3]);
     code = overlay_orbit_boxes[i].UpdateBox(overlay_cameras[i]);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // update viewport camera buffer
     if(viewport_states[i]) UpdateViewportCamera(i);

     // update overlay camera buffer
     if(overlay_states[i]) UpdateOverlayCamera(i);
    }

 return EC_SUCCESS;
}

void EnableViewport(uint32 index, bool state)
{
 if(!(index < n_viewports) || (state == viewport_states[index])) return;
 viewport_states[index] = state;
}

bool IsViewportEnabled(uint32 index)
{
 if(index < 4) return viewport_states[index];
 return false;
}

/** \fn GetViewport
 *  \brief Used to retrieve the viewport origin (upper-left) and the viewport dimensions (width and
 *  height) for the provided viewport index. If an invalid, out-of-range index is provided, this
 *  function returns nullptr.
 *  \param index
 *  \return An array of four values is returned: x0, y0, dx, and dy.
 */
const uint32* GetViewport(uint32 index)
{
 if(index < 4) return &viewport_list[index][0];
 return nullptr;
}

OrbitCamera* GetViewportCamera(uint32 index)
{
 if(!(index < n_viewports)) return nullptr;
 return &cameras[index];
}

// Overlay Functions
OrbitCamera* GetOverlayCamera(uint32 index)
{
 if(!(index < n_viewports)) return nullptr;
 return &overlay_cameras[index];
}

ErrorCode UpdateViewportCamera(uint32 index)
{
 // get clipping plane coordinates
 real32 coords[4];
 auto& camera = cameras[index];
 camera.GetClippingPlaneCoords(coords);

 // calculate perspective matrix
 using namespace DirectX;
 float dx = 2*coords[1];
 float dy = 2*coords[3];
 float zn = camera.GetNearPlane();
 float zf = camera.GetFarPlane();
 XMMATRIX P;
 P = XMMatrixPerspectiveRH(dx, dy, zn, zf);

 // calculate view matrix
 const real32* cam_E = camera.GetCameraOrigin();
 const real32* cam_X = camera.GetCameraXAxis();
 const real32* cam_Y = camera.GetCameraYAxis();
 const real32* cam_Z = camera.GetCameraZAxis();
 XMVECTOR E = XMVectorSet(cam_E[0], cam_E[1], cam_E[2], 0.0f);
 XMVECTOR X = XMVectorSet(cam_X[0], cam_X[1], cam_X[2], 0.0f);
 XMVECTOR Y = XMVectorSet(cam_Y[0], cam_Y[1], cam_Y[2], 0.0f);
 XMVECTOR Z = XMVectorSet(cam_Z[0], cam_Z[1], cam_Z[2], 0.0f);
 XMMATRIX V = XMMatrixLookToRH(E, X, Z);

 // calculate perspective view matrix
 XMMATRIX R = XMMatrixMultiply(V, P);
 R = XMMatrixTranspose(R);

 // update Direct3D resource
 ErrorCode code = UpdateDynamicMatrixConstBuffer(buffers[index], R);
 if(Fail(code)) return code;

 // update orbit box
 orbit_boxes[index].UpdateBox(camera);
 return EC_SUCCESS;
}

ErrorCode UpdateOverlayCamera(uint32 index)
{
 // get clipping plane coordinates
 real32 coords[4];
 auto& camera = overlay_cameras[index];
 camera.GetClippingPlaneCoords(coords);

 // calculate perspective matrix
 using namespace DirectX;
 float dx = 2*coords[1];
 float dy = 2*coords[3];
 float zn = camera.GetNearPlane();
 float zf = camera.GetFarPlane();
 XMMATRIX P;
 P = XMMatrixPerspectiveRH(dx, dy, zn, zf);

 // calculate view matrix
 const real32* cam_E = camera.GetCameraOrigin();
 const real32* cam_X = camera.GetCameraXAxis();
 const real32* cam_Y = camera.GetCameraYAxis();
 const real32* cam_Z = camera.GetCameraZAxis();
 XMVECTOR E = XMVectorSet(cam_E[0], cam_E[1], cam_E[2], 0.0f);
 XMVECTOR X = XMVectorSet(cam_X[0], cam_X[1], cam_X[2], 0.0f);
 XMVECTOR Y = XMVectorSet(cam_Y[0], cam_Y[1], cam_Y[2], 0.0f);
 XMVECTOR Z = XMVectorSet(cam_Z[0], cam_Z[1], cam_Z[2], 0.0f);
 XMMATRIX V = XMMatrixLookToRH(E, X, Z);

 // calculate perspective view matrix
 XMMATRIX R = XMMatrixMultiply(V, P);
 R = XMMatrixTranspose(R);

 // update Direct3D resource
 ErrorCode code = UpdateDynamicMatrixConstBuffer(overlay_buffers[index], R);
 if(Fail(code)) return code;

 // update orbit box
 overlay_orbit_boxes[index].UpdateBox(camera);
 return EC_SUCCESS;
}

ID3D11Buffer* GetViewportCameraBuffer(uint32 index)
{
 return buffers[index];
}

ID3D11Buffer* GetOverlayCameraBuffer(uint32 index)
{
 return overlay_buffers[index];
}

ErrorCode RenderViewportOrbitBox(uint32 index)
{
 return orbit_boxes[index].RenderBox();
}

ErrorCode RenderOverlayOrbitBox(uint32 index)
{
 return overlay_orbit_boxes[index].RenderBox();
}

uint32 GetViewportIndexFromPosition(int x, int y)
{
 for(uint32 i = 0; i < n_viewports; i++) {
     if(viewport_states[i]) {
        const uint32* vp = &viewport_list[i][0];
        uint32 x0 = vp[0]; uint32 x1 = vp[0] + vp[2];
        uint32 y0 = vp[1]; uint32 y1 = vp[1] + vp[3];
        if((x >= x0 && x <= x1) && (y >= y0 && y <= y1)) return i;
       }
    }
 return 0xFFFFFFFFul;
}

OrbitCamera* GetViewportCameraFromPosition(int x, int y)
{
 for(uint32 i = 0; i < n_viewports; i++) {
     if(viewport_states[i]) {
        const uint32* vp = &viewport_list[i][0];
        uint32 x0 = vp[0]; uint32 x1 = vp[0] + vp[2];
        uint32 y0 = vp[1]; uint32 y1 = vp[1] + vp[3];
        if(x >= x0 && x <= x1 && y >= y0 && y <= y1) return &cameras[i];
       }
    }
 return nullptr;
}