#include "elevio.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include "driver/elevio.h"
#include "logic.h"
#include "sensors.h"


int main(void){
    elevio_init();

    while(1){
        // elevio_motorDirection(DIRN_DOWN);
        // while(elevio_floorSensor() != 0){}
        // elevio_motorDirection(DIRN_UP);
        // while(elevio_floorSensor() != 3){}
        QueueManager q = createQueueManager();
        run(&q);
    }
}
