#include "sensors.h"

// use N_FLOORS macro from elevio.h instead of separate variable

void updateButton(CallButton* call){
    int active = elevio_callButton(call->story, call->buttonType);
    if (active == 1){
        call->active = true;
    } else {
        call->active = false;
    }
}



void updateStoryButtons(HeisPanel* panel){
    for(int i = 0; i < N_FLOORS; i++){
        // goalButtons and callButtons share the same layout; cast to reuse updateButton
        updateButton((CallButton*)&(panel->goalButtons[i]));
    }
}
void updatePanelButtons(EtasjePanel* panel){
    for(int i = 0; i < N_FLOORS; i++){
        updateButton(&(panel->callButtons[i]));
    }
}
bool checkStoryButton(HeisPanel* panel, int story){
    for(int i = 0; i < N_FLOORS; i++){
        if(panel->goalButtons[i].story == story && panel->goalButtons[i].active){
            return true;
        }
    }
    return false;
}
bool checkPanelButton(EtasjePanel* panel, int story, bool directionUp){
    for(int i = 0; i < N_FLOORS; i++){
        if(panel->callButtons[i].story == story && panel->callButtons[i].active && panel->callButtons[i].buttonType == (directionUp ? BUTTON_HALL_UP : BUTTON_HALL_DOWN)){
            return true;
        }
    }
    return false;
}

void updateObstruction(ObstructionButton* o){
    o->state = (bool)elevio_obstruction();
}