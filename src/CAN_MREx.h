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
 * @see CAN_MREx.cpp
 */

#ifndef CAN_MREX_H
#define CAN_MREX_H

#include <driver/twai.h>

#include "CM_Config.h"
#include "CM_EMCY.h"
#include "CM_Handler.h"
#include "CM_Heartbeat.h"
#include "CM_NMT.h"
#include "CM_ObjectDictionary.h"
#include "CM_PDO.h"
#include "CM_SDO.h"

/**
 * @brief Function that runs all CAN related tasks
 *
 * @param pvParameters  Pointer to the node ID (uint8_t) passed from task creation
 *
 * @return void
 */
void CAN_Task(void* pvParameters);

#endif