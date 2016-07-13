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
 * \file test_client.c
 *
 * \brief Test client of the test_client
 *
 */


#include "test_client_usage.h"
#include "configs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>


/*!
 * Entry point into the Cypress Multiplexer.   Processes command line arguments
 * and launches daemon into the background on success.
 *
 * \author bjkurlex (11/2/15)
 *
 * \param argc Number of arguments passed to program
 * \param argv Array containing the arguments passed to the program
 *
 * \return int exit status of the program
 */
int main(int argc, char *argv[])
{
    int     server_sock;            // Socket descriptor for new connections
    uint8_t buf[MAX_IPMB_PKT_SIZE+1]; // receive buffer
    ssize_t bytes_read;
    ssize_t bytes_sent;
    struct sockaddr_in server;      // listener/registration defintion for socket


    process_commandline(argc, argv);

    if (gPlanned > 0)
        plan(gPlanned);

    do {
        diag("Creating server socket");
        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == server_sock) { // Huch Klockwork
            if (gServer) ok(server_sock > 0, "Successful connection to server");
            else diag("Could not create listener socket: %s", strerror(errno));
            break;
        }
        else if (gServer) ok(server_sock > 0, "Successful connection to server");
        else if (gNo_server) ok(server_sock < 0, "Failed to connect to server");

        diag("Connecting to server socket on localhost interface");

        server.sin_family = AF_INET;
        server.sin_port = htons(LISTENER_PORT);
        server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);    // Only services on the
                                                            // localhost can connect
                                                            // to the MUX

        if (connect(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            diag("Unable to bind to server port %d: %s", LISTENER_PORT, strerror(errno));
            close(server_sock);
            break;
        }


        do {
            if (gConnection_holdtime) {
                diag("Holding connection for %ds", gConnection_holdtime);
                sleep(gConnection_holdtime);
            }

            diag("connected to server socket port %d", LISTENER_PORT);

            if (gClose_prematurely)
                break;

            bytes_sent = send(server_sock, gClient_type, gClient_type_len, 0);
            if (bytes_sent <= 0) {
                if (gHang_connection) {
                    // Sending on closed socket doesn't mean the socket is really closed,
                    // but if it errors, flag it as a passed test
                    ok(bytes_sent <= 0, "Server reset connection after hang");
                    close(server_sock);
                    break;
                }
                diag("send failure: %s", strerror(errno));
                break;
            }
            else {
                bytes_read = recv(server_sock, buf, sizeof(buf), 0);
                if (gHang_connection) {
                    // However, if we hung the connection and it wasn't caught on the
                    // send, it should definitely fail on the read
                    ok(bytes_read <= 0, "Server reset connection after hang");
                }
                else if (0 == bytes_read) break;
                else if (bytes_read < 0) {
                    diag("recv failure: %s", strerror(errno));
                    break;
                }

                if (0 == bytes_read) {
                    diag("Server response: (zero bytes)");
                }
                else {
                    ssize_t index;

                    diag("Server response: (%ld bytes)", bytes_read);
                    for (index = 0; index < bytes_read; index++) {
                        diag("%02d: %c 0x%02X", index, (isalpha(buf[index]) ? buf[index] : '.'), buf[index]);
                    }
                }
            }
        } while(false);

        diag("Closing connection to server");
        close(server_sock);

    } while (false);

    return 0;
}
