#pragma once
#include <stdbool.h>
#include <time.h>
#include "driver/elevio.h"  // for MotorDirection
#include "sensors.h"


typedef struct{
    //variabler
    MotorDirection direction;

}Elevator;

typedef struct QueueManager QueueManager;

//liksomMetoder
void elevatorChange(QueueManager* q, Elevator* e, bool on, bool newDirectionUp);

struct QueueManager {
    //variabler
    Elevator elevator;
    EtasjePanel etasjepanel;
    HeisPanel heispanel;
    int queue[4];
    int story;
    int upQueue[3];
    int downQueue[3];
    bool motorPauseActive;
    time_t motorPauseUntil;
    bool queueDirUp;
    bool lastQueueDirUp;
    ObstructionButton obstructionButton;
};

//liksomMetoder
void startMotorPause(QueueManager* q, double seconds);
bool isMotorPauseActive(QueueManager* q);
void updateQueue(QueueManager* q);
void updateStory(QueueManager* q);
void updateAllSensors(QueueManager* q);
void updateEverything(QueueManager* q);
void placeOrderInQueue(QueueManager* q, int story, bool directionUp);
void clearDirectionQueue(QueueManager* q, bool clearUpQueue);
void turnOffLampsOnStory(int story);
void turnOnLampForButton(int story, ButtonType buttonType);
void decideDirection(QueueManager* q);
void run(QueueManager* q);
QueueManager createQueueManager();

