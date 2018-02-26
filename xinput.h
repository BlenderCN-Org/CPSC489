#ifndef __CPSC489_XINPUT_H
#define __CPSC489_XINPUT_H

struct XBOX_CONTROLLER_STATE {
 // current joystick states
 real32 JS_L[2];
 real32 JS_R[2];
 real32 JS_L_NORM;
 real32 JS_R_NORM;
 // current button states
 bool DPAD_U;
 bool DPAD_D;
 bool DPAD_L;
 bool DPAD_R;
 bool GPAD_START;
 bool GPAD_BACK;
 bool GPAD_LTHUMB;
 bool GPAD_RTHUMB;
 bool GPAD_LSHOULDER;
 bool GPAD_RSHOULDER;
 bool GPAD_A;
 bool GPAD_B;
 bool GPAD_X;
 bool GPAD_Y;
 bool GPAD_LTRIGGER;
 bool GPAD_RTRIGGER;
 // button repeat counts
 uint32 DPAD_U_REPEAT_COUNT;
 uint32 DPAD_D_REPEAT_COUNT;
 uint32 DPAD_L_REPEAT_COUNT;
 uint32 DPAD_R_REPEAT_COUNT;
 uint32 GPAD_START_REPEAT_COUNT;
 uint32 GPAD_BACK_REPEAT_COUNT;
 uint32 GPAD_LTHUMB_REPEAT_COUNT;
 uint32 GPAD_RTHUMB_REPEAT_COUNT;
 uint32 GPAD_LSHOULDER_REPEAT_COUNT;
 uint32 GPAD_RSHOULDER_REPEAT_COUNT;
 uint32 GPAD_A_REPEAT_COUNT;
 uint32 GPAD_B_REPEAT_COUNT;
 uint32 GPAD_X_REPEAT_COUNT;
 uint32 GPAD_Y_REPEAT_COUNT;
 uint32 GPAD_LTRIGGER_REPEAT_COUNT;
 uint32 GPAD_RTRIGGER_REPEAT_COUNT;
};

ErrorCode InitControllers(void);
void FreeControllers(void);
void UpdateControllers(real32 dt);

// Dead Zone Functions
void SetDeadZonePercent(uint32 index, real32 value);
real32 GetDeadZonePercent(uint32 index);


#endif
