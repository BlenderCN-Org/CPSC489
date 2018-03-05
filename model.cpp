#include "stdafx.h"
#include "stdwin.h"
#include "stdgfx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "math.h"
#include "vector3.h"
#include "matrix4.h"
#include "gfx.h"
#include "layouts.h"
#include "blending.h"
#include "rasters.h"
#include "sampler.h"
#include "shaders.h"
#include "texture.h"
#include "axes.h"
#include "ascii.h"
#include "model.h"

MeshUTF::MeshUTF()
{
 has_weights = false;
 ja_buffer = 0;
}

MeshUTF::~MeshUTF()
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
 *     just resue the keyframe at 10.
 *     For b1: {0, 10, 20} = {kf1, kf2, kf2} (reuse kf2 to set missing keyframe at 20)
 *     For b2: {0, 10, 20} = {kf1, kf1, kf2} (reuse kf1 to set missing keyframe at 0}
 *  -# The transforms that are computed are defined as follows:
 *     - animations[anim index].animdata[key index].slist[bone index] (Scale Transform)
 *     - animations[anim index].animdata[key index].tlist[bone index] (Translation Transform)
 *     - animations[anim index].animdata[key index].qlist[bone index] (Quaternion Transform)
 *  -# Once done, every bone will have a keyframe for all keyframes.
 */
ErrorCode MeshUTF::ConstructAnimationData(void)
{
 // number of seconds per frame
 const real32 SECONDS_PER_FRAME = 1.0f/30.0f;

 // for each animation
 for(size_t anim = 0; anim < animations.size(); anim++)
    {
     // 
     size_t n_keys = animations[anim].keymap.size();
     animations[anim].animdata.reset(new MeshUTFAnimationData[n_keys]);

     // compute frame times
     for(auto iter = animations[anim].keymap.begin(); iter != animations[anim].keymap.end(); iter++) {
         uint32 frame = iter->first;
         size_t index = iter->second;
         animations[anim].animdata[index].frame = frame;
         animations[anim].animdata[index].delta = SECONDS_PER_FRAME*frame;
        }

     // create keyframe data
     for(size_t i = 0; i < n_keys; i++)
        {
         // create bone data
         animations[anim].animdata[i].keyed.reset(new bool[joints.size()]);
         for(size_t j = 0; j < joints.size(); j++) animations[anim].animdata[i].keyed[j] = false;

         // create bone transform data
         animations[anim].animdata[i].slist.reset(new std::array<real32, 3>[joints.size()]);
         animations[anim].animdata[i].tlist.reset(new std::array<real32, 3>[joints.size()]);
         animations[anim].animdata[i].qlist.reset(new std::array<real32, 4>[joints.size()]);
         animations[anim].animdata[i].mlist.reset(new matrix4D[joints.size()]);
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
         for(size_t j = 0; j < joints.size(); j++)
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

 return EC_SUCCESS;
}

ErrorCode MeshUTF::LoadModel(const wchar_t* filename)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // load lines
 std::deque<std::string> linelist;
 ErrorCode code = ASCIIParseFile(filename, linelist);
 if(Fail(code)) return code;

 //
 // READ SKELETON
 //

 // read number of joints
 uint32 n_jnts = 0;
 code = ASCIIReadUint32(linelist, &n_jnts);
 if(Fail(code)) return code;


 // read joints
 if(n_jnts)
   {
    // allocate joints
    has_weights = true;
    joints.resize(n_jnts);

    // read joints
    for(uint32 i = 0; i < n_jnts; i++)
       {
        // read name
        char buffer[1024];
        code = ASCIIReadString(linelist, buffer);
        if(Fail(code)) return code;

        // name must be valid
        joints[i].name = ConvertUTF8ToUTF16(buffer);
        if(!joints[i].name.length()) return EC_MODEL_BONENAME;

        // read parent
        code = ASCIIReadUint32(linelist, &joints[i].parent);
        if(Fail(code)) return code;

        // read position
        code = ASCIIReadVector3(linelist, &joints[i].position[0], false);
        if(Fail(code)) return code;

        // read matrix
        code = ASCIIReadMatrix4(linelist, &joints[i].m[0], false);
        if(Fail(code)) return code;

        // save position to relative format
        if(joints[i].parent == 0xFFFFFFFF) {
           joints[i].p_rel[0] = joints[i].position[0];
           joints[i].p_rel[1] = joints[i].position[1];
           joints[i].p_rel[2] = joints[i].position[2];
          }
        else {
           joints[i].p_rel[0] = joints[i].position[0] - joints[joints[i].parent].position[0];
           joints[i].p_rel[1] = joints[i].position[1] - joints[joints[i].parent].position[1];
           joints[i].p_rel[2] = joints[i].position[2] - joints[joints[i].parent].position[2];
          }

        // save matrix to absolute format
        joints[i].m_abs[0x0] = joints[i].m[0x0];
        joints[i].m_abs[0x1] = joints[i].m[0x1];
        joints[i].m_abs[0x2] = joints[i].m[0x2];
        joints[i].m_abs[0x3] = joints[i].position[0];
        joints[i].m_abs[0x4] = joints[i].m[0x4];
        joints[i].m_abs[0x5] = joints[i].m[0x5];
        joints[i].m_abs[0x6] = joints[i].m[0x6];
        joints[i].m_abs[0x7] = joints[i].position[1];
        joints[i].m_abs[0x8] = joints[i].m[0x8];
        joints[i].m_abs[0x9] = joints[i].m[0x9];
        joints[i].m_abs[0xA] = joints[i].m[0xA];
        joints[i].m_abs[0xB] = joints[i].position[2];
        joints[i].m_abs[0xC] = joints[i].m[0xC];
        joints[i].m_abs[0xD] = joints[i].m[0xD];
        joints[i].m_abs[0xE] = joints[i].m[0xE];
        joints[i].m_abs[0xF] = joints[i].m[0xF];

        // save inverse
        joints[i].m_inv = joints[i].m_abs;
        joints[i].m_inv.invert();

        // save matrix to relative format
        if(joints[i].parent == 0xFFFFFFFF) {
           joints[i].m_rel[0x0] = joints[i].m_abs[0x0];
           joints[i].m_rel[0x1] = joints[i].m_abs[0x1];
           joints[i].m_rel[0x2] = joints[i].m_abs[0x2];
           joints[i].m_rel[0x3] = joints[i].m_abs[0x3];
           joints[i].m_rel[0x4] = joints[i].m_abs[0x4];
           joints[i].m_rel[0x5] = joints[i].m_abs[0x5];
           joints[i].m_rel[0x6] = joints[i].m_abs[0x6];
           joints[i].m_rel[0x7] = joints[i].m_abs[0x7];
           joints[i].m_rel[0x8] = joints[i].m_abs[0x8];
           joints[i].m_rel[0x9] = joints[i].m_abs[0x9];
           joints[i].m_rel[0xA] = joints[i].m_abs[0xA];
           joints[i].m_rel[0xB] = joints[i].m_abs[0xB];
           joints[i].m_rel[0xC] = joints[i].m_abs[0xC];
           joints[i].m_rel[0xD] = joints[i].m_abs[0xD];
           joints[i].m_rel[0xE] = joints[i].m_abs[0xE];
           joints[i].m_rel[0xF] = joints[i].m_abs[0xF];
          }
        else {
           // R*P = A (relative * parent = absolute)
           // R = A*inv(P) (relative = absolute * parent inverse)
           // this is why we store the inverse matrices
           matrix4D Pinv = joints[joints[i].parent].m_rel;
           Pinv.invert();
           joints[i].m_rel = joints[i].m_abs * Pinv;
          }

        // add joint to jointmap
        jointmap.insert(std::map<STDSTRINGW, uint32>::value_type(joints[i].name, i));
       }
   }
 // auto-generate joint
 else
   {
    // allocate default joint
    has_weights = false;
    n_jnts = 1;
    joints.resize(n_jnts);

    // create default joint
    joints[0].name = L"default";
    joints[0].parent = 0xFFFFFFFFul;
    joints[0].position[0] = 0.0f;
    joints[0].position[1] = 0.0f;
    joints[0].position[2] = 0.0f;
    joints[0].m.load_identity();

    // save matrices
    joints[0].m_abs[0x0] = joints[0].m_rel[0x0] = joints[0].m[0x0];
    joints[0].m_abs[0x1] = joints[0].m_rel[0x1] = joints[0].m[0x1];
    joints[0].m_abs[0x2] = joints[0].m_rel[0x2] = joints[0].m[0x2];
    joints[0].m_abs[0x3] = joints[0].m_rel[0x3] = joints[0].m[0x3];
    joints[0].m_abs[0x4] = joints[0].m_rel[0x4] = joints[0].m[0x4];
    joints[0].m_abs[0x5] = joints[0].m_rel[0x5] = joints[0].m[0x5];
    joints[0].m_abs[0x6] = joints[0].m_rel[0x6] = joints[0].m[0x6];
    joints[0].m_abs[0x7] = joints[0].m_rel[0x7] = joints[0].m[0x7];
    joints[0].m_abs[0x8] = joints[0].m_rel[0x8] = joints[0].m[0x8];
    joints[0].m_abs[0x9] = joints[0].m_rel[0x9] = joints[0].m[0x9];
    joints[0].m_abs[0xA] = joints[0].m_rel[0xA] = joints[0].m[0xA];
    joints[0].m_abs[0xB] = joints[0].m_rel[0xB] = joints[0].m[0xB];
    joints[0].m_abs[0xC] = joints[0].m_rel[0xC] = joints[0].m[0xC];
    joints[0].m_abs[0xD] = joints[0].m_rel[0xD] = joints[0].m[0xD];
    joints[0].m_abs[0xE] = joints[0].m_rel[0xE] = joints[0].m[0xE];
    joints[0].m_abs[0xF] = joints[0].m_rel[0xF] = joints[0].m[0xF];
   }

 //
 // READ ANIMATIONS
 //

 // number of seconds per frame
 const real32 SECONDS_PER_FRAME = 1.0f/30.0f;

 // read number of animations
 uint32 n_anim = 0;
 code = ASCIIReadUint32(linelist, &n_anim);
 if(Fail(code)) return code;

 // read animations
 if(n_anim) animations.resize(n_anim);
 for(size_t i = 0; i < n_anim; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return code;

     // name must be valid
     animations[i].name = ConvertUTF8ToUTF16(buffer);
     if(!animations[i].name.length()) return EC_MODEL_ANIMATION_NAME;

     // set loop to default value of true
     animations[i].loop = true;

     // read number of keyframed bones
     uint32 n_keyframedbones = 0;
     code = ASCIIReadUint32(linelist, &n_keyframedbones);
     if(Fail(code)) return code;
     if(!n_keyframedbones) return EC_MODEL_KEYFRAMED_BONES;

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
         if(Fail(code)) return code;

         // name must be valid
         animations[i].bonelist[j].bone = ConvertUTF8ToUTF16(buffer);
         if(!animations[i].bonelist[j].bone.length()) return EC_MODEL_BONENAME;

         // lookup bone index
         auto iter = jointmap.find(animations[i].bonelist[j].bone);
         if(iter == jointmap.end()) return EC_MODEL_BONE_LOOKUP;
         animations[i].bonelist[j].bone_index = iter->second;

         // read number of keyframes
         uint32 n_keyframes = 0;
         code = ASCIIReadUint32(linelist, &n_keyframes);
         if(Fail(code)) return code;
         if(!n_keyframes) return EC_MODEL_KEYFRAMES;

         // read keyframes
         animations[i].bonelist[j].keyframes.resize(n_keyframes);
         for(size_t k = 0; k < n_keyframes; k++)
            {
             // read frame number
             code = ASCIIReadUint32(linelist, &animations[i].bonelist[j].keyframes[k].frame);
             if(Fail(code)) return code;

             // save frame number
             animations[i].keyset.insert(animations[i].bonelist[j].keyframes[k].frame);

             // read translation
             code = ASCIIReadVector3(linelist, &animations[i].bonelist[j].keyframes[k].translation[0]);
             if(Fail(code)) return code;

             // read rotation
             code = ASCIIReadVector4(linelist, &animations[i].bonelist[j].keyframes[k].quaternion[0]);
             if(Fail(code)) return code;

             // read scale
             code = ASCIIReadVector3(linelist, &animations[i].bonelist[j].keyframes[k].scale[0]);
             if(Fail(code)) return code;
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
     animations[i].duration = SECONDS_PER_FRAME*(animations[i].maxframe + 1);
    }

 // construct animation data
 code = ConstructAnimationData();
 if(Fail(code)) return code;

 //
 // READ MESHES
 //

 // read number of meshes
 uint32 n_mesh = 0;
 code = ASCIIReadUint32(linelist, &n_mesh);
 if(Fail(code)) return code;
 if(!n_mesh) return EC_MODEL_MESH;

 // read mesh list
 meshes.resize(n_mesh);
 for(uint32 i = 0; i < n_mesh; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return code;

     // name must be valid
     meshes[i].name = ConvertUTF8ToUTF16(buffer);
     if(!meshes[i].name.length()) return EC_MODEL_MESHNAME;

     // read number of vertices
     meshes[i].n_verts = 0;
     code = ASCIIReadUint32(linelist, &meshes[i].n_verts);
     if(Fail(code)) return code;
     if(!meshes[i].n_verts) return EC_MODEL_VERTICES;

     // read number of UV channels
     uint32 n_uvs = 0;
     code = ASCIIReadUint32(linelist, &n_uvs);
     if(Fail(code)) return code;
     if(n_uvs > 2) return EC_MODEL_UV_CHANNELS;

     // read number of color channels
     uint32 n_colors = 0;
     code = ASCIIReadUint32(linelist, &n_colors);
     if(Fail(code)) return code;
     if(n_colors > 2) return EC_MODEL_COLOR_CHANNELS;

     // read number of textures
     uint32 n_textures = 0;
     code = ASCIIReadUint32(linelist, &n_textures);
     if(Fail(code)) return code;
     if(n_textures > 8) return EC_MODEL_TEXTURES;

     // read textures
     if(n_textures) meshes[i].textures.resize(n_textures);
     for(uint32 j = 0; j < n_textures; j++)
        {
         // read semantic
         char buffer[1024];
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return code;

         // semantic must be valid
         meshes[i].textures[j].semantic = ConvertUTF8ToUTF16(buffer);
         if(!meshes[i].textures[j].semantic.length()) return EC_MODEL_TEXTURE_SEMANTIC;

         // read channel used
         uint32 n_use = 0;
         code = ASCIIReadUint32(linelist, &n_use);
         if(Fail(code)) return code;
         if(!(n_use < n_uvs)) return EC_MODEL_TEXTURE_CHANNEL;
         meshes[i].textures[j].channel = n_use;

         // read filename
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return code;

         // filename must be valid
         meshes[i].textures[j].filename = ConvertUTF8ToUTF16(buffer);
         if(!meshes[i].textures[j].filename.length()) return EC_MODEL_TEXTURE_FILENAME;
        }

     // create mesh data
     meshes[i].position.reset(new std::array<real32, 3>[meshes[i].n_verts]);
     meshes[i].normal.reset(new std::array<real32, 3>[meshes[i].n_verts]);
     meshes[i].uvs[0].reset(new std::array<real32, 2>[meshes[i].n_verts]);
     meshes[i].uvs[1].reset(new std::array<real32, 2>[meshes[i].n_verts]);
     if(joints.size()) {
        meshes[i].bi.reset(new std::array<uint16, 4>[meshes[i].n_verts]);
        meshes[i].bw.reset(new std::array<real32, 4>[meshes[i].n_verts]);
       }
     meshes[i].colors[0].reset(new std::array<real32, 3>[meshes[i].n_verts]);
     meshes[i].colors[1].reset(new std::array<real32, 3>[meshes[i].n_verts]);

     // read mesh data
     for(uint32 j = 0; j < meshes[i].n_verts; j++)
        {
         // read position
         code = ASCIIReadVector3(linelist, &meshes[i].position[j][0], false);
         if(Fail(code)) return code;

         // read normal
         code = ASCIIReadVector3(linelist, &meshes[i].normal[j][0], false);
         if(Fail(code)) return code;

         // read UVs
         meshes[i].uvs[0][j][0] = meshes[i].uvs[0][j][1] = 0.0f;
         meshes[i].uvs[1][j][0] = meshes[i].uvs[1][j][1] = 0.0f;
         for(uint32 k = 0; k < n_uvs; k++) {
             code = ASCIIReadVector2(linelist, &meshes[i].uvs[k][j][0], false);
             if(Fail(code)) return code;
             meshes[i].uvs[k][j][1] = 1.0f - meshes[i].uvs[k][j][1]; // TODO: have Blender do this instead
            }

         // initialize blendindices and blendweights just in case we only have one auto-generated bone
         meshes[i].bi[j][0] = 0;
         meshes[i].bi[j][1] = 0;
         meshes[i].bi[j][2] = 0;
         meshes[i].bi[j][3] = 0;
         meshes[i].bw[j][0] = 1.0f;
         meshes[i].bw[j][1] = 0.0f;
         meshes[i].bw[j][2] = 0.0f;
         meshes[i].bw[j][3] = 0.0f;

         // read blendindices and blendweights
         // only read if actual has a skeleton defined, if autogenerated, there are no weights
         if(has_weights) {
            code = ASCIIReadVector4(linelist, &meshes[i].bi[j][0], false); // set to repeat?
            if(Fail(code)) return code;
            code = ASCIIReadVector4(linelist, &meshes[i].bw[j][0], false); // set to repeat?
            if(Fail(code)) return code;
           }

         // read colors
         meshes[i].colors[0][j][0] = meshes[i].colors[0][j][1] = meshes[i].colors[0][j][2] = 0.5f;
         meshes[i].colors[1][j][0] = meshes[i].colors[1][j][1] = meshes[i].colors[1][j][2] = 0.0f;
         for(uint32 k = 0; k < n_colors; k++) {
             code = ASCIIReadVector3(linelist, &meshes[i].colors[k][j][0], false);
             if(Fail(code)) return code;
            }
        }

     // read number of faces
     code = ASCIIReadUint32(linelist, &meshes[i].n_faces);
     if(Fail(code)) return code;

     // allocate faces
     if(meshes[i].n_faces)
       {
        // allocate indices
        uint32 n_indices = meshes[i].n_faces*3;
        meshes[i].facelist.reset(new uint32[n_indices]);

        // read index buffer
        size_t curr = 0;
        for(size_t j = 0; j < meshes[i].n_faces; j++) {
            code = ASCIIReadVector3(linelist, &meshes[i].facelist[curr], false);
            if(Fail(code)) return code;
            curr += 3;
           }
       }     
    }

 //
 // CREATE BUFFERS
 //

 // prepare joint axes buffer
 struct JAXISBUFFER {
  real32 m[16];
  real32 scale[4];
 };

 // create Direct3D buffer (move this to gfx.h)
 D3D11_BUFFER_DESC desc;
 ZeroMemory(&desc, sizeof(desc));
 desc.Usage = D3D11_USAGE_DYNAMIC;
 desc.ByteWidth = n_jnts*sizeof(JAXISBUFFER);
 desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
 desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

 // create buffer
 HRESULT result = GetD3DDevice()->CreateBuffer(&desc, NULL, &ja_buffer);
 if(FAILED(result)) return EC_D3D_CREATE_BUFFER;

 D3D11_MAPPED_SUBRESOURCE msr;
 ZeroMemory(&msr, sizeof(msr));
 if(FAILED(GetD3DDeviceContext()->Map(ja_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) return EC_D3D_MAP_RESOURCE;
 JAXISBUFFER* data = reinterpret_cast<JAXISBUFFER*>(msr.pData);
 for(size_t i = 0; i < n_jnts; i++) {
     data[i].m[0x0] = joints[i].m_abs[0x0];
     data[i].m[0x1] = joints[i].m_abs[0x1];
     data[i].m[0x2] = joints[i].m_abs[0x2];
     data[i].m[0x3] = joints[i].m_abs[0x3];
     data[i].m[0x4] = joints[i].m_abs[0x4];
     data[i].m[0x5] = joints[i].m_abs[0x5];
     data[i].m[0x6] = joints[i].m_abs[0x6];
     data[i].m[0x7] = joints[i].m_abs[0x7];
     data[i].m[0x8] = joints[i].m_abs[0x8];
     data[i].m[0x9] = joints[i].m_abs[0x9];
     data[i].m[0xA] = joints[i].m_abs[0xA];
     data[i].m[0xB] = joints[i].m_abs[0xB];
     data[i].m[0xC] = 0.0f;
     data[i].m[0xD] = 0.0f;
     data[i].m[0xE] = 0.0f;
     data[i].m[0xF] = 1.0f;
     data[i].scale[0] = 1.0f;
     data[i].scale[1] = 1.0f;
     data[i].scale[2] = 1.0f;
     data[i].scale[3] = 1.0f;
    }
 GetD3DDeviceContext()->Unmap(ja_buffer, 0);

 // 
 // CREATE MESH BUFFERS
 //

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

 // initialize mesh buffers
 buffers.reset(new MeshUTFDirect3D[meshes.size()]);
 for(size_t i = 0; i < meshes.size(); i++) {
     buffers[i].vbuffer = nullptr;
     buffers[i].ibuffer = nullptr;
    }

 // create mesh buffers
 for(size_t i = 0; i < meshes.size(); i++)
    {
     std::unique_ptr<MESHBUFFER[]> data(new MESHBUFFER[meshes[i].n_verts]);
     for(size_t j = 0; j < meshes[i].n_verts; j++) {
         // position
         data[j].position[0] = meshes[i].position[j][0];
         data[j].position[1] = meshes[i].position[j][1];
         data[j].position[2] = meshes[i].position[j][2];
         data[j].position[3] = 1.0f;
         // normal
         data[j].normal[0] = meshes[i].normal[j][0];
         data[j].normal[1] = meshes[i].normal[j][1];
         data[j].normal[2] = meshes[i].normal[j][2];
         data[j].normal[3] = 1.0f;
         // uvs
         data[j].uv1[0] = meshes[i].uvs[0][j][0];
         data[j].uv1[1] = meshes[i].uvs[0][j][1];
         data[j].uv2[0] = meshes[i].uvs[1][j][0];
         data[j].uv2[1] = meshes[i].uvs[1][j][1];
         // blend indices
         data[j].bi[0] = meshes[i].bi[j][0];
         data[j].bi[1] = meshes[i].bi[j][1];
         data[j].bi[2] = meshes[i].bi[j][2];
         data[j].bi[3] = meshes[i].bi[j][3];
         // blend weights
         data[j].bw[0] = meshes[i].bw[j][0];
         data[j].bw[1] = meshes[i].bw[j][1];
         data[j].bw[2] = meshes[i].bw[j][2];
         data[j].bw[3] = meshes[i].bw[j][3];
         // colors
         data[j].color1[0] = meshes[i].colors[0][j][0];
         data[j].color1[1] = meshes[i].colors[0][j][1];
         data[j].color1[2] = meshes[i].colors[0][j][2];
         data[j].color1[3] = 1.0f;
         data[j].color2[0] = meshes[i].colors[1][j][0];
         data[j].color2[1] = meshes[i].colors[1][j][1];
         data[j].color2[2] = meshes[i].colors[1][j][2];
         data[j].color2[3] = 1.0f;
        }

     // create vertex buffer
     ID3D11Buffer* vb = nullptr;
     code = CreateVertexBuffer((LPVOID)data.get(), meshes[i].n_verts, sizeof(MESHBUFFER), &vb);
     if(Fail(code)) return code;

     // create index buffer
     ID3D11Buffer* ib = nullptr;
     code = CreateIndexBuffer((LPVOID)meshes[i].facelist.get(), 3*meshes[i].n_faces, sizeof(uint32), &ib);
     if(Fail(code)) return code;

     // assign buffers
     buffers[i].vbuffer = vb;
     buffers[i].ibuffer = ib;
    }

 // create shader resource views (shared textures)
 for(size_t i = 0; i < meshes.size(); i++)
    {
     // allocate space for resource views
     size_t n_tex = meshes[i].textures.size();
     if(n_tex) {
        buffers[i].rvlist.reset(new ID3D11ShaderResourceView*[n_tex]);
        for(size_t j = 0; j < n_tex; j++) buffers[i].rvlist[j] = nullptr;
       }

     // create resource views
     for(size_t j = 0; j < n_tex; j++) {
         ID3D11ShaderResourceView* resource = NULL;
         auto code = LoadTexture(meshes[i].textures[j].filename.c_str(), &resource);         
         if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
         buffers[i].rvlist[j] = resource;
        }
    }

 return EC_SUCCESS;
}

void MeshUTF::FreeModel(void)
{
 if(ja_buffer) ja_buffer->Release();
 ja_buffer = nullptr;

 // delete Direct3D shader resource views
 for(size_t i = 0; i < meshes.size(); i++) {
     size_t n_tex = meshes[i].textures.size();
     for(size_t j = 0; j < n_tex; j++) {
         FreeTexture(meshes[i].textures[j].filename.c_str());
         buffers[i].rvlist[j] = nullptr; // don't call Release(), texture manager will do it
        }
    }

 // delete Direct3D buffers
 for(size_t i = 0; i < meshes.size(); i++) {
     if(buffers[i].vbuffer) buffers[i].vbuffer->Release(); buffers[i].vbuffer = nullptr;
     if(buffers[i].ibuffer) buffers[i].ibuffer->Release(); buffers[i].ibuffer = nullptr;
    }

 animations.clear();
 meshes.clear();
 joints.clear();
}

ErrorCode MeshUTF::RenderSkeleton(void)
{
 if(ja_buffer) RenderAxes(ja_buffer, (UINT)joints.size());
 return EC_SUCCESS;
}

ErrorCode MeshUTF::RenderMeshList(void)
{
 return EC_SUCCESS;
}

ErrorCode MeshUTF::RenderModel(void)
{
 SetVertexShaderPerModelBuffer(GetIdentityMatrix());
 RenderSkeleton();
 return EC_SUCCESS;
}

MeshUTFInstance::MeshUTFInstance(const MeshUTF& ptr)
{
 // initialize animation data
 mesh = &ptr;
 time = 0.0f;
 anim = 0xFFFFFFFFul;

 // initialize constant data
 mv.load_identity();
 jm.reset(new matrix4D[mesh->joints.size()]);
 for(size_t bi = 0; bi < mesh->joints.size(); bi++) jm[bi].load_identity();

 // initialize buffers
 permodel = nullptr;
 perframe = nullptr;
}

MeshUTFInstance::~MeshUTFInstance()
{
 FreeInstance();
}

ErrorCode MeshUTFInstance::InitInstance(void)
{
 // initialize model matrix
 permodel = nullptr;
 ErrorCode code = CreateDynamicMatrixConstBuffer(&permodel);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
 code = UpdateDynamicMatrixConstBuffer(permodel, DirectX::XMMatrixIdentity());
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // initialize skinning matrices
 perframe = nullptr;
 if(mesh->joints.size()) {
    UINT size = (UINT)(mesh->joints.size()*sizeof(matrix4D));
    code = CreateDynamicConstBuffer(&perframe, size, jm.get());
    if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
   }
 // initialize skinning matrices (no bones, no big deal)
 else {
    code = CreateDynamicMatrixConstBuffer(&perframe);
    if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
    code = UpdateDynamicMatrixConstBuffer(perframe, DirectX::XMMatrixIdentity());
    if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
   }

 return EC_SUCCESS;
}

void MeshUTFInstance::FreeInstance(void)
{
 if(permodel) permodel->Release(); permodel = nullptr;
 if(perframe) perframe->Release(); perframe = nullptr;
}

ErrorCode MeshUTFInstance::SetAnimation(uint32 index)
{
 // stop animating, render in bind pose
 if(index == 0xFFFFFFFFul) {
    anim = 0xFFFFFFFFul;
    return ResetAnimation();
   }

 // set new animation
 if(!(index < mesh->animations.size())) return DebugErrorCode(EC_ANIM_INDEX, __LINE__, __FILE__);;
 anim = index;
 return ResetAnimation();
}

ErrorCode MeshUTFInstance::SetTime(real32 value)
{
 // validate
 if(anim == 0xFFFFFFFFul) return EC_SUCCESS; // no animation set
 if(value == time) return EC_SUCCESS; // no need to update 

 // update time, looping animation if necessary
 real32 duration = mesh->animations[anim].duration;
 time = value;
 if(time < 0.0f) {
    if(mesh->animations[anim].loop)
       while(time < 0.0f) time += duration;
    else
       time = 0.0f;
   }
 else if(time > duration) {
    if(mesh->animations[anim].loop)
       while(!(time < duration)) time -= duration;
    else
       time = duration;
   }

 // update model
 return Update();
}

ErrorCode MeshUTFInstance::ResetAnimation(void)
{
 time = 0.0f;
 return Update();
}

static float dt = 0.0f;
static std::ofstream crap("debug.txt");

ErrorCode MeshUTFInstance::Update(void)
{
 // validate
 if(anim == 0xFFFFFFFFul) return EC_SUCCESS; // no animation set

 const auto& animation = mesh->animations[anim];
 const auto& joints = mesh->joints;

 // for each bone that is animated
 for(size_t bi = 0; bi < joints.size(); bi++)
    {
     // using current time, find keyframes [a, b] that time is inbetween
     size_t n_keys = animation.keyset.size();
     for(size_t ki = 0; ki < (n_keys - 1); ki++)
        {
         // find [a, b] such that [a, time, b]
         auto& kf1 = animation.animdata[ki];
         auto& kf2 = animation.animdata[ki + 1];
         if((time >= kf1.delta) && (time <= kf2.delta))
           {
            // compute ratio between kf1 (0.0) and kf2 (1.0)
            real32 ratio = (time - kf1.delta)/(kf2.delta - kf1.delta);

            // interpolate scale
            real32 S[3];
            lerp3D(S, &kf1.slist[bi][0], &kf2.slist[bi][0], ratio);

            // interpolate translation
            real32 T[3];
            lerp3D(T, &kf1.tlist[bi][0], &kf2.tlist[bi][0], ratio);

            // interpolate quaternion
            real32 Q[4];
            qslerp(Q, &kf1.qlist[bi][0], &kf2.qlist[bi][0], ratio);
            if(::dt > 1.0f && ::dt < 2.0f) {
               std::stringstream ss;
               ss << "q = <" << Q[0] << ", " << Q[1] << ", " << Q[2] << ", " << Q[3] << ">, dt = " << dt;
               crap << ss.str().c_str() << std::endl;
              }

            // load scaling matrix
            matrix4D m;
            m.load_scaling(S[0], S[1], S[2]);

            // rotate, then scale
            matrix4D R;
            R.load_quaternion(Q);
            m = m * R;

            // translate
            m[0x3] += T[0];
            m[0x7] += T[1];
            m[0xB] += T[2];

            // set matrix
            jm[bi] = m;
            break;
           }
        }
    }

 // for each bone that is animated
 // these transformation matrices are interpolated in relative space
 for(size_t bi = 1; bi < joints.size(); bi++) {
     uint32 parent = mesh->joints[bi].parent;
     jm[bi] = jm[bi] * jm[parent];
    }
 for(size_t bi = 0; bi < joints.size(); bi++) {
     jm[bi] = mesh->joints[bi].m_inv * jm[bi] * mesh->joints[bi].m_abs;
    }

 for(size_t bi = 0; bi < joints.size(); bi++)
     jm[bi].transpose();

 // copy matrices to Direct3D
 UINT size = (UINT)(mesh->joints.size()*sizeof(matrix4D));
 ErrorCode code = UpdateDynamicConstBuffer(perframe, size, (const void*)jm.get());
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // success
 return EC_SUCCESS;
}

ErrorCode MeshUTFInstance::Update(real32 dt)
{
 ::dt += dt;
 return SetTime(time + dt);
}

ErrorCode MeshUTFInstance::RenderModel(void)
{
 // set per model vertex shader constants
 SetVertexShaderPerModelBuffer(permodel);
 SetVertexShaderPerFrameBuffer(perframe);

 // for each mesh in model
 for(size_t i = 0; i < mesh->meshes.size(); i++)
    {
     // set rasterization state
     ErrorCode code = SetRasterizerState(RS_MODEL);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // set blend state
     code = SetBlendState(BS_DEFAULT);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // set stencil state

     // set input layout
     code = SetInputLayout(IL_P4_N4_T2_T2_I4_W4_C4_C4);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // set vertex shader
     code = SetVertexShader(VS_MODEL);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // set pixel shader
     code = SetPixelShader(PS_MODEL);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // set sampler state
     code = SetSamplerState(SS_WRAP_LINEAR);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // set shader resources
     UINT n_tex = (UINT)mesh->meshes[i].textures.size();
     SetShaderResources(n_tex, mesh->buffers[i].rvlist.get());

     // set buffers and draw
     SetVertexBuffer(mesh->buffers[i].vbuffer, 104, 0);
     SetIndexBuffer(mesh->buffers[i].ibuffer, 0, DXGI_FORMAT_R32_UINT);
     DrawIndexedTriangleList(mesh->meshes[i].n_faces*3);
    }

 return EC_SUCCESS;
}