#include "logic.h"
#include "driver/elevio.h"
#include <string.h>  // for memcpy

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
    q->story = elevio_floorSensor();
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
        else
        {
            elevatorChange(&(q->elevator), false, true);
        }
        
    }
    else
    {
        elevatorChange(&(q->elevator), false, true);
    }
    //Mer her kanskje
}

// stub for updateQueue; actual queue behavior not implemented yet
//checkPanelButton sjekker om knapp er på
//checkStorybutton sjekker om knapp er på
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
    int temp[6] = {-3,1,2,0,2,1};
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