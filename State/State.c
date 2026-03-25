#include "State.h"

enum DeviceState currentState = LOADING;
enum TransitionState transitionState = DONE;
enum MainMenu_SelectedButton mainMenu_SelectedButton = SETUP;
enum SelectedExercise selectedExercise = PLANKS;

// Changes the device state and sets the transitioning state, but only if the target state is different
void ChangeState(enum DeviceState targetState){
    if(targetState != currentState){
        currentState = targetState;
        transitionState = TRANSITION;
    }
}
