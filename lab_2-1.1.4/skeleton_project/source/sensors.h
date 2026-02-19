#pragma once
#include <stdbool.h>
#include "logic.h"

typedef struct {
    ButtonType buttonType;
    int story;
    bool active;
} CallButton;

typedef struct {
    ButtonType buttonType;
    int story;
    bool active;
} GoalButton;
void updateButton(CallButton* call);


typedef struct 
{
    GoalButton goalButtons[4];
} HeisPanel;
void updateStoryButtons(HeisPanel* panel);
bool checkStoryButton(HeisPanel* panel, int story);


typedef struct 
{
    CallButton callButtons[6];

} EtasjePanel;
void updatePanelButtons(EtasjePanel* panel);
bool checkPanelButton(EtasjePanel* panel, int story, bool directionUp);


typedef struct{
    bool state;
}ObstructionButton;
//liksommetoder
void updateObstruction(ObstructionButton* o);
