#ifndef NETWORK_H_
#define NETWORK_H_

extern int iTLSSockID;

int setupNetwork();
int setupAWS();
int postCalorieStatsAWS(int iTLSSockID, int timeSpent, int numCalories);
int getWeeklySumAWS(int iTLSSockID, int* caloriesBurnt);

#endif
