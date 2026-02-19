#include "logic.h"
#include "elevio.h"

//metoder Elevator
void elevator_change(Elevator* e,bool on, bool newDirectionUp){
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
    updatePanelButtons(&(q->heispanel));
    updateStoryButtons(&(q->etasjepanel));
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
            elevator_change(&(q->elevator), true, true);
        }
        else if (target < q->story)
        {
            elevator_change(&(q->elevator), true, false);
        }
        else
        {
            elevator_change(&(q->elevator), false, true);
        }
        
    }
    else
    {
        elevator_change(&(q->elevator), false, true);
    }
    //Mer her kanskje
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