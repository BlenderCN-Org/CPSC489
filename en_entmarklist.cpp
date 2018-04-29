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
#include "map.h"
#include "en_entmarklist.h"

EntityMarkerList::EntityMarkerList()
{
 // list variables
 instance = nullptr;
 start = 0xFFFFFFFFul;
 n_markers = 0;

 // animation variables
 curr = 0xFFFFFFFFul;
 next = 0xFFFFFFFFul;
 base = 0.0f;
 time = 0.0f;
}

EntityMarkerList::~EntityMarkerList()
{
}

ErrorCode EntityMarkerList::SetModelInstance(const STDSTRINGW& name)
{
 Map* map = GetMap();
 if(!map) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 MeshInstance* ptr = map->GetDynamicMeshInstance(name);
 if(ptr == nullptr) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 instance = ptr;
 return EC_SUCCESS;
}

ErrorCode EntityMarkerList::SetModelInstance(uint32 model)
{
 Map* map = GetMap();
 if(!map) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 MeshInstance* ptr = map->GetDynamicMeshInstance(model);
 if(ptr == nullptr) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 instance = ptr;
 return EC_SUCCESS;
}

const MeshInstance* EntityMarkerList::GetModelInstance(void)const
{
 return instance;
}

void EntityMarkerList::SetMarkers(uint32 n, std::unique_ptr<EntityMarker[]>& data)
{
 n_markers = n;
 markers = std::move(data);

 // TODO: think about whether or not this should return an error
 // make sure time doesn't go backwards
 real32 prev_time = markers[0].GetTime();
 for(uint32 i = 1; i < n_markers; i++) {
     real32 curr_time = markers[i].GetTime();
     if(!(prev_time < curr_time)) {
        markers[i].SetTime(prev_time);
        prev_time = curr_time;
       }
    }
}

const EntityMarker* EntityMarkerList::GetMarkers(void)const
{
 return markers.get();
}

ErrorCode EntityMarkerList::SetStartMarker(uint32 index)
{
 // set starting index
 if(!(index < n_markers)) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 start = index;

 // set intervals
 curr = index;
 next = index + 1;

 // set base time
 base = markers[start].GetTime();
 time = base;

 return EC_SUCCESS;
}

uint32 EntityMarkerList::GetStartMarker(void)const
{
 return start;
}

const real32* EntityMarkerList::GetEntityPosition(void)const
{
 return &P[0];
}

const real32* EntityMarkerList::GetEntityEulerXYZ(void)const
{
 return &E[0];
}

void EntityMarkerList::Update(real32 dt)
{
 // nothing to do
 if(!n_markers) return;
 if(curr == 0xFFFFFFFFul || next == 0xFFFFFFFFul) return;

 // time values
 real32 curr_time = time + dt;
 real32 last_time = markers[n_markers - 1].GetTime();
 if(curr_time > last_time) curr_time = last_time;

 // interval times
 real32 AT = markers[curr].GetTime();
 real32 BT = markers[next].GetTime();
 real32 MT = curr_time;

 // shift intervals (if necessary)
 while(!(MT >= AT && MT < BT)) {
       if(next == n_markers - 1) break;
       curr++;
       next++;
       AT = markers[curr].GetTime();
       BT = markers[next].GetTime();
      }

 // on L-side of interval or not interpolating
 if(MT == AT || !markers[curr].GetInterpolateTimeFlag())
   {
    auto P = markers[curr].GetLocation();
    this->P.reset(P);
    auto E = markers[curr].GetEulerAngle();
    this->E[0] = E[0];
    this->E[1] = E[1];
    this->E[2] = E[2];
   }
 // on R-side of interval
 else if(MT == BT)
   {
    const real32* P = markers[next].GetLocation();
    this->P.reset(P);
    const real32* E = markers[next].GetEulerAngle();
    this->E[0] = E[0];
    this->E[1] = E[1];
    this->E[2] = E[2];
   }
 // inside of interval
 else
   {
    // compute ratio between AT and BT
    real32 denom = (BT - AT);
    real32 ratio = (denom ? (MT - AT)/denom : 0.0f);

    // interpolate location
    const real32* P1 = markers[curr].GetLocation();
    const real32* P2 = markers[next].GetLocation();
    lerp3D(&this->P[0], P1, P2, ratio);

    // interpolate euler angle
    const real32* E1 = markers[curr].GetEulerAngle();
    const real32* E2 = markers[next].GetEulerAngle();
    lerp3D(&this->E[0], E1, E2, ratio);
   }

 // convert euler angles to matrix
 real32 M[16];
 eulerXYZ_to_matrix4(&M[0], &this->E[0]);

 // set instance state
 instance->SetMatrix(&this->P[0], &M[0]);
 // instance->SetAnimation();

 // update time
 time = curr_time;
}

EntityMarker& EntityMarkerList::operator [](size_t index)
{
 return markers[index];
}

const EntityMarker& EntityMarkerList::operator [](size_t index)const
{
 return markers[index];
}