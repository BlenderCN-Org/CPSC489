#include "stdafx.h"
#include "ascii.h"
#include "math.h"
#include "gfx.h"
#include "vector3.h"
#include "matrix4.h"
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

ErrorCode Map::LoadMap(LPCWSTR filename)
{
 // free previous
 FreeMap();

 // parse file
 std::deque<std::string> linelist;
 ErrorCode code = ASCIIParseFile(filename, linelist);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // temporary data
 uint32 n = 0;

 //
 // PHASE 1:
 // READ STATIC MODELS
 //

 // read number of static models
 n = 0;
 code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read static models
 if(n)
   {
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
   }

 //
 // PHASE 2:
 // READ MOVING MODELS
 //

 // read number of moving models
 n = 0;
 code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read moving models
 if(n)
   {
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
   }

 //
 // PHASE 3:
 // READ SOUNDS
 //

 // read number of sounds
 n = 0;
 code = ASCIIReadUint32(linelist, &n);
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

 //
 // PHASE 4:
 // READ STATIC INSTANCES
 //

 // read number of static instances
 n = 0;
 code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read static instances
 if(n)
   {
    // allocate instances
     std::unique_ptr<MeshInstance[]> temp(new MeshInstance[n]);

    // load instances
    for(uint32 i = 0; i < n; i++)
       {
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

        // read quaternion
        real32 Q[4];
        code = ASCIIReadVector4(linelist, &Q[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // add instance
        MeshData* ptr = &static_models[reference];
        code = temp[i].InitInstance(ptr, P, Q);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
       }

    // set instance data
    n_static_instances = n;
    static_instances = std::move(temp);
   }

 //
 // PHASE 5:
 // READ MOVING INSTANCES
 //

 // read number of moving instances
 n = 0;
 code = ASCIIReadUint32(linelist, &n);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read moving instances
 if(n)
   {
    // allocate instances
     std::unique_ptr<MeshInstance[]> temp(new MeshInstance[n]);

    // load instances
    for(uint32 i = 0; i < n; i++)
       {
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

        // read quaternion
        real32 Q[4];
        code = ASCIIReadVector4(linelist, &Q[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // normalize Q and protect agains invalid values
        if(qnormalize(Q) < 1.0e-7f) {
           Q[0] = 1.0f;
           Q[1] = Q[2] = Q[3] = 0.0f;
          } 

        // add instance
        MeshData* ptr = &moving_models[reference];
        code = temp[i].InitInstance(ptr, P, Q);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
       }

    // set instance data
    n_moving_instances = n;
    moving_instances = std::move(temp);
   }

 //
 // PHASE 6:
 // READ DOOR CONTROLLERS
 //

 // read number of door controllers
 n = 0;
 code = ASCIIReadUint32(linelist, &n);
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
        // read position
        real32 P[3];
        code = ASCIIReadVector3(linelist, &P[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read quaternion
        real32 Q[4];
        code = ASCIIReadVector4(linelist, &Q[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // normalize Q and protect agains invalid values
        if(qnormalize(Q) < 1.0e-7f) {
           Q[0] = 1.0f;
           Q[1] = Q[2] = Q[3] = 0.0f;
          }

        // convert quaternion to matrix
        real32 M[9];
        quaternion_to_matrix3(M, Q);

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

        // set item
        DoorController& item = dcdata[i];
        item.door_index = reference;
        item.box.center[0] = P[0];
        item.box.center[1] = P[1];
        item.box.center[2] = P[2];
        item.box.widths[0] = H[0];
        item.box.widths[1] = H[1];
        item.box.widths[2] = H[2];
        item.box.x[0] = M[0]; item.box.x[1] = M[1]; item.box.x[2] = M[2];
        item.box.y[0] = M[3]; item.box.y[1] = M[4]; item.box.y[2] = M[5];
        item.box.z[0] = M[6]; item.box.z[1] = M[7]; item.box.z[2] = M[8];
        item.anim_start = (anim1 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim1));
        item.anim_enter = (anim2 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim2));
        item.anim_leave = (anim3 == -1 ? 0xFFFFFFFFul : static_cast<uint32>(anim3));
        item.sound_opening = s1;
        item.sound_closing = s2;
        item.inside = false;
        item.close_time = 0.0f;
       }

    // set data
    n_door_controllers = n;
    door_controllers = std::move(dcdata);
   }

 //
 // PHASE ?:
 // READ CAMERA ANIMATIONS
 //

 // read number of entities
 n_cam_anims = 0;
 code = ASCIIReadUint32(linelist, &n_cam_anims);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // read entities
 if(n)
   {
    // allocate animations
    cam_anims.reset(new CameraAnimation[n_cam_anims]);

    // read animations
    for(uint32 i = 0; i < n_cam_anims; i++)
       {
        // load filename
        code = ASCIIReadUTF8String(linelist, cam_anims[i].name);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read position
        code = ASCIIReadVector3(linelist, &cam_anims[i].location[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read orientation
        code = ASCIIReadMatrix4(linelist, &cam_anims[i].orientation[0], false);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read start index
        cam_anims[i].start = 0;
        code = ASCIIReadUint16(linelist, &cam_anims[i].start);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // read number of markers
        cam_anims[i].n_markers = 0;
        code = ASCIIReadUint32(linelist, &cam_anims[i].n_markers);
        if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

        // TODO: camera animation must have at least one marker

        // allocate markers
        cam_anims[i].markers.reset(new CameraMarker[cam_anims[i].n_markers]);

        // read markers
        for(uint32 j = 0; j < cam_anims[i].n_markers; j++)
           {
            auto& marker = cam_anims[i].markers[j];

            // read position
            code = ASCIIReadVector3(linelist, &marker.location[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read orientation
            code = ASCIIReadMatrix4(linelist, &marker.orientation[0], false);
            if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

            // read index
            marker.index = 0;
            code = ASCIIReadUint16(linelist, &marker.index);
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
   }

/*
 // read number of portal cells
 uint32 n_cells = 0;
 code = ASCIIReadUint32(linelist, &n_cells);
 if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

 // read cell graph (named connections)
 for(uint32 i = 0; i < n_cells; i++)
    {
     // load line that holds adjacency list
     char line[1024];
     code = ASCIIReadString(linelist, line);
     if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

     // separate line into paramters (you must have at least one parameter)
     std::deque<std::string> parameters;
     boost::split(parameters, linelist.front(), boost::is_any_of(" "));
     if(!parameters.size()) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

     // TODO:
     // This is where we want to lookup the names of the portals to match them to the names of the
     // static models so we can index them. Once we have the model, we can figure out bounding boxes
     // for the portals.
    }

 // read cell graph (portal connections)
 for(uint32 i = 0; i < n_cells; i++)
    {
     // load cell references
     std::vector<uint32> v;
     code = ASCIIReadArray(linelist, v);
     if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

     // bidirectional portal
     // room_001 room_002 room_004
     // room_004 room_001 room_003
     // 0 1 # room_001 -> room_004 using cell[1]
     // 1 3 # room_004 -> room_001 using cell[1] (give a warning if they don't match)

     // unidirectional portal
     // room_001 room_002 room_004
     // room_004 room_003
     // 0 1 # room_001 -> room_004 using cell[1]
     // 3   # room_004 -> room_003 using cell[3]

     // cell with no connections
     // room_001
     // room_002
     // -1 means no cell
     // -1 means no cell
    }
*/

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
 // free entities
 if(n_door_controllers) door_controllers.reset();
 if(n_cam_anims) cam_anims.reset();
 n_door_controllers = 0;
 n_cam_anims = 0;

 // free instances
 for(uint32 i = 0; i < n_static_instances; i++) static_instances[i].FreeInstance();
 for(uint32 i = 0; i < n_moving_instances; i++) moving_instances[i].FreeInstance();
 static_instances.reset();
 moving_instances.reset();
 n_static_instances = 0;
 n_moving_instances = 0;

 // free sounds
 if(n_sounds) {
    for(uint32 i = 0; i < n_sounds; i++) {
        ErrorCode code = FreeVoice(soundlist[i].c_str());
        if(Fail(code)) DebugErrorCode(code, __LINE__, __FILE__);
       }
    soundlist.reset();
    sounds.reset();
    n_sounds = 0;
   }
 sound_start = 0xFFFFFFFFul;

 // free models
 for(uint32 i = 0; i < n_static; i++) static_models[i].Free();
 for(uint32 i = 0; i < n_moving; i++) moving_models[i].Free();
 static_models.reset();
 moving_models.reset();
 n_static = 0;
 n_moving = 0;
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
           if(dc.sound_opening != 0xFFFFFFFFul) PlayVoice(sounds[dc.sound_opening], false);
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
           if(dc.sound_closing != 0xFFFFFFFFul) PlayVoice(sounds[dc.sound_closing], false);
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