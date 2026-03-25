#include <stdint.h>

#include "../Accelerometer/Accel.h"
#include "../Buzzer/Buzzer.h"
#include "../DistanceSensor/DistanceSensor.h"
#include "../Helpers/Helpers.h"
#include "../Network/Network.h"
#include "../OLED/OLED.h"
#include "../OLED/oled_test.h"
#include "../SysTick/SysTickHandlers.h"
#include "utils.h"

// TMP
#include "uart_if.h"

int results = -1;
int setting = 5;
int weightKg = 72;
int calorieGoal = 100;

//23-30cm

int planksRoutine() {
    int last_stage = -1;
    int remaining = setting;

    //int distanceStatus = -1;

    uint32_t last_sec_mark = numMs;
    uint32_t start_time_ms = numMs;
    uint32_t finish_time_ms = 0;
    bool finished = false;

    drawTimer(remaining, 98);
    uint32_t real_time;
    while (!finished) {
        int stage;
        char * new_str;

        uint32_t distance;
        HCSR04_GetDistanceCm(&distance);
//        if(distanceStatus < 0){
//            //Report("Could not read distance\r\n");
//        }
//        else{
//            //Report("Distance: %d cm\r\n", distance);
//        }

        bool isValidOrientation = checkValidOrientation(5, 3);
        bool isValidDistance = 15 < distance && distance < 25;

        if (isValidOrientation && isValidDistance) {
            stage = 1;
            new_str = "VALID";
        } else {
            stage = 0;
            new_str = "INVALID";
            if(distance <= 15){
                BuzzerBeepGoUp();
            }
            else if(distance >= 25){
                BuzzerBeepGoDown();
            }
            else{
                BuzzerBeep();
            }
        }

        if (last_stage != stage) {
            if (last_stage == 0) {
                writeCenteredString("INVALID", centerY(2), 2, 10, BG);
                writeCenteredString(new_str, centerY(2), 2, 10, GREEN);
            } else if (last_stage == 1) {
                writeCenteredString("VALID", centerY(2), 2, 10, BG);
                writeCenteredString(new_str, centerY(2), 2, 10, RED);
            } else{
                if(stage == 1){
                    writeCenteredString(new_str, centerY(2), 2, 10, GREEN);
                }
                else{
                    writeCenteredString(new_str, centerY(2), 2, 10, RED);
                }
            }
            last_stage = stage;
        }

        if (stage == 0) {
            last_sec_mark = numMs;
        } else {
            while (((uint32_t)(numMs - last_sec_mark) >= 1000) && (remaining > 0)) {
                remaining--;
                last_sec_mark += 1000;
                drawTimer(remaining, 98);

                if (remaining == 0) {
                    finish_time_ms = numMs;
                    finished = 1;
                    real_time = (uint32_t)((finish_time_ms - start_time_ms) / 1000);
                    break;
                }
            }
        }

        if (!finished)
            real_time = (uint32_t)((numMs - start_time_ms) / 1000);

        UtilsDelay(800000);
    }
    BuzzerBeepDone();

    return real_time;
}

float calculatePlankCalories(int timeElapsed){
    return 2.8 * 3.5 * (float)weightKg / 200.0 * (float)timeElapsed / 60.0;
}

float handlePlankResults(int timeElapsed){
    float caloriesBurnt = calculatePlankCalories(timeElapsed);

    int status = postCalorieStatsAWS(iTLSSockID, timeElapsed, (int)caloriesBurnt);
    if(status < 0){
        Report("Error occurred posting calorie stats\r\n");
    }

    return caloriesBurnt;
}

// Pushups
int pushupsRoutine() {
    // State variables
    bool isDown = false; // If the user has gone low enough this push-up
    bool isUp = false; // If the user has gone high enough this push-up
    int currentState = 1; // 0 = low, 1 = high
    int numPushups = setting; // Target number of pushups
    int numCompletedPushups = 0; // Number of pushups user has completed
    bool wasValid = true; // Was the last check valid? Used for checking if state changed
    bool isValid = true; // If the user's orientation is valid (accelerometer)

    // Status vars
    int distanceStatus;

    writeCenteredString("Remaining:", 48, 2, 12, WHITE);
    drawTimer(numPushups, 64);

    while (numPushups > 0) {
        switch (currentState) {
            // User is currently low
        case 0:
            BuzzerBeepGoUp();
            writeCenteredString("Up", 96, 2, 12, WHITE);
            while (!isUp) {
                isValid = checkValidOrientation(7, 3);

                if(isValid){
                    if(!wasValid){
                        writeCenteredString("Bad Form", 96, 2, 12, BG);
                        writeCenteredString("Up", 96, 2, 12, WHITE);
                        wasValid = true;
                    }

                    uint32_t distance;
                    distanceStatus = HCSR04_GetDistanceCm( & distance);
                    if (distanceStatus < 0) {
                        // Could not read
                        //Report("Could not read distance\r\n");
                    } else {
                        isUp = distance > 20;
                    }
                }
                // Invalid orientation
                else{
                    if(wasValid){
                        writeCenteredString("Up", 96, 2, 12, BG);
                        writeCenteredString("Bad Form", 96, 2, 12, RED);
                        wasValid = false;
                    }
                    BuzzerBeep();
                }
            }
            writeCenteredString("Up", 96, 2, 12, BG);
            currentState = 1;
            break;
            // User is currently high
        case 1:
            BuzzerBeepGoDown();
            writeCenteredString("Down", 96, 2, 12, WHITE);
            while (!isDown) {
                isValid = checkValidOrientation(5, 3);

                if(isValid){
                    if (!wasValid) {
                        writeCenteredString("Bad Form", 96, 2, 12, BG);
                        writeCenteredString("Down", 96, 2, 12, WHITE);
                        wasValid = true;
                    }

                    uint32_t distance;
                    distanceStatus = HCSR04_GetDistanceCm( & distance);
                    if (distanceStatus < 0) {
                        // Could not read
                        //Report("Could not read distance\r\n");
                    } else {
                        isDown = distance < 8;
                    }
                }
                // Invalid orientation
                else {
                    if (wasValid) {
                        writeCenteredString("Down", 96, 2, 12, BG);
                        writeCenteredString("Bad Form", 96, 2, 12, RED);
                        wasValid = false;
                    }
                    BuzzerBeep();
                }
            }
            writeCenteredString("Down", 96, 2, 12, BG);
            currentState = 0;
            break;
        }

        // User has completed an entire pushup
        if (isUp && isDown) {
            numPushups--;
            numCompletedPushups++;
            isUp = false;
            isDown = false;
            BuzzerBeepGood();
            writeCenteredString("Good!", 96, 2, 12, GREEN);
            drawTimer(numPushups, 64);
            DelaySeconds(0.5);
            writeCenteredString("Good!", 96, 2, 12, BG);
        }
    }

    BuzzerBeepDone();

    return numCompletedPushups;
}

float calculatePushupCalories(int numPushups){
    return (float)weightKg * 0.0028 * (float)numPushups;
}

float handlePushupResults(int numPushups){
    float caloriesBurnt = calculatePushupCalories(numPushups);

    int status = postCalorieStatsAWS(iTLSSockID, numPushups, (int)caloriesBurnt);
    if(status < 0){
        Report("Error occurred posting calorie stats\r\n");
    }

    return caloriesBurnt;
}

int getWeeklyStats(){
    int caloriesBurnt;
    int status = getWeeklySumAWS(iTLSSockID, &caloriesBurnt);

    return caloriesBurnt;
}
