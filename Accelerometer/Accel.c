#include <stdint.h>
#include <stdbool.h>

#include "AccelLib.h"


// Returns if the board is in a valid orientation for the exercise
bool checkValidOrientation(int thresholdX, int thresholdY){
    char x_buffer[64] = "readreg 0x18 0x03 1 \n\r";
    char y_buffer[64] = "readreg 0x18 0x05 1 \n\r";

    int xi = ParseNProcessCmd(x_buffer);
    int yi = ParseNProcessCmd(y_buffer);

    int8_t signedNumX = (int8_t)xi;
    int8_t signedNumY = (int8_t)yi;

    int changeX = signedNumX / 6;
    int changeY = signedNumY / 6;

    return changeX < thresholdX && changeX > -thresholdX && changeY < thresholdY && changeY > -thresholdY;
}
