#include <stdbool.h>
#include "sensors.h"

typedef struct{
    //variabler
    int voltage;
    bool directionUp;

}Elevator;

//liksomMetoder
void elevatorChange(Elevator* e,int newVoltage, bool newDirectionUp);

typedef struct {
    //variabler
    Elevator elevator;
    Etasjepanel etasjepanel;
    Heispanel heispanel;
    HallEffect sensors[4];
    int queue[6];
    Button obstructionButton;
    int story;

}QueueManager;
//liksomMetoder
void updateQueue(QueueManager* q);
void updateStory(QueueManager* q);
void updateAllSensors(QueueManager* q);
void updateEverything(QueueManager* q);
void run(QueueManager* q);


