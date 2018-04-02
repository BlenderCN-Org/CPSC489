#include "stdafx.h"
#include "model_v2.h"

#include "win.h"
#include "gfx.h"
#include "texture.h"
#include "ascii.h"
#include "fileio.h"
#include "bstream.h"

static const uint32 FRAMES_PER_SECOND = 60ul;
static const real32 SECONDS_PER_FRAME = 1.0f/60.0f;

MeshData::MeshData() : skeletal(false)
{
 graphics.jbuffer = nullptr;
}

MeshData::~MeshData()
{
}

/** ConstructAnimationData
 *  -# For each animation, this function collects all the keyframes and orders them. For example, if
 *     one bone has keyframes at {0, 10}, and another bone has keyframes at {10, 20}, the keyframes
 *     are collected into one collective and ordered set {0, 10, 20}.
 *  -# Then for each bone that has a keyframe defined already, those keyframes are assigned to this
 *     new collective set. For example:
 *     For b1: {0, 10, 20} = {set, set, unset}
 *     For b2: {0, 10, 20} = {unset, set, set}
 *  -# Once the known keyframes are set, the missing keyframes must be set. In these cases, we may
 *     or may not have to interpolate between known keyframes to compute keyframes for bones that
 *     did not originally have keyframes. For example:
 *     b1 has unset keyframe at 20. Since there is a keyframe before 20 but none after, we simply
 *     just reuse the keyframe at 10.
 *     For b1: {0, 10, 20} = {kf1, kf2, kf2} (reuse kf2 to set missing keyframe at 20)
 *     For b2: {0, 10, 20} = {kf1, kf1, kf2} (reuse kf1 to set missing keyframe at 0}
 *  -# The transforms that are computed are defined as follows:
 *     - animations[anim index].animdata[key index].slist[bone index] (Scale Transform)
 *     - animations[anim index].animdata[key index].tlist[bone index] (Translation Transform)
 *     - animations[anim index].animdata[key index].qlist[bone index] (Quaternion Transform)
 *  -# Once done, every bone will have a keyframe for all keyframes.
 */
void MeshData::ConstructAnimationData(void)
{
 // for each animation
 for(size_t anim = 0; anim < animations.size(); anim++)
    {
     // 
     size_t n_keys = animations[anim].keymap.size();
     animations[anim].animdata.reset(new MeshAnimationData[n_keys]);

     // compute frame times
     // remember, minframe does not have to start at zero!
     // you have to subtract minframe from frame to get zero offset
     for(auto iter = animations[anim].keymap.begin(); iter != animations[anim].keymap.end(); iter++) {
         uint32 frame = iter->first;
         size_t index = iter->second;
         animations[anim].animdata[index].frame = frame;
         animations[anim].animdata[index].delta = SECONDS_PER_FRAME*(frame - animations[anim].minframe);
        }

     // create keyframe data
     for(size_t i = 0; i < n_keys; i++)
        {
         // create bone data
         animations[anim].animdata[i].keyed.reset(new bool[bones.size()]);
         for(size_t j = 0; j < bones.size(); j++) animations[anim].animdata[i].keyed[j] = false;

         // create bone transform data
         animations[anim].animdata[i].slist.reset(new std::array<real32, 3>[bones.size()]);
         animations[anim].animdata[i].tlist.reset(new std::array<real32, 3>[bones.size()]);
         animations[anim].animdata[i].qlist.reset(new std::array<real32, 4>[bones.size()]);
         animations[anim].animdata[i].mlist.reset(new matrix4D[bones.size()]);
        }

     // for each bone that IS keyframed
     for(size_t i = 0; i < animations[anim].bonelist.size(); i++)
        {
         // for each keyframe
         for(size_t j = 0; j < animations[anim].bonelist[i].keyframes.size(); j++)
            {
             // find keyframe index from frame
             auto iter = animations[anim].keymap.find(animations[anim].bonelist[i].keyframes[j].frame);
             if(iter == animations[anim].keymap.end()) continue;

             size_t b_index = animations[anim].bonelist[i].bone_index;
             size_t k_index = iter->second;
             auto& kf = animations[anim].bonelist[i].keyframes[j];

             // mark as keyed
             animations[anim].animdata[k_index].keyed[b_index] = true;

             // set scale
             animations[anim].animdata[k_index].slist[b_index][0] = kf.scale[0];
             animations[anim].animdata[k_index].slist[b_index][1] = kf.scale[1];
             animations[anim].animdata[k_index].slist[b_index][2] = kf.scale[2];

             // set translation
             animations[anim].animdata[k_index].tlist[b_index][0] = kf.translation[0];
             animations[anim].animdata[k_index].tlist[b_index][1] = kf.translation[1];
             animations[anim].animdata[k_index].tlist[b_index][2] = kf.translation[2];

             // set quaternion
             animations[anim].animdata[k_index].qlist[b_index][0] = kf.quaternion[0];
             animations[anim].animdata[k_index].qlist[b_index][1] = kf.quaternion[1];
             animations[anim].animdata[k_index].qlist[b_index][2] = kf.quaternion[2];
             animations[anim].animdata[k_index].qlist[b_index][3] = kf.quaternion[3];
            }
        }

     // for each bone that IS NOT keyframed
     for(size_t i = 0; i < n_keys; i++)
        {
         for(size_t j = 0; j < bones.size(); j++)
            {
             if(animations[anim].animdata[i].keyed[j]) continue;

             // look for previous key (for this bone j)
             size_t prev_key = 0xFFFFFFFFul;
             for(size_t k = 0; k < i; k++) {
                 size_t k_rev = (i - 1) - k;
                 if(animations[anim].animdata[k_rev].keyed[j]) {
                    prev_key = k_rev;
                    break;
                   }
                }

             // look for following key (for this bone j)
             size_t next_key = 0xFFFFFFFFul;
             for(size_t k = i + 1; k < n_keys; k++) {
                 if(animations[anim].animdata[k].keyed[j]) {
                    next_key = k;
                    break;
                   }
                }

             // no previous key
             if(prev_key == 0xFFFFFFFFul) {
                animations[anim].animdata[i].keyed[j] = true;
                animations[anim].animdata[i].slist[j][0] = 1.0f;
                animations[anim].animdata[i].slist[j][1] = 1.0f;
                animations[anim].animdata[i].slist[j][2] = 1.0f;
                animations[anim].animdata[i].tlist[j][0] = 0.0f;
                animations[anim].animdata[i].tlist[j][1] = 0.0f;
                animations[anim].animdata[i].tlist[j][2] = 0.0f;
                animations[anim].animdata[i].qlist[j][0] = 1.0f;
                animations[anim].animdata[i].qlist[j][1] = 0.0f;
                animations[anim].animdata[i].qlist[j][2] = 0.0f;
                animations[anim].animdata[i].qlist[j][3] = 0.0f;
               }
             // no next key
             else if(next_key == 0xFFFFFFFFul) {
                animations[anim].animdata[i].keyed[j] = true;
                animations[anim].animdata[i].slist[j][0] = animations[anim].animdata[prev_key].slist[j][0];
                animations[anim].animdata[i].slist[j][1] = animations[anim].animdata[prev_key].slist[j][1];
                animations[anim].animdata[i].slist[j][2] = animations[anim].animdata[prev_key].slist[j][2];
                animations[anim].animdata[i].tlist[j][0] = animations[anim].animdata[prev_key].tlist[j][0];
                animations[anim].animdata[i].tlist[j][1] = animations[anim].animdata[prev_key].tlist[j][1];
                animations[anim].animdata[i].tlist[j][2] = animations[anim].animdata[prev_key].tlist[j][2];
                animations[anim].animdata[i].qlist[j][0] = animations[anim].animdata[prev_key].qlist[j][0];
                animations[anim].animdata[i].qlist[j][1] = animations[anim].animdata[prev_key].qlist[j][1];
                animations[anim].animdata[i].qlist[j][2] = animations[anim].animdata[prev_key].qlist[j][2];
                animations[anim].animdata[i].qlist[j][3] = animations[anim].animdata[prev_key].qlist[j][3];
               }
             // prev and next keys
             else {
                animations[anim].animdata[i].keyed[j] = true;
                real32 ratio = (animations[anim].animdata[i].delta - animations[anim].animdata[prev_key].delta)/(animations[anim].animdata[next_key].delta - animations[anim].animdata[prev_key].delta);
                lerp3D(animations[anim].animdata[i].slist[j].data(), animations[anim].animdata[prev_key].slist[j].data(), animations[anim].animdata[next_key].slist[j].data(), ratio);
                lerp3D(animations[anim].animdata[i].tlist[j].data(), animations[anim].animdata[prev_key].tlist[j].data(), animations[anim].animdata[next_key].tlist[j].data(), ratio);
                qslerp(animations[anim].animdata[i].qlist[j].data(), animations[anim].animdata[prev_key].qlist[j].data(), animations[anim].animdata[next_key].qlist[j].data(), ratio);
               }
            }
        }
    }
}

ErrorCode MeshData::ConstructGraphics(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return DebugErrorCode(EC_D3D_DEVICE, __LINE__, __FILE__);

 // initialize mesh buffers
 graphics.vbuffer.reset(new ID3D11Buffer*[meshes.size()]);
 graphics.ibuffer.reset(new ID3D11Buffer*[meshes.size()]);
 for(size_t i = 0; i < meshes.size(); i++) {
     graphics.vbuffer[i] = nullptr;
     graphics.ibuffer[i] = nullptr;
    } 
 graphics.jbuffer = nullptr;

 // prepare mesh buffer
 struct MESHBUFFER {
  real32 position[4];
  real32 normal[4];
  real32 uv1[2];
  real32 uv2[2];
  uint16 bi[4];
  real32 bw[4];
  real32 color1[4];
  real32 color2[4];
 };

 // create mesh buffers
 for(size_t i = 0; i < meshes.size(); i++)
    {
     // copy vertex data
     std::unique_ptr<MESHBUFFER[]> data(new MESHBUFFER[meshes[i].n_verts]);
     for(size_t j = 0; j < meshes[i].n_verts; j++) {
         // position
         data[j].position[0] = meshes[i].position[j].v[0];
         data[j].position[1] = meshes[i].position[j].v[1];
         data[j].position[2] = meshes[i].position[j].v[2];
         data[j].position[3] = 1.0f;
         // normal
         data[j].normal[0] = meshes[i].normal[j].v[0];
         data[j].normal[1] = meshes[i].normal[j].v[1];
         data[j].normal[2] = meshes[i].normal[j].v[2];
         data[j].normal[3] = 1.0f;
         // uvs
         data[j].uv1[0] = meshes[i].uvs[0][j].v[0]; // u
         data[j].uv1[1] = meshes[i].uvs[0][j].v[1]; // v
         data[j].uv2[0] = meshes[i].uvs[1][j].v[0]; // u
         data[j].uv2[1] = meshes[i].uvs[1][j].v[1]; // v
         // blend indices
         data[j].bi[0] = meshes[i].bi[j].v[0];
         data[j].bi[1] = meshes[i].bi[j].v[1];
         data[j].bi[2] = meshes[i].bi[j].v[2];
         data[j].bi[3] = meshes[i].bi[j].v[3];
         // blend weights
         data[j].bw[0] = meshes[i].bw[j].v[0];
         data[j].bw[1] = meshes[i].bw[j].v[1];
         data[j].bw[2] = meshes[i].bw[j].v[2];
         data[j].bw[3] = meshes[i].bw[j].v[3];
         // colors
         data[j].color1[0] = meshes[i].colors[0][j].v[0];
         data[j].color1[1] = meshes[i].colors[0][j].v[1];
         data[j].color1[2] = meshes[i].colors[0][j].v[2];
         data[j].color1[3] = 1.0f;
         data[j].color2[0] = meshes[i].colors[1][j].v[0];
         data[j].color2[1] = meshes[i].colors[1][j].v[1];
         data[j].color2[2] = meshes[i].colors[1][j].v[2];
         data[j].color2[3] = 1.0f;
        }

     // create face data
     std::unique_ptr<uint32[]> facebuffer;
     uint32 face_indices = 3*meshes[i].n_faces;
     if(face_indices) facebuffer.reset(new uint32[face_indices]);

     // copy face data
     size_t curr = 0;
     for(size_t j = 0; j < meshes[i].surfaces.size(); j++) {
         for(size_t k = 0; k < meshes[i].surfaces[j].n_faces; k++) {
             facebuffer[curr++] = meshes[i].surfaces[j].facelist[k].v[0];
             facebuffer[curr++] = meshes[i].surfaces[j].facelist[k].v[1];
             facebuffer[curr++] = meshes[i].surfaces[j].facelist[k].v[2];
            }
        }

     // create vertex buffer
     ID3D11Buffer* vb = nullptr;
     if(meshes[i].n_verts) {
        auto code = CreateVertexBuffer((LPVOID)data.get(), meshes[i].n_verts, sizeof(MESHBUFFER), &vb);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
       }

     // create index buffer
     ID3D11Buffer* ib = nullptr;
     if(face_indices) {
        auto code = CreateIndexBuffer((LPVOID)facebuffer.get(), face_indices, sizeof(uint32), &ib);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
       }

     // assign buffers
     graphics.vbuffer[i] = vb;
     graphics.ibuffer[i] = ib;
    }

 // prepare bone buffer
 if(bones.size()) {
    std::unique_ptr<AXISBUFFER[]> data(new AXISBUFFER[bones.size()]);
    for(size_t i = 0; i < bones.size(); i++) {
        data[i].m[0x0] = bones[i].m_abs[0x0];
        data[i].m[0x1] = bones[i].m_abs[0x1];
        data[i].m[0x2] = bones[i].m_abs[0x2];
        data[i].m[0x3] = bones[i].m_abs[0x3];
        data[i].m[0x4] = bones[i].m_abs[0x4];
        data[i].m[0x5] = bones[i].m_abs[0x5];
        data[i].m[0x6] = bones[i].m_abs[0x6];
        data[i].m[0x7] = bones[i].m_abs[0x7];
        data[i].m[0x8] = bones[i].m_abs[0x8];
        data[i].m[0x9] = bones[i].m_abs[0x9];
        data[i].m[0xA] = bones[i].m_abs[0xA];
        data[i].m[0xB] = bones[i].m_abs[0xB];
        data[i].m[0xC] = bones[i].m_abs[0xC];
        data[i].m[0xD] = bones[i].m_abs[0xD];
        data[i].m[0xE] = bones[i].m_abs[0xE];
        data[i].m[0xF] = bones[i].m_abs[0xF];
        data[i].scale[0] = 1.0f;
        data[i].scale[1] = 1.0f;
        data[i].scale[2] = 1.0f;
        data[i].scale[3] = 1.0f;
       }
    // create buffer
    auto code = CreateImmutableAxisBuffer(data.get(), (DWORD)bones.size(), &graphics.jbuffer);
    if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
   }

 // allocate resource list
 size_t rsize = 0;
 for(size_t i = 0; i < materials.size(); i++) rsize += materials[i].textures.size();
 if(rsize > 0xFFFFul) return DebugErrorCode(EC_MODEL_TEXTURE_RESOURCES, __LINE__, __FILE__);
 if(rsize) graphics.resources.resize(rsize, nullptr);

 // create shader resource views (shared textures)
 uint16 resource_index = 0;
 for(size_t i = 0; i < materials.size(); i++) {
     size_t n = materials[i].textures.size();
     for(size_t j = 0; j < n; j++) {
         ID3D11ShaderResourceView* resource = NULL;
         auto code = LoadTexture(materials[i].textures[j].filename.c_str(), &resource);         
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
         graphics.resources[resource_index++] = resource;
        }
    }

 return EC_SUCCESS;
}

ErrorCode MeshData::LoadMeshUTF(const wchar_t* filename)
{
 // read UTF8 file
 std::deque<std::string> linelist;
 ErrorCode code = ASCIIParseFile(filename, linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 //
 // PHASE #1
 // READ SKELETON
 //

 // read number of bones
 uint32 n_bones = 0;
 code = ASCIIReadUint32(linelist, &n_bones);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read bones
 if(n_bones)
   {
    // allocate bones
    skeletal = true;
    bones.resize(n_bones);

    // read joints
    for(uint32 i = 0; i < n_bones; i++)
       {
        // read name
        char buffer[1024];
        code = ASCIIReadString(linelist, buffer);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // name must be valid
        bones[i].name = ConvertUTF8ToUTF16(buffer);
        if(!bones[i].name.length()) return DebugErrorCode(EC_MODEL_BONENAME, __LINE__, __FILE__);

        // read parent
        code = ASCIIReadUint32(linelist, &bones[i].parent);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read position
        code = ASCIIReadVector3(linelist, &bones[i].position[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read matrix
        real32 m[16];
        code = ASCIIReadMatrix4(linelist, &m[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // save matrix to absolute format
        bones[i].m_abs[0x0] = m[0x0];
        bones[i].m_abs[0x1] = m[0x1];
        bones[i].m_abs[0x2] = m[0x2];
        bones[i].m_abs[0x3] = bones[i].position[0];
        bones[i].m_abs[0x4] = m[0x4];
        bones[i].m_abs[0x5] = m[0x5];
        bones[i].m_abs[0x6] = m[0x6];
        bones[i].m_abs[0x7] = bones[i].position[1];
        bones[i].m_abs[0x8] = m[0x8];
        bones[i].m_abs[0x9] = m[0x9];
        bones[i].m_abs[0xA] = m[0xA];
        bones[i].m_abs[0xB] = bones[i].position[2];
        bones[i].m_abs[0xC] = m[0xC];
        bones[i].m_abs[0xD] = m[0xD];
        bones[i].m_abs[0xE] = m[0xE];
        bones[i].m_abs[0xF] = m[0xF];

        // save inverse
        bones[i].m_inv = bones[i].m_abs;
        bones[i].m_inv.invert();

        // save matrix to relative format
        if(bones[i].parent == 0xFFFFFFFF) {
           bones[i].m_rel[0x0] = bones[i].m_abs[0x0];
           bones[i].m_rel[0x1] = bones[i].m_abs[0x1];
           bones[i].m_rel[0x2] = bones[i].m_abs[0x2];
           bones[i].m_rel[0x3] = bones[i].m_abs[0x3];
           bones[i].m_rel[0x4] = bones[i].m_abs[0x4];
           bones[i].m_rel[0x5] = bones[i].m_abs[0x5];
           bones[i].m_rel[0x6] = bones[i].m_abs[0x6];
           bones[i].m_rel[0x7] = bones[i].m_abs[0x7];
           bones[i].m_rel[0x8] = bones[i].m_abs[0x8];
           bones[i].m_rel[0x9] = bones[i].m_abs[0x9];
           bones[i].m_rel[0xA] = bones[i].m_abs[0xA];
           bones[i].m_rel[0xB] = bones[i].m_abs[0xB];
           bones[i].m_rel[0xC] = bones[i].m_abs[0xC];
           bones[i].m_rel[0xD] = bones[i].m_abs[0xD];
           bones[i].m_rel[0xE] = bones[i].m_abs[0xE];
           bones[i].m_rel[0xF] = bones[i].m_abs[0xF];
          }
        else {
           // R*P = A (relative * parent = absolute)
           // R = A*inv(P) (relative = absolute * parent inverse)
           // this is why we store the inverse matrices
           bones[i].m_rel = bones[i].m_abs * bones[bones[i].parent].m_inv;
          }

        // add bone to bonemap
        bonemap.insert(std::map<STDSTRINGW, uint32>::value_type(bones[i].name, i));
       }
   }
 else
   {
    // always create a default joint
    skeletal = false;
    n_bones = 1;
    bones.resize(n_bones);

    // create default joint
    bones[0].name = L"default";
    bones[0].parent = 0xFFFFFFFFul;
    bones[0].position[0] = 0.0f;
    bones[0].position[1] = 0.0f;
    bones[0].position[2] = 0.0f;
    bones[0].m_abs.load_identity();
    bones[0].m_rel.load_identity();
    bones[0].m_inv.load_identity();
   }

 //
 // PHASE #2
 // READ ANIMATIONS
 //

 // read number of animations
 uint32 n_anim = 0;
 code = ASCIIReadUint32(linelist, &n_anim);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read animations
 if(n_anim) animations.resize(n_anim);
 for(size_t i = 0; i < n_anim; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // name must be valid
     animations[i].name = ConvertUTF8ToUTF16(buffer);
     if(!animations[i].name.length()) return DebugErrorCode(EC_MODEL_ANIMATION_NAME, __LINE__, __FILE__);

     // set loop to default value of true
     animations[i].loop = true;

     // read number of keyframed bones
     uint32 n_keyframedbones = 0;
     code = ASCIIReadUint32(linelist, &n_keyframedbones);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
     if(!n_keyframedbones) return DebugErrorCode(EC_MODEL_KEYFRAMED_BONES, __LINE__, __FILE__);

     // keep track of start and end frames (per-animation)
     animations[i].minframe = 0xFFFFFFFFul;
     animations[i].maxframe = 0x00000000ul;

     // read keyframed bones
     animations[i].bonelist.resize(n_keyframedbones);
     for(size_t j = 0; j < n_keyframedbones; j++)
        {
         // read name
         char buffer[1024];
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

         // name must be valid
         STDSTRINGW name = ConvertUTF8ToUTF16(buffer);
         if(!name.length()) return DebugErrorCode(EC_MODEL_BONENAME, __LINE__, __FILE__);

         // lookup bone index
         auto iter = bonemap.find(name);
         if(iter == bonemap.end()) return DebugErrorCode(EC_MODEL_BONE_LOOKUP, __LINE__, __FILE__);
         animations[i].bonelist[j].bone_index = iter->second;

         // read number of keyframes
         uint32 n_keyframes = 0;
         code = ASCIIReadUint32(linelist, &n_keyframes);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
         if(!n_keyframes) return DebugErrorCode(EC_MODEL_KEYFRAMES, __LINE__, __FILE__);;

         // read keyframes
         animations[i].bonelist[j].keyframes.resize(n_keyframes);
         for(size_t k = 0; k < n_keyframes; k++)
            {
             // read frame number
             code = ASCIIReadUint32(linelist, &animations[i].bonelist[j].keyframes[k].frame);
             if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

             // save frame number
             animations[i].keyset.insert(animations[i].bonelist[j].keyframes[k].frame);

             // read translation
             code = ASCIIReadVector3(linelist, &animations[i].bonelist[j].keyframes[k].translation[0]);
             if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

             // read rotation
             code = ASCIIReadVector4(linelist, &animations[i].bonelist[j].keyframes[k].quaternion[0]);
             if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

             // read scale
             code = ASCIIReadVector3(linelist, &animations[i].bonelist[j].keyframes[k].scale[0]);
             if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
            }

         // set start and end frames (per-bone) (in frames)
         animations[i].bonelist[j].minframe = animations[i].bonelist[j].keyframes[0].frame;
         animations[i].bonelist[j].maxframe = animations[i].bonelist[j].keyframes[n_keyframes - 1].frame;

         // adjust per-animation frames
         if(animations[i].bonelist[j].minframe < animations[i].minframe) animations[i].minframe = animations[i].bonelist[j].minframe;
         if(animations[i].bonelist[j].maxframe > animations[i].maxframe) animations[i].maxframe = animations[i].bonelist[j].maxframe;
        }

     // create a keymap from the key set
     // keyset = {0, 10, 20, 30}
     // keymap = [{0, 0}, {10, 1}, {20, 2}, {30, 3}]
     size_t index = 0;
     for(auto iter = animations[i].keyset.begin(); iter != animations[i].keyset.end(); iter++)
         animations[i].keymap.insert(std::map<uint32, size_t>::value_type(*iter, index++));

     // set animation duration
     // since minframe doesn't have to start at 0, we need to compute the total number of frames
     // for example, if minframe = 100 and maxframe = 105, there are a total of 6 frames
     uint32 n_frames = animations[i].maxframe - animations[i].minframe + 1;
     animations[i].duration = SECONDS_PER_FRAME*static_cast<real32>(n_frames);
    }

 //
 // PHASE #3
 // BUILD ANIMATION DATA
 //

 ConstructAnimationData();

 //
 // PHASE #4
 // READ COLLISION MESHES
 //

 // read number of collision meshes
 uint32 n_collision = 0;
 code = ASCIIReadUint32(linelist, &n_collision);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read collision mesh list
 collisions.resize(n_collision);
 for(uint32 i = 0; i < n_collision; i++)
    {
     // read number of collision vertices
     collisions[i].n_verts = 0;
     code = ASCIIReadUint32(linelist, &collisions[i].n_verts);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
     if(!collisions[i].n_verts) return DebugErrorCode(EC_MODEL_VERTICES, __LINE__, __FILE__);

     // read collision mesh data
     collisions[i].position.reset(new c_point3D[collisions[i].n_verts]);
     for(uint32 j = 0; j < collisions[i].n_verts; j++) {
         code = ASCIIReadVector3(linelist, &collisions[i].position[j].v[0], false);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
        }

     // read number of collision faces
     code = ASCIIReadUint32(linelist, &collisions[i].n_faces);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // read collision faces
     if(collisions[i].n_faces) {
        collisions[i].facelist.reset(new c_triface[collisions[i].n_faces]);
        for(size_t j = 0; j < collisions[i].n_faces; j++) {
            code = ASCIIReadVector3(linelist, &collisions[i].facelist[j].v[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
           }
       } 
    }

 //
 // PHASE #5
 // READ MATERIALS
 //

 // read number of materials
 uint32 n_mats = 0;
 code = ASCIIReadUint32(linelist, &n_mats);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read mesh list
 std::vector<MeshMaterial> matlist;
 if(n_mats) matlist.resize(n_mats);
 for(uint32 i = 0; i < n_mats; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // name must be valid
     STDSTRINGW name = ConvertUTF8ToUTF16(buffer);
     if(!name.length()) return DebugErrorCode(EC_MODEL_MATERIAL_NAME, __LINE__, __FILE__);

     // read material index
     // it is very important to read this and use this as materials can be listed out of order
     uint32 matindex = 0;
     code = ASCIIReadUint32(linelist, &matindex);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // validate material index
     if(!(matindex < n_mats)) return DebugErrorCode(EC_MODEL_MATERIAL_INDEX, __LINE__, __FILE__);

     // assign name
     matlist[matindex].name = name;

     // read number of textures
     uint32 n_textures = 0;
     code = ASCIIReadUint32(linelist, &n_textures);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // TODO: Remove this constraint and assign a default texture in this case.
     if(!n_textures) return DebugErrorCode(EC_MODEL_TEXTURES, __LINE__, __FILE__);

     // read textures
     if(n_textures) matlist[matindex].textures.resize(n_textures);
     for(uint32 j = 0; j < n_textures; j++)
        {
         // read texture name
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

         // assign texture name
         matlist[matindex].textures[j].name = ConvertUTF8ToUTF16(buffer);
         if(!matlist[matindex].textures[j].name.length()) return DebugErrorCode(EC_MODEL_TEXTURE_NAME, __LINE__, __FILE__);

         // read texture semantic
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

         // assign texture semantic
         if(_strcmpi(buffer, "diffuse") == 0) matlist[matindex].textures[j].semantic = DIFFUSE_MAP;
         else return DebugErrorCode(EC_MODEL_TEXTURE_SEMANTIC, __LINE__, __FILE__);

         // read UV channel
         code = ASCIIReadUint16(linelist, &matlist[matindex].textures[j].uv_index);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

         // read texture filename
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

         // assign filename
         matlist[matindex].textures[j].filename = ConvertUTF8ToUTF16(buffer);
         if(!matlist[matindex].name.length()) return DebugErrorCode(EC_MODEL_TEXTURE_FILENAME, __LINE__, __FILE__);
        }
    }

 // once materials are sorted, assign resource index to each material
 // if you don't do this, textures get all messed up
 uint32 resource = 0;
 for(uint32 i = 0; i < n_mats; i++) {
     matlist[i].resource = static_cast<uint16>(resource);
     if(resource > 0xFFFFul) return DebugErrorCode(EC_MODEL_TEXTURE_RESOURCES, __LINE__, __FILE__);
     resource += static_cast<uint32>(matlist[i].textures.size());
    }

 // move material data
 materials = std::move(matlist);

 //
 // PHASE #6
 // READ MESHES
 //

 // read number of meshes
 uint32 n_mesh = 0;
 code = ASCIIReadUint32(linelist, &n_mesh);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
 if(!n_mesh) return DebugErrorCode(EC_MODEL_MESH, __LINE__, __FILE__); // TODO: Remove. It is OK to have nothing to draw.

 // read mesh list
 std::vector<MeshBuffers> meshlist;
 if(n_mesh) meshlist.resize(n_mesh);
 for(uint32 i = 0; i < n_mesh; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // name must be valid
     auto name = ConvertUTF8ToUTF16(buffer);
     if(!name.length()) return DebugErrorCode(EC_MODEL_MESHNAME, __LINE__, __FILE__);

     // read number of vertices
     uint32 n_verts = 0;
     code = ASCIIReadUint32(linelist, &n_verts);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
     if(!n_verts) return DebugErrorCode(EC_MODEL_VERTICES, __LINE__, __FILE__);

     // read number of UV channels
     uint32 n_uvs = 0;
     code = ASCIIReadUint32(linelist, &n_uvs);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
     if(n_uvs > 2) return DebugErrorCode(EC_MODEL_UV_CHANNELS, __LINE__, __FILE__);

     // read number of color channels
     uint32 n_colors = 0;
     code = ASCIIReadUint32(linelist, &n_colors);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
     if(n_colors > 2) return DebugErrorCode(EC_MODEL_COLOR_CHANNELS, __LINE__, __FILE__);

     // create mesh data
     std::unique_ptr<c_point3D[]> position(new c_point3D[n_verts]);
     std::unique_ptr<c_point3D[]> normal(new c_point3D[n_verts]);
     std::unique_ptr<c_point2D[]> uvs[2];
     uvs[0].reset(new c_point2D[n_verts]);
     uvs[1].reset(new c_point2D[n_verts]);
     std::unique_ptr<c_blend4i[]> bi(new c_blend4i[n_verts]);
     std::unique_ptr<c_blend4w[]> bw(new c_blend4w[n_verts]);
     std::unique_ptr<c_color4D[]> colors[2];
     colors[0].reset(new c_color4D[n_verts]);
     colors[1].reset(new c_color4D[n_verts]);

     // read mesh data
     for(uint32 j = 0; j < n_verts; j++)
        {
         // read position
         code = ASCIIReadVector3(linelist, &position[j].v[0], false);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

         // read normal
         code = ASCIIReadVector3(linelist, &normal[j].v[0], false);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

         // read UVs
         uvs[0][j].v[0] = uvs[0][j].v[1] = 0.0f;
         uvs[1][j].v[0] = uvs[1][j].v[1] = 0.0f;
         for(uint32 k = 0; k < n_uvs; k++) {
             code = ASCIIReadVector2(linelist, &uvs[k][j].v[0], false);
             if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
             uvs[k][j].v[1] = 1.0f - uvs[k][j].v[1]; // TODO: have Blender do this instead
            }

         // initialize blendindices and blendweights just in case we only have one auto-generated bone
         bi[j].v[0] = 0;
         bi[j].v[1] = 0;
         bi[j].v[2] = 0;
         bi[j].v[3] = 0;
         bw[j].v[0] = 1.0f;
         bw[j].v[1] = 0.0f;
         bw[j].v[2] = 0.0f;
         bw[j].v[3] = 0.0f;

         // read blendindices and blendweights
         // only read if actual has a skeleton defined, if autogenerated, there are no weights
         if(skeletal) {
            code = ASCIIReadVector4(linelist, &bi[j].v[0], false); // set to repeat?
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
            code = ASCIIReadVector4(linelist, &bw[j].v[0], false); // set to repeat?
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
           }

         // read colors
         colors[0][j].v[0] = colors[0][j].v[1] = colors[0][j].v[2] = colors[0][j].v[3] = 0.0f;
         colors[1][j].v[0] = colors[1][j].v[1] = colors[1][j].v[2] = colors[1][j].v[3] = 0.0f;
         for(uint32 k = 0; k < n_colors; k++) {
             code = ASCIIReadVector3(linelist, &colors[k][j].v[0], false);
             if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
            }
        }

     // read number of faces
     uint32 n_faces = 0;
     code = ASCIIReadUint32(linelist, &n_faces);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // read number of submeshes
     uint32 n_surfaces = 0;
     code = ASCIIReadUint32(linelist, &n_surfaces);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     std::vector<MeshSurface> surfaces;
     if(n_surfaces) surfaces.resize(n_surfaces);
     uint32 start = 0;
     for(uint32 j = 0; j < n_surfaces; j++)
        {
         // read number of surface faces
         surfaces[j].n_faces = 0;
         code = ASCIIReadUint32(linelist, &surfaces[j].n_faces);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

         // read material index
         surfaces[j].m_index = 0;
         code = ASCIIReadUint32(linelist, &surfaces[j].m_index);
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
         if(!(surfaces[j].m_index < materials.size())) return DebugErrorCode(EC_MODEL_MATERIAL_INDEX, __LINE__, __FILE__);

         // read index buffer
         if(surfaces[j].n_faces) {
            surfaces[j].facelist.reset(new c_triface[surfaces[j].n_faces]);
            for(size_t k = 0; k < surfaces[j].n_faces; k++) {
                code = ASCIIReadVector3(linelist, &surfaces[j].facelist[k].v[0], false);
                if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
               }
           }

         // set start value and then increment it
         surfaces[j].start = start;
         start += surfaces[j].n_faces;
        }

     // the sum of all faces must equal total number of faces
     if(start != n_faces)
        return DebugErrorCode(EC_MODEL_FACELIST, __LINE__, __FILE__);

     // assign data
     meshlist[i].name = name;
     meshlist[i].n_verts = n_verts;
     meshlist[i].n_faces = n_faces;
     meshlist[i].n_uvs = n_uvs;
     meshlist[i].n_colors = n_colors;
     meshlist[i].position = std::move(position);
     meshlist[i].normal = std::move(normal);
     meshlist[i].uvs[0] = std::move(uvs[0]);
     meshlist[i].uvs[1] = std::move(uvs[1]);
     meshlist[i].bi = std::move(bi);
     meshlist[i].bw = std::move(bw);
     meshlist[i].colors[0] = std::move(colors[0]);
     meshlist[i].colors[1] = std::move(colors[1]);
     meshlist[i].surfaces = std::move(surfaces);
    }

 // move mesh data
 meshes = std::move(meshlist);

 // construct graphics data and move on
 code = ConstructGraphics();
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
 return code;
}

ErrorCode MeshData::LoadMeshBIN(const wchar_t* filename)
{
 return EC_SUCCESS;
}

void MeshData::Free(void)
{
 // delete graphics buffers
 for(size_t i = 0; i < meshes.size(); i++) {
     if(graphics.vbuffer[i]) graphics.vbuffer[i]->Release();
     if(graphics.ibuffer[i]) graphics.ibuffer[i]->Release();
    }
 if(graphics.jbuffer) graphics.jbuffer->Release();
 graphics.jbuffer = nullptr;

 // delete graphics resources
 // do not call Release(), texture manager will do it when reference count goes to zero
 for(size_t i = 0; i < materials.size(); i++) {
     for(size_t j = 0; j < materials[i].textures.size(); j++) {
         ErrorCode code = FreeTexture(materials[i].textures[j].filename.c_str());
         if(Fail(code)) DebugErrorCode(code, __LINE__, __FILE__);
        }
    }
 graphics.resources.clear();

 // delete meshes
 meshes.clear();
 collisions.clear();

 // delete animations
 animations.clear();
 bones.clear();
 bonemap.clear();

 // reset skeletal flag
 skeletal = false;
}

ErrorCode MeshData::SaveMeshUTF(const wchar_t* filename)
{
 // create output file
 using namespace std;
 if(!filename) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);
 ofstream ofile(filename);
 if(!ofile) return DebugErrorCode(EC_FILE_OPEN, __LINE__, __FILE__);
 
 // save bones
 if(skeletal) {
    ofile << bones.size() << " # number of bones" << endl;
    for(size_t i = 0; i < bones.size(); i++) {
        STDSTRINGA name = ConvertUTF16ToUTF8(bones[i].name.c_str());
        ofile << name.c_str() << endl;
        if(bones[i].parent == 0xFFFFFFFFul) ofile << "-1" << endl;
        else ofile << bones[i].parent << endl;
        ofile << bones[i].position[0] << " " << bones[i].position[1] << " " << bones[i].position[2] << endl;
        ofile << bones[i].m_abs[0x0] << " " << bones[i].m_abs[0x1] << " " << bones[i].m_abs[0x2] << " " << bones[i].m_abs[0x3] << " ";
        ofile << bones[i].m_abs[0x4] << " " << bones[i].m_abs[0x5] << " " << bones[i].m_abs[0x6] << " " << bones[i].m_abs[0x7] << " ";
        ofile << bones[i].m_abs[0x8] << " " << bones[i].m_abs[0x9] << " " << bones[i].m_abs[0xA] << " " << bones[i].m_abs[0xB] << " ";
        ofile << bones[i].m_abs[0xC] << " " << bones[i].m_abs[0xD] << " " << bones[i].m_abs[0xE] << " " << bones[i].m_abs[0xF] << endl;
        if(ofile.fail()) return DebugErrorCode(EC_FILE_WRITE, __LINE__, __FILE__);
       }
   }
 else
    ofile << "0 # number of bones" << endl;

 // save animations
 ofile << animations.size() << " # number of animations" << endl;
 for(size_t i = 0; i < animations.size(); i++) {
     STDSTRINGA name = ConvertUTF16ToUTF8(animations[i].name.c_str());
     ofile << name.c_str() << endl;
     ofile << animations[i].bonelist.size() << " # number of keyframed bones" << endl;
     for(size_t j = 0; j < animations[i].bonelist.size(); j++) {
         uint32 bi = animations[i].bonelist[j].bone_index;
         ofile << ConvertUTF16ToUTF8(bones[bi].name.c_str()).c_str() << endl;
         ofile << animations[i].bonelist[j].keyframes.size() << " # number of keys" << endl;
         for(size_t k = 0; k < animations[i].bonelist[j].keyframes.size(); k++) {
             auto& kf = animations[i].bonelist[j].keyframes[k];
             ofile << kf.frame << endl;
             ofile << kf.translation[0] << " " << kf.translation[1] << " " << kf.translation[2] << endl;
             ofile << kf.quaternion[0] << " " << kf.quaternion[1] << " " << kf.quaternion[2] << " " << kf.quaternion[3] << endl;
             ofile << kf.scale[0] << " " << kf.scale[1] << " " << kf.scale[2] << endl;
            }
        }
     if(ofile.fail()) return DebugErrorCode(EC_FILE_WRITE, __LINE__, __FILE__);
    }

 // save collision meshes
 ofile << collisions.size() << " # number of collision meshes" << endl;
 for(size_t i = 0; i < collisions.size(); i++) {
     ofile << collisions[i].n_verts << " # number of collision vertices" << endl;
     for(uint32 j = 0; j < collisions[i].n_verts; j++) {
         ofile << collisions[i].position[j].v[0] << " " <<
                  collisions[i].position[j].v[1] << " " <<
                  collisions[i].position[j].v[2] << endl;
        }
     ofile << collisions[i].n_faces << " # number of collision faces" << endl;
     for(uint32 j = 0; j < collisions[i].n_faces; j++) {
         ofile << collisions[i].facelist[j].v[0] << " " <<
                  collisions[i].facelist[j].v[1] << " " <<
                  collisions[i].facelist[j].v[2] << endl;
        }
     if(ofile.fail()) return DebugErrorCode(EC_FILE_WRITE, __LINE__, __FILE__);
    }

 // save materials
 ofile << materials.size() << " # number of materials" << endl;
 for(size_t i = 0; i < materials.size(); i++) {
     ofile << ConvertUTF16ToUTF8(materials[i].name.c_str()).c_str() << endl;
     ofile << i << " # material index" << endl;
     ofile << materials[i].textures.size() << " # number of textures" << endl;
     for(size_t j = 0; j < materials[i].textures.size(); j++) {
         auto& obj = materials[i].textures[j];
         ofile << ConvertUTF16ToUTF8(obj.name.c_str()).c_str() << endl;
         if(obj.semantic == 0x00) ofile << "diffuse" << endl;
         else if(obj.semantic == 0x01) ofile << "specular" << endl;
         else if(obj.semantic == 0x02) ofile << "normal" << endl;
         ofile << obj.uv_index << endl;
         ofile << ConvertUTF16ToUTF8(obj.filename.c_str()).c_str() << endl;
        }
     if(ofile.fail()) return DebugErrorCode(EC_FILE_WRITE, __LINE__, __FILE__);
    }

 // save meshes
 ofile << meshes.size() << " # number of meshes" << endl;
 for(size_t i = 0; i < meshes.size(); i++) {
     ofile << ConvertUTF16ToUTF8(meshes[i].name.c_str()).c_str() << endl;
     ofile << meshes[i].n_verts << " # number of vertices" << endl;
     ofile << meshes[i].n_uvs << " # number of UV channels" << endl;
     ofile << meshes[i].n_colors << " # number of color channels" << endl;
     for(size_t j = 0; j < meshes[i].n_verts; j++) {
         ofile << meshes[i].position[j].v[0] << " " <<
                  meshes[i].position[j].v[1] << " " <<
                  meshes[i].position[j].v[2] << endl;
         ofile << meshes[i].normal[j].v[0] << " " <<
                  meshes[i].normal[j].v[1] << " " <<
                  meshes[i].normal[j].v[2] << endl;
         for(size_t k = 0; k < meshes[i].n_uvs; k++) {
             ofile << meshes[i].uvs[k][j].v[0] << " " <<
                      meshes[i].uvs[k][j].v[1] << endl;
            }
         for(size_t k = 0; k < meshes[i].n_colors; k++) {
             ofile << meshes[i].colors[k][j].v[0] << " " <<
                      meshes[i].colors[k][j].v[1] << " " <<
                      meshes[i].colors[k][j].v[2] << endl;
            }
        }
     ofile << meshes[i].n_faces << " # number of total faces" << endl;
     ofile << meshes[i].surfaces.size() << " # number of submeshes" << endl;
     for(size_t j = 0; j < meshes[i].surfaces.size(); j++) {
         ofile << meshes[i].surfaces[j].n_faces << " # number of faces" << endl;
         ofile << meshes[i].surfaces[j].m_index << " # material index" << endl;
         for(size_t k = 0; k < meshes[i].surfaces[j].n_faces; k++) {
             auto& face = meshes[i].surfaces[j].facelist[k];
             ofile << face.v[0] << " " << face.v[1] << " " << face.v[2] << endl;
            }
        }
     if(ofile.fail()) return DebugErrorCode(EC_FILE_WRITE, __LINE__, __FILE__);
    }

 return EC_SUCCESS;
}

ErrorCode MeshData::SaveMeshBIN(const wchar_t* filename)
{
 return EC_SUCCESS;
}
