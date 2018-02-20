#ifndef __CPSC489_BLENDING_H
#define __CPSC489_BLENDING_H

#define BS_DEFAULT        0
#define BS_ALPHA_BLENDING 1

ErrorCode InitBlendStates(void);
void FreeBlendStates(void);
ID3D11BlendState* GetBlendState(DWORD index);
ErrorCode SetBlendState(DWORD index);
ErrorCode SetBlendState(DWORD index, const float* factor);

#endif
