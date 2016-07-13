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
 * \file cyMUX.c
 *
 * \brief driver MUX for the Cypress USB to I2C
 *
 */


#include "connection_manager.h"
#include "receiver_thread.h"
#include "sender_thread.h"
#include "worker_thread.h"
#include "mux_usage.h"
#include "cy_i2c.h"
#include "configs.h"
#include "stringify.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


static void daemonize(void);
static int  setup_server(void);


bool gRunning = true;


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
    bool    foreground;             // Whether to leave in the foreground
    int     sd;                     // Socket descriptor for new connections

    /**
     * Initialize the debug/verbosity subsystem
     */
    init_diag();

    foreground = process_commandline(argc, argv);   // Set up environent

    if (!foreground) {
        daemonize();
    }

    /**
     * Initialized the driver to the CY7C65215
     */
    init_i2c();

    /**
     * Try to open and bind server socket.  Since only one task can
     * bind and listen to it, this also acts as a bit of mutex to
     * ensure only one instance is running.
     */
    sd = setup_server();

    /**
     * At this point, we know we are exclusively running, spin up
     * the various threads to handle i2c messaging and IPMB
     * dispatching between the threads
     */
    start_connection_manager();
    start_worker();
    start_i2c_sender();
    start_i2c_receiver();

#if 0
    if (!foreground) {

        /* Close stdin, stdout, stderr */

        close(0);
        close(1);
        close(2);
    }
#endif

    /**
     * Lastly, use the main process to handle new socket connections
     * and spin off threads to handle them.
     */
    process_new_connections(sd);

    /**
     * If we reach here, we've been requested to perform an orderly
     * shut down
     */
    close(sd);      // First close another new connection requests
    stop_connection_manager();
    stop_worker();
    stop_i2c_sender();
    stop_i2c_receiver();
    cleanup_i2c();
    cleanup_diag();

    return 0;
}


/*!
 * Place the process in the background.  Detach from the parent process tree
 * (keep from being reaped when the parent exits) and close all of the standard
 * I/O streams (stdin, stdout, stderr).
 *
 * \author bjkurlex (11/2/15)
 *
 * \param void Number of arguments passed to program
 */
static void daemonize(void)
{
    int pid;


#if 0
    if (gVerbosity) {
        gVerbosity = 0;     // Prevent any further logging to the console
        fprintf(stderr, "backgrounding as a daemon\n");
        fprintf(stderr, "All standard I/O will be closed\n");
    }
#endif

    /* Spawn ourself into the background */

    pid = fork();
    if (0 == pid) {

        /* Child */

        setsid();   // New session (keep from reaping when parent exits)
    }
    else if (pid < 0) {

        /* Error during fork() */

        perror("cyMUX: Unable to background");
        exit(-1);

    }
    else {

        /* Parent */

        usleep(100 * MILLISEC); // Give child a chance to change to new session
        exit(0);                // Parent process exits program
    }

    /* Child process will return to caller */
}


/*!
 * Create the server socket.   This is the socket that is used/bound to set up
 * new responder/client connections.   The program is exited with an error
 * message and non-zero status code on failure to set this up.
 *
 * \author bjkurlex (11/2/15)
 *
 * \param void
 *
 * \return socket descriptor to listen on for new connections
 *
 */
static int setup_server(void)
{
    int sd;         // Socket descriptor for new connections
    int val = 1;    // Enable reuse
    struct sockaddr_in server;     // listener/registration defintion for socket

    debug(LOG_INFO, "Creating server socket\n");
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sd) {
        perror("cyMUX: Could not create listener socket");
        exit(1);
    }
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    debug(LOG_INFO, "Binding server socket to localhost interface\n");

    server.sin_family = AF_INET;
    server.sin_port = htons(LISTENER_PORT);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);    // Only services on the
                                                        // localhost can connect
                                                        // to the MUX

    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("cyMUX: Unable to bind to server port " xstr(LISTENER_PORT));
        if (EADDRINUSE == errno) fprintf(stderr, "Is another server running?\n");
        exit(1);
    }

    debug(LOG_INFO, "Start listener to server socket port %d\n", LISTENER_PORT);
    if (listen(sd, 4) < 0) {
        perror("cyMUX: Unable to listen to server port " xstr(LISTENER_PORT));
        exit(1);
    }

    return sd;
}
