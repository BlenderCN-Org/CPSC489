/** \file    camera.h
 *  \brief   An orbit camera class.
 *  \details An orbit camera is commonly called a "turntable camera."
 *  \author  Steven F. Emory
 *  \date    02/05/2018
 */
#ifndef __CPSC489_CAMERA_H
#define __CPSC489_CAMERA_H

class OrbitCamera {
 // Viewport Properties
 private :
  sint32 viewport[4];
  real32 aspect;
 // Frustum Variables
 private :
  real32 nplane;
  real32 fplane;
  real32 fovy;
 // Camera Orientation Variables
 private :
  real32 cam_E[3];
  real32 cam_X[3];
  real32 cam_Y[3];
  real32 cam_Z[3];
 // Camera Orbit Variables
 private :
  real32 orbit_distance;
  real32 horz_angle;
  real32 vert_angle;
  real32 ref_X[3];
  real32 ref_Y[3];
  real32 ref_Z[3];
 // Viewport Functions
 public :
  const int* GetViewport(void)const { return &viewport[0]; }
  void SetViewport(const int* vp);
  void SetViewport(int x0, int y0, int dx, int dy);
  real32 GetAspectRatio(void)const { return aspect; }
 // Camera Property Functions
 public :
  void GetOrbitPoint(real32* pt)const {
   pt[0] = cam_E[0] + orbit_distance*cam_X[0];
   pt[1] = cam_E[1] + orbit_distance*cam_X[1];
   pt[2] = cam_E[2] + orbit_distance*cam_X[2];
  }
  real32 GetOrbitDistance(void)const { return orbit_distance; }
  real32 GetHorzAngle(void)const { return horz_angle; }
  real32 GetVertAngle(void)const { return vert_angle; }
 // Camera Reset Functions
 public :
  void Reset(void);
 // Frustum Functions
 public :
  real32 GetNearPlane(void)const;
  real32 GetFarPlane(void)const;
  std::pair<real32, real32> GetFrustumPlanes(void)const;
  bool SetNearPlane(real32 n);
  bool SetFarPlane(real32 f);
  bool SetFrustumPlanes(real32 n, real32 f);
  bool FrustumPlanesValid(void)const;
  bool GetClippingPlaneCoords(real32* coords)const;
 // FOVY Functions
 public :
  real32 GetFOVY(void)const;
  bool SetFOVY(real32 value);
 // Camera Functions (Public)
 public :
  const real32* GetCameraOrigin(void)const  { return &cam_E[0]; }
  const real32* GetCameraXAxis(void)const { return &cam_X[0]; }
  const real32* GetCameraYAxis(void)const { return &cam_Y[0]; }
  const real32* GetCameraZAxis(void)const { return &cam_Z[0]; }
 // Camera Functions (Private)
 private :
  void SetCameraOrigin(real32 x, real32 y, real32 z);
  void SetCameraXAxis(real32 x, real32 y, real32 z);
  void SetCameraYAxis(real32 x, real32 y, real32 z);
  void SetCameraZAxis(real32 x, real32 y, real32 z);
  void SetCameraOrigin(const real32* origin);
  void SetCameraXAxis(const real32* axis);
  void SetCameraYAxis(const real32* axis);
  void SetCameraZAxis(const real32* axis);
 // Screen-Space Functions
 public :
  bool ScreenToPlane(const int* p, real32 distance, real32* v)const;
  bool ScreenToPlane(int x, int y, real32 distance, real32* v)const;
  bool ScreenToNearPlane(const int* p, real32* v)const;
  bool ScreenToNearPlane(int x, int y, real32* v)const;
  bool ScreenToFarPlane(const int* p, real32* v)const;
  bool ScreenToFarPlane(int x, int y, real32* v)const;
  bool ScreenToGroundPlane(const int* p, real32* v)const;
  bool ScreenToGroundPlane(int x, int y, real32* v)const;
 // Mouse Tracking Functions
 public :
  bool Pan(int x0, int y0, int x1, int y1);
  bool Pan(int dx, int dy);
  bool Orbit(int x0, int y0, int x1, int y1);
  bool Dolly(int unit);
 // Thumbstick Functions
 public :
  void Move(real32 dx, real32 dy, real32 dz);
  void Move(const real32* v, real32 t);
  void ThumbstickOrbit(const real32* axis, real32 t);
 // Constructors/Destructors
 public :
  OrbitCamera();
  OrbitCamera(const OrbitCamera& vc);
  virtual ~OrbitCamera();
};

OrbitCamera* GetOrbitCamera(void);

#endif
