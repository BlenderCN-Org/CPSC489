#include "stdafx.h"
#include "gfx.h"
#include "layouts.h"
#include "blending.h"
#include "rasters.h"
#include "sampler.h"
#include "shaders.h"
#include "texture.h"
#include "axes.h"
#include "meshinst.h"

MeshInstance::MeshInstance(const MeshData* mptr) : mesh(mptr)
{
 // initialize animation data
 time = 0.0f;
 anim = 0xFFFFFFFFul;

 // initialize position/orientation data
 mv.load_identity();

 // initialize joint matrix data
 jm.reset(new matrix4D[mesh->bones.size()]);
 for(size_t bi = 0; bi < mesh->bones.size(); bi++) jm[bi].load_identity();

 // initialize buffers
 permodel = nullptr;
 perframe = nullptr;
}

MeshInstance::MeshInstance(const MeshData* mptr, const real32* P, const real32* Q) : mesh(mptr)
{
 // initialize animation data
 time = 0.0f;
 anim = 0xFFFFFFFFul;

 // initialize position/orientation data
 mv.load_quaternion(Q);
 mv[0x3] = P[0];
 mv[0x7] = P[1];
 mv[0xB] = P[2];

 // initialize skinning matrices
 jm.reset(new matrix4D[mesh->bones.size()]);
 for(size_t bi = 0; bi < mesh->bones.size(); bi++) jm[bi].load_identity();

 // initialize buffers
 permodel = nullptr;
 perframe = nullptr;
}

MeshInstance::~MeshInstance()
{
 FreeInstance();
}

ErrorCode MeshInstance::InitInstance(void)
{
 // create model matrix
 permodel = nullptr;
 ErrorCode code = CreateDynamicMatrixConstBuffer(&permodel);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // set model matrix
 DirectX::XMMATRIX m(&mv[0]);
 DirectX::XMMatrixTranspose(m);
 code = UpdateDynamicMatrixConstBuffer(permodel, m);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // create skinning matrices
 perframe = nullptr;
 if(mesh->bones.size()) {
    UINT size = (UINT)(mesh->bones.size()*sizeof(matrix4D));
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

void MeshInstance::FreeInstance(void)
{
 if(permodel) permodel->Release(); permodel = nullptr;
 if(perframe) perframe->Release(); perframe = nullptr;
}

ErrorCode MeshInstance::SetAnimation(uint32 index, bool repeat)
{
 // stop animating, render in bind pose
 if(index == 0xFFFFFFFFul) {
    anim = 0xFFFFFFFFul;
    loop = false;
    return ResetAnimation();
   }

 // nothing has changed
 if(index == anim) return EC_SUCCESS; 

 // set new animation
 if(!(index < mesh->animations.size())) return DebugErrorCode(EC_ANIM_INDEX, __LINE__, __FILE__);
 anim = index;
 loop = repeat;
 return ResetAnimation();
}

ErrorCode MeshInstance::SetTime(real32 value)
{
 // validate
 if(anim == 0xFFFFFFFFul) return EC_SUCCESS; // no animation set
 if(value == time) return EC_SUCCESS; // no need to update 

 // update time, looping animation if necessary
 real32 duration = mesh->animations[anim].duration;
 time = value;
 if(time < 0.0f) {
    if(loop) // mesh->animations[anim].loop)
       while(time < 0.0f) time += duration;
    else
       time = 0.0f;
   }
 else if(time > duration) {
    if(loop) // mesh->animations[anim].loop)
       while(!(time < duration)) time -= duration;
    else
       time = duration;
   }

 // update model
 return Update();
}

ErrorCode MeshInstance::ResetAnimation(void)
{
 time = 0.0f;
 return Update();
}

ErrorCode MeshInstance::Update(void)
{
 // validate
 if(anim == 0xFFFFFFFFul) return EC_SUCCESS; // no animation set

 const auto& animation = mesh->animations[anim];
 const auto& bones = mesh->bones;

 // for each bone that is animated
 matrix4D m;
 for(size_t bi = 0; bi < bones.size(); bi++)
    {
     size_t n_keys = animation.keyset.size();
     if(time <= animation.animdata[0].delta) {
        auto& kf = animation.animdata[0];
        m.load_scaling(kf.slist[bi][0], kf.slist[bi][1], kf.slist[bi][2]);
        matrix4D R;
        R.load_quaternion(&kf.qlist[bi][0]);
        m = m * R;
        m[0x3] += kf.tlist[bi][0];
        m[0x7] += kf.tlist[bi][1];
        m[0xB] += kf.tlist[bi][2];
        jm[bi] = m * bones[bi].m_rel;
       }
     else if(time >= animation.animdata[n_keys - 1].delta) {
        auto& kf = animation.animdata[n_keys - 1];
        m.load_scaling(kf.slist[bi][0], kf.slist[bi][1], kf.slist[bi][2]);
        matrix4D R;
        R.load_quaternion(&kf.qlist[bi][0]);
        m = m * R;
        m[0x3] += kf.tlist[bi][0];
        m[0x7] += kf.tlist[bi][1];
        m[0xB] += kf.tlist[bi][2];
        jm[bi] = m * bones[bi].m_rel;
       }
     else
       {
        // using current time, find keyframes [a, b] that time is inbetween
        for(size_t ki = 0; ki < (n_keys - 1); ki++)
           {
            // find [a, b] such that [a, time, b]
            auto& kf1 = animation.animdata[ki];
            auto& kf2 = animation.animdata[ki + 1];

            if((time >= kf1.delta) && (time < kf2.delta))
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
               qnormalize(Q);

               // load scaling matrix
               m.load_scaling(S[0], S[1], S[2]);

               // rotate, then scale
               matrix4D R;
               R.load_quaternion(Q);
               m = m * R;

               // translate
               //matrix4D V;
               //V.load_translation(T[0], T[1], T[2]);
               m[0x3] += T[0];
               m[0x7] += T[1];
               m[0xB] += T[2];

               // set matrix
               jm[bi] = m;
               break;
              }
           }
       }
    }

 // for each bone that is animated
 // these transformation matrices are interpolated in relative space
 for(size_t bi = 1; bi < bones.size(); bi++) {
     uint32 parent = bones[bi].parent;
     jm[bi] = jm[bi] * jm[parent];
    }
 for(size_t bi = 0; bi < bones.size(); bi++) {
     jm[bi] = bones[bi].m_rel * jm[bi] * bones[bi].m_inv;
     jm[bi].transpose();
    }

 // copy matrices to Direct3D
 UINT size = (UINT)(bones.size()*sizeof(matrix4D));
 ErrorCode code = UpdateDynamicConstBuffer(perframe, size, (const void*)jm.get());
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // success
 return EC_SUCCESS;
}

ErrorCode MeshInstance::Update(real32 dt)
{
 return SetTime(time + dt);
}

ErrorCode MeshInstance::RenderSkeleton(void)
{
 return EC_SUCCESS;
}

ErrorCode MeshInstance::RenderModel(void)
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

     // set vertex buffer
     auto& graphics = mesh->graphics;
     SetVertexBuffer(graphics.vbuffer[i], 104, 0);

     // render surfaces
     for(size_t j = 0; j < mesh->meshes[i].surfaces.size(); j++)
        {
         // surface data
         auto& surface = mesh->meshes[i].surfaces[j];
         auto& material = mesh->materials[surface.m_index];

         // set shader resources
         UINT n_tex = (UINT)material.textures.size();
         SetShaderResources(n_tex, &mesh->graphics.resources[material.resource]);

         // set index buffer and draw
         UINT face_bytes = 3*sizeof(uint32);
         UINT offset = surface.start*face_bytes;
         SetIndexBuffer(graphics.ibuffer[i], offset, DXGI_FORMAT_R32_UINT);
         DrawIndexedTriangleList(surface.n_faces*3);
        }
    }

 return EC_SUCCESS;
}
