#ifndef STATE_H_
#define STATE_H_

enum DeviceState{
    LOADING,
    MAINMENU,
    EXERCISEMENU,
    RESULTSMENU,
    STATSMENU,
    EXERCISING,
    GETREADY,
    SETUPMENU,
    SETUPMENU2
};
enum TransitionState{
    TRANSITION,
    DONE
};

enum MainMenu_SelectedButton{
    SETUP,
    STATS,
    EXERCISEPLANKS,
    EXERCISEPUSHUPS
};

enum SelectedExercise{
    PLANKS,
    PUSHUPS
};

extern enum DeviceState currentState;
extern enum TransitionState transitionState;
extern enum MainMenu_SelectedButton mainMenu_SelectedButton;
extern enum SelectedExercise selectedExercise;

void ChangeState(enum DeviceState targetState);

#endif
