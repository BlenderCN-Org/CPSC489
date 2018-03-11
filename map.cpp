#include "stdafx.h"
#include "errors.h"
#include "ascii.h"
#include "math.h"
#include "vector3.h"
#include "matrix4.h"
#include "model.h"
#include "portal.h"
#include "map.h"

static std::vector<std::shared_ptr<MeshUTF>> m_static; // static models
static std::vector<std::shared_ptr<MeshUTF>> m_dynamic; // dynamic models
static std::vector<std::shared_ptr<MeshUTFInstance>> instances;

// portal variables
static std::vector<std::vector<uint32>> cell_graph;

ErrorCode LoadMap(LPCWSTR name)
{
 // parse file
 std::deque<std::string> linelist;
 ErrorCode code = ASCIIParseFile(name, linelist);
 if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

 // read number of static models
 uint32 n_models = 0;
 code = ASCIIReadUint32(linelist, &n_models);
 if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

 // load static models
 for(uint32 i = 0; i < n_models; i++)
    {
     // load room name
     STDSTRINGW name;
     code = ASCIIReadUTF8String(linelist, name);
     if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

     // load room
     std::shared_ptr<MeshUTF> model(new MeshUTF);
     code = model->LoadModel(name.c_str());
     if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

     // add static model
     m_static.push_back(model);
    }

 // read number of dynamic models
 uint32 n_dynamics = 0;
 code = ASCIIReadUint32(linelist, &n_dynamics);
 if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

 // load dynamic models
 // dyanmic models are things that move, like doors, pickup items, keys, etc.
 for(uint32 i = 0; i < n_dynamics; i++)
    {
     // load model name
     STDSTRINGW name;
     code = ASCIIReadUTF8String(linelist, name);
     if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

     // load model
     std::shared_ptr<MeshUTF> model(new MeshUTF);
     code = model->LoadModel(name.c_str());
     if(Fail(code)) return DebugErrorCode(EC_LOAD_LEVEL, __LINE__, __FILE__);

     // add dynamic model
     m_dynamic.push_back(model);
    }

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

 return EC_SUCCESS;
}

void FreeMap(void)
{
}

void RenderMap(void)
{
}