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

// singleton map variable
static Map map;
Map* GetMap(void) { return &map; }

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

 // entities
 n_door_controllers = 0;
 n_cam_anims = 0;
}

Map::~Map()
{
 FreeMap();
}

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
        code = ASCIIReadUTF8String(linelist, temp[i].name);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read position
        code = ASCIIReadVector3(linelist, &temp[i].location[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read orientation
        code = ASCIIReadMatrix4(linelist, &temp[i].orientation[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read start index
        temp[i].start = 0;
        code = ASCIIReadUint32(linelist, &temp[i].start);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read number of markers
        temp[i].n_markers = 0;
        code = ASCIIReadUint32(linelist, &temp[i].n_markers);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // TODO: camera animation must have at least one marker

        // allocate markers
        temp[i].markers.reset(new CameraMarker[temp[i].n_markers]);

        // read markers
        for(uint32 j = 0; j < temp[i].n_markers; j++)
           {
            // read name
            auto& marker = temp[i].markers[j];
            code = ASCIIReadUTF8String(linelist, marker.name);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read location
            code = ASCIIReadVector3(linelist, &marker.location[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read orientation
            code = ASCIIReadMatrix4(linelist, &marker.orientation[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read euler
            code = ASCIIReadVector3(linelist, &marker.euler[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read index
            marker.index = 0;
            code = ASCIIReadUint32(linelist, &marker.index);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read speed
            marker.speed = 1.0f;
            code = ASCIIReadReal32(linelist, &marker.speed);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read interpolate_speed
            marker.interpolate_speed = 0;
            code = ASCIIReadBool(linelist, &marker.interpolate_speed);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read fovy
            marker.fovy = 1.0f;
            code = ASCIIReadReal32(linelist, &marker.fovy);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read interpolate_fovy
            marker.interpolate_fovy = 0;
            code = ASCIIReadBool(linelist, &marker.interpolate_fovy);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
           }
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
        code = ASCIIReadUTF8String(linelist, temp[i].name);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read location
        code = ASCIIReadVector3(linelist, &temp[i].location[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read orientation
        code = ASCIIReadMatrix4(linelist, &temp[i].orientation[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read entity name
        STDSTRINGW ref;
        code = ASCIIReadUTF8String(linelist, ref);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // instance reference must exist
        auto refiter = moving_instance_map.find(ref);
        if(refiter == moving_instance_map.end()) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
        temp[i].instance = refiter->second;

        // read number of markers
        temp[i].n_markers = 0;
        code = ASCIIReadUint32(linelist, &temp[i].n_markers);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // must have at least one marker
        if(temp[i].n_markers == 0)
           return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);

        // allocate markers
        temp[i].markers.reset(new EntityMarker[temp[i].n_markers]);

        // read markers
        std::map<STDSTRINGW, uint32> refmap;
        for(uint32 j = 0; j < temp[i].n_markers; j++)
           {
            // read name
            auto& marker = temp[i].markers[j];
            code = ASCIIReadUTF8String(linelist, marker.name);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read location
            code = ASCIIReadVector3(linelist, &marker.location[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read orientation
            code = ASCIIReadMatrix4(linelist, &marker.orientation[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read euler
            code = ASCIIReadVector3(linelist, &marker.euler[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read index
            marker.index = 0;
            code = ASCIIReadUint32(linelist, &marker.index);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read speed
            marker.speed = 1.0f;
            code = ASCIIReadReal32(linelist, &marker.speed);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read interpolate_speed
            marker.interpolate_speed = 0;
            code = ASCIIReadBool(linelist, &marker.interpolate_speed);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read animation index
            marker.anim = 0xFFFFFFFFul;
            code = ASCIIReadUint32(linelist, &marker.anim);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read animation state
            marker.anim_loop = true;
            code = ASCIIReadBool(linelist, &marker.anim_loop);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read sound index
            marker.sound = 0xFFFFFFFFul;
            code = ASCIIReadUint32(linelist, &marker.sound);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read sound state
            marker.sound_loop = true;
            code = ASCIIReadBool(linelist, &marker.sound_loop);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
           }
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
    std::unique_ptr<DoorController[]> dcdata;
    dcdata.reset(new DoorController[n]);

    // read data
    for(uint32 i = 0; i < n; i++)
       {
        // read instance name
        STDSTRINGW iname;
        code = ASCIIReadUTF8String(linelist, iname);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // map name to index
        auto iter = door_controller_map.insert(std::make_pair(iname, i));
        if(!iter.second) return DebugErrorCode(EC_MAP_INSTANCE_NAME, __LINE__, __FILE__);

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
        DoorController& item = dcdata[i];
        item.door_index = reference;
        item.box.center[0] = P[0];
        item.box.center[1] = P[1];
        item.box.center[2] = P[2];
        item.box.widths[0] = H[0];
        item.box.widths[1] = H[1];
        item.box.widths[2] = H[2];
        item.box.x[0] = M[0x0]; item.box.x[1] = M[0x1]; item.box.x[2] = M[0x2];
        item.box.y[0] = M[0x4]; item.box.y[1] = M[0x5]; item.box.y[2] = M[0x6];
        item.box.z[0] = M[0x8]; item.box.z[1] = M[0x9]; item.box.z[2] = M[0xA];
        item.anim_start = (anim1 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim1));
        item.anim_enter = (anim2 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim2));
        item.anim_leave = (anim3 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim3));
        item.sound_opening = s1;
        item.sound_closing = s2;
        item.inside = false;
        item.close_time = close_time;
        item.stay_open = (open_flag ? true : false);
       }

    // set data
    n_door_controllers = n;
    door_controllers = std::move(dcdata);
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
 door_controllers.reset();
 n_door_controllers = 0;
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
/*
 // read starting sound index
 uint32 u32_temp = 0;
 code = ASCIIReadUint32(linelist, &u32_temp);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // play starting sound
 if(u32_temp < n_sounds) {
    sound_start = u32_temp;
    PlayVoice(sounds[sound_start], true);
   }
*/
 return EC_SUCCESS;
}

void Map::FreeMap(void)
{
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

void Map::RenderMap(real32 dt)
{
 // get camera object and orbit point
 auto camera = GetRenderCamera();
 real32 orbit[3];
 camera->GetOrbitPoint(orbit);

 // INEFFICIENT!!!
 for(uint32 i = 0; i < n_door_controllers; i++)
    {
     // camera is inside door controller
     auto& dc = door_controllers[i];
     if(OBB_intersect(door_controllers[i].box, orbit))
       {
        uint32 door_index = dc.door_index;
        uint32 anim_index = dc.anim_enter;
        moving_instances[door_index].SetAnimation(anim_index, false);
        if(!dc.inside) {
           //if(dc.sound_opening != 0xFFFFFFFFul) PlayVoice(sounds[dc.sound_opening], false);
           dc.inside = true;
          }
       }
     // outside, but last frame we were inside
     else if(dc.inside) {
        dc.close_time = 5.0f;
        dc.inside = false;
       }
     // outside
     else if(dc.close_time) {
        dc.close_time -= dt;
        if(!(dc.close_time > 0.0f)) {
           moving_instances[dc.door_index].SetAnimation(dc.anim_leave, false);
           //if(dc.sound_closing != 0xFFFFFFFFul) PlayVoice(sounds[dc.sound_closing], false);
           dc.close_time = 0.0f;
          }
       }
    }

 // INEFFICIENT!!!
 // RENDER ALL STATIC MODEL INSTANCES
 for(uint32 i = 0; i < n_static_instances; i++)
     static_instances[i].RenderModel();

 // INEFFICIENT!!!
 // RENDER ALL MOVING MODEL INSTANCES
 for(uint32 i = 0; i < n_moving_instances; i++) {
     moving_instances[i].Update(dt);
     moving_instances[i].RenderModel();
    }
}