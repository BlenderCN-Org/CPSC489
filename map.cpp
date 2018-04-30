/*
** CPSC489 Game Development Project
** Copyright (c) 2018  Steven Emory <mobysteve@gmail.com>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "ascii.h"
#include "gfx.h"
#include "camera.h"
#include "collision.h"
#include "portal.h"
#include "map.h"

#include "viewport.h"

#pragma region SPECIAL_MEMBER_FUNCTIONS

Map::Map()
{
 // sounds
 n_sounds = 0;
 sound_start = 0xFFFFFFFFul;

 // models
 n_static = 0;
 n_moving = 0;
 n_static_instances = 0;
 n_moving_instances = 0;

 // data
 dcd.size = 0;
 cmd.size = 0;
 emd.size = 0;
 portals.size = 0;
 cells.size = 0;
}

Map::~Map()
{
 FreeMap();
}

#pragma endregion SPECIAL_MEMBER_FUNCTIONS

#pragma region PRIVATE_LOADING_FUNCTIONS

ErrorCode Map::LoadStaticModels(std::deque<std::string>& linelist)
{
 // read number of static models
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // nothing to do
 if(!n) return EC_SUCCESS;

 // allocate meshdata
 std::unique_ptr<MeshData[]> temp(new MeshData[n]);

 // load models
 for(uint32 i = 0; i < n; i++)
    {
     // load filename
     STDSTRINGW filename;
     code = ASCIIReadUTF8String(linelist, filename);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // load model
     code = temp[i].LoadMeshUTF(filename.c_str());
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
    }

 // set models
 n_static = n;
 static_models = std::move(temp);

 return EC_SUCCESS;
}

ErrorCode Map::LoadDynamicModels(std::deque<std::string>& linelist)
{
 // read number of dynamic models
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // nothing to do
 if(!n) return EC_SUCCESS;

 // allocate meshdata
 std::unique_ptr<MeshData[]> temp(new MeshData[n]);

 // load models
 for(uint32 i = 0; i < n; i++)
    {
     // load filename
     STDSTRINGW filename;
     code = ASCIIReadUTF8String(linelist, filename);
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

     // load model
     code = temp[i].LoadMeshUTF(filename.c_str());
     if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
    }

 // set models
 n_moving = n;
 moving_models = std::move(temp);

 return EC_SUCCESS;
}

ErrorCode Map::LoadSounds(std::deque<std::string>& linelist)
{
 // read number of sounds
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read sounds
 if(n)
   {
    // load filenames
    std::unique_ptr<STDSTRINGW[]> filelist(new STDSTRINGW[n]);
    for(uint32 i = 0; i < n; i++) {
        STDSTRINGW filename;
        code = ASCIIReadUTF8String(linelist, filename);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
        filelist[i] = filename;
       }

    // load sound files
    std::unique_ptr<SoundData*[]> sdlist(new SoundData*[n]);
    for(uint32 i = 0; i < n; i++) {
        SoundData* ptr = nullptr;
        ErrorCode code = LoadVoice(filelist[i].c_str(), &ptr);
        if(Fail(code)) {
           for(uint32 j = 0; j < i; j++) {
               ErrorCode code = FreeVoice(filelist[j].c_str());
               if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
              }
           return DebugErrorCode(code, __LINE__, __FILE__);
          }
        sdlist[i] = ptr;
       }

    // set sound data
    n_sounds = n;
    soundlist = std::move(filelist);
    sounds = std::move(sdlist);
   }

 return EC_SUCCESS;
}

ErrorCode Map::LoadStaticInstances(std::deque<std::string>& linelist)
{
 // read number of static instances
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read static instances
 if(n)
   {
    // allocate instances
     std::unique_ptr<MeshInstance[]> temp(new MeshInstance[n]);

    // load instances
    for(uint32 i = 0; i < n; i++)
       {
        // load instance name
        STDSTRINGW iname;
        code = ASCIIReadUTF8String(linelist, iname);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // map name to index
        auto iter = static_instance_map.insert(std::make_pair(iname, i));
        if(!iter.second) return DebugErrorCode(EC_MAP_INSTANCE_NAME, __LINE__, __FILE__);

        // load model reference
        uint32 reference = 0;
        code = ASCIIReadUint32(linelist, &reference);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // validate reference
        if(!(reference < n_static))
           return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

        // read position
        real32 P[3];
        code = ASCIIReadVector3(linelist, &P[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read orientation
        real32 M[16];
        code = ASCIIReadMatrix4(linelist, &M[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // add instance
        MeshData* ptr = &static_models[reference];
        code = temp[i].InitInstance(ptr, P, M);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
       }

    // set instance data
    n_static_instances = n;
    static_instances = std::move(temp);
   }

 return EC_SUCCESS;
}

ErrorCode Map::LoadDynamicInstances(std::deque<std::string>& linelist)
{
 // read number of moving instances
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read moving instances
 if(n)
   {
    // allocate instances
     std::unique_ptr<MeshInstance[]> temp(new MeshInstance[n]);

    // load instances
    for(uint32 i = 0; i < n; i++)
       {
        // load instance name
        STDSTRINGW iname;
        code = ASCIIReadUTF8String(linelist, iname);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // map name to index
        auto iter = moving_instance_map.insert(std::make_pair(iname, i));
        if(!iter.second) return DebugErrorCode(EC_MAP_INSTANCE_NAME, __LINE__, __FILE__);

        // load model reference
        uint32 reference = 0;
        code = ASCIIReadUint32(linelist, &reference);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // validate reference
        if(!(reference < n_moving))
           return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

        // read position
        real32 P[3];
        code = ASCIIReadVector3(linelist, &P[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read orientation
        real32 M[16];
        code = ASCIIReadMatrix4(linelist, &M[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // add instance
        MeshData* ptr = &moving_models[reference];
        code = temp[i].InitInstance(ptr, P, M);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
       }

    // set instance data
    n_moving_instances = n;
    moving_instances = std::move(temp);
   }

 return EC_SUCCESS;
}

ErrorCode Map::LoadCameraMarkerLists(std::deque<std::string>& linelist)
{
 // read number of lists
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read entities
 if(n)
   {
    // allocate data
    std::unique_ptr<CameraMarkerList[]> temp;
    temp.reset(new CameraMarkerList[n]);

    // read data
    for(uint32 i = 0; i < n; i++)
       {
        // read name
        STDSTRINGW name;
        code = ASCIIReadUTF8String(linelist, name);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read position
        real32 P[3];
        code = ASCIIReadVector3(linelist, &P[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read orientation
        real32 M[16];
        code = ASCIIReadMatrix4(linelist, &M[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read start index
        uint32 start = 0;
        code = ASCIIReadUint32(linelist, &start);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read number of markers
        uint32 n_markers = 0;
        code = ASCIIReadUint32(linelist, &n_markers);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // camera animations must have at least one marker
        if(n_markers < 1) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
        if(!(start < n_markers)) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);

        // allocate markers
        std::unique_ptr<CameraMarker[]> markers;
        markers.reset(new CameraMarker[n_markers]);

        // read markers
        for(uint32 j = 0; j < n_markers; j++)
           {
            // read name
            STDSTRINGW name;
            code = ASCIIReadUTF8String(linelist, name);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read location
            real32 P[3];
            code = ASCIIReadVector3(linelist, &P[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read orientation
            real32 M[16];
            code = ASCIIReadMatrix4(linelist, &M[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read euler
            real32 E[3];
            code = ASCIIReadVector3(linelist, &E[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read index
            uint32 index;
            code = ASCIIReadUint32(linelist, &index);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
            if(!(index < n_markers)) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);

            // read time
            real32 time;
            code = ASCIIReadReal32(linelist, &time);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read interpolate_time
            bool interpolate_time = true;
            code = ASCIIReadBool(linelist, &interpolate_time);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read fovy
            real32 fovy = 1.0f;
            code = ASCIIReadReal32(linelist, &fovy);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read interpolate_fovy
            bool interpolate_fovy = true;
            code = ASCIIReadBool(linelist, &interpolate_fovy);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read stop flag
            bool stop = true;
            code = ASCIIReadBool(linelist, &stop);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read wait time
            real32 wait = 0.0f;
            code = ASCIIReadReal32(linelist, &wait);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // set data
            markers[index].SetMap(this);
            markers[index].SetName(name);
            markers[index].SetLocation(P);
            markers[index].SetOrientation(M);
            markers[index].SetEulerAngle(E);
            markers[index].SetTime(time);
            markers[index].SetInterpolateTimeFlag(interpolate_time);
            markers[index].SetFOVY(fovy);
            markers[index].SetInterpolateFOVYFlag(interpolate_fovy);
            markers[index].SetStopFlag(stop);
            markers[index].SetWaitTime(wait);
           }

        // set data
        temp[i].SetMap(this);
        temp[i].SetName(name);
        temp[i].SetLocation(P);
        temp[i].SetOrientation(M);
        temp[i].SetMarkers(n_markers, markers); // set markers first before setting start marker
        temp[i].SetStartMarker(start);
       }

    // set instance data
    this->cmd.size = n;
    this->cmd.data = std::move(temp);
   }

 return EC_SUCCESS;
}

ErrorCode Map::LoadEntityMarkerLists(std::deque<std::string>& linelist)
{
 // read number of lists
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read lists
 if(n)
   {
    // allocate lists
    std::unique_ptr<EntityMarkerList[]> temp;
    temp.reset(new EntityMarkerList[n]);

    // read lists
    for(uint32 i = 0; i < n; i++)
       {
        // read name
        STDSTRINGW name;
        code = ASCIIReadUTF8String(linelist, name);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read location
        real32 P[3];
        code = ASCIIReadVector3(linelist, &P[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read orientation
        real32 M[16];
        code = ASCIIReadMatrix4(linelist, &M[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read entity name
        STDSTRINGW ref;
        code = ASCIIReadUTF8String(linelist, ref);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // instance reference must exist
        auto refiter = moving_instance_map.find(ref);
        if(refiter == moving_instance_map.end()) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
        uint32 instance = refiter->second;

        // read start index
        uint32 start = 0;
        code = ASCIIReadUint32(linelist, &start);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read number of markers
        uint32 n_markers = 0;
        code = ASCIIReadUint32(linelist, &n_markers);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // must have at least one marker
        if(n_markers == 0)
           return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);

        // allocate markers
        std::unique_ptr<EntityMarker[]> markers;
        markers.reset(new EntityMarker[n_markers]);

        // read markers
        std::map<STDSTRINGW, uint32> refmap;
        for(uint32 j = 0; j < n_markers; j++)
           {
            // read name
            STDSTRINGW name;
            code = ASCIIReadUTF8String(linelist, name);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read location
            real32 P[3];
            code = ASCIIReadVector3(linelist, &P[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read orientation
            real32 M[16];
            code = ASCIIReadMatrix4(linelist, &M[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read euler
            real32 E[3];
            code = ASCIIReadVector3(linelist, &E[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read index
            uint32 index = 0;
            code = ASCIIReadUint32(linelist, &index);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read speed
            real32 time;
            code = ASCIIReadReal32(linelist, &time);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read interpolate_speed
            bool interpolate_time = true;
            code = ASCIIReadBool(linelist, &interpolate_time);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read animation index
            uint32 anim = 0xFFFFFFFFul;
            code = ASCIIReadUint32(linelist, &anim);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read animation state
            bool anim_loop = true;
            code = ASCIIReadBool(linelist, &anim_loop);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read sound index
            uint32 sound = 0xFFFFFFFFul;
            code = ASCIIReadUint32(linelist, &sound);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read sound state
            bool sound_loop = true;
            code = ASCIIReadBool(linelist, &sound_loop);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read stop flag
            bool stop = true;
            code = ASCIIReadBool(linelist, &stop);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read wait time
            real32 wait = 0.0f;
            code = ASCIIReadReal32(linelist, &wait);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // set data
            markers[index].SetName(name);
            markers[index].SetMap(this);
            markers[index].SetLocation(P);
            markers[index].SetOrientation(M);
            markers[index].SetEulerAngle(E);
            markers[index].SetTime(time);
            markers[index].SetInterpolateTimeFlag(interpolate_time);
            markers[index].SetAnimation(anim);
            markers[index].SetAnimationLoopFlag(anim_loop);
            markers[index].SetSound(sound);
            markers[index].SetSoundLoopFlag(sound_loop);
            markers[index].SetStopFlag(stop);
            markers[index].SetWaitTime(wait);
           }

        // set data
        temp[i].SetName(name);
        temp[i].SetMap(this);
        temp[i].SetLocation(P);
        temp[i].SetOrientation(M);
        temp[i].SetModelInstance(ref);
        temp[i].SetMarkers(n_markers, markers); // set markers first before setting start marker
        temp[i].SetStartMarker(start);
       }

    // set data    
    this->emd.size = n;
    this->emd.data = std::move(temp);
   }

 return EC_SUCCESS;
}

ErrorCode Map::LoadDoorControllers(std::deque<std::string>& linelist)
{
 // read number of door controllers
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read door controllers
 if(n)
   {
    // allocate data
    std::unique_ptr<DoorController[]> temp;
    temp.reset(new DoorController[n]);

    // read data
    for(uint32 i = 0; i < n; i++)
       {
        // read instance name
        STDSTRINGW name;
        code = ASCIIReadUTF8String(linelist, name);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read position
        real32 P[3];
        code = ASCIIReadVector3(linelist, &P[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read orientation
        real32 M[16];
        code = ASCIIReadMatrix4(linelist, &M[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read half-widths
        real32 H[3];
        code = ASCIIReadVector3(linelist, &H[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read reference
        uint32 reference = 0;
        code = ASCIIReadUint32(linelist, &reference);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // validate reference
        if(!(reference < n_moving_instances))
           return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

        // read animations
        sint32 anim1 = -1;
        sint32 anim2 = -1;
        sint32 anim3 = -1;
        code = ASCIIReadSint32(linelist, &anim1);
        code = ASCIIReadSint32(linelist, &anim2);
        code = ASCIIReadSint32(linelist, &anim3);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // TODO:
        // WE NEED TO VALIDATE ANIMATION INDICES HERE!!!

        // read sounds
        sint32 sound1 = -1;
        sint32 sound2 = -1;
        code = ASCIIReadSint32(linelist, &sound1);
        code = ASCIIReadSint32(linelist, &sound2);
        uint32 s1 = (sound1 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(sound1));
        uint32 s2 = (sound2 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(sound2));
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
        if(!(s1 < n_sounds)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);
        if(!(s2 < n_sounds)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

        // read close time
        real32 close_time = 5.0f;
        code = ASCIIReadReal32(linelist, &close_time);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read stay open
        uint32 open_flag = 0;
        code = ASCIIReadUint32(linelist, &open_flag);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // set item
        uint32 anims[3] = {
         (anim1 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim1)),
         (anim2 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim2)),
         (anim3 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim3))
        };

        // set door controller properties
        temp[i].SetMap(this);
        temp[i].SetName(name);
        temp[i].SetLocation(P);
        temp[i].SetOrientation(M);
        temp[i].SetOBB(H);
        temp[i].SetDoorIndex(reference);
        temp[i].SetAnimations(anims[0], anims[1], anims[2]);
        temp[i].SetSounds(0xFFFFFFFFul, s1, s2);
        temp[i].SetClosingTime(close_time);
        temp[i].SetActiveFlag(true);
       }

    // set data
    this->dcd.size = n;
    this->dcd.data = std::move(temp);
   }

 return EC_SUCCESS;
}

ErrorCode Map::LoadPortals(std::deque<std::string>& linelist)
{
 // read number of portals
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

 // read portals
 if(n)
   {
    // allocate data
    std::unique_ptr<Portal[]> temp;
    temp.reset(new Portal[n]);

    // read data
    for(uint32 i = 0; i < n; i++)
       {
        // read name
        code = ASCIIReadUTF8String(linelist, temp[i].name);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read number of vertices
        auto code = ASCIIReadUint32(linelist, &temp[i].n);
        if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

        // must be three or four vertices
        if(!(temp[i].n == 3 || temp[i].n == 4))
           return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);

        // allocate vertices
        temp[i].vb.reset(new vector3D[temp[i].n]);
        temp[i].ib.reset(new uint16[temp[i].n]);

        // read vertices
        for(uint32 j = 0; j < temp[i].n; j++) {
            code = ASCIIReadVector3(linelist, &temp[i].vb[j].v[0]);
            if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);
           }

        // read indices
        if(temp[i].n == 3) {
           code = ASCIIReadVector3(linelist, temp[i].ib.get());
           if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);
          }
        else {
           code = ASCIIReadVector4(linelist, temp[i].ib.get());
           if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);
          }
       }

    // set data
    this->portals.size = n;
    this->portals.data = std::move(temp);
   }

 return EC_SUCCESS;
}

ErrorCode Map::LoadCells(std::deque<std::string>& linelist)
{
 // read number of cells
 uint32 n = 0;
 auto code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

 // read cells
 if(n)
   {
    // allocate data
    std::unique_ptr<PortalCell[]> temp;
    temp.reset(new PortalCell[n]);

    // read data
    for(uint32 i = 0; i < n; i++)
       {
       }

    // set data
    this->cells.size = n;
    this->cells.data = std::move(temp);
   }

 return EC_SUCCESS;
}

#pragma endregion PRIVATE_LOADING_FUNCTIONS

#pragma region PRIVATE_UNLOADING_FUNCTIONS

void Map::FreeStaticModels(void)
{
 for(uint32 i = 0; i < n_static; i++) static_models[i].Free();
 static_models.reset();
 n_static = 0;
}

void Map::FreeDynamicModels(void)
{
 for(uint32 i = 0; i < n_moving; i++) moving_models[i].Free();
 moving_models.reset();
 n_moving = 0;
}

void Map::FreeSounds(void)
{
 for(uint32 i = 0; i < n_sounds; i++) {
     ErrorCode code = FreeVoice(soundlist[i].c_str());
     if(Fail(code)) DebugErrorCode(code, __LINE__, __FILE__);
    }
 soundlist.reset();
 sounds.reset();
 n_sounds = 0;
 sound_start = 0xFFFFFFFFul;
}

void Map::FreeStaticInstances(void)
{
 for(uint32 i = 0; i < n_static_instances; i++) static_instances[i].FreeInstance();
 static_instances.reset();
 n_static_instances = 0;
 static_instance_map.clear();
}

void Map::FreeDynamicInstances(void)
{
 for(uint32 i = 0; i < n_moving_instances; i++) moving_instances[i].FreeInstance();
 moving_instances.reset();
 n_moving_instances = 0;
 moving_instance_map.clear();
}

void Map::FreeCameraMarkerLists(void)
{
 cmd.data.reset();
 cmd.size = 0;
}

void Map::FreeEntityMarkerLists(void)
{
 emd.data.reset();
 emd.size = 0;
}

void Map::FreeDoorControllers(void)
{
 dcd.data.reset();
 dcd.size = 0;
}

void Map::FreePortals(void)
{
 portals.data.reset();
 portals.size = 0;
}

void Map::FreeCells(void)
{
 cells.data.reset();
 cells.size = 0;
}

#pragma endregion PRIVATE_UNLOADING_FUNCTIONS

ErrorCode Map::LoadMap(LPCWSTR filename)
{
 // free previous
 FreeMap();

 // parse file
 std::deque<std::string> linelist;
 ErrorCode code = ASCIIParseFile(filename, linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read map title
 code = ASCIIReadUTF8String(linelist, name);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read static models
 code = LoadStaticModels(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read dynamic models
 code = LoadDynamicModels(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read sound list
 code = LoadSounds(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read static instances
 code = LoadStaticInstances(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read dynamic instances
 code = LoadDynamicInstances(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read camera marker lists
 code = LoadCameraMarkerLists(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read entity marker lists
 code = LoadEntityMarkerLists(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read door controllers
 code = LoadDoorControllers(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read portals
 code = LoadPortals(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read cells
 code = LoadCells(linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 //
 // PHASE FINAL:
 // READ STARTING PROPERTIES
 //

 // read starting sound index
 uint32 u32_temp = 0;
 code = ASCIIReadUint32(linelist, &u32_temp);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // play starting sound
 if(u32_temp < n_sounds) {
    sound_start = u32_temp;
    PlayVoice(sounds[sound_start], true);
   }

 return EC_SUCCESS;
}

void Map::FreeMap(void)
{
 sound_start = 0xFFFFFFFFul;

 // free data
 FreeCells();
 FreePortals();
 FreeDoorControllers();
 FreeEntityMarkerLists();
 FreeCameraMarkerLists();
 FreeDynamicInstances();
 FreeStaticInstances();
 FreeSounds();
 FreeDynamicModels();
 FreeStaticModels();

 // free name
 name.clear();
}

void Map::Update(real32 dt)
{
 // INEFFICIENT!!! Poll all door controllers with orbit point
 for(uint32 i = 0; i < dcd.size; i++)
     dcd.data[i].Poll(dt);

 // INEFFICIENT!!!
 // RENDER ALL MOVING MODEL INSTANCES
 for(uint32 i = 0; i < n_moving_instances; i++)
     moving_instances[i].Update(dt);
}

void Map::Render(void)
{
 // INEFFICIENT!!!
 // RENDER ALL STATIC MODEL INSTANCES
 for(uint32 i = 0; i < n_static_instances; i++)
     static_instances[i].RenderModel();

 // INEFFICIENT!!!
 // RENDER ALL MOVING MODEL INSTANCES
 for(uint32 i = 0; i < n_moving_instances; i++)
     moving_instances[i].RenderModel();
}

MeshInstance* Map::GetStaticMeshInstance(const STDSTRINGW& name)const
{
 auto iter = static_instance_map.find(name);
 if(iter == static_instance_map.end()) return nullptr;
 if(!(iter->second < n_static_instances)) return nullptr;
 return &static_instances[iter->second];
}

MeshInstance* Map::GetStaticMeshInstance(uint32 index)const
{
 if(!(index < n_static_instances)) return nullptr;
 return &static_instances[index];
}

MeshInstance* Map::GetDynamicMeshInstance(const STDSTRINGW& name)const
{
 auto iter = moving_instance_map.find(name);
 if(iter == moving_instance_map.end()) return nullptr;
 if(!(iter->second < n_moving_instances)) return nullptr;
 return &moving_instances[iter->second];
}

MeshInstance* Map::GetDynamicMeshInstance(uint32 index)const
{
 if(!(index < n_moving_instances)) return nullptr;
 return &moving_instances[index];
}

SoundData* Map::GetSoundData(uint32 index)const
{
 if(!(index < n_sounds)) return nullptr;
 return sounds[index];
}