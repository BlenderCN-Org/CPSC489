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
#include "en_entmark.h"

EntityMarker::EntityMarker()
{
 euler[0] = euler[1] = euler[2] = 0.0f;
 speed = 1.0f;
 interpolate_speed = true;
 anim = 0xFFFFFFFFul;
 anim_loop = false;
 sound = 0xFFFFFFFFul;
 sound_loop = false;
}

EntityMarker::~EntityMarker()
{
}

void EntityMarker::SetEulerAngle(const real32* angle)
{
 euler[0] = angle[0];
 euler[1] = angle[1];
 euler[2] = angle[2];
}

const real32* EntityMarker::GetEulerAngle(void)const
{
 return &euler[0];
}

void EntityMarker::SetSpeed(real32 value)
{
 speed = value;
}

real32 EntityMarker::GetSpeed(void)const
{
 return speed;
}

void EntityMarker::SetInterpolateSpeedFlag(bool flag)
{
 interpolate_speed = flag;
}

bool EntityMarker::GetInterpolateSpeedFlag(void)const
{
 return interpolate_speed;
}

ErrorCode EntityMarker::SetAnimation(uint32 index)
{
 anim = index;
 return EC_SUCCESS;
}

uint32 EntityMarker::GetAnimation(void)const
{
 return anim;
}

void EntityMarker::SetAnimationLoopFlag(bool flag)
{
 anim_loop = flag;
}

bool EntityMarker::GetAnimationLoopFlag(void)const
{
 return anim_loop;
}

ErrorCode EntityMarker::SetSound(uint32 index)
{
 Map* map = GetMap();
 if(!map->GetSoundData(index)) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 sound = index;
 return EC_SUCCESS;
}

uint32 EntityMarker::GetSound(void)const
{
 return sound;
}

void EntityMarker::SetSoundLoopFlag(bool flag)
{
 sound_loop = flag;
}

bool EntityMarker::GetSoundLoopFlag(void)const
{
 return sound_loop;
}