#ifndef __CS489_SOUNDS_H
#define __CS489_SOUNDS_H

struct SoundData {
 uint32 format_size;
 std::unique_ptr<char[]> format;
 uint32 data_size;
 std::unique_ptr<char[]> data;
};

// Texture Functions
ErrorCode LoadSound(LPCWSTR filename, SoundData** snd);
ErrorCode FreeSound(LPCWSTR filename);
SoundData* FindSound(LPCWSTR filename);

#endif

