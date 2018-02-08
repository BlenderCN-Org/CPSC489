#ifndef __CPSC489_AXES_H
#define __CPSC489_AXES_H

// Axes Model Functions
ErrorCode InitAxesModel(void);
void FreeAxesModel(void);
ErrorCode RenderAxes(ID3D11Buffer* instance, UINT n);

#endif
