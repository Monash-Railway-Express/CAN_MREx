/**
 * CAN MREX CAN_MREx.cpp file 
 *
 * File:            CM.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/01/2026
 * Last Modified:   14/03/2026
 * Version:         1.13.0
 *
 */

#include "CAN_MREx.h"

void CAN_Task(void *pvParameters) {
    uint8_t nodeID = *(uint8_t*)pvParameters;

    while (true) {
        handleCAN(nodeID, nullptr);
        vTaskDelay(1);   // yield to scheduler
    }
}