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
 instance = nullptr;
 n_markers = 0;
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
}

const EntityMarker* EntityMarkerList::GetMarkers(void)const
{
 return markers.get();
}

EntityMarker& EntityMarkerList::operator [](size_t index)
{
 return markers[index];
}

const EntityMarker& EntityMarkerList::operator [](size_t index)const
{
 return markers[index];
}