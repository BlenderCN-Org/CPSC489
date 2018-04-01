#ifndef __CS489_XAUDIO_H
#define __CS489_XAUDIO_H

#include "errors.h"

struct SoundData {
 IXAudio2SourceVoice* voice;
 std::unique_ptr<char[]> wavdata;
 uint32 wavsize;
};

// XAudio Functions
ErrorCode InitAudio(void);
void FreeAudio(void);

// Sound Functions
ErrorCode LoadVoice(LPCWSTR filename, SoundData** snd);
ErrorCode FreeVoice(LPCWSTR filename);
SoundData* FindVoice(LPCWSTR filename);
ErrorCode PlayVoice(SoundData* snd, bool loop = false);

#endif
