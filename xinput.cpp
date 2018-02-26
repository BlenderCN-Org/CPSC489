#include "stdafx.h"
#include "errors.h"
#include "xinput.h"

struct CONTROLLER_STATE {
 XINPUT_STATE state;
 bool connected;
 float dead_zone_percent;
};

static const int MAX_CONTROLLERS = 4;
static CONTROLLER_STATE statelist[MAX_CONTROLLERS];

ErrorCode InitControllers(void)
{
 for(int i = 0; i < MAX_CONTROLLERS; i++) {
     statelist[i].connected = false;
     statelist[i].dead_zone_percent = 0.24f;
    }
 return EC_SUCCESS;
}

void FreeControllers(void)
{
}

void QueryControllers(void)
{
 for(int i = 0; i < MAX_CONTROLLERS; i++) {
     DWORD result = XInputGetState(i, &statelist[i].state);
     if(result == ERROR_SUCCESS) statelist[i].connected = true;
     else statelist[i].connected = false;
    }
}

void ControllerFrame(real32 dt)
{
 QueryControllers();

 // When computing the magnitude and direction of the thumbstick, the deadzone
 // is a small portion of the inner circle that prevents movement as the game
 // controller begins to shift from the middle to the outside. It is used to
 // control the sensitivity of your controller.
 // vl = <1.0, 0.0>

 for(int i = 0; i < MAX_CONTROLLERS; i++)
    {
     // determine direction and magnitude that L controller is pushed
     float vl[2] = { (float)statelist[i].state.Gamepad.sThumbLX, (float)statelist[i].state.Gamepad.sThumbLY };
     float ml = std::sqrt(vl[0]*vl[0] + vl[1]*vl[1]); // magnitude
     vl[0] /= ml; // this is a 2D unit vector
     vl[1] /= ml; // this is a 2D unit vector

     // determine direction and magnitude that R controller is pushed
     float vr[2] = { (float)statelist[i].state.Gamepad.sThumbRX, (float)statelist[i].state.Gamepad.sThumbRY };
     float mr = std::sqrt(vr[0]*vr[0] + vr[1]*vr[1]); // magnitude
     vr[0] /= mr; // this is a 2D unit vector
     vr[1] /= mr; // this is a 2D unit vector

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
    }
}