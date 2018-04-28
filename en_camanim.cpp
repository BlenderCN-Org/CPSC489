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
#include "en_camanim.h"

CameraMarkerList::CameraMarkerList()
{
 // fixed variables
 start = 0xFFFFFFFFul;
 n_markers = 0;

 // timed variables
 curr = 0xFFFFFFFFul;
 next = 0xFFFFFFFFul;
 base = 0.0f;
 time = 0.0f;
}

CameraMarkerList::~CameraMarkerList()
{
}

void CameraMarkerList::SetPlayerFocus(uint32 n)
{
 player = n;
}

uint32 CameraMarkerList::GetPlayerFocus(void)const
{
 return player;
}

void CameraMarkerList::SetMarkers(uint32 n, std::unique_ptr<CameraMarker[]>& data)
{
 // set markers
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

const CameraMarker* CameraMarkerList::GetMarkers(void)const
{
 return markers.get();
}

ErrorCode CameraMarkerList::SetStartMarker(uint32 index)
{
 // set starting index
 if(!(index < n_markers)) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 start = index;

 // set intervals
 curr = index;
 if(!(next < n_markers)) next = index;
 else next = curr + 1;

 // set base time
 base = markers[start].GetTime();

 return EC_SUCCESS;
}

uint32 CameraMarkerList::GetStartMarker(void)const
{
 return start;
}

CameraMarker& CameraMarkerList::operator [](size_t index)
{
 return markers[index];
}

const CameraMarker& CameraMarkerList::operator [](size_t index)const
{
 return markers[index];
}

void CameraMarkerList::Update(real32 dt)
{
 // START: 1
 // BASE:  5
 // TIME:  2 RANGE is [0, 4] -> [5, 9]
 // -------------------------
 // MARK: 0     1     2     3
 // TIME: 0     5     8     9
 // CURR:       x
 // NEXT:             x
 // 

 // nothing to do, SetStartMarker not called
 if(!n_markers) return;
 if(start == 0xFFFFFFFFul) return;
 if(curr == 0xFFFFFFFFul || next == 0xFFFFFFFFul) return;

 // time values
 real32 curr_time = time + dt;
 real32 last_time = markers[n_markers - 1].GetTime();

 // interval times
 real32 AT = markers[curr].GetTime(); // AT = 5.0, curr = 1
 real32 BT = markers[next].GetTime(); // BT = 8.0, next = 2
 real32 MT = base + curr_time;        // MT = 5.0 + 2.0 = 7.0

 // on L-side of interval
 if(MT == AT)
   {
   }
 // on R-side of interval
 else if(MT == BT)
   {
   }
 // inside of interval
 else if(MT > AT && MT < BT)
   {
   }
 // outside of interval
 else
   {
    while(!(MT >= AT && MT < BT)) {

         }
   }

 // update time
 time = curr_time;
}