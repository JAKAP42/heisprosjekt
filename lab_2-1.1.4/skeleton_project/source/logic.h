#pragma once
#include <stdbool.h>
#include "driver/elevio.h"  // for MotorDirection
#include "sensors.h"


typedef struct{
    //variabler
    MotorDirection direction;

}Elevator;

//liksomMetoder
void elevatorChange(Elevator* e,bool on, bool newDirectionUp);

typedef struct {
    //variabler
    Elevator elevator;
    EtasjePanel etasjepanel;
    HeisPanel heispanel;
    bool requests[N_FLOORS][N_BUTTONS];
    int queue[6];
    int story;
    ObstructionButton obstructionButton;
}QueueManager;
//liksomMetoder
void updateQueue(QueueManager* q);
void updateStory(QueueManager* q);
void updateAllSensors(QueueManager* q);
void updateEverything(QueueManager* q);
void run(QueueManager* q);
QueueManager createQueueManager();

