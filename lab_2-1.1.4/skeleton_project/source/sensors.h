
typedef struct {
    int story;
    bool active;
    bool directionUp;


    void update();
} CallButton;

typedef struct 
{
    int story;
    bool active;


    void update();
} HallEffect;

typedef struct 
{
    int story;
    bool active;


    void update();
} GoalButton;


typedef struct 
{
    GoalButton goalButtons[4];

    void updateButtons();
    bool checkButtons(int story);
} HeisPanel;

typedef struct 
{
    CallButton callButtons[6];

    void updateButtons();
    bool checkButtons(int story, bool directionUp);
} HeisPanel;