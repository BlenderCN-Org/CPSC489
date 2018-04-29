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
 time = 0.0f;
 interpolate_time = true;
 anim = 0xFFFFFFFFul;
 anim_loop = false;
 sound = 0xFFFFFFFFul;
 sound_loop = false;
 wait = 0.0f;
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

void EntityMarker::SetTime(real32 value)
{
 time = value;
}

real32 EntityMarker::GetTime(void)const
{
 return time;
}

void EntityMarker::SetInterpolateTimeFlag(bool flag)
{
 interpolate_time = flag;
}

bool EntityMarker::GetInterpolateTimeFlag(void)const
{
 return interpolate_time;
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
 // no sound to play at this marker
 if(index == 0xFFFFFFFFul) sound = index;
 // set sound
 else {
    Map* map = GetMap();
    if(!map->GetSoundData(index)) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
    sound = index;
   }
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

void EntityMarker::SetWaitTime(real32 value)
{
 wait = value;
}

real32 EntityMarker::GetWaitTime(void)const
{
 return wait;
}