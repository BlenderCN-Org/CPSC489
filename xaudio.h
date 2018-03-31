#ifndef __CS489_XAUDIO_H
#define __CS489_XAUDIO_H

#include "errors.h"

// XAudio Functions
ErrorCode InitAudio(void);
void FreeAudio(void);

// Sound Functions
ErrorCode LoadVoice(LPCWSTR filename, IXAudio2SourceVoice** snd, bool loop = false);
ErrorCode FreeVoice(LPCWSTR filename);
IXAudio2SourceVoice* FindVoice(LPCWSTR filename);
ErrorCode PlayVoice(IXAudio2SourceVoice* snd);

#endif
