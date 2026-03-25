#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "interrupt.h"

// Common interface includes
#include "uart_if.h"
#include "pin_mux_config.h"

// OLED files
#include "OLED/Adafruit_OLED.h"
#include "OLED/OLED.h"
#include "OLED/oled_test.h"

// Sensor files
#include "DistanceSensor/DistanceSensor.h"
#include "IR/IR.h"

// Networking files
#include "Network/Network.h"

// Systick files
#include "systick.h"
#include "SysTick/SysTickHandlers.h"

// I2C files
#include "I2C/I2C.h"

// Helper files
#include "Helpers/Helpers.h"

// State files
#include "State/State.h"

// Exercise files
#include "Exercises/Exercises.h"


#define APPLICATION_VERSION     "1.0"
//*****************************************************************************
//
// Application Master/Slave mode selector macro
//
// MASTER_MODE = 1 : Application in master mode
// MASTER_MODE = 0 : Application in slave mode
//
//*****************************************************************************


#define SPI_IF_BIT_RATE  300000
#define TR_BUFF_SIZE     100


#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

void SPIInit()
{

    // Reset SPI
    //
    MAP_SPIReset(GSPI_BASE);

    //
    // Configure SPI interface
    //
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                     SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                     (SPI_SW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVEHIGH |
                     SPI_WL_8));

    //
    // Enable SPI for communication
    //
    MAP_SPIEnable(GSPI_BASE);
}

static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//! Main function for spi demo application
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************
void main() {
    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Muxing UART and SPI lines.
    //
    PinMuxConfig();

    //
    // Enable the SPI module clock
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI, PRCM_RUN_MODE_CLK);
    MAP_SPIReset(GSPI_BASE);

    MAP_PRCMPeripheralReset(PRCM_GSPI);

    InitTerm();
    SPIInit();
    Adafruit_Init();

    fillScreen(BLACK);

    // Display loading animation
    displayLoadingScreen();

    // Initialization happens here
    writeCenteredStringLoading("Connecting to network", 72, 1, 6, WHITE);
    int ret = setupNetwork();
    if (ret < 0) {
        // Error occurred
        writeCenteredStringLoading("Connecting to network", 72, 1, 6, RED);
        writeCenteredStringLoading("Press Reset switch", 112, 1, 6, RED);
        return;
    } else {
        writeCenteredStringLoading("Connecting to network", 72, 1, 6, GREEN);
    }
    writeCenteredStringLoading("Connecting to AWS", 80, 1, 6, WHITE);
    ret = setupAWS();
    if (ret < 0) {
        // Error occurred
        writeCenteredStringLoading("Connecting to AWS", 80, 1, 6, RED);
        writeCenteredStringLoading("Check certificates", 112, 1, 6, RED);
        return;
    } else {
        writeCenteredStringLoading("Connecting to AWS", 80, 1, 6, GREEN);
    }
    writeCenteredStringLoading("Setting up sensors", 88, 1, 6, WHITE);
    SetupSysTick();
    SetupIR();
    SetupI2C();
    HCSR04_Init();
    writeCenteredStringLoading("Setting up sensors", 88, 1, 6, GREEN);

    fillScreen(BG);

    // Display fancy bootup screen
    displaySplash();
    DelaySeconds(2);

    ChangeState(MAINMENU);

    while (1) {
        // Transition state: Changing screen
        if (transitionState == TRANSITION) {
            clearMenu();
            switch (currentState) {
            case MAINMENU:
                mainMenu_SelectedButton = SETUP;
                displayMainMenu();
                writeCenteredString("Setup", 116, 1, 6, WHITE);
                transitionState = DONE;
                break;
            case EXERCISEMENU:
                displayExerciseMenu();
                transitionState = DONE;
                break;
            case EXERCISING:
                switch(selectedExercise){
                case PLANKS:
                    displayPlanksInterface();
                    break;
                case PUSHUPS:
                    displayPushupsInterface();
                    break;
                }
                transitionState = DONE;
                break;
            case STATSMENU:
                displayStatsMenu();
                transitionState = DONE;
                break;
            case RESULTSMENU:
                displayResultsMenu();
                transitionState = DONE;
                break;
            case GETREADY:
                displayGetReadyMenu();
                transitionState = DONE;
                break;
            case SETUPMENU:
                displaySetupBodyWeight();
                transitionState = DONE;
                break;
            case SETUPMENU2:
                displaySetupCalorieGoal();
                transitionState = DONE;
                break;
            }
        }
        // Not transitioning: Main logic loop
        else {
            switch (currentState) {
            case MAINMENU: {
                enum MainMenu_SelectedButton oldState = mainMenu_SelectedButton;
                int command = InputHandler();
                if (command > 0) {
                    switch (command) {
                    case BUTTON_RIGHT:
                    case BUTTON_LEFT:
                        mainMenu_SelectedButton = mainMenu_SelectedButton ^ 1;
                        break;
                    case BUTTON_UP:
                    case BUTTON_DOWN:
                        mainMenu_SelectedButton = mainMenu_SelectedButton ^ 2;
                        break;
                    case BUTTON_OK:
                        switch (mainMenu_SelectedButton) {
                        case EXERCISEPLANKS:
                            selectedExercise = PLANKS;
                            ChangeState(EXERCISEMENU);
                            break;
                        case EXERCISEPUSHUPS:
                            selectedExercise = PUSHUPS;
                            ChangeState(EXERCISEMENU);
                            break;
                        case STATS:
                            ChangeState(STATSMENU);
                            break;
                        case SETUP:
                            ChangeState(SETUPMENU);
                            break;
                        }
                    }
                }

                // Update menu state
                if (oldState != mainMenu_SelectedButton) {
                    fillRect(1, 116, 126, 8, BG);
                    selectIcon((int)mainMenu_SelectedButton);
                    switch (mainMenu_SelectedButton) {
                    case SETUP:
                        writeCenteredString("Setup", 116, 1, 6, WHITE);
                        break;
                    case EXERCISEPLANKS:
                        writeCenteredString("Plank Exercising", 116, 1, 6, WHITE);
                        break;
                    case EXERCISEPUSHUPS:
                        writeCenteredString("Push-Up Exercising", 116, 1, 6, WHITE);
                        break;
                    case STATS:
                        writeCenteredString("Stats", 116, 1, 6, WHITE);
                        break;
                    }
                }
                break;
            }
            case EXERCISEMENU:{
                int command = InputHandler();
                if (command > 0) {
                    int oldSetting = setting;
                    switch (command) {
                    case BUTTON_ENTER:
                        ChangeState(GETREADY);
                        break;
                    case BUTTON_BACK:
                        setting = setting / 10;
                        break;
                    case BUTTON_ONE:
                        setting = setting * 10;
                        setting++;
                        break;
                    case BUTTON_TWO:
                        setting = setting * 10;
                        setting += 2;
                        break;
                    case BUTTON_THREE:
                        setting = setting * 10;
                        setting += 3;
                        break;
                    case BUTTON_FOUR:
                        setting = setting * 10;
                        setting += 4;
                        break;
                    case BUTTON_FIVE:
                        setting = setting * 10;
                        setting += 5;
                        break;
                    case BUTTON_SIX:
                        setting = setting * 10;
                        setting += 6;
                        break;
                    case BUTTON_SEVEN:
                        setting = setting * 10;
                        setting += 7;
                        break;
                    case BUTTON_EIGHT:
                        setting = setting * 10;
                        setting += 8;
                        break;
                    case BUTTON_NINE:
                        setting = setting * 10;
                        setting += 9;
                        break;
                    case BUTTON_ZERO:
                        setting = setting * 10;
                        break;
                    }
                    if(setting != oldSetting){
                        if(setting > 99999){
                            setting = 99999;
                        }
                        drawTimer(setting, 84);
                    }
                }
                break;
            }
            case EXERCISING: {
                switch(selectedExercise){
                case PLANKS:
                    results = planksRoutine();
                    break;
                case PUSHUPS:
                    results = pushupsRoutine();
                    break;
                }

                // Done with exercise
                ChangeState(RESULTSMENU);
                break;
            }
            case RESULTSMENU:
            case STATSMENU: {
                int command = InputHandler();
                if (command > 0) {
                    switch (command) {
                    case BUTTON_OK:
                        ChangeState(MAINMENU);
                        break;
                    }
                }
                break;
            }
            case GETREADY: {
                int command = InputHandler();
                if(command > 0){
                    switch(command){
                    case BUTTON_ONE:
                    case BUTTON_TWO:
                    case BUTTON_THREE:
                    case BUTTON_FOUR:
                    case BUTTON_FIVE:
                    case BUTTON_SIX:
                    case BUTTON_SEVEN:
                    case BUTTON_EIGHT:
                    case BUTTON_NINE:
                    case BUTTON_ZERO:
                    case BUTTON_BACK:
                    case BUTTON_ENTER:
                        ChangeState(EXERCISING);
                        break;
                    }
                }
                break;
            }
            case SETUPMENU: {
                int command = InputHandler();
                int oldWeight = weightKg;
                if(command > 0){
                    switch(command){
                    case BUTTON_ENTER:
                        ChangeState(SETUPMENU2);
                        break;
                    case BUTTON_BACK:
                        weightKg = weightKg / 10;
                    break;
                    case BUTTON_ONE:
                        weightKg = weightKg * 10;
                        weightKg++;
                    break;
                    case BUTTON_TWO:
                        weightKg = weightKg * 10;
                        weightKg += 2;
                    break;
                    case BUTTON_THREE:
                        weightKg = weightKg * 10;
                        weightKg += 3;
                    break;
                    case BUTTON_FOUR:
                        weightKg = weightKg * 10;
                        weightKg += 4;
                    break;
                    case BUTTON_FIVE:
                        weightKg = weightKg * 10;
                        weightKg += 5;
                    break;
                    case BUTTON_SIX:
                        weightKg = weightKg * 10;
                        weightKg += 6;
                    break;
                    case BUTTON_SEVEN:
                        weightKg = weightKg * 10;
                        weightKg += 7;
                    break;
                    case BUTTON_EIGHT:
                        weightKg = weightKg * 10;
                        weightKg += 8;
                    break;
                    case BUTTON_NINE:
                        weightKg = weightKg * 10;
                        weightKg += 9;
                    break;
                    case BUTTON_ZERO:
                        weightKg = weightKg * 10;
                    break;
                    }
                    if (weightKg != oldWeight) {
                        if (weightKg > 99999) {
                            weightKg = 99999;
                        }
                        drawTimer(weightKg, 84);
                    }
                }
                break;
            }
            case SETUPMENU2: {
                int command = InputHandler();
                int oldGoal = calorieGoal;
                if (command > 0) {
                    switch (command) {
                    case BUTTON_ENTER:
                        ChangeState(MAINMENU);
                        break;
                    case BUTTON_BACK:
                        calorieGoal = calorieGoal / 10;
                        break;
                    case BUTTON_ONE:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal++;
                        break;
                    case BUTTON_TWO:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal += 2;
                        break;
                    case BUTTON_THREE:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal += 3;
                        break;
                    case BUTTON_FOUR:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal += 4;
                        break;
                    case BUTTON_FIVE:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal += 5;
                        break;
                    case BUTTON_SIX:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal += 6;
                        break;
                    case BUTTON_SEVEN:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal += 7;
                        break;
                    case BUTTON_EIGHT:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal += 8;
                        break;
                    case BUTTON_NINE:
                        calorieGoal = calorieGoal * 10;
                        calorieGoal += 9;
                        break;
                    case BUTTON_ZERO:
                        calorieGoal = calorieGoal * 10;
                        break;
                    }
                    if (calorieGoal != oldGoal) {
                        if (calorieGoal > 99999) {
                            calorieGoal = 99999;
                        }
                        drawTimer(calorieGoal, 84);
                    }
                }
                }

                break;

        }

        }
    }
}
