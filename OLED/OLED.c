#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "oled_test.h"
#include "Adafruit_OLED.h"
#include "Adafruit_GFX.h"
#include "utils.h"
#include "../Exercises/Exercises.h"
#include "../State/State.h"



int centerY(int fontSize){
    // Characters are fontSize * 8 pixels tall
    return 64 - (fontSize * 8 / 2);
}

void centeredStringWriter(char* str, int yPos, int fontSize, int charSpacing, int color, int bgColor){
    // The screen is 128 pixels wide
    // Every character is shifted charSpacing pixels
    // So the string takes up len(str) * charSpacing pixels
    // So to center it, we should start at x = (len(str) * charSpacing) / 2 away from the center
    int x = 64 - ((strlen(str) * charSpacing) / 2);

    int i = 0;
    while(str[i] != '\0'){
        drawChar(x, yPos, str[i], color, bgColor, fontSize);
        x += charSpacing;
        i++;
    }
}

void writeCenteredStringLoading(char* str, int yPos, int fontSize, int charSpacing, int color){
    centeredStringWriter(str, yPos, fontSize, charSpacing, color, BLACK);
}

void writeCenteredString(char* str, int yPos, int fontSize, int charSpacing, int color){
    centeredStringWriter(str, yPos, fontSize, charSpacing, color, BG);
}

void displayLoadingScreen(){
    writeCenteredStringLoading("LOADING   ", centerY(2) - 8, 2, 12, WHITE);
    writeCenteredStringLoading("LOADING.  ", centerY(2) - 8, 2, 12, WHITE);
    writeCenteredStringLoading("LOADING.. ", centerY(2) - 8, 2, 12, WHITE);
    writeCenteredStringLoading("LOADING...", centerY(2) - 8, 2, 12, WHITE);
}

void displaySplash(){
    drawRect(0, 0, 128, 128, WHITE);
    writeCenteredString("Fitness", 16, 2, 12, WHITE);
    writeCenteredString("Helper", 32, 2, 12, WHITE);
    writeCenteredString("Version 1.0", 52, 1, 8, WHITE);

    writeCenteredString("(C) Zicheng Huang &", 100, 1, 6, GRAY);
    writeCenteredString("Oliver Lee-McKinney", 110, 1, 6, GRAY);
}

void fillRotatedSquare45(int cx, int cy, int side, unsigned int color)
{
    int r = side * 0.70710678;   // side / sqrt(2)

    int x0 = cx;
    int y0 = cy - r;

    int x1 = cx + r;
    int y1 = cy;

    int x2 = cx;
    int y2 = cy + r;

    int x3 = cx - r;
    int y3 = cy;

    fillTriangle(x0, y0, x1, y1, x3, y3, color);

    fillTriangle(x1, y1, x3, y3, x2, y2, color);
}

void drawSettingsIcon(int xPos, int yPos){
    int iconSize = 48;
    int centerX = xPos + iconSize / 2;
    int centerY = yPos + iconSize / 2;
    int gearRadius = 16;
    int toothOffset = gearRadius + 7;
    int toothWidth = 10;
    int toothHeight = 6;

    fillRoundRect(xPos, yPos, iconSize, iconSize, 20, WHITE);

    // Draw a gear
    fillCircle(centerX, centerY, gearRadius, GRAY);
    fillCircle(centerX, centerY, 6, WHITE);

    fillRect(centerX - 4, centerY + gearRadius - 1, toothWidth, toothHeight, GRAY);
    fillRect(centerX - 4, centerY - toothOffset + 2, toothWidth, toothHeight, GRAY);
    fillRect(centerX - toothOffset + 2, centerY - 4, toothHeight, toothWidth, GRAY);
    fillRect(centerX + gearRadius - 1, centerY - 4, toothHeight, toothWidth, GRAY);

    fillRotatedSquare45(centerX + (gearRadius/2 + toothHeight/2), centerY - (gearRadius/2 + toothHeight/2), toothWidth, GRAY);
    fillRotatedSquare45(centerX - (gearRadius/2 + toothHeight/2), centerY - (gearRadius/2 + toothHeight/2), toothWidth, GRAY);
    fillRotatedSquare45(centerX + (gearRadius/2 + toothHeight/2), centerY + (gearRadius/2 + toothHeight/2), toothWidth, GRAY);
    fillRotatedSquare45(centerX - (gearRadius/2 + toothHeight/2), centerY + (gearRadius/2 + toothHeight/2), toothWidth, GRAY);
}

void drawStatsIcon(int xPos, int yPos){
    int iconSize = 48;
    int centerX = xPos + iconSize / 2;
    int centerY = yPos + iconSize / 2;

    fillRoundRect(xPos, yPos, iconSize, iconSize, 20, WHITE);

    // Draw a bar graph
    fillRect(centerX - 17, centerY + 8, 34, 6, GRAY);
    fillRect(centerX - 4, centerY - 17, 8, 23, GREEN);
    fillRect(centerX - 15, centerY - 9, 8, 15, YELLOW);
    fillRect(centerX + 7, centerY - 3, 8, 9, RED);
}

void drawPlanksIcon(int xPos, int yPos){
    int iconSize = 48;
    int centerX = xPos + iconSize / 2;
    int centerY = yPos + iconSize / 2;

    fillRoundRect(xPos, yPos, iconSize, iconSize, 20, WHITE);

    // Draw someone planking
    fillCircle(centerX + 14, centerY - 8, 4, BLACK);
    fillTriangle(centerX + 8, centerY - 8, centerX + 10, centerY - 4, centerX - 20, centerY + 4, BLACK);
    fillTriangle(centerX + 10, centerY - 4, centerX - 20, centerY + 4, centerX - 19, centerY + 6, BLACK);
    fillRect(centerX + 7, centerY - 4, 4, 10, BLACK);
    fillRect(centerX + 11, centerY + 3, 8, 3, BLACK);
}

void drawPushupsIcon(int xPos, int yPos){
    int iconSize = 48;
    int centerX = xPos + iconSize / 2;
    int centerY = yPos + iconSize / 2;

    // Draw someone doing a pushup
    fillRoundRect(xPos, yPos, iconSize, iconSize, 20, WHITE);
    fillCircle(centerX + 14, centerY - 8, 4, BLACK);
    fillTriangle(centerX + 11, centerY - 9, centerX - 2, centerY - 6, centerX + 12, centerY - 5, BLACK);
    fillTriangle(centerX - 2, centerY - 6, centerX + 12, centerY - 5, centerX - 1, centerY - 2, BLACK);
    fillTriangle(centerX - 2, centerY - 6, centerX - 1, centerY - 2, centerX - 18, centerY, BLACK);
    fillTriangle(centerX - 1, centerY - 2, centerX - 18, centerY, centerX - 17, centerY + 2, BLACK);
    fillRect(centerX - 18, centerY, 3, 4, BLACK);
    fillTriangle(centerX - 18, centerY + 4, centerX - 16, centerY + 4, centerX - 18, centerY + 6, BLACK);
    fillRect(centerX + 9, centerY - 5, 3, 11, BLACK);
}

int selectedIcon = -1;

void drawIconSelection(int xPos, int yPos, bool deselect){
    if(deselect){
        drawRoundRect(xPos - 1, yPos - 1, 50, 50, 21, BG);
        drawRoundRect(xPos - 2, yPos - 2, 52, 52, 22, BG);
        drawRoundRect(xPos - 3, yPos - 3, 54, 54, 23, BG);
    }
    else{
        drawRoundRect(xPos - 1, yPos - 1, 50, 50, 21, CYAN);
        drawRoundRect(xPos - 2, yPos - 2, 52, 52, 22, CYAN_DARK);
        drawRoundRect(xPos - 3, yPos - 3, 54, 54, 23, CYAN_DARK2);
    }
}

void deselectIcon(){
    switch(selectedIcon){
    case 0:
        drawIconSelection(12, 8, true);
        break;
    case 1:
        drawIconSelection(68, 8, true);
        break;
    case 2:
        drawIconSelection(12, 64, true);
        break;
    case 3:
        drawIconSelection(68, 64, true);
        break;
    }
}

void selectIcon(int iconNum){
    deselectIcon();
    switch(iconNum){
    case 0:
        drawIconSelection(12, 8, false);
        break;
    case 1:
        drawIconSelection(68, 8, false);
        break;
    case 2:
        drawIconSelection(12, 64, false);
        break;
    case 3:
        drawIconSelection(68, 64, false);
        break;
    }
    selectedIcon = iconNum;
}

void displayMainMenu(){
    drawSettingsIcon(12, 8);
    drawStatsIcon(68, 8);
    drawPlanksIcon(12, 64);
    drawPushupsIcon(68, 64);
    selectedIcon = -1;
    selectIcon(0);
}

int displayedDigits = -1;
void drawTimer(int sec, int yPos)
{
    int numDigits;

    if(sec < 0) sec = 0;
    if(sec > 99999) sec = 99999;

    char buf[7];

    if (sec >= 10000) {
        buf[0] = '0' + (sec / 10000); //ASCII
        buf[1] = '0' + ((sec / 1000) % 1000);
        buf[3] = '0' + ((sec / 100) % 100);
        buf[4] = '0' + ((sec / 10) % 10);
        buf[5] = '0' + (sec % 10);
        buf[6] = '\0';
        numDigits = 5;
    }
    else if (sec >= 1000) {
        buf[0] = '0' + (sec / 1000); //ASCII
        buf[1] = '0' + ((sec / 100) % 100);
        buf[2] = '0' + ((sec / 10) % 10);
        buf[3] = '0' + (sec % 10);
        buf[4] = '\0';
        numDigits = 4;
    }
    else if (sec >= 100)
    {
        buf[0] = '0' + (sec / 100); //ASCII
        buf[1] = '0' + ((sec / 10) % 10);
        buf[2] = '0' + (sec % 10);
        buf[3] = '\0';
        numDigits = 3;
    }
    else if (sec >= 10)
    {
        buf[0] = '0' + ((sec / 10) % 10);
        buf[1] = '0' + (sec % 10);
        buf[2] = '\0';
        numDigits = 2;
    }
    else
    {
        buf[0] = '0' + (sec % 10);
        buf[1] = '\0';
        numDigits = 1;
    }

    if(numDigits != displayedDigits){
        fillRect(1, yPos, 126, 16, BG);
        displayedDigits = numDigits;
    }
    writeCenteredString(buf, yPos, 2, 12, WHITE);
}

void displayStatsMenu(){
    writeCenteredString("Stats", 12, 2, 10, WHITE);
    writeCenteredString("This week,", centerY(1) - 28, 1, 6, WHITE);
    writeCenteredString("you burnt", centerY(1) - 16, 1, 6, WHITE);

    int caloriesBurnt = getWeeklyStats();

    char caloriesBurntText[100];
    sprintf(caloriesBurntText, "%d", caloriesBurnt);
    writeCenteredString(caloriesBurntText, centerY(1) - 4, 2, 12, RED);
    writeCenteredString("calories! You're", centerY(1) + 12, 1, 6, WHITE);
    int progress = (caloriesBurnt * 100 / calorieGoal);
    char progressText[100];
    sprintf(progressText, "%d%%", progress);
    writeCenteredString(progressText, centerY(1) + 24, 2, 12, YELLOW);
    writeCenteredString("to your goal.", centerY(1) + 40, 1, 6, WHITE);
    if(progress >= 100){
        writeCenteredString("Amazing job!", centerY(1) + 52, 1, 6, WHITE);
    }
    else if(progress >= 75){
        writeCenteredString("Almost there!", centerY(1) + 52, 1, 6, WHITE);
    }
    else if(progress >= 30){
        writeCenteredString("You're doing great!", centerY(1) + 52, 1, 6, WHITE);
    }
    else{
        writeCenteredString("Keep going!", centerY(1) + 52, 1, 6, WHITE);
    }

}

void displayExerciseMenu(){
    writeCenteredString("Config", 12, 2, 12, WHITE);
    switch(selectedExercise){
    case PLANKS:
        writeCenteredString("Use the remote to", 36, 1, 6, WHITE);
        writeCenteredString("input how long you", 44, 1, 6, WHITE);
        writeCenteredString("want to plank for,", 52, 1, 6, WHITE);
        writeCenteredString("then press enter", 60, 1, 6, WHITE);
        writeCenteredString("to get started!", 68, 1, 6, WHITE);
        setting = 20;
        drawTimer(setting, 84);
        writeCenteredString("seconds", 108, 1, 6, WHITE);
        break;
    case PUSHUPS:
        writeCenteredString("Use the remote to", 36, 1, 6, WHITE);
        writeCenteredString("input how many", 44, 1, 6, WHITE);
        writeCenteredString("push-ups you want", 52, 1, 6, WHITE);
        writeCenteredString("to do, then press", 60, 1, 6, WHITE);
        writeCenteredString("enter to start!", 68, 1, 6, WHITE);
        setting = 5;
        drawTimer(setting, 84);
        writeCenteredString("push-ups", 108, 1, 6, WHITE);
        break;
    }
}

void displayPlanksInterface(){
    writeCenteredString("Planks", 16, 2, 12, WHITE);
}

void displayPushupsInterface(){
    writeCenteredString("Push-Ups", 16, 2, 12, WHITE);
}

void clearMenu(){
    fillRect(1, 1, 126, 126, BG);
}

void displayPlankResults(){
    writeCenteredString("FINISHED!", 14, 2, 10, GREEN);
    writeCenteredString("You planked for", 34, 1, 6, WHITE);
    drawTimer(setting, 46);
    writeCenteredString("seconds in good form", 66, 1, 6, WHITE);
    char realTime[100];
    sprintf(realTime, "over %d seconds.", results);
    writeCenteredString(realTime, 74, 1, 6, WHITE);
    writeCenteredString("You burned", 82, 1, 6, WHITE);
    char caloriesBurned[100];
    sprintf(caloriesBurned, "%.2f", handlePlankResults(setting));
    writeCenteredString(caloriesBurned, 94, 2, 12, RED);
    writeCenteredString("calories. Great job!", 114, 1, 6, WHITE);
}

void displayPushupResults(){
    writeCenteredString("FINISHED!", 14, 2, 10, GREEN);
    writeCenteredString("You performed", 34, 1, 6, WHITE);
    drawTimer(results, 46);
    writeCenteredString("push-ups.", 66, 1, 6, WHITE);
    writeCenteredString("You burned", 82, 1, 6, WHITE);
    char caloriesBurned[100];
    sprintf(caloriesBurned, "%.2f", handlePushupResults(results));
    writeCenteredString(caloriesBurned, 94, 2, 12, RED);
    writeCenteredString("calories. Great job!", 114, 1, 6, WHITE);
}

void displayResultsMenu(){
    switch(selectedExercise){
    case PLANKS:
        displayPlankResults();
        break;
    case PUSHUPS:
        displayPushupResults();
        break;
    }
}

void displayGetReadyMenu(){
    writeCenteredString("Get Ready", 12, 2, 12, WHITE);
    writeCenteredString("Strap the device", 44, 1, 6, WHITE);
    writeCenteredString("to your body,", 52, 1, 6, WHITE);
    writeCenteredString("then press any", 60, 1, 6, WHITE);
    writeCenteredString("button to start!", 68, 1, 6, WHITE);
}

void displaySetupBodyWeight(){
    writeCenteredString("Body Setup", 12, 2, 12, WHITE);
    writeCenteredString("Body weight (kg)", 44, 1, 6, WHITE);
    drawTimer(weightKg, 84);
    writeCenteredString("kilograms", 108, 1, 6, WHITE);
}

void displaySetupCalorieGoal(){
    writeCenteredString("Goal Setup", 12, 2, 12, WHITE);
    writeCenteredString("Weekly calorie goal", 44, 1, 6, WHITE);
    drawTimer(calorieGoal, 84);
    writeCenteredString("calories burned", 108, 1, 6, WHITE);
}
