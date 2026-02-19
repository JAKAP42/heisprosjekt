#include "logic.h"
#include "elevio.h"

//metoder Elevator
void Elevator_change(Elevator* e,int newVoltage, bool newDirectionUp){
    e->voltage = newVoltage;
    e->directionUp = newDirectionUp;
    
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
    //Mer her kanskje
}


