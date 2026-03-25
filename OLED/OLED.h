#ifndef OLED_H_
#define OLED_H_

int centerY(int fontSize);
void writeCenteredStringLoading(char* str, int yPos, int fontSize, int charSpacing, int color);
void writeCenteredString(char* str, int yPos, int fontSize, int charSpacing, int color);
void displayLoadingScreen();
void displaySplash();
void displayMainMenu();
void selectIcon(int iconNum);
void displayStatsMenu();
void displayExerciseMenu();
void displayPlanksInterface();
void displayPushupsInterface();
void clearMenu();
void drawTimer(int sec, int yPos);
void displayResultsMenu();
void displayGetReadyMenu();
void displaySetupBodyWeight();
void displaySetupCalorieGoal();

#endif
