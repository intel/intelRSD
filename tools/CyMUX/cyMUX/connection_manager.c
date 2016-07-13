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
 * \file connection_manager.c
 *
 * \brief The main thread that handles dispatching of IPMB messages between the
 *        i2c threads and the client/responder threads
 *
 */


#include "connection_manager.h"
#include "sender_thread.h"
#include "worker_thread.h"
#include "configs.h"
#include "shim.h"
#include "ll.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


static void* client_thread(void*);
static int   lock_connection_data(void);
static int   unlock_connection_data(void);
static bool  client_registration(int client_sock, connection_t* client_type);
static bool  reserve_connection_resource(connection_t client_type, int sock, int* resource);
static void  unreserve_connection_resource(int resource);
static int   ack_registration(int sock);
static int   nack_registration(int sock);
static void  dispatch_as_responder(int resource);
static void  dispatch_as_client(int resource);
static int   post_no_responder(Message_t const* const m);
static void* client_responder_thread(void* pArg);
static void  ping_responder(void);


typedef struct {
    pthread_t       id;                 // Thread id of the client/responder
    int             client_socket;      // socket associated with the client/responder
} Resource_t;


static pthread_mutex_t gConnection_mutex;
static int             gResponder_idx                  = -1;
static Resource_t      gDispatchResource[MAX_SOCKET_THREADS];
static pthread_t       gThread_id[MAX_SOCKET_THREADS];
static int             gConnections_in_use             = 0;
static MsgQ_t          gResponderQ;


MsgQ_t                 gClientQ;


#define RESPONDER_QUEUE_SIZ 5


/*!
 *
 * Manage new connection requests.  Spawns new client thread, dedicated to
 * managing requests for the duration of the connection.  Routine only exits
 * when a orderly shutdown requests is provided.
 *
 * \author bjkurlex (11/3/15)
 *
 * \param server_sock server socket already bound and listening for new
 *                    connections
 *
 */
void process_new_connections(int server_sock)
{
    int                client_sock;    // The new client socket connection
    struct sockaddr_in client;         // Info on the new client socket connection
    socklen_t          sock_len;       // size of the above data info
    pthread_t          thread_id;
    int*               param;


    assert(server_sock > 0);

    do {
        debug(LOG_INFO, "Waiting for incoming connections...\n");

        /* Wait for a new connection request */
        sock_len = sizeof(struct sockaddr_in);
        client_sock = accept(server_sock, (struct sockaddr*)&client, &sock_len);

        if (client_sock < 0) {
            /* Unexplained error, log it and move on */
            debug(LOG_ERR, "new client socket connection failed: %s\n",
                  strerror(errno));

            continue;
        }

        debug(LOG_INFO, "New client connection\n");

        /* spin up a thread to handle the new connection */
        param = malloc(sizeof(int));
        assert(NULL != param);
        *param = client_sock;

        if (pthread_create(&thread_id, NULL, client_thread, param) < 0) {
            /* Unexplained error, log it and move on */
            debug(LOG_CRIT, "failed to start new client thread connection: %s\n",
                  strerror(errno));

            close(client_sock);
            free(param);
            continue;
        }
    } while (true);
}


/*!
 * Thread responsible for handling the socket connection.
 * Performs registration, and determining mode.  Then launches the appropriate
 * manager for the registration type
 *
 * \author bjkurlex (11/3/15)
 *
 * \param pArg the new client socket to use
 *
 * \return void* pthread exit value (not used)
 *
 */
static void* client_thread(void* pArg)
{
    int          client_sock;
    int          sflags;
    int          resource;
    connection_t client_type;


    assert(NULL != pArg);

    client_sock = *(int*)pArg;
    assert(client_sock >= 0);

    free(pArg);

    debug(LOG_INFO, "client(%d) connected\n", client_sock);

    sflags = fcntl(client_sock, F_GETFL, 0);
    fcntl(client_sock, F_SETFL, sflags | O_NONBLOCK);

    /* Start connection with simple registration to determine type */
    if (client_registration(client_sock, &client_type)) {

        /* attempt to reserve resources */
        if (CT_INVALID == client_type) {

            debug(LOG_ERR, "%ld: rejecting unrecognized type (%d) connection\n",
                  pthread_self(), client_sock);
            nack_registration(client_sock);
        }
        else if (reserve_connection_resource(client_type, client_sock, &resource)) {

            if (CT_RESPONDER == client_type) dispatch_as_responder(resource);
            else                             dispatch_as_client(resource);

            unreserve_connection_resource(resource);
        }
        else { // No space (e.g., responder taken, or too many clients)

            debug(LOG_ERR, "%ld: rejecting client (%d) connection\n",
                  pthread_self(), client_sock);
            nack_registration(client_sock);
        }

    }

    debug(LOG_INFO, "%ld: Terminating client (%d) connection\n",
          pthread_self(), client_sock);
    close(client_sock);


    // We are going to self destruct, so detach so no resources are held
    // (no reaper/join needed)
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}


/*!
 * Initialize all of the thread related variables used by/between the connection
 * manager threads
 *
 * \author bjkurlex (11/3/15)
 *
 * \param void
 *
 */
void start_connection_manager(void)
{
    int                 retval;
    int                 i;
    pthread_mutexattr_t attr;


    retval = msgq_init(&gResponderQ, RESPONDER_QUEUE_SIZ);
    if (0 != retval) {
        debug(LOG_EMERG, "Unable to initialize responder message queue: %s\n",
              strerror(retval));
        exit(1);
    }

    retval = msgq_init(&gClientQ, RESPONDER_QUEUE_SIZ*2);
    if (0 != retval) {
        debug(LOG_EMERG, "Unable to initialize client message queue: %s\n",
              strerror(retval));
        exit(1);
    }


    /* Initialize mutex used for thread variables */
    retval = pthread_mutexattr_init(&attr);
    if (0 != retval) {
        debug(LOG_EMERG, "Unable to initialize mutex attribute: %s\n",
              strerror(retval));
        exit(1);
    }

    retval = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (0 != retval) {
        debug(LOG_EMERG, "Unable to set mutex attribute to recursive: %s\n",
              strerror(retval));
        exit(1);
    }

    retval = pthread_mutex_init(&gConnection_mutex, NULL);
    if (0 != retval) {
        debug(LOG_EMERG, "Unable to initialize connection mutex: %s\n",
              strerror(retval));
        exit(1);
    }


    retval = pthread_mutexattr_destroy(&attr);
    if (0 != retval) {
        debug(LOG_EMERG, "Unable to destroy mutex attribute: %s\n",
              strerror(retval));
        exit(1);
    }


    /* Initialize the remaining thread variables */

    fprintf(stderr, "Launching %d responder threads to processes responses\n",
            MAX_SOCKET_THREADS);
    for (i = 0; i < MAX_SOCKET_THREADS; i++) {
        gDispatchResource[i].id = -1;
        gDispatchResource[i].client_socket = -1;
        if (pthread_create(&gThread_id[i], NULL, client_responder_thread, NULL) < 0) {
            // Unexplained error, log it and move on
            fprintf(stderr, "failed to start new client responder thread #%d: %s\n",
                  i, strerror(errno));
            exit(1);
        }
    }

    gResponder_idx = -1;
    gConnections_in_use = 0;
}


/*!
 * Destroy all of the thread related variables used by/between the connection
 * manager threads
 *
 * \author bjkurlex (11/13/15)
 *
 * \param void
 *
 */
void stop_connection_manager(void)
{
    int retval;


    //TODO clean up any outstanding threads, connections

    retval = msgq_destroy(&gResponderQ);
    assert(0 == retval);

    retval = msgq_destroy(&gClientQ);
    assert(0 == retval);

    retval = pthread_mutex_destroy(&gConnection_mutex);
    assert(0 == retval);
}


/*!
 * Lock the connection manager thread data for read/write
 *
 * \author bjkurlex (11/3/15)
 *
 * \param void
 *
 * \return int success (0) or error status of failing request during lock attempt
 *
 */
static int lock_connection_data(void)
{
    struct timespec ts;


    ts.tv_sec = time(NULL) + gTimeout;
    ts.tv_nsec = 0L;
    return pthread_mutex_timedlock(&gConnection_mutex, &ts);
}


/*!
 * Unlock the connection manager thread data
 *
 * \author bjkurlex (11/3/15)
 *
 * \param void
 *
 * \return int success (0) or error status of unlock request attempt
 *
 */
static int unlock_connection_data(void)
{
    return pthread_mutex_unlock(&gConnection_mutex);
}


/*!
 * Performs the first part of the handshake with the client--waiting for the
 * client's registration request.   This should be either a 'C' for client or
 * 'R' for responder.  Waits for the client to make the request, but only for
 * gTimeout seconds before it will then fail and close the connection.
 *
 * \author bjkurlex (11/4/15)
 *
 * \param client_sock The socket opened to communicate with the client
 * \param client_type The registration type requested by the client
 *
 * \return bool success or failure receiving registration request
 *
 */
static bool client_registration(int client_sock, connection_t* client_type)
{
    fd_set         rfds;
    struct timeval tv;
    int            retval;
    ssize_t        read_size;
    uint8_t        buf[MAX_IPMB_PKT_SIZE];
    pthread_t      id = pthread_self();


    assert(client_sock >= 0);
    assert(NULL != client_type);

    debug(LOG_INFO, "%ld: Waiting for client(%d) registration request\n",
          id, client_sock);

    tv.tv_sec = gTimeout;
    tv.tv_usec = 0;

    FD_ZERO(&rfds);
    FD_SET(client_sock, &rfds);

    /* sleep on the pipe,... but only for so long */

    retval = select(client_sock + 1, &rfds, NULL, NULL, &tv);

    debug(LOG_DEBUG, "%ld: select for client (%d) on registration recv %d\n",
          id, client_sock, retval);

    if (0 == retval) {
        /* timeout happened */
        debug(LOG_INFO, "%ld: select timed out waiting on client(%d) registration request\n",
              id, client_sock);

        return false;
    }
    else if (-1 == retval) {

        debug(LOG_WARNING, "%ld: select failed waiting on client(%d) registration request: %s\n",
              id, client_sock, strerror(errno));

        return false;               // Failed to provide registration data
    }


    /* data in the pipe, read it */
    read_size = recv(client_sock, buf, 1, 0);

    if (0 == read_size) {
        return false;               // Client closed the pipe
    }
    else if (-1 == read_size) {

        /* error trying to read the pipe */

        debug(LOG_WARNING, "%ld: recv failed waiting on client(%d) registration request: %s\n",
              id, client_sock, strerror(errno));

        return false;               // Failed to provide registration data
    }

    assert(1 == read_size);
    switch (*buf) {
    case 'C':
    case 'c':
        *client_type = CT_CLIENT;
        break;

    case 'R':
    case 'r':
        *client_type = CT_RESPONDER;
        break;

    default:
        /* The client did provide something, so we will NAK the request */
        *client_type = CT_INVALID;
        break;
    }

    return true;                   // provided registration data
}


/*!
 * Atomically attempt to register the client/responder request.
 * First, there needs to be an available resource so we can track the thread
 * and shut it down in an orderly matter when requested.  Next, need to
 * confirm if it is a responder request that it is available.
 *
 * \author bjkurlex (11/3/15)
 *
 * \param client_type the type of connection this is (client or responder)
 * \param sock the socket between the client/responder and the server
 * \param resource the resource index reserved if successful
 *
 * \return bool success or failure of the requested registration
 *
 */
static bool reserve_connection_resource(connection_t client_type, int sock, int* resource)
{
    int  retval;
    int  slot;
    int  slot_avail = -1;
    bool success    = false;
    pthread_t id    = pthread_self();


    if (CT_RESPONDER != client_type && CT_CLIENT != client_type) {
        debug(LOG_EMERG, "Invalid client type (%d) encountered reserving connection resource\n",
              client_type);
        exit(1);
    }

    if (NULL == resource) {
        debug(LOG_EMERG, "Null pointer passed as resource to reserver_connection_resource()\n");
        exit(1);
    }
    assert(NULL != resource);

    /* Lock the thread data area */
    retval = lock_connection_data();
    if (0 != retval) {
        debug(LOG_ALERT, "%ld: Unable to lock connection manager thread data: %s\n",
              id, strerror(retval));
        return false;
    }

    for (slot = 0; slot < MAX_SOCKET_THREADS; slot++) {
        if (-1UL == gDispatchResource[slot].id) {
            slot_avail = slot;
            break;
        }
    }

    if (!(slot_avail < 0)) {
        if (CT_RESPONDER == client_type) {
            if (-1 == gResponder_idx) {
                /* responder request that is available, register it */
                gResponder_idx = slot_avail;
                success = true;
            }
            else {
                // Ping the responder to make sure it is still alive
                // Note, this is to work around the issue that terminated sockets
                // don't necessarily tell the other end they've been closed
                // (particularly if the program is abrubtly terminated).  However,
                // trying to write to a closed socket *will* return an error that
                // can be dealt with.
                debug(LOG_NOTICE, "%ld: New responder request (s: %d) pinging existing (r: %d; s: %d) to see if still alive\n",
                      id, sock, gResponder_idx, gDispatchResource[gResponder_idx].client_socket);
                ping_responder();
                // Let the connection fail...this time.  If the ping fails,
                // the connection will automatically close and be available upon
                // the next connection request (which the responder should likely
                // retry).
            }
        }
        else {
            /* client request, register it */
            success = true;
        }
    }
    else {
        debug(LOG_NOTICE, "%ld: No slots available for (s: %d)\n", id, sock);
    }

    if (success) {
        gDispatchResource[slot_avail].id = id;
        gDispatchResource[slot_avail].client_socket = sock;
        *resource = slot_avail;

        debug(LOG_INFO, "%ld: reserved (r: %d; s: %d)\n", id, slot_avail, sock);
    }
    else {
        debug(LOG_INFO, "%ld: unable to reserved (s: %d)\n", id, sock);
    }

    /* release the thread data area */
    retval = unlock_connection_data();
    if (0 != retval) {
        // Shouldn't ever fail to unlock
        debug(LOG_EMERG, "Unable to unlock connection data: %s\n",
              strerror(retval));
        exit(1);
    }

    return success;
}


/*!
 * Given the current thread id, deregister/unreserve it from the resources.
 *
 * \author bjkurlex (11/3/15)
 *
 * \param resource The resource to unreserve
 *
 */
static void unreserve_connection_resource(int resource)
{
    int       retval;
    pthread_t pthread_id = pthread_self();


    /* Lock the thread data area */
    retval = lock_connection_data();
    assert(pthread_id == gDispatchResource[resource].id);

    if (0 != retval) {
        debug(LOG_ALERT, "%ld: Unable to lock connection manager thread data: %s\n",
              pthread_id, strerror(retval));

        return; // Ignore.  We may have gotten called due to a previous error
    }


    if (gResponder_idx == resource) {
        debug(LOG_INFO, "%ld: unregistering responder (%d)\n",
              pthread_id, resource);
        gResponder_idx = -1;          // Remove/unreserve
    }
    gDispatchResource[resource].id = -1;   // Remove/unreserve
    debug(LOG_INFO, "%ld: unreserving (%d)\n", pthread_id, resource);
    retval = ll_flush(resource);             // Drop any outstanding requests related to this resource
    if (0 != retval) {
        debug(LOG_ALERT, "Unable to flush chain for resource %d: %s\n",
              resource, strerror(retval));
        exit(1);
    }

    // closed within dispatcher
    gDispatchResource[resource].client_socket = -1;


    /* release the thread data area */
    retval = unlock_connection_data();
    if (0 != retval) {
        debug(LOG_ALERT, "%ld: Unable to unlock connection manager thread data: %s\n",
              pthread_id, strerror(retval));
        exit(1);
    }
}


/*!
 * Send acknowledgement back to the client socket
 *
 * \author bjkurlex (11/3/15)
 *
 * \param client_sock The client socket
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
static int ack_registration(int client_sock)
{
    fd_set         wfds;
    struct timeval tv;
    ssize_t        write_size;
    int            retval;
    uint8_t        buf[]        = "OK";
    pthread_t      id = pthread_self();


    assert(client_sock >= 0);

    debug(LOG_INFO, "%ld: Waiting for client (s: %d) socket to be ready for write\n",
          id, client_sock);

    tv.tv_sec = gTimeout;
    tv.tv_usec = 0;

    FD_ZERO(&wfds);
    FD_SET(client_sock, &wfds);

    /* sleep on the pipe,... but only for so long */

    retval = select(client_sock + 1, NULL, &wfds, NULL, &tv);

    if (-1 == retval) {
        retval = errno;

        debug(LOG_ERR, "%ld: select failed waiting on client (s: %d) registration response: %s\n",
              id, client_sock, strerror(retval));

        return retval;              // Failed to write acknowledgement
    }

    debug(LOG_DEBUG, "%ld: Sending %s to client (s: %d)\n",
          id, buf, client_sock);

    /* pipe ready to write to, write it */
    write_size = sys_send(client_sock, buf, 2, MSG_EOR | MSG_NOSIGNAL);

    if (write_size < 0) {
        retval = errno;

        debug(LOG_ERR, "%ld: write failed on client (s: %d) registration response: %s\n",
              id, client_sock, strerror(retval));

        return retval;              // Failed to write acknowledgement
    }

    return 0;
}


/*!
 * Send rejection acknowledgement back to the client socket
 *
 * \author bjkurlex (11/3/15)
 *
 * \param client_sock The client socket
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
static int nack_registration(int client_sock)
{
    fd_set         wfds;
    struct timeval tv;
    ssize_t        write_size;
    int            retval;
    uint8_t        buf[]        = "NOK";


    assert(client_sock >= 0);

    debug(LOG_ERR, "%ld: NACKing client(%d) socket\n", pthread_self(), client_sock);
    debug(LOG_INFO, "%ld: Waiting for client(%d) socket to be ready for write\n",
          pthread_self(), client_sock);

    tv.tv_sec = gTimeout;
    tv.tv_usec = 0;

    FD_ZERO(&wfds);
    FD_SET(client_sock, &wfds);

    /* sleep on the pipe,... but only for so long */

    retval = select(client_sock + 1, NULL, &wfds, NULL, &tv);

    if (-1 == retval) {
        retval = errno;

        debug(LOG_ERR, "%ld: select failed waiting on client(%d) registration response: %s\n",
              pthread_self(), client_sock, strerror(retval));

        return retval;              // Failed to write acknowledgement
    }

    debug(LOG_INFO, "%ld: Sending %s to client(%d)\n",
          pthread_self(), buf, client_sock);

    /* pipe ready to write to, write it */
    write_size = sys_send(client_sock, buf, 3, MSG_EOR | MSG_NOSIGNAL);

    if (write_size < 0) {
        retval = errno;

        debug(LOG_ERR, "%ld: write failed on client(%d) registration response: %s\n",
              pthread_self(), client_sock, strerror(retval));

        return retval;              // Failed to write acknowledgement
    }

    return 0;
}


/*!
 * process any further socket communications as the responder
 *
 * \author bjkurlex (11/10/15)
 *
 * \param resource index into the resource table for this thread
 *
 */
static void dispatch_as_responder(int resource)
{
    fd_set    rfds;
    fd_set    wfds;
    struct timeval tv;
    ssize_t   bytes_read;
    ssize_t   bytes_sent;
    int       retval;
    Message_t msg;
    uint8_t   buf[MAX_IPMB_PKT_SIZE + 1]; // receive buffer
    pthread_t id = gDispatchResource[resource].id;
    int       responder_sock = gDispatchResource[resource].client_socket;


    assert(responder_sock > 0);
    debug(LOG_INFO, "%ld: dispatched responder (%d)\n", id, responder_sock);
    if (0 == ack_registration(responder_sock)) {

        while (gRunning) {
            debug(LOG_INFO, "%ld: Responder thread waiting for a request (r: %d; s: %d)\n",
                  id, resource, responder_sock);

            retval = msgq_get(&gResponderQ, &msg);
            assert(0 == retval);
            msgq_debug(&gResponderQ, "gResponderQ");

            message_dump("%ld: Received message for responder (r: %d; s: %d)",
                         &msg, id, resource, responder_sock);
            //TODO validate message received in queue

            *buf = msg.DstAddr;
            memcpy(&buf[1], msg.Data, msg.Length);

            tv.tv_sec = gTimeout;
            tv.tv_usec = 0;

            FD_ZERO(&wfds);
            FD_SET(responder_sock, &wfds);

            debug(LOG_INFO, "%ld: Waiting to write to responder (r: %d; s: %d)\n",
                  id, resource, responder_sock);
            if ((retval = select(responder_sock + 1, NULL, &wfds, NULL, &tv)) > 0) {

                debug(LOG_INFO, "%ld: Writing to responder (r: %d; s: %d; %ld bytes)\n",
                      id, resource, responder_sock, msg.Length + 1UL);
                bytes_sent = sys_send(responder_sock, buf, msg.Length + 1UL, MSG_EOR | MSG_NOSIGNAL);
                if (bytes_sent < 0) {
                    debug(LOG_ERR, "%ld: Write error to responder (r: %d; s: %d): %s..dropping responder connection\n",
                          id, resource, responder_sock, strerror(errno));
                    close(responder_sock);
                    retval = post_no_responder(&msg);
                    assert(0 == retval);
                    return;
                }
                else if ((size_t)bytes_sent != (msg.Length + 1UL)) {
                    debug(LOG_ERR, "%ld: Write error to responder (r: %d; s: %d) (sent %ld != %ld)..dropping responder connection\n",
                          id, resource, responder_sock, msg.Length + 1UL, bytes_sent);
                    close(responder_sock);
                    retval = post_no_responder(&msg);
                    assert(0 == retval);
                    return;
                }
            }
            else if (retval < 0) {
                debug(LOG_ERR, "%ld: write select error waiting on responder (r: %d; s: %d): %s..dropping responder connection\n",
                      id, resource, responder_sock, strerror(errno));
                close(responder_sock);
                retval = post_no_responder(&msg);
                assert(0 == retval);
                return;
            }
            else {
                debug(LOG_ERR, "%ld: responder (r: %d; s: %d) time out on write select..dropping responder connection\n",
                      id, resource, responder_sock);
                close(responder_sock);
                retval = post_no_responder(&msg);
                assert(0 == retval);
                return;
            }
            FD_ZERO(&rfds);
            FD_SET(responder_sock, &rfds);

            tv.tv_sec = gTimeout;
            tv.tv_usec = 0;

            if (0 == (retval = select(responder_sock + 1, &rfds, NULL, NULL, &tv))) {
                debug(LOG_ERR, "%ld: responder (r: %d; s: %d) time out on read select..dropping responder connection\n",
                      id, resource, responder_sock);
                close(responder_sock);
                retval = post_no_responder(&msg);
                assert(0 == retval);
                return;
            }
            else if (retval < 0) {
                debug(LOG_ERR, "%ld: Select error waiting to read from responder (r: %d; s: %d): %s..dropping responder connection\n",
                      id, resource, responder_sock, strerror(errno));
                close(responder_sock);
                retval = post_no_responder(&msg);
                assert(0 == retval);
                return;
            }

            // socket ready with bytes waiting to be read
            debug(LOG_INFO, "%ld: read select for responder (r: %d; s: %d) recv %d\n",
                                           id, resource, responder_sock, retval);

            bytes_read = recv(responder_sock, buf, sizeof(buf), 0);
            if (0 == bytes_read) {
                debug(LOG_ERR, "%ld: responder (r: %d; s: %d) time out on read..dropping responder connection\n",
                      id, resource, responder_sock);
                close(responder_sock);
                retval = post_no_responder(&msg);
                assert(0 == retval);
                return;
            }
            else if (bytes_read < 0) {
                debug(LOG_ERR, "%ld: dispatched responder (r: %d; s: %d) IPMB recv failure: %s..dropping responder connection\n",
                      id, resource, responder_sock, strerror(errno));
                close(responder_sock);
                return;
            }

            // Bytes successfully read
            buffer_dump("%ld: dispatched responder (r: %d; s: %d) received:",
                        buf, bytes_read, id, resource, responder_sock);

            //TODO validate message received from responder
            // Message received and valid, post it it for processing.
            msg.DstAddr = *buf;
            msg.Length = bytes_read - 1;
            memcpy(msg.Data, &buf[1], msg.Length);
            debug(LOG_INFO, "Posting message to worker queue\n");
            message_dump("%ld: Posting message to to worker queue", &msg, id);
            retval = msgq_post(&gWorkerQ, &msg);
            assert(0 == retval);
            msgq_debug(&gWorkerQ, "gWorkerQ");
        }
    }
}


/*!
 * process any further socket communications as the client
 *
 * \author bjkurlex (11/10/15)
 *
 * \param resource index into the resource table for this thread
 *
 */
static void dispatch_as_client(int resource)
{
    fd_set    rfds;
    int       retval;
    ssize_t   bytes_read = 0;
    uint8_t   buf[MAX_IPMB_PKT_SIZE + 1]; // receive buffer
    Message_t msg;
    pthread_t id = gDispatchResource[resource].id;
    int       client_sock = gDispatchResource[resource].client_socket;


    assert(client_sock >= 0);
    debug(LOG_INFO, "%ld: dispatched client (r: %d; s: %d)\n",
          id, resource, client_sock);
    if (0 == ack_registration(client_sock)) {

        FD_ZERO(&rfds);
        FD_SET(client_sock, &rfds);

        // Add timeout on read?   Just how long can the client persist on the connection?
        while ((retval = select(client_sock + 1, &rfds, NULL, NULL, NULL)) > 0) {
            debug(LOG_INFO, "%ld: select for client (r: %d; s: %d) recv %d\n",
                  id, resource, client_sock, retval);

            bytes_read = recv(client_sock, buf, sizeof(buf), 0);
            if (0 == bytes_read) {
                debug(LOG_INFO, "%ld: client (r: %d; s: %d) closed socket\n",
                      id, resource, client_sock);
                break; // closed socket
            }
            else if (bytes_read < 0) {
                debug(LOG_ERR, "%ld: dispatched client (r: %d; s: %d) IPMB recv failure: %s\n",
                      id, resource, client_sock, strerror(errno));
                return;
            }
            buffer_dump("%ld: dispatched client (r: %d; s: %d) received:",
                        buf, bytes_read, id, resource, client_sock);

            if (!valid_request(buf, bytes_read)) {
                debug(LOG_ERR, "%ld: client (r: %d; s: %d) sent malformed IPMB request, closing connection\n",
                      id, resource, client_sock);
                break;
            }

            // Message received and valid, post it it for processing.
            msg.client_resource = resource;
            msg.DstAddr = *buf;
            msg.Length = bytes_read - 1;
            memcpy(msg.Data, &buf[1], msg.Length);
            msg.client_resource = resource;
            retval = msgq_post(&gWorkerQ, &msg);
            assert(0 == retval);
            msgq_debug(&gWorkerQ, "gWorkerQ");

            FD_ZERO(&rfds);
            FD_SET(client_sock, &rfds);
            debug(LOG_INFO, "%ld: looping client (r: %d; s: %d) for more data\n",
                  id, resource, client_sock);
        }
        debug(LOG_INFO, "%ld: client (r: %d; s: %d) exiting loop\n",
              id, resource, client_sock);

        if (-1 == retval) {

            debug(LOG_ERR, "%ld: select failed waiting on client (r: %d; s: %d) IPMB data request: %s\n",
                  id, resource, client_sock, strerror(errno));
        }
        else if (0 == retval) {
            debug(LOG_ERR, "%ld: dispatched client (r: %d; s: %d) select failure waiting for IPMB message: %s\n",
                  id, resource, client_sock, strerror(errno));
        }
    }
}


/*!
 * Take the message and determine whether there is a listening responder.  If
 * there is, send it to the responder on its corresponding socket.  If not, send
 * an illegal netfn/command back to the requestor.
 *
 * \author bjkurlex (11/12/15)
 *
 * \param m The message to send to the responder
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
int post_responderQ(Message_t const* const m)
{
    int     retval;


    /* Lock the thread data area */
    retval = lock_connection_data();
    if (0 != retval) {
        debug(LOG_ALERT, "%ld: Unable to lock connection manager thread data: %s\n",
              pthread_self(), strerror(retval));
        return retval;
    }

    if (-1 == gResponder_idx) {
        // No responder, return an invalid/NetFn to requestor
        retval = post_no_responder(m);
        assert(0 == retval);
    }
    else {
        // Post message to responder Q
        retval = msgq_post(&gResponderQ, m);
        assert(0 == retval);
        msgq_debug(&gResponderQ, "gResponderQ");
        //TODO Add code to flush queue when the responder goes away
    }

    /* release the thread data area */
    return unlock_connection_data();
}


/*!
 * Given the message, recraft a response to the requester as an illegal
 * netfn/command.
 *
 * \author bjkurlex (11/12/15)
 *
 * \param m The request message to use to craft the response
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
static int post_no_responder(Message_t const* const m)
{
    Message_t   m_rsp;
    int         retval;

    // Flush the ResponderQ
    retval = msgq_flush(&gResponderQ);
    if (0 != retval) {
        debug(LOG_CRIT, "Can't flush Responder Q\n");
        return retval;
    }
    msgq_debug(&gResponderQ, "gResponderQ");

    if (0 != craft_error_response(m, &m_rsp, CC_DEST_UNAVAILABLE)) {   // Unavailable
        debug(LOG_CRIT, "Can't recraft message as response\n");
        return EINVAL;
    }

    debug(LOG_INFO, "Posting message to worker queue\n");
    retval = msgq_post(&gWorkerQ, &m_rsp);
    msgq_debug(&gWorkerQ, "gWorkerQ");
    return retval;
}


/*!
 *
 *
 * \author bjkurlex (11/19/15)
 *
 * \param resource
 * \param msg
 *
 * \return int
 */
int send_response(int resource, Message_t const* const msg)
{
    fd_set    wfds;
    ssize_t   bytes_sent = 0;
    int       retval;
    int       client_sock;
    uint8_t   buf[MAX_IPMB_PKT_SIZE + 1]; // receive buffer
    pthread_t id = pthread_self();

    assert(NULL != msg);
    assert(resource >= 0);

    retval = lock_connection_data();
    assert(0 == retval);
    assert(gDispatchResource[resource].id > 0);
    assert(gDispatchResource[resource].client_socket >= 0);
    assert(msg->Length < sizeof(buf));

    client_sock = gDispatchResource[resource].client_socket;

    /* release the thread data area */
    retval = unlock_connection_data();
    assert(0 == retval);    // Shouldn't ever fail to unlock

    if (client_sock < 0) {
        debug(LOG_WARNING, "%ld: socket (%d) associated with resource (%d) not valid... dropping packet\n",
              id, client_sock, resource);
        return 0;
    }

    FD_ZERO(&wfds);
    FD_SET(client_sock, &wfds);

    *buf = msg->DstAddr;
    memcpy(&buf[1], msg->Data, msg->Length);


    buffer_dump("%ld: buf ready to write to client (r: %d; s: %d): length %ld addr %02x",
                buf, msg->Length + 1UL, id, resource, client_sock,
                msg->Length + 1UL, msg->DstAddr);

    debug(LOG_INFO, "%ld: Waiting to write to client (r: %d; s: %d)\n",
          id, resource, client_sock);
    retval = select(client_sock + 1, NULL, &wfds, NULL, NULL);
    assert(retval > 0);

    bytes_sent = sys_send(client_sock, buf, msg->Length + 1UL, MSG_EOR | MSG_NOSIGNAL);
    assert(bytes_sent > 0);
    assert((size_t)bytes_sent == (msg->Length + 1UL));


    return 0;
}


/*!
 * Thread responsible for handling the responses to previous requests.
 * This thread takes the next response message from the queue and handles doling
 * out the response to the associated socket.  This is a pool of thread runners,
 * one for each of the maximum sockets we can handle so that a write request
 * doesn't block other requests from being handled.
 *
 * \author bjkurlex (11/19/15)
 *
 * \param pArg not used
 *
 * \return void* pthread exit value (not used)
 *
 */
static void* client_responder_thread(void __attribute__((__unused__))* pArg)
{
    int          resource;
    pthread_t    id = pthread_self();
    Message_t    msg;
    int          retval;

    debug(LOG_INFO, "%ld: client responder thread started\n", id);

    while (gRunning) {
        retval = msgq_get(&gClientQ, &msg);
        assert(0 == retval);

        message_dump("%ld: worker thread received a response packet, looking for groom",
                     &msg, id);

        // Marry the bride(response) to the waiting groom(request)
        retval = ll_get(&resource, &msg, false);
        assert(-1 != retval);
        if (retval) {
            debug(LOG_INFO, "%ld: matching request for received response found (resource: %d)\n", id, resource);
            // use resource to send message back
            retval = send_response(resource, &msg);
            assert(0 == retval);
        }
        else {
            // Leave the bride at the alter (drop the packet)
            debug(LOG_WARNING, "%ld: Unable to find matching request for received response...dropping\n", id);
        }
    }

    // Shouldn't actually get here as it is a forever loop.  Eh, either assert or just exit
    debug(LOG_INFO, "%ld: client responder exiting loop\n", id);


    // We are going to self destruct, so detach so no resources are held
    // (no reaper/join needed)
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}


/*!
 * routine to ping the responder.
 *
 * Note, we may need to add code later to prevent a packet storm by gating the
 * frequency in between pings.  For now, we'll make the note here.
 *
 * This simply crafts a Get Device ID to be sent to the responder with a special
 * device handle to be non-routed (dropped) when a response is received.
 *
 * \author bjkurlex (3/9/16)
 *
 * \return void
 *
 */
static void ping_responder(void)
{
    Message_t       ping;
    IpmbReq_t*      pReq = (IpmbReq_t*)(ping.Data);
    static uint8_t  seqno = 0;
    int             retval;

    ping.client_resource = PING_RESOURCE;   // Flag response is to be dropped
    ping.DstAddr = I2C_ADDR << 1;           // Send it to responder
    ping.Length = sizeof(IpmbReq_t) + 1UL;
    pReq->NetFn = 0x6 << 2 | 1;             // Get Device ID request
    pReq->MasterAddr = 0x30;
    pReq->SeqNum = seqno++;
    pReq->Command = 1;
    m_checksum(&ping);

    retval = msgq_post(&gWorkerQ, &ping);
    assert(0 == retval);
    msgq_debug(&gWorkerQ, "gWorkerQ");
}
