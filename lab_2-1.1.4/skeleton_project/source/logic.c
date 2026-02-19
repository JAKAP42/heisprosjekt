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


