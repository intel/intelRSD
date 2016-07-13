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
 * \file test_responder.c
 *
 * \brief Mock/Test responder
 *
 */


#include "test_responder_usage.h"
#include "IPMB.h"
#include "configs.h"
#include "debug.h"
#include "shim.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <arpa/inet.h>


static int post_devid(Message_t const* const m, int server_sock);
static int post_illegal(Message_t const* const m, int server_sock);
static int post_msg(Message_t const* const m, int sock);


/*!
 * Entry point into the Cypress Multiplexer.   Processes command line arguments
 * and launches daemon into the background on success.
 *
 * \author bjkurlex (11/13/15)
 *
 * \param argc Number of arguments passed to program
 * \param argv Array containing the arguments passed to the program
 *
 * \return int exit status of the program
 */
int main(int argc, char* argv[])
{
    int                server_sock;            // Socket descriptor for new connections
    uint8_t            buf[MAX_IPMB_PKT_SIZE + 1]; // receive buffer
    ssize_t            bytes_read;
    ssize_t            bytes_sent;
    struct sockaddr_in server;      // listener/registration defintion for socket
    Message_t          m;
    ssize_t            i;
    int                sflags;
    int                retval;
    fd_set             rfds;
    struct timeval     tv;
    time_t             timeout = 2;


    process_commandline(argc, argv);

    do {
        printf("Creating socket to cyMUX\n");
        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == server_sock) {
            printf("Could not create listener socket: %s\n", strerror(errno));
            break;
        }

        printf("Connecting to server socket on localhost interface\n");

        server.sin_family = AF_INET;
        server.sin_port = htons(LISTENER_PORT);
        server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);    // Only services on the
                                                            // localhost can connect
                                                            // to the MUX
        if (connect(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            printf("Unable to connect to server port %d: %s\n", LISTENER_PORT, strerror(errno));
            close(server_sock);
            break;
        }

        sflags = fcntl(server_sock, F_GETFL, 0);
        fcntl(server_sock, F_SETFL, sflags | O_NONBLOCK);

        do {
            printf("connected to server socket port %d\n", LISTENER_PORT);

            bytes_sent = send(server_sock, "R", 1, 0);
            if (bytes_sent <= 0) {
                printf("send failure: %s\n", strerror(errno));
                break;
            } else {
                tv.tv_sec = timeout;
                tv.tv_usec = 0;

                FD_ZERO(&rfds);
                FD_SET(server_sock, &rfds);

                retval = select(server_sock+1, &rfds, NULL, NULL, &tv);
                if (0 == retval) {
                    fprintf(stderr, "Negotiation failure (timed out on read)\n");
                    break;
                }
                else if (retval < 0) {
                    fprintf(stderr, "Negotiation failure (waiting on select to read): %s\n", strerror(errno));
                    break;
                }
                bytes_read = recv(server_sock, buf, sizeof(buf), 0);
                if (0 == bytes_read) {
                    printf("Server response: (zero bytes)\n");
                }
                else if (bytes_read < 0) {
                    printf("recv failure: %s\n", strerror(errno));
                    break;
                }
                else {

                    printf("Server response: (%ld bytes)\n", bytes_read);
                    for (i = 0; i < bytes_read; i++) {
                        printf("%02ld: %c 0x%02X\n", i, (isalpha(buf[i]) ? buf[i] : '.'), buf[i]);
                    }
                    if (0 != memcmp(buf, "OK", 2)) {
                        printf("Negotiation with cyMUX failed\n");
                        break;
                    }

                    bzero(buf, sizeof(buf));

                    // Reenable blocking.   Just wait until we have data
                    sflags = fcntl(server_sock, F_GETFL, 0);
                    fcntl(server_sock, F_SETFL, sflags & ~O_NONBLOCK);

                    // Mock responder (loop until pipe closes)
                    while ((bytes_read = recv(server_sock, buf, sizeof(buf), 0)) > 0) {

                        bzero(&m, sizeof(m));

                        printf("received message (%ld bytes):", bytes_read);
                        for (i = 0; i < bytes_read; i++) {
                            if (0 == (i % 16)) printf("\n%04ld:", i);
                            printf(" %02x", buf[i]);
                        }
                        printf("\n\n");

                        if (!valid_request(buf, bytes_read)) {
                            printf("read insufficient bytes (%ld)\n", bytes_read);
                            // TODO properly flush buffers and recover
                            break;
                            continue;
                        }
                        m.DstAddr = *buf;
                        m.Length = bytes_read - 1UL;
                        memcpy(m.Data, &buf[1], m.Length);
                        if (6 == (m.Data[0] >> 2) && 1 == m.Data[4]) {
                            post_devid(&m, server_sock);
                        } else {
                            post_illegal(&m, server_sock);
                        }
                        bzero(buf, sizeof(buf));
                    }
                    if (0 == bytes_read) break;
                    if (bytes_read < 0) {
                        printf("recv failure: %s\n", strerror(errno));
                        break;
                    }
                }
            }
        } while (false);

        printf("Closing connection to server\n");
        close(server_sock);

    } while (false);

    return 0;
}


/*!
 * Post a mock Device ID back to the requestor
 *
 * \author bjkurlex (11/13/15)
 *
 * \param m The request message received from the requestor
 * \param server_sock Where to write the response
 *
 * \return int errno value (0 on success)
 */
static int post_devid(Message_t const* const m, int server_sock)
{
    Message_t   m_rsp;
    IpmbReq_t*  req;
    IpmbResp_t* rsp        = (IpmbResp_t*)(&m_rsp.Data[0]);
    uint8_t     cksum      = 0;
    int         i;
    uint8_t     buf[MAX_IPMB_PKT_SIZE + 1]; // receive buffer

    if (NULL == m) return EINVAL;
    if (m->Length < 6) return EINVAL;
    if (0 == m->DstAddr) return EINVAL;
    if ((m->Data[0] % 2) != 0) return EINVAL;

    bzero(buf, sizeof(buf));
    bzero(&m_rsp, sizeof(m_rsp));

    req = (IpmbReq_t*)m->Data;
    m_rsp.DstAddr = req->MasterAddr;    // Save the slave address to respond to
    rsp->NetFn = req->NetFn | (1 << 2); // Response is +1 NetFn request #
    rsp->Checksum = m_rsp.DstAddr + rsp->NetFn;
    rsp->Checksum ^= 0xFF;
    rsp->Checksum++;
    rsp->SlaveAddr = I2C_ADDR << 1;     // Our configured slave address (8-bit)
    rsp->SeqNum = req->SeqNum;          // Some reuse the same seq, some use a new one
    rsp->Command = req->Command;
    rsp->CompCode = 0x0;               // Illegal NetFn or command
    m_rsp.Length = 6UL;

    printf("Addy:      %02X\n", m_rsp.DstAddr);
    printf("NetFn/Lun: %02X (%02X/%02X)\n", rsp->NetFn, (rsp->NetFn >> 2), rsp->NetFn & 3);
    printf("Checksum:  %02X\n", rsp->Checksum);
    printf("SlaveAddr: %02X\n", rsp->SlaveAddr);
    printf("SeqNum:    %02X\n", rsp->SeqNum);
    printf("Command:   %02X\n", rsp->Command);
    printf("CC:        %02X\n", rsp->CompCode);
    printf("Data:      ");
    cksum = rsp->SlaveAddr + rsp->Command + rsp->SeqNum + rsp->CompCode;

    for (i = 0; i < 15; i++) {
        rsp->Data[i] = i;
        cksum += rsp->Data[i];
        printf("%02X ", rsp->Data[i]);
        m_rsp.Length++;
    }
    printf("\n");
    cksum ^= 0xFF;
    cksum++;
    m_rsp.Length++;
    rsp->Data[i] = cksum;
    printf("Checksum:  %02X\n", rsp->Data[i]);

    return post_msg(&m_rsp, server_sock);
}


static int post_illegal(Message_t const* const m, int server_sock)
{
    Message_t   m_rsp;
    IpmbReq_t*  req;
    IpmbResp_t* rsp        = (IpmbResp_t*)(&m_rsp.Data[0]);

    if (NULL == m) return EINVAL;
    if (m->Length < 6) return EINVAL;
    if (0 == m->DstAddr) return EINVAL;
    if ((m->Data[0] % 2) != 0) return EINVAL;


    req = (IpmbReq_t*)m->Data;
    m_rsp.DstAddr = req->MasterAddr;    // Save the slave address to respond to
    rsp->NetFn = req->NetFn | (1 << 2); // Response is +1 NetFn request #
    rsp->Checksum = m_rsp.DstAddr + rsp->NetFn;
    rsp->Checksum ^= 0xFF;
    rsp->Checksum++;
    rsp->SlaveAddr = I2C_ADDR << 1;     // Our configured slave address (8-bit)
    rsp->Command = req->Command;
    rsp->SeqNum = req->SeqNum;          // Some reuse the same seq, some use a new one
    rsp->CompCode = 0xc1;               // Illegal NetFn or command
    rsp->Data[0] = rsp->SlaveAddr + rsp->Command + rsp->SeqNum + rsp->CompCode;
    rsp->Data[0] ^= 0xFF;
    rsp->Data[0]++;
    m_rsp.Length = 7;

    return post_msg(&m_rsp, server_sock);
}


/*!
 * Given a crafted message, post it to the socket
 *
 * \author bjkurlex (11/13/15)
 *
 * \param m Message to post
 * \param sock socket to post message to
 *
 * \return int Number of bytes written, or -1 on error (possibly returned by
 *             send).  errno is set implicitly or explicitly
 *
 */
static int post_msg(Message_t const* const m, int sock)
{
    ssize_t     write_size;
    uint8_t     buf[MAX_IPMB_PKT_SIZE + 1]; // receive buffer

    if (NULL == m) {
         errno = EINVAL;
         return -1;
    }

    bzero(buf, sizeof(buf));

    *buf = m->DstAddr;
    memcpy(&buf[1], m->Data, m->Length);

    buffer_dump("sending message (%ld bytes):", buf, m->Length + 1UL, m->Length + 1UL);

    write_size = sys_send(sock, buf, m->Length + 1UL, MSG_EOR | MSG_NOSIGNAL);
    if (write_size < 0) {
        debug(LOG_WARNING, "Error responding: %s\n", strerror(errno));
    }
    if ((size_t)write_size != m->Length + 1UL) {
        debug(LOG_WARNING, "Didn't write entire message (wrote %ld, intended %ld)\n", write_size, m->Length + 1UL);
    }
    return write_size;
}
