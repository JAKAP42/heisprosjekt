#include "sensors.h"
int N_FLOORS = 4;

updateButton(CallButton* call){
    int active = elevio_callButton(call->story, call->buttonType);
    if (active == 1){
        call->active = true;
    } else {
        call->active = false;
    }
}



void updateStoryButtons(HeisPanel* panel){
    for(int i = 0; i < N_FLOORS; i++){
        updateButton(&(panel->goalButtons[i]));
    }
}
void updatePanelButtons(EtasjePanel* panel){
    for(int i = 0; i < N_FLOORS; i++){
        updateButton(&(panel->callButtons[i]));
    }
}
