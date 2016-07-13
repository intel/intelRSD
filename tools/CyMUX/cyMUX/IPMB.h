/*!
 * \copyright
 * Copyright (c) 2015, 2016 Intel Corporation. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \file IPMB.h
 *
 * \brief IPMB basic structure definitions
 *
 */


#ifndef IPMB_H
#define IPMB_H


#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/cdefs.h>


#pragma pack(push, 1)


/**
 * Fundamental basis for a standard IPMB request packet.  Data
 * of variable length.  The last byte of the data is the
 * checksum (thus, data will be at minimum 1 in length)
 */
typedef struct
{
    uint8_t  NetFn;                 // for slave to parse
    uint8_t  Checksum;
    uint8_t  MasterAddr;            // sender's address
    uint8_t  SeqNum;
    uint8_t  Command;
    uint8_t  Data[];                // can be the checksum if no data is associated
}   IpmbReq_t;


/**
 * Fundamental basis for a standard IPMB response packet.  The
 * last byte of the data is the checksum (thus, data will be at
 * minimum 1 in length)
 */
typedef struct
{
    uint8_t  NetFn;                 // from slave
    uint8_t  Checksum;
    uint8_t  SlaveAddr;             // sender's address
    uint8_t  SeqNum;
    uint8_t  Command;
    uint8_t  CompCode;              // CC
    uint8_t  Data[];
}   IpmbResp_t;


/**
 * Fundamental basis for a standard IPMB bridged request packet.
 * The last byte of the data is the checksum (thus, data will be
 * at minimum 1 in length)
 */
typedef struct
{
    uint8_t   Channel;              // 0x41 (track/path to sleds)
    uint8_t   RespAddr;             // Responders (Target) Address
    uint8_t   NetFn;                // Bridged
    uint8_t   Checksum;
    uint8_t   ReqAddr;              // Bridge's Address
    uint8_t   RqSeq;
    uint8_t   Command;              // Bridged
    uint8_t   Data[];               // bridged data/can be checksum if no data
}   IpmbBridgedReq_t;


#pragma pack(pop)


#define MAX_IPMB_PKT_SIZE   128UL


typedef struct
{
    int         client_resource;
    size_t      Length;
    uint8_t     DstAddr;
    uint8_t     Data[MAX_IPMB_PKT_SIZE];
}   Message_t;


extern bool valid_request(uint8_t const* const b, size_t l) __nonnull((1));
extern bool valid_request_msg(Message_t const* const m) __nonnull((1));
extern int craft_error_response(Message_t const* const m, Message_t* const m_rsp, uint8_t cc) __nonnull((1, 2));
extern void m_checksum(Message_t* msg);


#define PACKET_DEBUG  // Turn on more detailed debugging of the packet checks

// Response Mneumonic codes used
#define CC_NORMAL                   0x00
#define CC_SUCCESS                  CC_NORMAL
#define CC_NODE_BUSY                0xC0
#define CC_INV_CMD                  0xC1
#define CC_TIMEOUT                  0xC3
#define CC_OUT_OF_SPACE             0xC4
#define CC_PARAM_OUT_OF_RANGE       0xC9
#define CC_COULD_NOT_PROVIDE_RESP   0xCE
#define CC_DEST_UNAVAILABLE         0xD3
#define CC_UNSPECIFIED_ERR          0xFF

#endif /* IPMB_H */
