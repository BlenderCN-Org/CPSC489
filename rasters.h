#ifndef __CS489_RASTERS_H
#define __CS489_RASTERS_H

#include "errors.h"

#define RS_WIREFRAME 0
#define RS_MODEL     1

// Rasterizer State Functions
ErrorCode InitRasterizerStates(void);
void FreeRasterizerStates(void);
ID3D11RasterizerState* GetRasterizerState(DWORD index);
ErrorCode SetRasterizerState(DWORD index);

#endif
