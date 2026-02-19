#pragma once

#include <stdbool.h>

typedef struct {
    int story;
    bool active;
    bool directionUp;

} CallButton;
void callUpdate(CallButton* call);

typedef struct 
{
    int story;
    bool active;

} HallEffect;
void hallUpdate(HallEffect* hall);


typedef struct 
{
    int story;
    bool active;


} GoalButton;
void goalUpdate(GoalButton* goal);


typedef struct 
{
    GoalButton goalButtons[4];

} HeisPanel;
void updateStoryButtons(HeisPanel* panel);
bool checkStoryButtons(HeisPanel* panel, int story);


typedef struct 
{
    CallButton callButtons[6];

} EtasjePanel;
void updatePanelButtons(EtasjePanel* panel);
bool checkPanelButtons(EtasjePanel* panel, int story, bool directionUp);
