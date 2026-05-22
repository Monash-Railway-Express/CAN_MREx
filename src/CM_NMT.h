/**
 * @file CM_NMT.h
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
 * @see CM_NMT.cpp
 */

#ifndef CM_NMT_H
#define CM_NMT_H

#include <driver/twai.h>
#include <stdint.h>

// NMT state values
#define NMT_STOPPED 0x04
#define NMT_PRE_OPERATIONAL 0x7F
#define NMT_OPERATIONAL 0x05

// NMT command specifiers (what goes on the wire)
#define NMT_CMD_START 0x01
#define NMT_CMD_STOP 0x02
#define NMT_CMD_ENTER_PREOP 0x80

enum class NMTCommand { Start, EnterPreOp, Stop };

void _HandleNMT(const twai_message_t& rxMsg, uint8_t nodeID);
void sendNMT(NMTCommand command, uint8_t targetNodeID);

#endif