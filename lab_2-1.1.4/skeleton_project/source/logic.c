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

void startMotorPause(QueueManager* q, double seconds){
    q->motorPauseActive = true;
    q->motorPauseUntil = time(NULL) + (time_t)seconds;
    elevio_doorOpenLamp(1);
    elevatorChange(&(q->elevator), false, true);
}

bool isMotorPauseActive(QueueManager* q){
    if (!q->motorPauseActive){
        return false;
    }
    if (time(NULL) < q->motorPauseUntil){
        return true;
    }

    q->motorPauseActive = false;
    elevio_doorOpenLamp(0);
    return false;
}

static void clearOrdersAtFloor(QueueManager* q, int floor){
    if (floor < 0 || floor >= N_FLOORS){
        return;
    }

    q->heispanel.goalButtons[floor].active = false;

    for (int i = 0; i < 6; i++){
        if (q->etasjepanel.callButtons[i].story == floor){
            q->etasjepanel.callButtons[i].active = false;
        }
    }
}


void run(QueueManager* q){
    updateEverything(q);

    for (int floor = 0; floor < N_FLOORS; floor++){
        elevio_buttonLamp(floor, BUTTON_CAB, q->heispanel.goalButtons[floor].active);
    }

    for (int i = 0; i < 6; i++){
        elevio_buttonLamp(
            q->etasjepanel.callButtons[i].story,
            q->etasjepanel.callButtons[i].buttonType,
            q->etasjepanel.callButtons[i].active
        );
    }
    if (isMotorPauseActive(q)){
        elevatorChange(&(q->elevator), false, true);
        return;
    }
    int target = q->queue[0];
    if (target != -1)
    {
        if (target == q->story)
        {
            clearOrdersAtFloor(q, q->story);
            startMotorPause(q, 3.0);
            for (int i = 0; i < 3; i++)
            {
                q->queue[i] = q->queue[i+1];
            }
            q->queue[3] = -1;
        }
        else
        {
            /* obey the current scheduling direction; ignore target relative position */
            if (q->queueDirUp)
            {
                elevatorChange(&(q->elevator), true, true);
            }
            else
            {
                elevatorChange(&(q->elevator), true, false);
            }
        }
    }
    else
    {
        elevatorChange(&(q->elevator), false, true);
    }
    //Mer her kanskje
}


void decideDirection(QueueManager* q){
    if (q->queue[0] == -1)
    {
        q->queueDirUp = !q->queueDirUp;
    }
} 



void placeOrderInQueue(QueueManager* q, int story, bool directionUp){
    //logikk for å plassere den relevante forespørselen i køen
    if (directionUp)
    {
        // don't add if already present
        for (int i = 0; i < 3; ++i) {
            if (q->upQueue[i] == story) {
                return; /* already queued */
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if (q->upQueue[i] == -1)
            {
                q->upQueue[i] = story;
                break;
            }
        }
    }
    else
    {
        // avoid duplicates in down queue as well
        for (int i = 0; i < 3; ++i) {
            if (q->downQueue[i] == story) {
                return;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if (q->downQueue[i] == -1)
            {
                q->downQueue[i] = story;
                break;
            }
        }
    }

}


// stub for updateQueue; actual queue behavior not implemented yet
void updateQueue(QueueManager* q){
    //q->queue starter som en array slik {-1,-1,-1,-1,-1,-1}. -1 betegner ingen etasje er satt da står d stille.
    //heisen er 0 indeksert med 4 etasjer.
    for (int i = 0; i<3; i++){
        if(checkStoryButton(&q->heispanel,i)){
            if (q->story < i)
            {
                //logikk for å plassere den relevante forespørselen i opp køen
                placeOrderInQueue(q, i, true);
            }
            else if (q->story > i)
            {
                //logikk for å plassere den relevante forespørselen i ned køen
                placeOrderInQueue(q, i, false);
            }
        }
        if(checkPanelButton(&q->etasjepanel,i,true)){
            if (q->story < i)
            {
                //logikk for å plassere den relevante forespørselen i opp køen
                placeOrderInQueue(q, i, true);
            }
            else if (q->story > i)
            {
                //logikk for å plassere den relevante forespørselen i ned køen
                placeOrderInQueue(q, i, false);
            }
        }
        if(checkPanelButton(&q->etasjepanel,i,false)){
            if (q->story > i)
            {
                //logikk for å plassere den relevante forespørselen i ned køen
                placeOrderInQueue(q, i, true);
            }
            else if (q->story > i)
            {
                //logikk for å plassere den relevante forespørselen i ned køen
                placeOrderInQueue(q, i, false);
            }
        }
    }

    
    /* decide direction before assigning next target */
    decideDirection(q);
    
    /* copy all items from the active direction queue into the main queue */
    if (q->queueDirUp)
    {
        /* sort upQueue in ascending order */
        for (int i = 0; i < 3; ++i) {
            q->queue[i] = q->upQueue[i];
        }
        /* bubble sort ascending */
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (q->queue[j] != -1 && q->queue[j+1] != -1 && q->queue[j] > q->queue[j+1]) {
                    int temp = q->queue[j];
                    q->queue[j] = q->queue[j+1];
                    q->queue[j+1] = temp;
                }
            }
        }
        q->queue[3] = -1;
    }
    else
    {
        /* sort downQueue in descending order */
        for (int i = 0; i < 3; ++i) {
            q->queue[i] = q->downQueue[i];
        }
        /* bubble sort descending */
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (q->queue[j] != -1 && q->queue[j+1] != -1 && q->queue[j] < q->queue[j+1]) {
                    int temp = q->queue[j];
                    q->queue[j] = q->queue[j+1];
                    q->queue[j+1] = temp;
                }
            }
        }
        q->queue[3] = -1;
    }
    
}

QueueManager createQueueManager(){
    QueueManager q = {0};

    q.elevator.direction = DIRN_STOP;
    q.story = -1;
    q.obstructionButton.state = false;
    q.motorPauseActive = false;
    q.motorPauseUntil = 0;
    elevio_doorOpenLamp(0);

    for (int i = 0; i < 4; ++i) {
        q.queue[i] = -1;
    }

    for (int i = 0; i < 3; ++i) {
        q.upQueue[i]   = -1;
        q.downQueue[i] = -1;
    }
    q.queueDirUp = true;

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
