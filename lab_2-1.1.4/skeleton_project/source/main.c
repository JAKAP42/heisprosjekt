#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include "driver/elevio.h"
#include "logic.h"
#include "sensors.h"
#include "driver/con_load.h"


//hallo verden

int main(){
    elevio_init();
    
    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    elevio_motorDirection(DIRN_UP);


    QueueManager q = createQueueManager();

    while(1){
        run(&q);
    }

    return 0;
}
