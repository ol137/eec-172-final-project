#ifndef DISTANCE_SENSOR_H_
#define DISTANCE_SENSOR_H_

#include <stdint.h>

void HCSR04_Init(void);
int HCSR04_GetDistanceCm(uint32_t *distance_cm);

#endif
