#ifndef IR_H_
#define IR_H_

#include <stdint.h>

typedef enum {
    BUTTON_UP = 259,
    BUTTON_DOWN = 271,
    BUTTON_LEFT = 263,
    BUTTON_RIGHT = 267,
    BUTTON_OK = 265,
    BUTTON_ENTER = 375,
    BUTTON_BACK = 297,
    BUTTON_ONE = 257,
    BUTTON_TWO = 259,
    BUTTON_THREE = 261,
    BUTTON_FOUR = 263,
    BUTTON_FIVE = 265,
    BUTTON_SIX = 267,
    BUTTON_SEVEN = 269,
    BUTTON_EIGHT = 271,
    BUTTON_NINE = 273,
    BUTTON_ZERO = 275
} InputButtons;

static volatile uint32_t lastEdge;
static volatile uint32_t currentEdge;

void SetupIR(void);
static void IRHandler(void);
int InputHandler(void);

#endif
