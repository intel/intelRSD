#include "../../ll.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


static void checksum(Message_t* msg);


void* freed_block = NULL;
static void sys_free (void* __ptr)
{
    freed_block = __ptr;
    diag("freeing block %p", __ptr);
    free(__ptr);
}



int
main(void)
{
    LinkedList_t    head_test;
    Message_t       msg;
    Lnode_t*        nodes[3] = { NULL, NULL, NULL };
    Lnode_t         node_test;
    Message_t       msg_test;
    Message_t       msg_rsp;
    IpmbReq_t*      pReq = (IpmbReq_t*)(msg.Data);
    IpmbResp_t*     pRsp = (IpmbResp_t*)(msg_rsp.Data);
    int             resource = 0;
    size_t          i = 0;


    bzero(&msg, sizeof(msg));
    bzero(&marriage_broker, sizeof(marriage_broker));

    // Set up a valid request
    msg.DstAddr = 0x24;
    msg.Length = sizeof(IpmbReq_t) + 1UL;
    msg.client_resource = 7;
    pReq->NetFn = 0x6 << 2;
    pReq->MasterAddr = 0x10;
    pReq->SeqNum = 0x30;
    pReq->Command = 1;
    checksum(&msg);
    msg_test = msg;

    // Set up valid matching mock response
    msg_rsp.DstAddr = pReq->MasterAddr;
    msg_rsp.Length = sizeof(IpmbResp_t) + 1UL;
    msg_rsp.client_resource = 7;
    pRsp->NetFn = 0x7 << 2;
    pRsp->SlaveAddr = msg.DstAddr;
    pRsp->SeqNum = pReq->SeqNum;
    pRsp->Command = pReq->Command;
    pRsp->CompCode = CC_NORMAL;
    checksum(&msg_rsp);

    plan(51);

    errno = 0;
    msg_test = msg_rsp;
    head_test = marriage_broker;
    ok(-1 == ll_get(NULL, &msg_rsp, true), "Error returned with Resource pointer is NULL");
    ok(EINVAL == errno, "errno set to EINVAL");
    ok(0 == memcmp(&msg_rsp, &msg_test, sizeof(msg_test)), "message remains untouched");
    ok(0 == memcmp(&marriage_broker, &head_test, sizeof(head_test)), "marriage_broker remains untouched");

    errno = 0;
    resource = 3;
    ok(-1 == ll_get(&resource, NULL, true), "Error returned with message pointer is NULL");
    ok(EINVAL == errno, "errno set to EINVAL");
    ok(3 == resource, "resource is untouched");
    ok(0 == memcmp(&marriage_broker, &head_test, sizeof(head_test)), "marriage_broker remains untouched");

    errno = 0;
    resource = 3;
    mock_pthread_mutex_lock_failure = EINTR;
    ok(-1 == ll_get(&resource, &msg_rsp, true), "Error returned with pthread_mutex_lock failured");
    ok(EINTR == errno, "errno set to EINTR");
    ok(3 == resource, "resource is untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test, sizeof(msg_test)), "message remains untouched");
    ok(0 == memcmp(&marriage_broker, &head_test, sizeof(head_test)), "marriage_broker remains untouched");

    errno = mock_pthread_mutex_lock_failure = 0;
    resource = 3;
    ok(0 == ll_get(&resource, &msg_rsp, false), "Packet not found");
    ok(0 == errno, "errno untouched");
    ok(3 == resource, "resource is untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test, sizeof(msg_test)), "message remains untouched");
    ok(0 == memcmp(&marriage_broker, &head_test, sizeof(head_test)), "marriage_broker remains untouched");

    nodes[0] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[0]);
    diag("nodes[0] = %p; nodes[0]->prev = %p; nodes[0]->next = %p",
         (void*)nodes[0], (void*)nodes[0]->prev, (void*)nodes[0]->next);
    marriage_broker.head = nodes[0];
    head_test = marriage_broker;
    nodes[0]->request = msg;
    nodes[0]->request.client_resource = -1;
    node_test = *nodes[0];
    ok(0 == ll_get(&resource, &msg_rsp, false), "Packet not found (packet in queue for I2C)");
    ok(0 == errno, "errno untouched");
    ok(3 == resource, "resource is untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test, sizeof(msg_test)), "message remains untouched");
    ok(0 == memcmp(&marriage_broker, &head_test, sizeof(head_test)), "marriage_broker remains untouched");
    ok(0 == memcmp(nodes[0], &node_test, sizeof(node_test)), "Node remains untouched");

    nodes[0]->request.client_resource = 7;
    node_test = *nodes[0];
    freed_block = NULL;
    ok(1 == ll_get(&resource, &msg_rsp, false), "Packet found (packet in queue for non-I2C)");
    ok(0 == errno, "errno untouched");
    ok(7 == resource, "resource is updated");
    ok(0 == memcmp(&msg_rsp, &msg_test, sizeof(msg_test)), "message remains untouched");
    ok(NULL == marriage_broker.head, "marriage_broker last node removed");
    ok(freed_block == nodes[0], "Previous allocated node was freed");

    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        nodes[i] = bcalloc(1, sizeof(Lnode_t));
        nodes[i]->request = msg;
        nodes[i]->request.client_resource = 7;
    }
    marriage_broker.head = nodes[0];
    nodes[0]->next = nodes[1];
    nodes[1]->prev = nodes[0];
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        diag("nodes[%lu] = %p; nodes[%lu]->prev = %p; nodes[%lu]->next = %p",
             i, (void*)nodes[i], i, (void*)nodes[i]->prev, i, (void*)nodes[i]->next);
    }

    freed_block = NULL;
    ok(1 == ll_get(&resource, &msg_rsp, false), "Packet found (packet in queue for non-I2C)");
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        if (freed_block == nodes[i]) {
            diag("nodes[%lu] = %p; (deallocated)", i, (void*)nodes[i]);
            continue;
        }
        diag("nodes[%lu] = %p; nodes[%lu]->prev = %p; nodes[%lu]->next = %p",
             i, (void*)nodes[i], i, (void*)nodes[i]->prev, i, (void*)nodes[i]->next);
    }
    ok(0 == errno, "errno untouched");
    ok(7 == resource, "resource is updated");
    ok(0 == memcmp(&msg_rsp, &msg_test, sizeof(msg_test)), "message remains untouched");
    ok(nodes[1] == marriage_broker.head, "marriage_broker removed first match");
    ok(freed_block == nodes[0], "First allocated node was freed");
    ok(NULL == nodes[1]->prev, "Node pointer (now first) was updated correctly");
    nodes[0] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[0]);
    nodes[0]->request = msg;
    nodes[0]->request.client_resource = -1;
    nodes[0]->next = nodes[1];
    nodes[1]->prev = nodes[0];
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        diag("nodes[%lu] = %p; nodes[%lu]->prev = %p; nodes[%lu]->next = %p",
             i, (void*)nodes[i], i, (void*)nodes[i]->prev, i, (void*)nodes[i]->next);
    }

    marriage_broker.head = nodes[0];
    resource = 3;
    freed_block = NULL;
    ok(1 == ll_get(&resource, &msg_rsp, false), "Packet found (packet in queue for non-I2C)");
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        if (freed_block == nodes[i]) {
            diag("nodes[%lu] = %p; (deallocated)", i, (void*)nodes[i]);
            continue;
        }
        diag("nodes[%lu] = %p; nodes[%lu]->prev = %p; nodes[%lu]->next = %p",
             i, (void*)nodes[i], i, (void*)nodes[i]->prev, i, (void*)nodes[i]->next);
    }
    ok(0 == errno, "errno untouched");
    ok(7 == resource, "resource is updated");
    ok(0 == memcmp(&msg_rsp, &msg_test, sizeof(msg_test)), "message remains untouched");
    ok(nodes[0] == marriage_broker.head, "marriage_broker head still points to first node");
    ok(NULL == nodes[0]->next && NULL == nodes[0]->prev, "chain properly dropped node off (leaving only tail node)");
    ok(freed_block == nodes[1], "second allocated node was freed");
    nodes[1] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[0]);
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        nodes[i]->request = msg;
        nodes[i]->request.client_resource = -1;
    }
    nodes[0]->next = nodes[1];
    nodes[1]->next = nodes[2];
    nodes[1]->prev = nodes[0];
    nodes[2]->prev = nodes[1];
    nodes[1]->request.client_resource = 7;
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        diag("nodes[%lu] = %p; nodes[%lu]->prev = %p; nodes[%lu]->next = %p",
             i, (void*)nodes[i], i, (void*)nodes[i]->prev, i, (void*)nodes[i]->next);
    }

    freed_block = NULL;
    resource = 3;
    ok(1 == ll_get(&resource, &msg_rsp, false), "Packet found (packet in queue for non-I2C)");
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        if (freed_block == nodes[i]) {
            diag("nodes[%lu] = %p; (deallocated)", i, (void*)nodes[i]);
            continue;
        }
        diag("nodes[%lu] = %p; nodes[%lu]->prev = %p; nodes[%lu]->next = %p",
             i, (void*)nodes[i], i, (void*)nodes[i]->prev, i, (void*)nodes[i]->next);
    }
    ok(0 == errno, "errno untouched");
    ok(7 == resource, "resource is updated");
    ok(0 == memcmp(&msg_rsp, &msg_test, sizeof(msg_test)), "message remains untouched");
    ok(nodes[0] == marriage_broker.head, "marriage_broker updated after dropping first node");
    ok(nodes[0]->next == nodes[2] && nodes[2]->prev == nodes[0], "chain properly dropped node");
    ok(freed_block == nodes[1], "second allocated node was freed");
    free(nodes[0]);
    free(nodes[2]);
}


/// Stub routines
bool valid_request_msg(Message_t const __attribute((__unused__))* const m)
{
    return true;
}


static void *sys_calloc (size_t __attribute((__unused__))__nmemb, size_t __attribute((__unused__))__size)
{
    return NULL;
}


static void checksum(Message_t* msg)
{
    size_t      i;
    uint8_t     cksum = 0;
    IpmbReq_t*  pReq = NULL;
    size_t      data_len = 0;

    if (NULL == msg) return;

    pReq = (IpmbReq_t*) msg->Data;
    data_len = msg->Length - sizeof(IpmbReq_t) - 1UL;

    pReq->Checksum = -(msg->DstAddr + pReq->NetFn);
    cksum = pReq->MasterAddr + pReq->SeqNum + pReq->Command;

    for(i = 0; i < data_len; i++) {
        cksum += pReq->Data[i];
    }
    pReq->Data[data_len] = -cksum;
}
