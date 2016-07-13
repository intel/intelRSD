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
 * \file IPMB.c
 *
 * \brief Generic routines dealing with IPMB messages
 *
 */


#include "IPMB.h"
#include "configs.h"
#include "debug.h"
#include "shim.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>


/*!
 * Determine if the byte stream is of the expected IPMB request message.
 * First byte should be the slave address.
 * The next 6 bytes should be an IPMB request mesasge.
 * All of the checksums are checked to make sure they are correct.
 *
 * \author bjkurlex (11/12/15)
 *
 * \param b byte stream to check is valid
 * \param l length of the byte stream to check
 *
 * \return bool whether the message is a valid request or not
 */
bool valid_request(uint8_t const* const b, size_t l)
{
    uint8_t cksm = 0;
    size_t  i;

    if (NULL == b) return false;

#ifndef PACKET_DEBUG
    if (l > (MAX_IPMB_PKT_SIZE + 1UL)) return false;   // Too many bytes
    if (l <= (sizeof(IpmbReq_t) + 1UL)) return false;  // Insufficient bytes
#else
    if (l > (MAX_IPMB_PKT_SIZE + 1UL)) {
        debug(LOG_DEBUG, "too many bytes (%ld > %ld)\n", l, (MAX_IPMB_PKT_SIZE + 1UL));
        return false;   // Too many bytes
    }
    if (l <= (sizeof(IpmbReq_t) + 1UL)) {
        debug(LOG_DEBUG, "insufficient bytes (%ld <= %ld)\n", l, (sizeof(IpmbReq_t) + 1UL));
        return false;   // Insufficient bytes
    }
#endif

#ifndef PACKET_DEBUG
    if (((b[1] >> 2) % 2) != 0) return false;        // Not a request message
    if ((b[0] % 2) != 0)        return false;        // Not a valid slave address
#else
    if (((b[1] >> 2) % 2) != 0) {
        debug(LOG_DEBUG, "NetFn(%02x) isn't a request message\n", (b[1] >> 2));
        return false;        // Not a request message
    }
    if ((b[0] % 2) != 0) {
        debug(LOG_DEBUG, "Slave Address(0x%02x) bit 0 set (address must be even)\n", (b[0]));
        return false;        // Not a valid slave address
    }
#endif

    for (i = 0; i <= 2; i++) cksm += b[i];

#ifndef PACKET_DEBUG
    if (0 != cksm) return false;   // First checksum mismatch
#else
    if (0 != cksm) {
        debug(LOG_DEBUG, "first checksum mismatch: %02x\n", cksm);
        return false;   // First checksum mismatch
    }
#endif

    for (i = 3; i < l; i++) cksm += b[i];

#ifndef PACKET_DEBUG
    return (0 == cksm);                                 // Second checksum match
#else
    if (0 == cksm)                                 // Second checksum match
        return true;
    debug(LOG_DEBUG, "second checksum mismatch: %02x\n", cksm);
    return false;
#endif
}


/*!
 * Determine if the Message_t is of the expected IPMB request message.
 * First byte should be the slave address.
 * The next 6 bytes should be an IPMB request mesasge.
 * All of the checksums are checked to make sure they are correct.
 *
 * \author bjkurlex (11/13/15)
 *
 * \param m Message_t containing the IPMB request message
 *
 * \return bool whether the message is a valid request or not
 */
bool valid_request_msg(Message_t const* const m)
{
    uint8_t cksm = 0;
    uint8_t const* b;
    size_t  i;

    if (NULL == m) return false;
    b = m->Data;

#ifndef PACKET_DEBUG
    if (m->Length > MAX_IPMB_PKT_SIZE) return false;   // Too many bytes
    if (m->Length <= sizeof(IpmbReq_t)) return false;  // Insufficient bytes
#else
    if (m->Length > MAX_IPMB_PKT_SIZE) {
        debug(LOG_DEBUG, "too many bytes (%ld > %ld)\n", m->Length, MAX_IPMB_PKT_SIZE);
        return false;   // Too many bytes
    }
    if (m->Length <= sizeof(IpmbReq_t)) {
        debug(LOG_DEBUG, "insufficient bytes (%ld <= %ld)\n", m->Length, sizeof(IpmbReq_t));
        return false;   // Insufficient bytes
    }
#endif

    cksm = m->DstAddr;
    for (i = 0; i < 2; i++) cksm += b[i];

#ifndef PACKET_DEBUG
    if (0 != cksm) return false;   // First checksum mismatch
#else
    if (0 != cksm) {
        debug(LOG_DEBUG, "first checksum mismatch: %02x\n", cksm);
        return false;   // First checksum mismatch
    }
#endif

#ifndef PACKET_DEBUG
    if (((m->Data[0] >> 2) % 2) != 0) return false;        // Not a request message
    if ((m->DstAddr % 2) != 0) return false;        // Not a valid slave address
#else
    if (((m->Data[0] >> 2) % 2) != 0) {
        debug(LOG_DEBUG, "NetFn(%02x) isn't a request message\n", (m->Data[0] >> 2));
        return false;        // Not a request message
    }

    if ((m->DstAddr % 2) != 0) {
        debug(LOG_DEBUG, "Slave Address(0x%02x) bit 0 set (address must be even)\n", (b[0]));
        return false;        // Not a valid slave address
    }
#endif

    for (i = 2; i < m->Length; i++) cksm += b[i];

#ifndef PACKET_DEBUG
    return (0 == cksm);                                 // Second checksum match
#else
    if (0 == cksm)                                 // Second checksum match
        return true;
    debug(LOG_DEBUG, "second checksum mismatch: %02x\n", cksm);
    return false;
#endif
}


/*!
 * Simple command to craft an invalid NetFn/Command based on the Request message
 *
 * \author bjkurlex (11/13/15)
 *
 * \param m Request Message to use to craft response with
 * \param m_rsp Response containing invalid NetFn/Cmd message
 * \param cc The control code to use in the resposne
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
int craft_error_response(Message_t const* const m, Message_t* const m_rsp, uint8_t cc)
{
    IpmbReq_t*  req;
    IpmbResp_t* rsp;

    if (NULL == m) return EINVAL;
    if (!valid_request_msg(m)) return EINVAL;
    if (NULL == m_rsp) return EINVAL;
    if (m_rsp == m) return EINVAL;

    m_rsp->client_resource = m->client_resource;
    message_dump("Crafting error (%02x) response from", m, cc);


    req = (IpmbReq_t*)m->Data;
    rsp = (IpmbResp_t*)(&m_rsp->Data[0]);
    m_rsp->DstAddr = req->MasterAddr;   // Save the slave address to respond to
    rsp->NetFn = req->NetFn | (1 << 2); // Response is +1 NetFn request #
    rsp->Checksum = -(m_rsp->DstAddr + rsp->NetFn);
    rsp->SlaveAddr = m->DstAddr;        // Our configured slave address (8-bit)
    rsp->Command = req->Command;
    rsp->SeqNum = req->SeqNum;          // Some reuse the same seq, some use a new one
    rsp->CompCode = cc;                 // Error code to return
    rsp->Data[0] = -(rsp->SlaveAddr + rsp->Command + rsp->SeqNum + rsp->CompCode);
    m_rsp->Length = 7;

    message_dump("New crafted error response is", m_rsp);
    return 0;
}


/*!
 * Simple command to checksum a Message_t
 *
 * \author bjkurlex (1/4/16)
 *
 * \param m Request Message to checksum
 *
 */
void m_checksum(Message_t* msg)
{
    size_t     i;
    uint8_t    cksum    = 0;
    IpmbReq_t* pReq     = NULL;
    size_t     data_len = 0;

    if (NULL == msg) return;

    pReq = (IpmbReq_t*)msg->Data;
    data_len = msg->Length - sizeof(IpmbReq_t) - 1UL;

    pReq->Checksum = -(msg->DstAddr + pReq->NetFn);
    cksum = pReq->MasterAddr + pReq->SeqNum + pReq->Command;

    for (i = 0; i < data_len; i++) {
        cksum += pReq->Data[i];
    }
    pReq->Data[data_len] = -cksum;
}
