#include "logic.h"
#include "driver/elevio.h"
#include <stdio.h>   
#include <string.h>  

//metoder Elevator
void elevatorChange(Elevator* e,bool on, bool newDirectionUp){
    if (on)
    {
        if (newDirectionUp)
        {
            e->direction = DIRN_UP;
        }
        else
        {
            e->direction = DIRN_DOWN;
        }
    }
    else
    {
        e->direction = DIRN_STOP;
    }
    elevio_motorDirection(e->direction);
}

//liksomMetoder Queue manager
void updateEverything(QueueManager* q){
    updateQueue(q);
    updateAllSensors(q);
    updateStory(q);
}

void updateAllSensors(QueueManager* q){
    // heispanel holds goal buttons, etasjepanel holds call buttons
    updateStoryButtons(&(q->heispanel));
    updatePanelButtons(&(q->etasjepanel));
}

void updateStory(QueueManager* q){
    int sensor = elevio_floorSensor();
    if (sensor >= 0 && sensor < N_FLOORS) {
        q->story = sensor;
        elevio_floorIndicator(sensor);
    }
}

void run(QueueManager* q){
    updateEverything(q);
    int target = q->queue[0];
    if (target != -1)
    {
        if (target > q->story)
        {
            elevatorChange(&(q->elevator), true, true);
        }
        else if (target < q->story)
        {
            elevatorChange(&(q->elevator), true, false);
        }
        else if (target == q->story)
        {
            elevatorChange(&(q->elevator), false, true);
            for (int i = 0; i < 5; i++)
            {
                q->queue[i] = q->queue[i+1];
            }
            q->queue[5] = -1;
        } 
    }
    else
    {
        elevatorChange(&(q->elevator), false, true);
    }
    //Mer her kanskje
}

// stub for updateQueue; actual queue behavior not implemented yet
void updateQueue(QueueManager* q){
    //q->queue starter som en array slik {-1,-1,-1,-1,-1,-1}. -1 betegner ingen etasje er satt da står d stille.
    //heisen er 0 indeksert med 4 etasjer.
    for (int i = 0; i<3; i++){
        if(checkStoryButton(&q->heispanel,i)){
            //logikk for å putte inn etasjeforespørselen
        }
    }

    for(int i = 0; i<3;i++){ //sjekker opp knappene
        if(checkPanelButton(&q->etasjepanel,i,true)){
            //logikk for å plassere den relevante forespørselen i køen
        }
    }
    for(int i = 1; i<4;i++){ //sjekker ned knappene
        if(checkPanelButton(&q->etasjepanel,i,false)){
            //logikk for å plassere den relevante forespørselen i køen
        }
    }
    q->queue
}

QueueManager createQueueManager(){
    QueueManager q = {0};

    q.elevator.direction = DIRN_STOP;
    q.story = -1;
    q.obstructionButton.state = false;
    int temp[6] = {2,1,3,0,2,1};
    memcpy(q.queue, temp, sizeof(temp));


    for(int floor = 0; floor < 4; floor++){
        q.heispanel.goalButtons[floor].buttonType = BUTTON_CAB;
        q.heispanel.goalButtons[floor].story = floor;
        q.heispanel.goalButtons[floor].active = false;
    }

    q.etasjepanel.callButtons[0] = (CallButton){.buttonType = BUTTON_HALL_UP,   .story = 0, .active = false};
    q.etasjepanel.callButtons[1] = (CallButton){.buttonType = BUTTON_HALL_UP,   .story = 1, .active = false};
    q.etasjepanel.callButtons[2] = (CallButton){.buttonType = BUTTON_HALL_DOWN, .story = 1, .active = false};
    q.etasjepanel.callButtons[3] = (CallButton){.buttonType = BUTTON_HALL_UP,   .story = 2, .active = false};
    q.etasjepanel.callButtons[4] = (CallButton){.buttonType = BUTTON_HALL_DOWN, .story = 2, .active = false};
    q.etasjepanel.callButtons[5] = (CallButton){.buttonType = BUTTON_HALL_DOWN, .story = 3, .active = false};

    return q;
}