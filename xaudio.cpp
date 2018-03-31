#include "stdafx.h"
#include "xaudio.h"

/** IXAudio2 interface
 *  \brief   Notes about the IXAudio2 interface.
 *  \details No other XAudio2 objects are reference-counted; their lifetimes are explicitly con-
 *  trolled using create and destroy calls, and are bounded by the lifetime of the XAudio2 object
 *  that owns them. Much of this code is based on the Microsoft DirectXTK kit for DirectX 11, which
 *  is availble on the github https://github.com/Microsoft/DirectXTK website.
 */

// Mastering Voice
// Represents the audio device.

// Source Voice
// Used to submit audio data into the XAudio2 pipeline.

// Submix Voice
// A voice to which many source voices will be sent to. You can use a submix
// voice to ensure that a particular set of voice data is converted to the same format and to
// have a particular effect chain processed on the collective result. Submix voices mix audio
// and operate on the result You will not hear it unless submitted to a mastering voice (the
// audio device).

// PLAN:
// Sound Entity
//  * bool active     (currently active)
//  * bool loop       (loop sound)
//  * vector location (location of this entity)
//  * real32 falloff  (distance from location for which you can no longer hear sound)
//  * real32 hotspot  (distance from location for which sound is always 100%)
//  * string name     (name of this entity)

static IXAudio2* xaudio = nullptr;
static IXAudio2MasteringVoice* master_voice = nullptr;
static IUnknown* reverb_effect = nullptr;
static IXAudio2SubmixVoice* reverb_voice = nullptr;

ErrorCode InitAudio(void)
{
 // free previous
 FreeAudio();

 // create interface
 HRESULT result;
 result = XAudio2Create(&xaudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
 if(FAILED(result)) {
    FreeAudio();
    return DebugErrorCode(EC_AUDIO_INIT, __LINE__, __FILE__);
   }

 // create mastering voice
 result = xaudio->CreateMasteringVoice(&master_voice);
 if(FAILED(result)) {
    FreeAudio();
    return DebugErrorCode(EC_AUDIO_INIT, __LINE__, __FILE__);
   }

 // extract mastering voice details
 XAUDIO2_VOICE_DETAILS details;
 master_voice->GetVoiceDetails(&details);

 // create reverb voice
 result = XAudio2CreateReverb(&reverb_effect, 0);
 if(FAILED(result)) {
    FreeAudio();
    return DebugErrorCode(EC_AUDIO_REVERB, __LINE__, __FILE__);
   }

 XAUDIO2_EFFECT_DESCRIPTOR xed[] = { { reverb_effect, TRUE, 1 } };
 XAUDIO2_EFFECT_CHAIN xec = { 1, xed };
 result = xaudio->CreateSubmixVoice(&reverb_voice, 1, details.InputSampleRate, XAUDIO2_VOICE_USEFILTER, 0, nullptr, &xec);
 if(FAILED(result)) return DebugErrorCode(EC_AUDIO_REVERB, __LINE__, __FILE__);

 return EC_SUCCESS;
}

void FreeAudio(void)
{
 // release reverb effect
 if(reverb_voice) {
    reverb_voice->DestroyVoice();
    reverb_voice = nullptr;
   }
 if(reverb_effect) {
    reverb_effect->Release();
    reverb_effect = nullptr;
   }
 // release mastering voice
 if(master_voice) {
    master_voice->DestroyVoice();
    master_voice = nullptr;
   }
 // release xaudio
 if(xaudio) {
    xaudio->Release();
    xaudio = nullptr;
   }
}

const WAVEFORMATEX* GetWaveFormatExFromMemory(const char* ptr)
{
 return reinterpret_cast<const WAVEFORMATEX*>(ptr);
}

#include "sounds.h"
ErrorCode CreateSourceVoice(const SoundData& sd, IXAudio2SourceVoice** voice)
{
 // create voice from format
 IXAudio2SourceVoice* asv = nullptr;
 HRESULT result = xaudio->CreateSourceVoice(&asv, (WAVEFORMATEX*)GetWaveFormatExFromMemory(sd.format.get()));
 if(FAILED(result)) return DebugErrorCode(EC_AUDIO_SOURCE_VOICE, __LINE__, __FILE__);

 // create buffer descriptor
 XAUDIO2_BUFFER buffer;
 buffer.Flags = 0;
 buffer.AudioBytes = sd.data_size;
 buffer.pAudioData = reinterpret_cast<const BYTE*>(sd.data.get());
 buffer.PlayBegin = 0;  // play all
 buffer.PlayLength = 0; // play all
 buffer.LoopBegin = 0;
 buffer.LoopLength = 0;
 buffer.LoopCount = 0;
 buffer.pContext = NULL;

 // submit source buffer
 result = asv->SubmitSourceBuffer(&buffer);
 if(FAILED(result)) return DebugErrorCode(EC_AUDIO_SOURCE_VOICE, __LINE__, __FILE__);

 // to play
 // asv->Start(0) 

 return EC_SUCCESS;
}
