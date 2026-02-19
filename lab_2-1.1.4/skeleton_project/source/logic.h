#include "logic.c"


typedef struct {
    //variabler
    Elevator elevator;
    Etasjepanel etasjepanel;
    Heispanel heispanel;
    Sensor[4] sensors;
    int[6] queue;
    Button obstructionButton;
    int story;

    //metoder
    void updateQueue();
    void updateStory();
    void updateAllSensors();
    void run();
}QueueManager;