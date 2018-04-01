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

static IXAudio2SourceVoice* snd = nullptr;

BOOL InitSoundTest(void)
{
 ErrorCode code = LoadVoice(L"sounds\\sample.wav", &snd, false);
 if(Fail(code)) return FALSE;
 PlayVoice(snd);
 return TRUE;
}

void FreeSoundTest(void)
{
 FreeVoice(L"sounds\\sample.wav");
}

void RenderSoundTest(real32 dt)
{
}