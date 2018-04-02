#ifndef __CS489_AXES_H
#define __CS489_AXES_H

#include "errors.h"

// Axes Model Functions
ErrorCode InitAxesModel(void);
void FreeAxesModel(void);
ErrorCode RenderAxes(ID3D11Buffer* instance, UINT n);

// AABB Model Functions
ErrorCode InitAABBModel(void);
void FreeAABBModel(void);
ErrorCode RenderAABB(ID3D11Buffer* instance, UINT n);
ErrorCode RenderAABBMinMax(ID3D11Buffer* instance, UINT n);

#endif
