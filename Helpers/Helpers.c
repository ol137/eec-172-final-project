#include "utils.h"

// Helper: Delays program for specified amount of time
void DelaySeconds(float numSeconds){
    unsigned long numTicks = (unsigned long)(numSeconds * 80000000 / 5);
    UtilsDelay(numTicks);
}
