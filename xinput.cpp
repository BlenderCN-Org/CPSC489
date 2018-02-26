#include "stdafx.h"
#include "errors.h"
#include "xinput.h"

struct CONTROLLER_STATE {
 XBOX_CONTROLLER_STATE keys;
 XINPUT_STATE state;
 bool connected;
 bool reserved;
 float dead_zone_percent;
};

static const int MAX_CONTROLLERS = 4;
static CONTROLLER_STATE statelist[MAX_CONTROLLERS];
static const real32 MAX_POLLTIME = 5.0f;
static real32 polltime = 0.0f;

ErrorCode InitControllers(void)
{
 for(int i = 0; i < MAX_CONTROLLERS; i++) {
     statelist[i].connected = false;
     statelist[i].reserved = false;
     statelist[i].dead_zone_percent = 0.24f;
     ZeroMemory(&statelist[i].keys, sizeof(statelist[i].keys));
     ZeroMemory(&statelist[i].state, sizeof(statelist[i].state));
    }
 polltime = 0.0f;
 return EC_SUCCESS;
}

void FreeControllers(void)
{
 polltime = 0.0f;
 for(int i = 0; i < MAX_CONTROLLERS; i++) {
     statelist[i].connected = false;
     statelist[i].reserved = false;
     statelist[i].dead_zone_percent = 0.24f;
     ZeroMemory(&statelist[i].keys, sizeof(statelist[i].keys));
     ZeroMemory(&statelist[i].state, sizeof(statelist[i].state));
    }
}

void QueryControllers(bool queryEmptyUsers)
{
 for(int i = 0; i < MAX_CONTROLLERS; i++) {
     if(statelist[i].connected || queryEmptyUsers) {
        DWORD result = XInputGetState(i, &statelist[i].state);
        if(result == ERROR_SUCCESS) statelist[i].connected = true;
        else statelist[i].connected = false;
       }
    }
}

void UpdateControllers(real32 dt)
{
 // For performance reasons, only query empty slots every five seconds. Calling XInputGetState four
 // times every frame when there are no controllers or only one controller is overkill.
 bool queryEmptyUsers = false;
 polltime += dt;
 if(polltime > MAX_POLLTIME) {
    polltime = 0.0f;
    queryEmptyUsers = true;
   }
 QueryControllers(queryEmptyUsers);

 // When computing the magnitude and direction of the thumbstick, the deadzone
 // is a small portion of the inner circle that prevents movement as the game
 // controller begins to shift from the middle to the outside. It is used to
 // control the sensitivity of your controller.
 // vl = <1.0, 0.0>

 for(int i = 0; i < MAX_CONTROLLERS; i++)
    {
     // skip if not connected
     if(!statelist[i].connected) continue;

     // determine direction and magnitude that L controller is pushed
     float vl[2] = { (float)statelist[i].state.Gamepad.sThumbLX, (float)statelist[i].state.Gamepad.sThumbLY };
     float ml = std::sqrt(vl[0]*vl[0] + vl[1]*vl[1]); // magnitude
     if(ml) {
        vl[0] /= ml; // this is a 2D unit vector
        vl[1] /= ml; // this is a 2D unit vector
       }
     else {
        vl[0] = 0.0f;
        vl[1] = 0.0f;
       }

     // determine direction and magnitude that R controller is pushed
     float vr[2] = { (float)statelist[i].state.Gamepad.sThumbRX, (float)statelist[i].state.Gamepad.sThumbRY };
     float mr = std::sqrt(vr[0]*vr[0] + vr[1]*vr[1]); // magnitude
     if(mr) {
        vr[0] /= mr; // this is a 2D unit vector
        vr[1] /= mr; // this is a 2D unit vector
       }
     else {
        vr[0] = 0.0f;
        vr[1] = 0.0f;
       }

     // deadzone constant
     float INPUT_DEADZONE = statelist[i].dead_zone_percent*32767.0f;
     if(INPUT_DEADZONE < 0.0f) INPUT_DEADZONE = 0.0f; // no deadzone
     else if(INPUT_DEADZONE > 32767.0f) INPUT_DEADZONE = 32767.0f; // all deadzone

     // L stick
     if(ml < INPUT_DEADZONE) {
        statelist[i].state.Gamepad.sThumbLX = 0;
        statelist[i].state.Gamepad.sThumbLY = 0;
       }
     else {
        if(ml > 32767.0f) ml = 32767.0f;
        ml -= INPUT_DEADZONE;
        ml = ml/(32767.0f - INPUT_DEADZONE);
       }

     // R stick
     if(mr < INPUT_DEADZONE) {
        statelist[i].state.Gamepad.sThumbRX = 0;
        statelist[i].state.Gamepad.sThumbRY = 0;
       }
     else {
        if(mr > 32767.0f) mr = 32767.0f;
        mr -= INPUT_DEADZONE;
        mr = mr/(32767.0f - INPUT_DEADZONE);
       }

     // set JS_L
     statelist[i].keys.JS_L[0] = vl[0];
     statelist[i].keys.JS_L[1] = vl[1];
     statelist[i].keys.JS_L_NORM = ml;

     // set JS_R
     statelist[i].keys.JS_R[0] = vr[0];
     statelist[i].keys.JS_R[1] = vr[1];
     statelist[i].keys.JS_R_NORM = mr;

     // button state
     WORD buttons = statelist[i].state.Gamepad.wButtons;
     auto padupdate = [](bool ps, bool cs, uint32& rc) { if(cs) { if(ps) { if(rc < 0xFFFFFFFFul) rc++; } else rc = 1; } else rc = 0; };

     // set DPAD
     bool DPAD_U_PREV = statelist[i].keys.DPAD_U;
     bool DPAD_D_PREV = statelist[i].keys.DPAD_D;
     bool DPAD_L_PREV = statelist[i].keys.DPAD_L;
     bool DPAD_R_PREV = statelist[i].keys.DPAD_R;
     statelist[i].keys.DPAD_U = (buttons & XINPUT_GAMEPAD_DPAD_UP) != 0;
     statelist[i].keys.DPAD_D = (buttons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
     statelist[i].keys.DPAD_L = (buttons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
     statelist[i].keys.DPAD_R = (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

     // set DPAD repeat counts
     padupdate(DPAD_U_PREV, statelist[i].keys.DPAD_U, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(DPAD_D_PREV, statelist[i].keys.DPAD_D, statelist[i].keys.DPAD_D_REPEAT_COUNT);
     padupdate(DPAD_L_PREV, statelist[i].keys.DPAD_L, statelist[i].keys.DPAD_L_REPEAT_COUNT);
     padupdate(DPAD_R_PREV, statelist[i].keys.DPAD_R, statelist[i].keys.DPAD_R_REPEAT_COUNT);

     // set GPAD
     bool GPAD_START_PREV = statelist[i].keys.GPAD_START;
     bool GPAD_BACK_PREV = statelist[i].keys.GPAD_BACK;
     bool GPAD_LTHUMB_PREV = statelist[i].keys.GPAD_LTHUMB;
     bool GPAD_RTHUMB_PREV = statelist[i].keys.GPAD_RTHUMB;
     bool GPAD_LSHOULDER_PREV = statelist[i].keys.GPAD_LSHOULDER;
     bool GPAD_RSHOULDER_PREV = statelist[i].keys.GPAD_RSHOULDER;
     bool GPAD_A_PREV = statelist[i].keys.GPAD_A;
     bool GPAD_B_PREV = statelist[i].keys.GPAD_B;
     bool GPAD_X_PREV = statelist[i].keys.GPAD_X;
     bool GPAD_Y_PREV = statelist[i].keys.GPAD_Y;
     bool GPAD_LTRIGGER_PREV = statelist[i].keys.GPAD_LTRIGGER;
     bool GPAD_RTRIGGER_PREV = statelist[i].keys.GPAD_RTRIGGER;
     statelist[i].keys.GPAD_START = (buttons & XINPUT_GAMEPAD_START) != 0;
     statelist[i].keys.GPAD_BACK  = (buttons & XINPUT_GAMEPAD_BACK) != 0;
     statelist[i].keys.GPAD_LTHUMB = (buttons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
     statelist[i].keys.GPAD_RTHUMB = (buttons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
     statelist[i].keys.GPAD_LSHOULDER = (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
     statelist[i].keys.GPAD_RSHOULDER = (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
     statelist[i].keys.GPAD_A = (buttons & XINPUT_GAMEPAD_A) != 0;
     statelist[i].keys.GPAD_B = (buttons & XINPUT_GAMEPAD_B) != 0;
     statelist[i].keys.GPAD_X = (buttons & XINPUT_GAMEPAD_X) != 0;
     statelist[i].keys.GPAD_Y = (buttons & XINPUT_GAMEPAD_Y) != 0;
     statelist[i].keys.GPAD_LTRIGGER = (statelist[i].state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
     statelist[i].keys.GPAD_RTRIGGER = (statelist[i].state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

     // set GPAD repeat counts
     padupdate(GPAD_START_PREV, statelist[i].keys.GPAD_START, statelist[i].keys.GPAD_START_REPEAT_COUNT);
     padupdate(GPAD_BACK_PREV, statelist[i].keys.GPAD_BACK, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_LTHUMB_PREV, statelist[i].keys.GPAD_LTHUMB, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_RTHUMB_PREV, statelist[i].keys.GPAD_RTHUMB, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_LSHOULDER_PREV, statelist[i].keys.GPAD_LSHOULDER, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_RSHOULDER_PREV, statelist[i].keys.GPAD_RSHOULDER, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_A_PREV, statelist[i].keys.GPAD_A, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_B_PREV, statelist[i].keys.GPAD_B, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_X_PREV, statelist[i].keys.GPAD_X, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_Y_PREV, statelist[i].keys.GPAD_Y, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_LTRIGGER_PREV, statelist[i].keys.GPAD_LTRIGGER, statelist[i].keys.DPAD_U_REPEAT_COUNT);
     padupdate(GPAD_RTRIGGER_PREV, statelist[i].keys.GPAD_RTRIGGER, statelist[i].keys.DPAD_U_REPEAT_COUNT);
    }
}

#pragma region CONTROLLER_ACCESS_FUNCTIONS

bool IsControllerConnected(uint32 index)
{
 if(!(index < MAX_CONTROLLERS)) return false;
 return statelist[index].connected;
}

bool IsControllerReserved(uint32 index)
{
 if(!(index < MAX_CONTROLLERS)) return false;
 return statelist[index].reserved;
}

bool ReserveController(uint32 index)
{
 if(!(index < MAX_CONTROLLERS) || statelist[index].reserved) return false;
 statelist[index].reserved = true;
 return true;
}

bool ReleaseController(uint32 index)
{
 if(!(index < MAX_CONTROLLERS) || !statelist[index].reserved) return false;
 statelist[index].reserved = false;
 return true;
}

#pragma endregion CONTROLLER_ACCESS_FUNCTIONS

/**
 * /brief   Dead zone functions.
 * /details These functions allow users to assign a 'dead zone' value to each of the two axis
 *          controllers on an XBox One controller. It is used to control the sensitivity of the
 *          joysticks so that they don't activate too soon. The 'dead zone' value should be a value
 *          between [0.0f, 1.0f] with 0.0f representing full sensitivity and 1.0f representing zero
 *          sensitivity.
 */
#pragma region DEAD_ZONE_FUNCTIONS

void SetDeadZonePercent(uint32 index, real32 value)
{
 if(index < MAX_CONTROLLERS) {
    if(value < 0.0f) statelist[index].dead_zone_percent = 0.0f;
    else if(value > 1.0f) statelist[index].dead_zone_percent = 1.0f;
    else statelist[index].dead_zone_percent = value;
   }
}

real32 GetDeadZonePercent(uint32 index)
{
 if(index < MAX_CONTROLLERS) return statelist[index].dead_zone_percent;
 return 0.0f;
}

#pragma endregion DEAD_ZONE_FUNCTIONS