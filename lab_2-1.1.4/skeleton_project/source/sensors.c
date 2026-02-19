#include "sensors.h"

updateCallButton(CallButton* call){
    int active = elevio_callButton(call->buttonType);
    if (active == 1){
        call->active = true;
    } else {
        call->active = false;
    }
}



void updateStoryButtons(HeisPanel* panel){
    for(int i = 0; i < N_FLOORS; i++){
        int active = elevio_callButton(i, BUTTON_CAB);
        if (active == 1){
            panel->goalButtons[i].active = true;
        } else {
            panel->goalButtons[i].active = false;
        }
    }
}