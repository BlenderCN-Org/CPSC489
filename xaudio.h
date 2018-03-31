#ifndef __CS489_XAUDIO_H
#define __CS489_XAUDIO_H

#include "errors.h"

// XAudio Functions
ErrorCode InitAudio(void);
void FreeAudio(void);

// Sound Functions
ErrorCode LoadSound(LPCWSTR filename, IXAudio2SourceVoice** snd, bool loop = false);
ErrorCode FreeSound(LPCWSTR filename);
IXAudio2SourceVoice* FindSound(LPCWSTR filename);

#endif
