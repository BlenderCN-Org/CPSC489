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
#include "en_cammark.h"

CameraMarker::CameraMarker()
{
 euler[0] = euler[1] = euler[2] = 0.0f;
 delta = 1.0f;
 interpolate_time = true;
 fovy = 60.0f;
 interpolate_fovy = true;
 wait = 0.0f;
}

CameraMarker::~CameraMarker()
{
}

void CameraMarker::SetEulerAngle(const real32* angle)
{
 euler[0] = angle[0];
 euler[1] = angle[1];
 euler[2] = angle[2];
}

const real32* CameraMarker::GetEulerAngle(void)const
{
 return &euler[0];
}

void CameraMarker::SetTime(real32 value)
{
 delta = value;
}

real32 CameraMarker::GetTime(void)const
{
 return delta;
}

void CameraMarker::SetInterpolateTimeFlag(bool flag)
{
 interpolate_time = flag;
}

bool CameraMarker::GetInterpolateTimeFlag(void)const
{
 return interpolate_time;
}

void CameraMarker::SetFOVY(real32 value)
{
 if(value >= 15.0f && value <= 90.0f) fovy = value;
 else fovy = 60.0f;
}

real32 CameraMarker::GetFOVY(void)const
{
 return fovy;
}

void CameraMarker::SetInterpolateFOVYFlag(bool flag)
{
 interpolate_fovy = flag;
}

bool CameraMarker::GetInterpolateFOVYFlag(void)const
{
 return interpolate_fovy;
}

void CameraMarker::SetWaitTime(real32 value)
{
 wait = value;
}

real32 CameraMarker::GetWaitTime(void)const
{
 return wait;
}