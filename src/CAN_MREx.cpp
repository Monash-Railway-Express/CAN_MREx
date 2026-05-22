/**
 * @file CAN_MREx.cpp
 * @brief CAN MREx entry point
 *
 * @details
 * This file includes the task that gets pinned to a core that runs CAN MREx.
 *
 * @author Chiara Gillam
 *
 * @date_created     13/09/2025
 * @date_modified    22/05/2026
 *
 * @version 1.13.1
 *
 * @organisation MREX
 *
 * @see CAN_MREx.h
 */

#include "CAN_MREx.h"

void CAN_Task(void* pvParameters) {
    uint8_t nodeID = *(uint8_t*)pvParameters;

    while (true) {
        HandleCAN(nodeID);
        vTaskDelay(1);  // yield to scheduler
    }
}