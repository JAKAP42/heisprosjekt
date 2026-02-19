
typedef struct {
    ButtonType buttonType;
    bool active;
} CallButton;

typedef struct {
    ButtonType buttonType;
    bool active;
} GoalButton;
void updateCallButton(CallButton* call);





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
