#include <stdio.h>

#include "gpio_if.h"
#include "network_utils.h"
#include "uart_if.h"

#include "../State/State.h"

//NEED TO UPDATE THIS FOR IT TO WORK!
#define DATE                4    /* Current Date */
#define MONTH               3   /* Month 1-12 */
#define YEAR                2026  /* Current year */
#define HOUR                16    /* Time - hours */
#define MINUTE              51    /* Time - minutes */
#define SECOND              0     /* Time - seconds */

#define SERVER_NAME           "a4pt28vwmfqir-ats.iot.us-east-2.amazonaws.com"
#define GOOGLE_DST_PORT       8443

int iTLSSockID = -1;

static int set_time() {
    long retVal;

    g_time.tm_day  = DATE;
    g_time.tm_mon  = MONTH;
    g_time.tm_year = YEAR;
    g_time.tm_hour = HOUR;
    g_time.tm_min  = MINUTE;
    g_time.tm_sec  = SECOND;

    retVal = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
                       SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
                       sizeof(SlDateTime),
                       (unsigned char *)(&g_time));

    ASSERT_ON_ERROR(retVal);
    return SUCCESS;
}

int setupNetwork(){
    g_app_config.host = SERVER_NAME;
    g_app_config.port = GOOGLE_DST_PORT;

    int lRetVal = connectToAccessPoint();

    return lRetVal;
}

int setupAWS(){
    int lRetVal = set_time();
    if (lRetVal < 0) {
        UART_PRINT("Unable to set time in the device\n\r");
        return lRetVal;
    }

    lRetVal = tls_connect();
    if (lRetVal < 0) {
        ERR_PRINT(lRetVal);
        return lRetVal;
    }

    iTLSSockID = lRetVal;

    return lRetVal;
}

#define POSTHEADER "POST /things/Oliver_CC3200Board/shadow HTTP/1.1\r\n"
#define GETHEADER "GET /things/Oliver_CC3200Board/shadow HTTP/1.1\r\n"
#define HOSTHEADER "Host: a4pt28vwmfqir-ats.iot.us-east-2.amazonaws.com\r\n"
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"

int postCalorieStatsAWS(int iTLSSockID, int timeSpent, int numCalories) {

    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[32];
    char dataPayload[256];
    char *pcBufHeaders;
    int lRetVal = 0;

    memset(acSendBuff, 0, sizeof(acSendBuff));
    memset(acRecvbuff, 0, sizeof(acRecvbuff));
    memset(dataPayload, 0, sizeof(dataPayload));

    // Build JSON payload:
    // - message: used by your email/SNS rule
    // - value: used by Lambda to update weekly sum
    switch(selectedExercise){
    case PLANKS:
        sprintf(dataPayload,
                "{"
                  "\"state\":{"
                    "\"desired\":{"
                      "\"message\":\"%d/%d/%d %02d:%02d\\n\\n"
                                   "Time Spent: %d\\n"
                                   "Calories Burnt: %d\","
                      "\"value\":%d"
                    "}"
                  "}"
                "}\r\n\r\n",
                MONTH, DATE, YEAR, HOUR, MINUTE,
                timeSpent, numCalories,
                numCalories
            );
        break;
    case PUSHUPS:
        sprintf(dataPayload,
                "{"
                  "\"state\":{"
                    "\"desired\":{"
                      "\"message\":\"%d/%d/%d %02d:%02d\\n\\n"
                                   "Push-ups Performed: %d\\n"
                                   "Calories Burnt: %d\","
                      "\"value\":%d"
                    "}"
                  "}"
                "}\r\n\r\n",
                MONTH, DATE, YEAR, HOUR, MINUTE,
                timeSpent, numCalories,
                numCalories
            );
    }


    pcBufHeaders = acSendBuff;

    strcpy(pcBufHeaders, POSTHEADER);
    pcBufHeaders += strlen(POSTHEADER);

    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);

    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);

    strcpy(pcBufHeaders, CTHEADER);
    pcBufHeaders += strlen(CTHEADER);

    strcpy(pcBufHeaders, CLHEADER1);
    pcBufHeaders += strlen(CLHEADER1);

    sprintf(cCLLength, "%d", (int)strlen(dataPayload));
    strcpy(pcBufHeaders, cCLLength);
    pcBufHeaders += strlen(cCLLength);

    strcpy(pcBufHeaders, CLHEADER2);
    pcBufHeaders += strlen(CLHEADER2);

    strcpy(pcBufHeaders, dataPayload);
    pcBufHeaders += strlen(dataPayload);

    UART_PRINT("\n\r=== POST REQUEST ===\n\r%s\n\r", acSendBuff);

    lRetVal = sl_Send(iTLSSockID, acSendBuff, strlen(acSendBuff), 0);
    if (lRetVal < 0) {
        UART_PRINT("POST failed. Error Number: %i\n\r", lRetVal);
        sl_Close(iTLSSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    lRetVal = sl_Recv(iTLSSockID, acRecvbuff, sizeof(acRecvbuff) - 1, 0);
    if (lRetVal < 0) {
        UART_PRINT("POST receive failed. Error Number: %i\n\r", lRetVal);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    acRecvbuff[lRetVal] = '\0';

    UART_PRINT("\n\r=== POST RESPONSE ===\n\r%s\n\r", acRecvbuff);

    return 0;
}

static int parse_week_sum(const char *resp, float *weekSumOut) {
    char *p;

    if (resp == NULL || weekSumOut == NULL) {
        return -1;
    }

    // Look for "weekSum": in the HTTP response body
    p = strstr(resp, "\"weekSum\":");
    if (p == NULL) {
        return -1;
    }

    p += strlen("\"weekSum\":");

    // Skip spaces if any
    while (*p == ' ' || *p == '\t') {
        p++;
    }

    if (sscanf(p, "%f", weekSumOut) == 1) {
        return 0;
    }

    return -1;
}

int getWeeklySumAWS(int iTLSSockID, int* caloriesBurnt) {
    char acSendBuff[256];
    char acRecvbuff[2048];
    char *pcBufHeaders;
    int lRetVal = 0;
    float weekSum = 0.0f;

    memset(acSendBuff, 0, sizeof(acSendBuff));
    memset(acRecvbuff, 0, sizeof(acRecvbuff));

    pcBufHeaders = acSendBuff;

    strcpy(pcBufHeaders, GETHEADER);
    pcBufHeaders += strlen(GETHEADER);

    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);

    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);

    strcpy(pcBufHeaders, "\r\n");
    pcBufHeaders += strlen("\r\n");

    UART_PRINT("\n\r=== GET REQUEST ===\n\r%s\n\r", acSendBuff);

    lRetVal = sl_Send(iTLSSockID, acSendBuff, strlen(acSendBuff), 0);
    if (lRetVal < 0) {
        UART_PRINT("GET failed. Error Number: %i\n\r", lRetVal);
        sl_Close(iTLSSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    lRetVal = sl_Recv(iTLSSockID, acRecvbuff, sizeof(acRecvbuff) - 1, 0);
    if (lRetVal < 0) {
        UART_PRINT("GET receive failed. Error Number: %i\n\r", lRetVal);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    acRecvbuff[lRetVal] = '\0';

    UART_PRINT("\n\r=== GET RESPONSE ===\n\r%s\n\r", acRecvbuff);

    if (parse_week_sum(acRecvbuff, &weekSum) == 0) {
        UART_PRINT("\n\rParsed weekly sum = %.1f\n\r", weekSum);
    } else {
        UART_PRINT("\n\rCould not find weekSum in GET response.\n\r");
    }

    *caloriesBurnt = (int)weekSum;

    return 0;
}
