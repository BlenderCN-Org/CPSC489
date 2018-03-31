#ifndef __CS489_SOUNDS_H
#define __CS489_SOUNDS_H

struct SoundData {
};

// Texture Functions
ErrorCode LoadSound(LPCWSTR filename, SoundData** srv);
ErrorCode FreeSound(LPCWSTR filename);
SoundData* FindSound(LPCWSTR filename);

#endif

