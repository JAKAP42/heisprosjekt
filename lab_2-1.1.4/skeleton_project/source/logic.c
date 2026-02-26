#include "logic.h"
#include "driver/elevio.h"
#include <stdio.h>

static int queueSize(const QueueManager* q){
    return (int)(sizeof(q->queue) / sizeof(q->queue[0]));
}

static void clearQueue(QueueManager* q){
    int size = queueSize(q);
    for (int i = 0; i < size; i++)
    {
        q->queue[i] = -1;
    }
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
    if (floor < 0 || floor >= N_FLOORS)
    {
        return;
    }

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

static bool hasRequestAtFloor(const QueueManager* q, int floor){
    if (floor < 0 || floor >= N_FLOORS)
    {
        return false;
    }

    for (int button = 0; button < N_BUTTONS; button++)
    {
        if (q->requests[floor][button])
        {
            return true;
        }
    }
    return false;
}

static void clearRequestsAtFloor(QueueManager* q, int floor){
    if (floor < 0 || floor >= N_FLOORS)
    {
        return;
    }

    for (int button = 0; button < N_BUTTONS; button++)
    {
        q->requests[floor][button] = false;
    }
}

static void latchRequestsFromButtons(QueueManager* q){
    for (int floor = 0; floor < N_FLOORS; floor++)
    {
        if (checkStoryButton(&(q->heispanel), floor))
        {
            q->requests[floor][BUTTON_CAB] = true;
        }
    }

    for (int floor = 0; floor < N_FLOORS - 1; floor++)
    {
        if (checkPanelButton(&(q->etasjepanel), floor, true))
        {
            q->requests[floor][BUTTON_HALL_UP] = true;
        }
    }

    for (int floor = 1; floor < N_FLOORS; floor++)
    {
        if (checkPanelButton(&(q->etasjepanel), floor, false))
        {
            q->requests[floor][BUTTON_HALL_DOWN] = true;
        }
    }
}

static void buildScanUp(QueueManager* q){
    for (int floor = q->story; floor < N_FLOORS; floor++)
    {
        if (hasRequestAtFloor(q, floor))
        {
            enqueueIfMissing(q, floor);
        }
    }

    for (int floor = q->story - 1; floor >= 0; floor--)
    {
        if (hasRequestAtFloor(q, floor))
        {
            enqueueIfMissing(q, floor);
        }
    }
}

static void buildScanDown(QueueManager* q){
    for (int floor = q->story; floor >= 0; floor--)
    {
        if (hasRequestAtFloor(q, floor))
        {
            enqueueIfMissing(q, floor);
        }
    }

    for (int floor = q->story + 1; floor < N_FLOORS; floor++)
    {
        if (hasRequestAtFloor(q, floor))
        {
            enqueueIfMissing(q, floor);
        }
    }
}

static void buildScanIdle(QueueManager* q){
    int nearestUp = -1;
    int nearestDown = -1;

    for (int floor = q->story; floor < N_FLOORS; floor++)
    {
        if (hasRequestAtFloor(q, floor))
        {
            nearestUp = floor;
            break;
        }
    }

    for (int floor = q->story; floor >= 0; floor--)
    {
        if (hasRequestAtFloor(q, floor))
        {
            nearestDown = floor;
            break;
        }
    }

    if (nearestUp == -1 && nearestDown == -1)
    {
        return;
    }
    if (nearestDown == -1)
    {
        buildScanUp(q);
        return;
    }
    if (nearestUp == -1)
    {
        buildScanDown(q);
        return;
    }

    if ((nearestUp - q->story) <= (q->story - nearestDown))
    {
        buildScanUp(q);
    }
    else
    {
        buildScanDown(q);
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

    // Startup: continue moving until we have a known floor.
    if (q->story < 0)
    {
        if (q->elevator.direction == DIRN_STOP)
        {
            elevatorChange(&(q->elevator), true, true);
        }
        return;
    }

    int size = queueSize(q);
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
            clearRequestsAtFloor(q, target);

            for (int i = 0; i < size - 1; i++)
            {
                q->queue[i] = q->queue[i+1];
            }
            q->queue[size - 1] = -1;
        }
    }
    else
    {
        elevatorChange(&(q->elevator), false, true);
    }
    //Mer her kanskje
}

void updateQueue(QueueManager* q){
    // Latch aktive knapper til intern request-tilstand.
    latchRequestsFromButtons(q);

    // Ikke rebuild når etasje er ukjent (startup mellom etasjer).
    if (q->story < 0)
    {
        return;
    }

    clearQueue(q);

    if (q->elevator.direction == DIRN_UP)
    {
        buildScanUp(q);
    }
    else if (q->elevator.direction == DIRN_DOWN)
    {
        buildScanDown(q);
    }
    else
    {
        buildScanIdle(q);
    }
}

QueueManager createQueueManager(){
    QueueManager q = {0};

    q.elevator.direction = DIRN_STOP;
    q.story = -1;
    q.obstructionButton.state = false;
    clearQueue(&q);

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
