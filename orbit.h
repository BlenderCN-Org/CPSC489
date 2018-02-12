#ifndef __CPSC489_ORBIT_H
#define __CPSC489_ORBIT_H

ErrorCode InitOrbitBox(void);
void FreeOrbitBox(void);
ErrorCode RenderOrbitBox(void);
ErrorCode UpdateOrbitBox(void);
bool GetOrbitBoxScale(void);
ErrorCode SetOrbitBoxScale(bool state);

#endif