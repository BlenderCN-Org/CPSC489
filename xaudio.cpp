#include "stdafx.h"
#include "xaudio.h"

/** IXAudio2 interface
 *  \brief   Notes about the IXAudio2 interface.
 *  \details No other XAudio2 objects are reference-counted; their lifetimes are explicitly con-
 *  trolled using create and destroy calls, and are bounded by the lifetime of the XAudio2 object
 *  that owns them. Much of this code is based on the Microsoft DirectXTK kit for DirectX 11, which
 *  is availble on the github https://github.com/Microsoft/DirectXTK website.
 */

// PLAN:
// Sound Entity
//  * bool active     (currently active)
//  * bool loop       (loop sound)
//  * vector location (location of this entity)
//  * real32 falloff  (distance from location for which you can no longer hear sound)
//  * real32 hotspot  (distance from location for which sound is always 100%)
//  * string name     (name of this entity)

static IXAudio2* xaudio = nullptr;
static IXAudio2MasteringVoice* xmvoice = nullptr;

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
 result = xaudio->CreateMasteringVoice(&xmvoice);
 if(FAILED(result)) {
    FreeAudio();
    return DebugErrorCode(EC_AUDIO_INIT, __LINE__, __FILE__);
   }

 return EC_SUCCESS;
}

void FreeAudio(void)
{
 if(xmvoice) {
    // note: all XAudio2 interfaces are released when the engine is destroyed, this is why
    // IXAudio2MasteringVoice does not have a Release function.
    xmvoice->DestroyVoice();
    xmvoice = nullptr;
   }
 if(xaudio) {
    xaudio->Release();
    xaudio = nullptr;
   }
}