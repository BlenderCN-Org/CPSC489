#include "stdafx.h"
#include "bstream.h"
#include "xaudio.h"
#include "sounds.h"

/** \details Very similar to the way textures are handled, sound files should not be loaded more
 *  than once. Best way to handle this is to associate each filename with its sound data and put
 *  it in a hash map.
 */

struct SoundResource {
 SoundData* data;
 DWORD refs;
};
typedef std::unordered_map<std::wstring, SoundResource, WideStringHash, WideStringInsensitiveEqual> hashmap_type;
static hashmap_type hashmap;

ErrorCode LoadSound(LPCWSTR filename, SoundData** srv)
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

 struct WAVDATA {
  unique_ptr<char[]> format;
 };
 std::vector<WAVDATA> wavdata;

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
          WAVDATA wd;
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
                 if(sc_type != 0x20746D66ul) return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);
                 if(sc_size < 0x0E) return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);

                 // peek at format
                 uint16 format = bs.LE_peek_uint16();
                 if(bs.fail()) return DebugErrorCode(EC_STREAM_SEEK, __LINE__, __FILE__);

                 // process format
                 // provided that a few things are correct, assume the data is valid
                 if(format == WAVE_FORMAT_PCM) {
                    if(sc_size == sizeof(PCMWAVEFORMAT)) {
                       wd.format.reset(new char[sc_size]);
                       bs.read((char*)wd.format.get(), sc_size);
                       if(bs.fail()) return DebugErrorCode(EC_STREAM_READ, __LINE__, __FILE__);
                      }
                    else
                       return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);
                   }
                 else if(format == WAVE_FORMAT_IEEE_FLOAT) {
                    if(sc_size == sizeof(PCMWAVEFORMAT) || sc_size == sizeof(WAVEFORMATEX)) {
                       wd.format.reset(new char[sc_size]);
                       bs.read((char*)wd.format.get(), sc_size);
                       if(bs.fail()) return DebugErrorCode(EC_STREAM_READ, __LINE__, __FILE__);
                      }
                    else
                       return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);
                   }
                 else {
                    if(sc_size < sizeof(WAVEFORMATEX)) return DebugErrorCode(EC_AUDIO_INVALID, __LINE__, __FILE__);
                    wd.format.reset(new char[sc_size]);
                    bs.read((char*)wd.format.get(), sc_size);
                    if(bs.fail()) return DebugErrorCode(EC_STREAM_READ, __LINE__, __FILE__);
                   }
                }
              else if(sc_type == 0x61746164ul)
                {
                }
              // unknown subchunk
              else {
                 bs.move(sc_size);
                 if(bs.fail()) return DebugErrorCode(EC_STREAM_SEEK, __LINE__, __FILE__);
                }
             }
         }
       // skip unknown chunks
       else {
          bs.move(chunk_size);
          if(bs.fail()) return DebugErrorCode(EC_STREAM_SEEK, __LINE__, __FILE__);
         }
      }

 return EC_SUCCESS;
}

ErrorCode FreeSound(LPCWSTR filename)
{
 return EC_SUCCESS;
}

SoundData* FindSound(LPCWSTR filename)
{
 return nullptr;
}