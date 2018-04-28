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
 speed = 1.0f;
 interpolate_speed = true;
 fovy = 60.0f;
 interpolate_fovy = true;
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

void CameraMarker::SetSpeed(real32 value)
{
 speed = value;
}

real32 CameraMarker::GetSpeed(void)const
{
 return speed;
}

void CameraMarker::SetInterpolateSpeedFlag(bool flag)
{
 interpolate_speed = flag;
}

bool CameraMarker::GetInterpolateSpeedFlag(void)const
{
 return interpolate_speed;
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