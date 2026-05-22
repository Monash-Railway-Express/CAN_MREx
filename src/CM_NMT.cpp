/**
 * @file CM_NMT.cpp
 * @brief CAN MREX Network managment tool file
 *
 * @details
 * Longer description of what this file does, what it uses,
 * what other files are connected, etc.
 *
 * @author Chiara Gillam
 *
 * @date_created     12/09/2025
 * @date_modified    22/05/2026
 *
 * @version 1.13.1
 *
 * @organisation MREX
 *
 * @see CM_NMT.h
 */

#include "CM_NMT.h"

#include <Arduino.h>
#include <driver/twai.h>

#include "CM_EMCY.h"
#include "CM_ObjectDictionary.h"

static bool isValidTransition(uint8_t currentMode, uint8_t cmd);
static uint8_t commandToState(uint8_t cmd);

void _HandleNMT(const twai_message_t& rxMsg, uint8_t nodeID) {
    if (rxMsg.data[1] != nodeID && rxMsg.data[1] != 0x00)
        return;

    uint8_t cmd = rxMsg.data[0];
    if (isValidTransition(nodeOperatingMode, cmd)) {
        nodeOperatingMode = commandToState(cmd);
    } else {
        sendEMCY(0x01, nodeID, 0x00000202);
    }
}

void sendNMT(NMTCommand command, uint8_t targetNodeID) {
    uint8_t cmd;
    switch (command) {
        case NMTCommand::Start:
            cmd = NMT_CMD_START;
            break;
        case NMTCommand::EnterPreOp:
            cmd = NMT_CMD_ENTER_PREOP;
            break;
        case NMTCommand::Stop:
            cmd = NMT_CMD_STOP;
            break;
        default:
            sendEMCY(0x01, targetNodeID, 0x00000203);
            return;
    }
    twai_message_t txMsg;
    txMsg.identifier = 0x000;
    txMsg.data_length_code = 2;
    txMsg.data[0] = cmd;
    txMsg.data[1] = targetNodeID;
    if (twai_transmit(&txMsg, pdMS_TO_TICKS(100)) != ESP_OK) {
        sendEMCY(0x00, targetNodeID, 0x00000201);
    }
}

static uint8_t commandToState(uint8_t cmd) {
    switch (cmd) {
        case NMT_CMD_START:
            return NMT_OPERATIONAL;
        case NMT_CMD_STOP:
            return NMT_STOPPED;
        case NMT_CMD_ENTER_PREOP:
            return NMT_PRE_OPERATIONAL;
        default:
            return nodeOperatingMode;
    }
}

static bool isValidTransition(uint8_t currentMode, uint8_t cmd) {
    switch (currentMode) {
        case NMT_STOPPED:
            return (cmd == NMT_CMD_ENTER_PREOP);
        case NMT_PRE_OPERATIONAL:
            return (cmd == NMT_CMD_START || cmd == NMT_CMD_STOP);
        case NMT_OPERATIONAL:
            return (cmd == NMT_CMD_ENTER_PREOP || cmd == NMT_CMD_STOP);
        default:
            return false;
    }
}