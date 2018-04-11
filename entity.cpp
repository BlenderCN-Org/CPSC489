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
#include "entity.h"

uint32 Entity::baseID = 0;
std::map<uint32, Entity*> Entity::entity_map = std::map<uint32, Entity*>();

Entity::Entity()
{
 // set ID and increment base
 id = baseID;
 baseID++;
 entity_map.insert(std::make_pair(id, this));
}

Entity::~Entity()
{
 entity_map.erase(id);
}

Entity* Entity::GetEntity(uint32 eid)
{
 auto iter = entity_map.find(eid);
 if(iter == entity_map.end()) return nullptr;
 return iter->second;
}