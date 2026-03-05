#include "logic.h"
#include "driver/elevio.h"
#include <stdio.h>   
#include <string.h>  

static void removeStoryFromArray(int* queue, int size, int story);

void obstruction(QueueManager* q){
    deleteAllOrders(q);
    for (int i = 0; i < N_FLOORS; i++)
    {
        turnOffLampsOnStory(i);
    }
    if (elevio_floorSensor() != -1)
    {
        elevio_doorOpenLamp(1);
    }
    
}


static int storyRelation(const QueueManager* q, int targetStory){
    int sensorStory = elevio_floorSensor();
    if (sensorStory >= 0) {
        if (sensorStory < targetStory) return -1;
        if (sensorStory > targetStory) return 1;
        return 0;
    }

    if (q->story < targetStory) return -1;
    if (q->story > targetStory) return 1;
    if (q->elevator.direction == DIRN_UP) return 1;
    if (q->elevator.direction == DIRN_DOWN) return -1;
    return 0;
}

void turnOffLampsOnStory(int story){
    if (story < 0 || story >= N_FLOORS) {
        return;
    }

    elevio_buttonLamp(story, BUTTON_CAB, 0);
    if (story < N_FLOORS - 1) {
        elevio_buttonLamp(story, BUTTON_HALL_UP, 0);
    }
    if (story > 0) {
        elevio_buttonLamp(story, BUTTON_HALL_DOWN, 0);
    }
}

void turnOnLampForButton(int story, ButtonType buttonType){
    if (story < 0 || story >= N_FLOORS) {
        return;
    }
    if (buttonType == BUTTON_HALL_UP && story == N_FLOORS - 1) {
        return;
    }
    if (buttonType == BUTTON_HALL_DOWN && story == 0) {
        return;
    }
    elevio_buttonLamp(story, buttonType, 1);
}

void deleteAllOrders(QueueManager* q){
    for (int i = 0; i < 4; ++i) {
        q->queue[i] = -1;
    }
    for (int i = 0; i < 3; ++i) {
        q->upQueue[i] = -1;
        q->downQueue[i] = -1;
    }
}

void startMotorPause(QueueManager* q, double seconds){
    q->motorPauseActive = true;
    q->motorPauseUntil = time(NULL) + (time_t)seconds;
    elevio_doorOpenLamp(1);
    elevatorChange(q, &(q->elevator), false, true);
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


//metoder Elevator
void elevatorChange(QueueManager* q, Elevator* e, bool on, bool newDirectionUp){
    if (on && !isMotorPauseActive(q) && !q->obstructionButton.state)
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
    updateStory(q);
    updateAllSensors(q);
    updateQueue(q);
    updateObstruction(&q->obstructionButton);
    if (q->obstructionButton.state)
    {
        obstruction(q);
    }
    
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
        if (target == q->story)
        {
            turnOffLampsOnStory(target);
            startMotorPause(q, 3.0);
            elevatorChange(q, &(q->elevator), false, true);
            removeStoryFromArray(q->upQueue, 3, target);
            removeStoryFromArray(q->downQueue, 3, target);
            for (int i = 0; i < 3; i++)
            {
                q->queue[i] = q->queue[i+1];
            }
            q->queue[3] = -1;
        }
        else
        {
            if (q->story < target)
            {
                elevatorChange(q, &(q->elevator), true, true);
            }
            else if (q->story > target)
            {
                elevatorChange(q, &(q->elevator), true, false);
            }
            else
            {
                elevatorChange(q, &(q->elevator), false, true);
            }
        }
    }
    else
    {
        elevatorChange(q, &(q->elevator), false, true);
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

void clearDirectionQueue(QueueManager* q, bool clearUpQueue){
    int* queueToClear = clearUpQueue ? q->upQueue : q->downQueue;
    for (int i = 0; i < 3; ++i) {
        queueToClear[i] = -1;
    }
}

static void removeStoryFromArray(int* queue, int size, int story){
    for (int i = 0; i < size; ++i) {
        if (queue[i] == story) {
            for (int j = i; j < size - 1; ++j) {
                queue[j] = queue[j + 1];
            }
            queue[size - 1] = -1;
            --i;
        }
    }
}


// stub for updateQueue; actual queue behavior not implemented yet
void updateQueue(QueueManager* q){
    //q->queue starter som en array slik {-1,-1,-1,-1,-1,-1}. -1 betegner ingen etasje er satt da står d stille.
    //heisen er 0 indeksert med 4 etasjer.
    for (int i = 0; i < N_FLOORS; i++){
        int relation = storyRelation(q, i);
        if(checkStoryButton(&q->heispanel,i)){
            turnOnLampForButton(i, BUTTON_CAB);
            if (relation < 0)
            {
                //logikk for å plassere den relevante forespørselen i opp køen
                placeOrderInQueue(q, i, true);
            }
            else if (relation > 0)
            {
                //logikk for å plassere den relevante forespørselen i ned køen
                placeOrderInQueue(q, i, false);
            }
        }
        if(checkPanelButton(&q->etasjepanel,i,true)){
            turnOnLampForButton(i, BUTTON_HALL_UP);
            if (relation < 0)
            {
                //logikk for å plassere den relevante forespørselen i opp køen
                placeOrderInQueue(q, i, true);
            }
            else if (relation > 0)
            {
                //logikk for å plassere den relevante forespørselen i ned køen
                placeOrderInQueue(q, i, false);
            }
        }
        if(checkPanelButton(&q->etasjepanel,i,false)){
            turnOnLampForButton(i, BUTTON_HALL_DOWN);
            if (relation > 0)
            {
                placeOrderInQueue(q, i, false);
            }
            else if (relation < 0)
            {
                placeOrderInQueue(q, i, true);
            }
        }
    }

    
    /* decide direction before assigning next target */
    decideDirection(q);
    if (q->queueDirUp != q->lastQueueDirUp) {
        /* clear the queue from the previous travel direction on direction switch */
        clearDirectionQueue(q, q->lastQueueDirUp);
        q->lastQueueDirUp = q->queueDirUp;
    }

    /* Merge new orders from the active direction queue into the running main queue. */
    if (q->queueDirUp) {
        for (int i = 0; i < 3; ++i) {
            int story = q->upQueue[i];
            if (story == -1) {
                continue;
            }
            bool exists = false;
            for (int j = 0; j < 4; ++j) {
                if (q->queue[j] == story) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                for (int j = 0; j < 4; ++j) {
                    if (q->queue[j] == -1) {
                        q->queue[j] = story;
                        break;
                    }
                }
            }
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (q->queue[j] == -1 && q->queue[j + 1] != -1) {
                    int tmp = q->queue[j];
                    q->queue[j] = q->queue[j + 1];
                    q->queue[j + 1] = tmp;
                } else if (q->queue[j] != -1 && q->queue[j + 1] != -1 && q->queue[j] > q->queue[j + 1]) {
                    int tmp = q->queue[j];
                    q->queue[j] = q->queue[j + 1];
                    q->queue[j + 1] = tmp;
                }
            }
        }
    } else {
        for (int i = 0; i < 3; ++i) {
            int story = q->downQueue[i];
            if (story == -1) {
                continue;
            }
            bool exists = false;
            for (int j = 0; j < 4; ++j) {
                if (q->queue[j] == story) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                for (int j = 0; j < 4; ++j) {
                    if (q->queue[j] == -1) {
                        q->queue[j] = story;
                        break;
                    }
                }
            }
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (q->queue[j] == -1 && q->queue[j + 1] != -1) {
                    int tmp = q->queue[j];
                    q->queue[j] = q->queue[j + 1];
                    q->queue[j + 1] = tmp;
                } else if (q->queue[j] != -1 && q->queue[j + 1] != -1 && q->queue[j] < q->queue[j + 1]) {
                    int tmp = q->queue[j];
                    q->queue[j] = q->queue[j + 1];
                    q->queue[j + 1] = tmp;
                }
            }
        }
    }
    
    /* only refill the main queue when it is empty */
    if (q->queue[0] == -1) {
        /* copy all items from the active direction queue into the main queue */
        if (q->queueDirUp)
        {
            /* copy only valid floors from upQueue */
            for (int i = 0; i < 4; ++i) {
                q->queue[i] = -1;
            }
            int next = 0;
            for (int i = 0; i < 3; ++i) {
                if (q->upQueue[i] != -1 && next < 4) {
                    q->queue[next++] = q->upQueue[i];
                }
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
            /* copy only valid floors from downQueue */
            for (int i = 0; i < 4; ++i) {
                q->queue[i] = -1;
            }
            int next = 0;
            for (int i = 0; i < 3; ++i) {
                if (q->downQueue[i] != -1 && next < 4) {
                    q->queue[next++] = q->downQueue[i];
                }
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
    
}

QueueManager createQueueManager(){
    QueueManager q = {0};

    q.elevator.direction = DIRN_STOP;
    q.story = -1;
    q.obstructionButton.state = false;
    q.motorPauseActive = false;
    q.motorPauseUntil = 0;

    for (int i = 0; i < 4; ++i) {
        q.queue[i] = -1;
    }

    for (int i = 0; i < 3; ++i) {
        q.upQueue[i]   = -1;
        q.downQueue[i] = -1;
    }
    q.queueDirUp = true;
    q.lastQueueDirUp = true;

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
