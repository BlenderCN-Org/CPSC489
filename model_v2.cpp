#include "stdafx.h"
#include "model_v2.h"

#include "errors.h"
#include "gfx.h"
#include "ascii.h"

static const uint32 FRAMES_PER_SECOND = 60ul;
static const real32 SECONDS_PER_FRAME = 1.0f/60.0f;

MeshData::MeshData() : skeletal(false)
{
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

bool MeshData::ConstructGraphics(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return Fail(EC_D3D_DEVICE, __LINE__, __FILE__);

 return true;
}

bool MeshData::LoadMeshUTF(const wchar_t* filename)
{
 // read UTF8 file
 std::deque<std::string> linelist;
 ErrorCode code = ASCIIParseFile(filename, linelist);
 if(Fail(code)) return Fail(code, __LINE__, __FILE__);

 //
 // PHASE #1
 // READ SKELETON
 //

 // read number of bones
 uint32 n_bones = 0;
 code = ASCIIReadUint32(linelist, &n_bones);
 if(Fail(code)) return Fail(code, __LINE__, __FILE__);

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
        if(Fail(code)) return Fail(code, __LINE__, __FILE__);

        // name must be valid
        bones[i].name = ConvertUTF8ToUTF16(buffer);
        if(!bones[i].name.length()) return Fail(EC_MODEL_BONENAME, __LINE__, __FILE__);

        // read parent
        code = ASCIIReadUint32(linelist, &bones[i].parent);
        if(Fail(code)) return Fail(code, __LINE__, __FILE__);

        // read position
        code = ASCIIReadVector3(linelist, &bones[i].position[0], false);
        if(Fail(code)) return Fail(code, __LINE__, __FILE__);

        // read matrix
        real32 m[16];
        code = ASCIIReadMatrix4(linelist, &m[0], false);
        if(Fail(code)) return Fail(code, __LINE__, __FILE__);

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
 if(Fail(code)) return Fail(code, __LINE__, __FILE__);

 // read animations
 if(n_anim) animations.resize(n_anim);
 for(size_t i = 0; i < n_anim; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return Fail(code, __LINE__, __FILE__);

     // name must be valid
     animations[i].name = ConvertUTF8ToUTF16(buffer);
     if(!animations[i].name.length()) return Fail(EC_MODEL_ANIMATION_NAME, __LINE__, __FILE__);

     // set loop to default value of true
     animations[i].loop = true;

     // read number of keyframed bones
     uint32 n_keyframedbones = 0;
     code = ASCIIReadUint32(linelist, &n_keyframedbones);
     if(Fail(code)) return Fail(code, __LINE__, __FILE__);
     if(!n_keyframedbones) return Fail(EC_MODEL_KEYFRAMED_BONES, __LINE__, __FILE__);

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
         if(Fail(code)) return Fail(code, __LINE__, __FILE__);

         // name must be valid
         STDSTRINGW name = ConvertUTF8ToUTF16(buffer);
         if(!name.length()) return Fail(EC_MODEL_BONENAME, __LINE__, __FILE__);

         // lookup bone index
         auto iter = bonemap.find(name);
         if(iter == bonemap.end()) return Fail(EC_MODEL_BONE_LOOKUP, __LINE__, __FILE__);
         animations[i].bonelist[j].bone_index = iter->second;

         // read number of keyframes
         uint32 n_keyframes = 0;
         code = ASCIIReadUint32(linelist, &n_keyframes);
         if(Fail(code)) return Fail(code, __LINE__, __FILE__);
         if(!n_keyframes) return Fail(EC_MODEL_KEYFRAMES, __LINE__, __FILE__);;

         // read keyframes
         animations[i].bonelist[j].keyframes.resize(n_keyframes);
         for(size_t k = 0; k < n_keyframes; k++)
            {
             // read frame number
             code = ASCIIReadUint32(linelist, &animations[i].bonelist[j].keyframes[k].frame);
             if(Fail(code)) return Fail(code, __LINE__, __FILE__);

             // save frame number
             animations[i].keyset.insert(animations[i].bonelist[j].keyframes[k].frame);

             // read translation
             code = ASCIIReadVector3(linelist, &animations[i].bonelist[j].keyframes[k].translation[0]);
             if(Fail(code)) return Fail(code, __LINE__, __FILE__);

             // read rotation
             code = ASCIIReadVector4(linelist, &animations[i].bonelist[j].keyframes[k].quaternion[0]);
             if(Fail(code)) return Fail(code, __LINE__, __FILE__);

             // read scale
             code = ASCIIReadVector3(linelist, &animations[i].bonelist[j].keyframes[k].scale[0]);
             if(Fail(code)) return Fail(code, __LINE__, __FILE__);
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
 if(Fail(code)) return Fail(code, __LINE__, __FILE__);

 // read collision mesh list
 collisions.resize(n_collision);
 for(uint32 i = 0; i < n_collision; i++)
    {
     // read number of collision vertices
     collisions[i].n_verts = 0;
     code = ASCIIReadUint32(linelist, &collisions[i].n_verts);
     if(Fail(code)) return Fail(code, __LINE__, __FILE__);
     if(!collisions[i].n_verts) return Fail(EC_MODEL_VERTICES, __LINE__, __FILE__);

     // read collision mesh data
     collisions[i].position.reset(new c_point3D[collisions[i].n_verts]);
     for(uint32 j = 0; j < collisions[i].n_verts; j++) {
         code = ASCIIReadVector3(linelist, &collisions[i].position[j].v[0], false);
         if(Fail(code)) return Fail(code, __LINE__, __FILE__);
        }

     // read number of collision faces
     code = ASCIIReadUint32(linelist, &collisions[i].n_faces);
     if(Fail(code)) return Fail(code, __LINE__, __FILE__);

     // read collision faces
     if(collisions[i].n_faces) {
        meshes[i].facelist.reset(new c_triface[collisions[i].n_faces]);
        for(size_t j = 0; j < meshes[i].n_faces; j++) {
            code = ASCIIReadVector3(linelist, &meshes[i].facelist[j].v[0], false);
            if(Fail(code)) return Fail(code, __LINE__, __FILE__);
           }
       } 
    }

 //
 // PHASE #5
 // READ MESHES
 //

 return true;
}

bool MeshData::LoadMeshBIN(const wchar_t* filename)
{
 return true;
}

void MeshData::Free(void)
{
 // delete graphics data
 for(size_t i = 0; i < meshes.size(); i++) {
     if(graphics.vbuffer[i]) graphics.vbuffer[i]->Release();
     if(graphics.ibuffer[i]) graphics.ibuffer[i]->Release();
    std::unique_ptr<ID3D11ShaderResourceView*[]> rvlist;
    }
 if(graphics.jbuffer) graphics.jbuffer->Release();
 graphics.jbuffer = nullptr;

 // delete meshes
 meshes.clear();

 // delete collision meshes
 collisions.clear();

 // delete animations
 animations.clear();

 // delete bones
 bones.clear();
 bonemap.clear();

 // reset skeletal flag
 skeletal = false;
}

bool MeshData::SaveMeshUTF(const wchar_t* filename)
{
 return true;
}

bool MeshData::SaveMeshBIN(const wchar_t* filename)
{
 return true;
}