
typedef struct {
    ButtonType buttonType;
    bool active;
} CallButton;

typedef struct {
    ButtonType buttonType;
    bool active;
} GoalButton;
void updateButton(CallButton* call);







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
