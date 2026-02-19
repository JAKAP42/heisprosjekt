#include "sensors.h"
#include "driver/elevio.h"

void callUpdate(CallButton* call) {
    if (call == NULL) {
        return;
    }

    if (call->story < 0 || call->story >= N_FLOORS) {
        call->active = false;
        return;
    }

    ButtonType button = call->directionUp ? BUTTON_HALL_UP : BUTTON_HALL_DOWN;
    call->active = elevio_callButton(call->story, button) != 0;
}
