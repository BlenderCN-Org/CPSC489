#include "stdafx.h"
#include "xaudio.h"

#include "bstream.h"

/** IXAudio2 interface
 *  \brief   Notes about the IXAudio2 interface.
 *  \details No other XAudio2 objects are reference-counted; their lifetimes are explicitly con-
 *  trolled using create and destroy calls, and are bounded by the lifetime of the XAudio2 object
 *  that owns them. Much of this code is based on the Microsoft DirectXTK kit for DirectX 11, which
 *  is availble on the github https://github.com/Microsoft/DirectXTK website.
 */

/** \details Very similar to the way textures are handled, sound files should not be loaded more
 *  than once. Best way to handle this is to associate each filename with its sound data and put
 *  it in a hash map.
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

struct SoundResource {
 IXAudio2SourceVoice* data;
 std::unique_ptr<char[]> wavdata; // keep the data!!!
 DWORD refs;
};
typedef std::unordered_map<std::wstring, SoundResource, WideStringHash, WideStringInsensitiveEqual> hashmap_type;
static hashmap_type hashmap;

ErrorCode CreateSourceVoice(const WAVEFORMATEX* format, const char* data, uint32 size, IXAudio2SourceVoice** voice, bool loop);

#pragma region XAUDIO_FUNCTIONS

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

#pragma endregion XAUDIO_FUNCTIONS

#pragma region HIDDEN_XAUDIO_FUNCTIONS

ErrorCode CreateSourceVoice(const WAVEFORMATEX* format, const char* data, uint32 size, IXAudio2SourceVoice** voice, bool loop)
{
 // create voice from format
 IXAudio2SourceVoice* asv = nullptr;
 HRESULT result = xaudio->CreateSourceVoice(&asv, format);
 if(FAILED(result)) return DebugErrorCode(EC_AUDIO_SOURCE_VOICE, __LINE__, __FILE__);

 // create buffer descriptor
 XAUDIO2_BUFFER buffer;
 buffer.Flags = 0;
 buffer.AudioBytes = size;
 buffer.pAudioData = reinterpret_cast<const BYTE*>(data);
 buffer.PlayBegin = 0;  // play all
 buffer.PlayLength = 0; // play all
 buffer.LoopBegin = 0;
 buffer.LoopLength = 0;
 buffer.LoopCount = (loop ? XAUDIO2_LOOP_INFINITE : 0);
 buffer.pContext = NULL;

 // submit source buffer
 result = asv->SubmitSourceBuffer(&buffer);
 if(FAILED(result)) {
    asv->DestroyVoice();
    return DebugErrorCode(EC_AUDIO_SOURCE_VOICE, __LINE__, __FILE__);
   }

 // set voice
 *voice = asv;
 return EC_SUCCESS;
}

#pragma endregion HIDDEN_XAUDIO_FUNCTIONS

#pragma region SOUND_FUNCTIONS

ErrorCode LoadVoice(LPCWSTR filename, IXAudio2SourceVoice** snd, bool loop)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return DebugErrorCode(EC_FILE_OPEN, __LINE__, __FILE__);

 // get filesize
 ifile.seekg(0, ios::end);
 size_t filesize = static_cast<size_t>(ifile.tellg());
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return DebugErrorCode(EC_FILE_SEEK, __LINE__, __FILE__);
 if(!filesize) return DebugErrorCode(EC_FILE_EMPTY, __LINE__, __FILE__);

 // read file
 std::unique_ptr<char[]> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

 // iterate through file chunks
 binary_stream bs(filedata, filesize);
 while(!bs.at_end())
      {
       // read chunk
       uint32 chunk_type = bs.LE_read_uint32();
       uint32 chunk_size = bs.LE_read_uint32();
       if(bs.fail()) return DebugErrorCode(EC_STREAM_READ, __LINE__, __FILE__);

       // RIFF chunk
       if(chunk_type == 0x46464952)
         {
          uint32 format = bs.LE_read_uint32();
          if(format == 0x45564157ul) ;      // WAVE
          else if(format == 0x414D5758ul) ; // XWMA
          else return DebugErrorCode(EC_AUDIO_FORMAT, __LINE__, __FILE__);

          // read subchunks to form WAVDATA
          std::unique_ptr<char[]> f_data;
          uint32 f_size = 0;
          std::unique_ptr<char[]> w_data;
          uint32 w_size = 0;

          // read subchunks to form WAVDATA
          for(int i = 0; i < 2; i++)
             {
              // read subchunk
              uint32 sc_type = bs.LE_read_uint32();
              uint32 sc_size = bs.LE_read_uint32();
              if(bs.fail()) return DebugErrorCode(EC_STREAM_READ, __LINE__, __FILE__);

              // fmt subchunk
              if(sc_type == 0x20746D66ul)
                {
                 // validate fmt subchunk
                 if(sc_size < 0x0E) return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);

                 // peek at format
                 uint16 format = bs.LE_peek_uint16();
                 if(bs.fail()) return DebugErrorCode(EC_STREAM_SEEK, __LINE__, __FILE__);

                 // process format
                 // provided that a few things are correct, assume the data is valid
                 if(format == WAVE_FORMAT_PCM) {
                    if(sc_size == sizeof(PCMWAVEFORMAT)) {
                       // make f_size a little bigger since CreateSourceVoice requires WAVEFORMATEX*
                       f_size = sizeof(WAVEFORMATEX);
                       f_data.reset(new char[f_size]);
                       ZeroMemory(f_data.get(), f_size);
                       bs.read((char*)f_data.get(), sc_size); // only read as much as you need to
                       if(bs.fail()) return DebugErrorCode(EC_STREAM_READ, __LINE__, __FILE__);
                      }
                    else
                       return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);
                   }
                 else if(format == WAVE_FORMAT_IEEE_FLOAT) {
                    if(sc_size == sizeof(PCMWAVEFORMAT) || sc_size == sizeof(WAVEFORMATEX)) {
                       // make f_size a little bigger since CreateSourceVoice requires WAVEFORMATEX*
                       f_size = sizeof(WAVEFORMATEX);
                       f_data.reset(new char[f_size]);
                       ZeroMemory(f_data.get(), f_size);
                       bs.read((char*)f_data.get(), sc_size);
                       if(bs.fail()) return DebugErrorCode(EC_STREAM_READ, __LINE__, __FILE__);
                      }
                    else
                       return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);
                   }
                 // otherwise WAVEFORMATEX or something larger
                 else {
                    if(sc_size < sizeof(WAVEFORMATEX)) return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);
                    f_size = sc_size;
                    f_data.reset(new char[f_size]);
                    bs.read((char*)f_data.get(), f_size);
                    if(bs.fail()) return DebugErrorCode(EC_STREAM_READ, __LINE__, __FILE__);
                   }
                }
              // data subchunk
              else if(sc_type == 0x61746164ul) {
                 if(sc_size < 0x04) return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);
                 w_size = sc_size;
                 w_data.reset(new char[w_size]);
                 bs.read((char*)w_data.get(), w_size);
                }
              // unknown subchunk
              else {
                 bs.move(sc_size);
                 if(bs.fail()) return DebugErrorCode(EC_STREAM_SEEK, __LINE__, __FILE__);
                }
             }

          // create resource
          IXAudio2SourceVoice* voice = nullptr;
          ErrorCode code = CreateSourceVoice(reinterpret_cast<const WAVEFORMATEX*>(f_data.get()), w_data.get(), w_size, &voice, loop);
          if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

          // insert resource into hash map
          SoundResource entry;
          entry.wavdata = std::move(w_data);
          entry.data = voice;
          entry.refs = 1;
          auto pairiter = hashmap.insert(make_pair(wstring(filename), std::move(entry)));
          if(pairiter.second == false) {
             voice->DestroyVoice();
             voice = nullptr;
             return DebugErrorCode(EC_AUDIO_INSERT_RESOURCE, __LINE__, __FILE__);
            }

          // set sound resource and exit while loop
          *snd = entry.data;
          break;
         }
       // skip unknown chunks
       // example: LIST chunks are commonly seen at the end of WAV files
       else {
          bs.move(chunk_size);
          if(bs.fail()) return DebugErrorCode(EC_STREAM_SEEK, __LINE__, __FILE__);
         }
      }

 return EC_SUCCESS;
}

ErrorCode FreeVoice(LPCWSTR filename)
{
 // lookup filename in hash table
 auto entry = hashmap.find(filename);
 if(entry == std::end(hashmap)) return DebugErrorCode(EC_AUDIO_RESOURCE, __LINE__, __FILE__);

 // a free request should NEVER have a reference count of zero
 SoundResource& resource = entry->second;
 if(resource.refs == 0) return DebugErrorCode(EC_AUDIO_RESOURCE_REFERENCE_COUNT, __LINE__, __FILE__);

 // delete audio resource if no longer referenced
 resource.refs--;
 if(resource.refs == 0) {
    if(resource.data) {
       resource.wavdata.reset();
       resource.data->DestroyVoice();
      }
    resource.data = nullptr;
    resource.refs = 0;
    hashmap.erase(entry);
   }
 
 return EC_SUCCESS;
}

IXAudio2SourceVoice* FindVoice(LPCWSTR filename)
{
 auto entry = hashmap.find(filename);
 if(entry == std::end(hashmap)) return NULL;
 return entry->second.data;
}

ErrorCode PlayVoice(IXAudio2SourceVoice* snd)
{
 if(!snd) return EC_SUCCESS;
 auto result = snd->Start();
 if(FAILED(result)) return DebugErrorCode(EC_AUDIO_START, __LINE__, __FILE__);
 return EC_SUCCESS;
}

#pragma endregion SOUND_FUNCTIONS
