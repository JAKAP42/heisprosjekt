#include "logic.h"
#include "driver/elevio.h"
#include <stdio.h>   
#include <string.h>  

static int queueSize(const QueueManager* q){
    return (int)(sizeof(q->queue) / sizeof(q->queue[0]));
}

static bool queueContains(const QueueManager* q, int floor){
    int size = queueSize(q);
    for (int i = 0; i < size; i++)
    {
        if (q->queue[i] == floor)
        {
            return true;
        }
    }
    return false;
}

static void enqueueIfMissing(QueueManager* q, int floor){
    if (queueContains(q, floor))
    {
        return;
    }

    int size = queueSize(q);
    for (int i = 0; i < size; i++)
    {
        if (q->queue[i] == -1)
        {
            q->queue[i] = floor;
            return;
        }
    }
}

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
    updateAllSensors(q);
    updateStory(q);
    updateQueue(q);
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

void updateQueue(QueueManager* q){
    for (int floor = 0; floor < N_FLOORS; floor++)
    {
        if (checkStoryButton(&(q->heispanel), floor))
        {
            enqueueIfMissing(q, floor);
        }
    }

    for (int floor = 0; floor < N_FLOORS; floor++)
    {
        bool upValid = floor < (N_FLOORS - 1);
        bool downValid = floor > 0;

        if (upValid && checkPanelButton(&(q->etasjepanel), floor, true))
        {
            enqueueIfMissing(q, floor);
        }
        if (downValid && checkPanelButton(&(q->etasjepanel), floor, false))
        {
            enqueueIfMissing(q, floor);
        }
    }
}

QueueManager createQueueManager(){
    QueueManager q = {0};

    q.elevator.direction = DIRN_STOP;
    q.story = -1;
    q.obstructionButton.state = false;
    int temp[6] = {-1,-1,-1,-1,-1,-1};
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
