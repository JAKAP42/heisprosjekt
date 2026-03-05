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

    for (int i = 0; i < N_FLOORS; i++)
    {
        turnOffLampsOnStory(i);
    }
    elevio_motorDirection(DIRN_DOWN);


    QueueManager q = createQueueManager();

    while(1){
        run(&q);
    }

    return 0;
}
