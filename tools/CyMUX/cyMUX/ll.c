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
 * \file ll.c
 *
 * \brief Simple linked list implementation to manage messages requests waiting
 *        for responses and which clients they belong to
 *
 * \note Should migrate to boost libraries when moved to C++.  The binary tree
 *       was overkill and was reduced to a linked list in the first
 *       implementation.  It turns out that for our purposes, doubly linked list
 *       is sufficient.
 *
 */


#include "ll.h"
#include "IPMB.h"
#include "debug.h"
#include "shim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>


// Link list Node elements
typedef struct llnode Lnode_t;
struct llnode {
    Message_t       request;        // Current outstanding request
    Lnode_t*        prev;           // Next outstanding request in linked list
    Lnode_t*        next;           // Previous outstanding request in linked list
};


// Linked List chain definition
typedef struct ll {
    pthread_mutex_t     lock;
    Lnode_t*            head;
} LinkedList_t;


static int ll_add(Lnode_t** n, Message_t const* const m);
static Lnode_t* ll_find(Lnode_t* const head, Message_t const* const m, bool i2c);

// shim for calloc/free for testing interception
static void sys_free (void *__ptr);
static void *sys_calloc (size_t __nmemb, size_t __size);


// Singleton
static LinkedList_t marriage_broker;


/*!
 * Initialize the linked list and its resources
 *
 * \return int 0 if successful
 *            -1 on error with errno set
 */
int ll_init(void)
{
    int retval;


    marriage_broker.head = NULL;

    retval = pthread_mutex_init(&marriage_broker.lock, NULL);
    if (retval) {
        errno = retval;
        return -1;
    }
    return 0;
}


/*!
 * Tear down the message head and its resources
 *
 * \return int 0 if successful
 *            -1 on error with errno set
 *
 */
int ll_destroy(void)
{
    int         retval;
    Lnode_t*    current = NULL;
    Lnode_t*    next = NULL;


    for (current = marriage_broker.head; NULL != current; current = next) {
        next = current->next;
        sys_free(current);
    }

    bzero(&marriage_broker, sizeof(LinkedList_t));

    retval = pthread_mutex_destroy(&marriage_broker.lock);
    if (retval) {
        errno = retval;
        return -1;
    }

    return 0;
}


/*!
 * Add node containing IPMB request and associated requestor queue to linked
 * list for later matching/retrieval.
 *
 * \param n head node/leaf node to start search with
 * \param q requestor message queue to add
 * \param m associated requestor's request
 *
 * \return int 0 if successful
 *            -1 on error with errno set
 *
 * \todo Implemented as doubly linked list for PoC.  Refactor to linked list later
 *       (use boost libraries when migrated to C++)
 *
 */
static int ll_add(Lnode_t** n, Message_t const* const m)
{
    if (NULL == n || NULL == m) {
        errno = EINVAL;
        return -1;
    }
    if (NULL == *n) {
        *n = sys_calloc(1U, sizeof(Lnode_t));
        if (NULL == *n) {
            errno = ENOMEM;
            return -1;
        }
        (*n)->request = *m;
        (*n)->prev = NULL;
        (*n)->next = NULL;
    }
    else {
        // Node pointer must be a node in the list,
        // find end and tack on a new node
        Lnode_t*    cur_node;

        for (cur_node = (*n); NULL != cur_node->next; cur_node = cur_node->next);
        cur_node->next = sys_calloc(1U, sizeof(Lnode_t));
        if (NULL == cur_node->next) {
            errno = ENOMEM;
            return -1;
        }
        cur_node->next->request = *m;
        cur_node->next->prev = cur_node;
        cur_node->next->next = NULL;
    }
    return 0;   // Success
}


/*!
 * Take a message, along with its requestor queue and save it in the tree for
 * later traversal.
 *
 * Others listening on this head will be notified of the new message.  The
 * message is copied to the head so that there is no reliance on the storage
 * of the message passed.  Only the pointer to the requestor's is saved to point
 * back where to post responses.
 *
 * \brief Post message to specified head
 *
 * \param t linked list to post message to
 * \param m The message to marry with response later
 *
 * \return int -1 - errno set with error encountered
 *              0 - No entry found
 *
 */
int ll_post(Message_t const* const m)
{
    int retval;
    int old_errno = 0;

    if (NULL == m) {
        errno = EINVAL;
        return -1;
    }
    if (!valid_request_msg(m)) {
        errno = EINVAL;
        return -1;
    }

    retval = pthread_mutex_lock(&marriage_broker.lock);
    if (0 != retval) {
        errno = retval;
        return -1;
    }

    if (0 != ll_add(&marriage_broker.head, m)) {
        old_errno = errno;
    }

    // The only error that pthread_mutex_unlock could possibly return is EPERM,
    // which would be a logic error as we should be the sole holder of the lock
    // (regardless whether the add succeeded or not, we need to unlock the
    // regsion)
    retval = pthread_mutex_unlock(&marriage_broker.lock);
    assert(EPERM != retval);    // Logic fault (also get KW to quiet)

    // See if the add succeeded or not
    if (0 != old_errno) {
        errno = old_errno;
        return -1;
    }
    return 0;
}


/*!
 * Give the head of the linked list, find a matching node for the message request
 *
 * \param head Top of linked list
 * \param m request to match against
 * \param i2c Whether to look for i2c resource
 *
 * \return Lnode_t* node match, NULL if none found
 *
 * \todo (use boost libraries when migrated to C++)
 *
 */
static Lnode_t* ll_find(Lnode_t* const head, Message_t const* const m, bool i2c)
{
    // Prevent SEGV
    if (NULL == m || NULL == head) return NULL;

    // just traverse the linked list finding the first match
    IpmbReq_t const*        node_req   = NULL;
    IpmbResp_t const* const resp = (IpmbResp_t const* const)(m->Data);
    Lnode_t*                node       = NULL;
    Lnode_t*                found_node = NULL;
    uint8_t                 netfn      = (resp->NetFn >> 2) & (~1); // make response back into a request
    uint8_t                 node_netfn;

    debug(LOG_DEBUG2,
          "m->DstAddr: %02x; resp->SlaveAddr: %02x; netfn: %02x(%02x); cmd: %02x; seq: %02x; head: %p\n",
          m->DstAddr, resp->SlaveAddr, resp->NetFn >> 2, netfn, resp->Command, resp->SeqNum, (void*) head);
    for (node = head; NULL != node; node = node->next) {
        if (i2c) {
            // Looking for an I2C match
            if (-1 != node->request.client_resource)
                continue;   // Skip any node with a socket resource
        }
        else if (-1 == node->request.client_resource)
            continue;       // Skip any node with an I2C resource

        node_req = (IpmbReq_t const* const)(node->request.Data);
        node_netfn = (node_req->NetFn >> 2); // make response back into a request
        debug(LOG_DEBUG2, "node->DstAddr: %02x\n", node->request.DstAddr);

        if (resp->SlaveAddr != node->request.DstAddr) continue;

        debug(LOG_DEBUG2, "node_req->NetFn: %02x (%02x)\n", node_req->NetFn, node_netfn);

        // Message came from the address requested, check NetFn
        if (netfn != node_netfn)   // fuzzy matching
            continue;

        debug(LOG_DEBUG2, "node_req->Command: %02x\n", node_req->Command);

        // NetFn identical, check command
        if (resp->Command == node_req->Command) {
            // Command identical, check data
            debug(LOG_DEBUG2, "match\n");
            // Good enough, but let's see if there is a better match
            if (resp->SeqNum == node_req->SeqNum) {
                // Really can't get any better,
                debug(LOG_DEBUG2, "perfect match\n");
                found_node = node;
                break;
            }
            else if (NULL == found_node) {
                // Save it as not all devices match the seqno in
                // their response to a request.
                // Only return the first encounter (unless
                // subsequent nodes have a perfect SeqNo match)
                found_node = node;
            }
            // found_node points at something, but attempt to be greedy
            // and loop to see if we can get a better match
        }
    }
    debug(LOG_DEBUG2, "found_node: %p\n", (void*)found_node);
    return found_node;
}


/*!
 * Fetch a message queue that corresponds to the message previously requested
 * and store it to the address provided.
 *
 * \brief Get message from specified head
 *
 * \param t linked list to get message from
 * \param r pointer to the resource handle to use
 * \param m message response used to find original request
 * \param i2c Whether to look for i2c resource
 *
 * \return int -1 - errno set with error encountered
 *              0 - No entry found
 *              1 - Entry found (q points to queue of matching entry)
 *
 */
int ll_get(int* const r, Message_t const* const m, bool i2c)
{
    int      retval;
    bool     found  = false;
    Lnode_t* node   = NULL;

    if (NULL == r || NULL == m) {
        errno = EINVAL;
        return -1;
    }

    retval = pthread_mutex_lock(&marriage_broker.lock);
    if (0 != retval) {
        errno = retval;
        return -1;
    }

    node = ll_find(marriage_broker.head, m, i2c);
    if (NULL == node) {
        found = false;
    }
    else {
        // Save the resource associated with this node
        *r = node->request.client_resource;
        // Node found, update the queue, and remove the node, adjusting linked
        // list if needed
        if (marriage_broker.head == node) {
            marriage_broker.head = node->next;      // Drop link
            if (NULL != marriage_broker.head)
                marriage_broker.head->prev = NULL;  // End of the line
        }
        else {
            // Node not the head
            node->prev->next = node->next;          // Drop link
            if (NULL != node->next) {
                // There's a node that follows
                node->next->prev = node->prev;      // Drop reverse linkage
            }
        }
        sys_free(node);
        found = true;
    }

    // The only error that pthread_mutex_unlock could possibly return is EPERM,
    // which would be a logic error as we should be the sole holder of the lock
    // (regardless whether the get succeeded or not, we need to unlock the
    // regsion)
    retval = pthread_mutex_unlock(&marriage_broker.lock);
    assert(EPERM != retval);    // Logic fault (also get KW to quiet)

    return found;
}


/*!
 * Remove all outstanding requests associated with the resource in question
 *
 * \param resource The resource to look for
 *
 * \return int -1 - errno set with error encountered
 *              0 - No error
 *
 */
int ll_flush(int resource)
{
    Lnode_t* node    = NULL;
    Lnode_t* tmpnode = NULL;
    int      retval;

    retval = pthread_mutex_lock(&marriage_broker.lock);
    if (0 != retval) {
        errno = retval;
        return -1;
    }


    for (node = marriage_broker.head; NULL != node;) {
        if (node->request.client_resource == resource) {
            // Drop the node
            message_dump("Dropping request for resource %d", &node->request, resource);
            if (marriage_broker.head == node) {
                marriage_broker.head = node->next;      // Drop link
                if (NULL != marriage_broker.head)
                    marriage_broker.head->prev = NULL;  // End of the line
                tmpnode = marriage_broker.head;         // Next node to look at
            }
            else {
                // Node not the head
                node->prev->next = node->next;          // Drop link
                if (NULL != node->next) {
                    // There's a node that follows
                    node->next->prev = node->prev;      // Drop reverse linkage
                }
                tmpnode = node->next;                   // Next node to look at
            }
            sys_free(node);
            node = tmpnode;                             // Update next node to review
        }
        else
            node = node->next;                          // Next node to look at
    }

    // The only error that pthread_mutex_unlock could possibly return is EPERM,
    // which would be a logic error as we should be the sole holder of the lock
    retval = pthread_mutex_unlock(&marriage_broker.lock);
    assert(EPERM != retval);    // Logic fault (also get KW to quiet)

    return 0;
}


#ifndef _TAP_SOURCE
/// System shims to use.   testing provides their own routines for simulating
/// errors or confirming proper usage

static void sys_free (void *__ptr)
{
    free(__ptr);
}


static void *sys_calloc (size_t __nmemb, size_t __size)
{
    return calloc(__nmemb, __size);
}
#endif
