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
 start = 0xFFFFFFFFul;
 n_markers = 0;
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
 n_markers = n;
 markers = std::move(data);
}

const CameraMarker* CameraMarkerList::GetMarkers(void)const
{
 return markers.get();
}

ErrorCode CameraMarkerList::SetStartMarker(uint32 index)
{
 if(!(index < n_markers)) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 start = index;
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