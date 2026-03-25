#ifndef EXERCISES_H_
#define EXERCISES_H_

int planksRoutine();
float calculatePlankCalories(int elapsedTime);
float handlePlankResults(int timeElapsed);
float handlePushupResults(int numPushups);
int pushupsRoutine();
int getWeeklyStats();

extern int setting;
extern int results;
extern int weightKg;
extern int calorieGoal;

#endif
