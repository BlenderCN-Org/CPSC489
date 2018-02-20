#ifndef __CPSC489_SAMPLER_H
#define __CPSC489_SAMPLER_H

#define SS_WRAP_LINEAR 0

ErrorCode InitSamplerStates(void);
void FreeSamplerStates(void);
ID3D11SamplerState* GetSamplerState(DWORD index);
ErrorCode SetSamplerState(DWORD id);
ErrorCode SetSamplerState(const DWORD* slots, DWORD n);

#endif
