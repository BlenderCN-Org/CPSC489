#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../app.h"
#include "../math.h"
#include "../vector3.h"
#include "../matrix4.h"
#include "../model.h"
#include "../camera.h"
#include "../xinput.h"
#include "../xaudio.h"
#include "../gfx.h"
#include "../axes.h"

#include "tests.h"
#include "t_sounds.h"

static SoundData* snd = nullptr;

BOOL InitSoundTest(void)
{
 ErrorCode code = LoadVoice(L"sounds\\sample.wav", &snd);
 if(Fail(code)) return FALSE;
 PlayVoice(snd, true);
 return TRUE;
}

void FreeSoundTest(void)
{
 FreeVoice(L"sounds\\sample.wav");
}

void UpdateSoundTest(real32 dt)
{
}

void RenderSoundTest(void)
{
}