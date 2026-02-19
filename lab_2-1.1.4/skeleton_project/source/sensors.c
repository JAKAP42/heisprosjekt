#include "sensors.h"

updateButton(CallButton* call){
    int active = elevio_callButton(call->buttonType);
    if (active == 1){
        call->active = true;
    } else {
        call->active = false;
    }
}
