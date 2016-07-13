#include "../../ll.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


static void diag_nodes(char const * const __str);
static void checksum(Message_t* msg);


/// TAP control variables
int         blocks_freed = 0;
void*       freed_blocks[3] = { NULL, NULL, NULL };
Lnode_t*    nodes[3] = { NULL, NULL, NULL };

static void sys_free (void* __ptr)
{
    freed_blocks[blocks_freed++] = __ptr;
    diag("freeing block %p", __ptr);
    free(__ptr);
}



int
main(void)
{
    LinkedList_t    head_test;
    Message_t       msg;
    Lnode_t         node_test;
    Message_t       msg_rsp;
    IpmbReq_t*      pReq = (IpmbReq_t*)(msg.Data);
    IpmbResp_t*     pRsp = (IpmbResp_t*)(msg_rsp.Data);
    int             resource = 0;
    size_t          i;


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
    //msg_test = msg;

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

    plan(45);

    errno = 0;
    resource = 3;
    blocks_freed = 0;
    bzero(freed_blocks, sizeof(freed_blocks));
    mock_pthread_mutex_lock_failure = EINTR;
    head_test = marriage_broker;
    ok(-1 == ll_flush(resource), "Error returned with pthread_mutex_lock failured");
    ok(EINTR == errno, "errno set to EINTR");
    ok(0 == memcmp(&marriage_broker, &head_test, sizeof(head_test)), "marriage_broker remains untouched");

    errno = mock_pthread_mutex_lock_failure = 0;
    ok(0 == ll_flush(resource), "Packet not found");
    ok(0 == errno, "errno untouched");
    ok(0 == memcmp(&marriage_broker, &head_test, sizeof(head_test)), "marriage_broker remains untouched");
    ok(0 == blocks_freed, "No blocks were deallocated");

    nodes[0] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[0]);
    diag("nodes[0] = %p; nodes[0]->prev = %p; nodes[0]->next = %p",
         (void*)nodes[0], (void*)nodes[0]->prev, (void*)nodes[0]->next);
    marriage_broker.head = nodes[0];
    head_test = marriage_broker;
    nodes[0]->request = msg;
    nodes[0]->request.client_resource = -1;
    node_test = *nodes[0];
    diag_nodes("Single node allocated as I2C resource");
    ok(0 == ll_flush(resource), "Packet not found (packet in queue for I2C)");
    diag_nodes("No change expected after flush");
    ok(0 == errno, "errno untouched");
    ok(0 == memcmp(&marriage_broker, &head_test, sizeof(head_test)), "marriage_broker remains untouched");
    ok(0 == memcmp(nodes[0], &node_test, sizeof(node_test)), "Node remains untouched");

    nodes[0]->request.client_resource = 3;
    node_test = *nodes[0];
    blocks_freed = 0;
    bzero(freed_blocks, sizeof(freed_blocks));
    diag_nodes("Single node allocated as client resource(3)");
    ok(0 == ll_flush(resource), "No error encountered during flushing attempt (packet in queue for non-I2C)");
    diag_nodes("Only node[0] should be deallocated");
    ok(0 == errno, "errno untouched");
    ok(NULL == marriage_broker.head, "marriage_broker last node removed");
    ok(1 == blocks_freed, "Block was deallocated");
    ok(freed_blocks[0] == nodes[0], "Previous allocated node was freed");

    blocks_freed = 0;
    bzero(freed_blocks, sizeof(freed_blocks));
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        nodes[i] = bcalloc(1, sizeof(Lnode_t));
        nodes[i]->request = msg;
        nodes[i]->request.client_resource = 7;
    }
    marriage_broker.head = nodes[0];
    nodes[0]->next = nodes[1];
    nodes[1]->prev = nodes[0];
    nodes[0]->request.client_resource = 3;
    diag_nodes("All nodes allocated as client resource(7) except node 1, only first two nodes in chain");
    ok(0 == ll_flush(resource), "No error encountered during flushing attempt (packet in queue for non-I2C)");
    diag_nodes("Expect first node in chain to be deallocated");
    ok(0 == errno, "errno untouched");
    ok(nodes[1] == marriage_broker.head, "marriage_broker removed first match");
    ok(1 == blocks_freed, "single block was deallocated");
    ok(freed_blocks[0] == nodes[0], "First allocated node was freed");
    ok(NULL == nodes[1]->prev, "Node pointer (now first) was updated correctly");

    nodes[0] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[0]);
    nodes[0]->request = msg;
    nodes[0]->request.client_resource = -1;
    nodes[0]->next = nodes[1];
    nodes[1]->prev = nodes[0];
    marriage_broker.head = nodes[0];
    blocks_freed = 0;
    bzero(freed_blocks, sizeof(freed_blocks));
    resource = 7;

    diag_nodes("All nodes allocated as client resource(7) except node 1, only first two nodes in chain");
    ok(0 == ll_flush(resource), "No error encountered during flushing attempt (packet in queue for non-I2C)");
    diag_nodes("Expect second node in chain to be deallocated");
    ok(0 == errno, "errno untouched");
    ok(nodes[0] == marriage_broker.head, "marriage_broker head still points to first node");
    ok(NULL == nodes[0]->next && NULL == nodes[0]->prev, "chain properly dropped node off (leaving only tail node)");
    ok(1 == blocks_freed, "Single block was deallocated");
    ok(freed_blocks[0] == nodes[1], "second allocated node was freed");

    blocks_freed = 0;
    bzero(freed_blocks, sizeof(freed_blocks));
    nodes[1] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[1]);
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        nodes[i]->request = msg;
        nodes[i]->request.client_resource = -1;
    }
    nodes[0]->next = nodes[1];
    nodes[1]->next = nodes[2];
    nodes[1]->prev = nodes[0];
    nodes[2]->prev = nodes[1];
    nodes[1]->request.client_resource = 7;
    marriage_broker.head = nodes[0];

    resource = 7;
    diag_nodes("All nodes allocated as I2C resource(-1) except node 1, all nodes in chain");
    ok(0 == ll_flush(resource), "No error encountered during flushing attempt (packet in queue for non-I2C)");
    diag_nodes("Expect only second node in chain to be deallocated");
    ok(0 == errno, "errno untouched");
    ok(nodes[0] == marriage_broker.head, "marriage_broker updated after dropping first node");
    ok(nodes[0]->next == nodes[2] && nodes[2]->prev == nodes[0], "chain properly dropped node");
    ok(1 == blocks_freed, "Single block was deallocated");
    ok(freed_blocks[0] == nodes[1], "second allocated node was freed");


    blocks_freed = 0;
    bzero(freed_blocks, sizeof(freed_blocks));
    nodes[1] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[1]);
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        nodes[i]->request = msg;
        nodes[i]->request.client_resource = -1;
    }
    nodes[0]->next = nodes[1];
    nodes[1]->next = nodes[2];
    nodes[1]->prev = nodes[0];
    nodes[2]->prev = nodes[1];
    nodes[1]->request.client_resource = 7;
    marriage_broker.head = nodes[0];

    resource = -1;
    diag_nodes("All nodes allocated as I2C resource(-1) except node 1, all three nodes in chain");
    ok(0 == ll_flush(resource), "No error encountered during flushing attempt (packet in queue for non-I2C)");
    diag_nodes("Expect all but second node in chain to be deallocated");
    ok(0 == errno, "errno untouched");
    ok(nodes[1] == marriage_broker.head, "marriage_broker updated after dropping first and third nodes");
    ok(NULL == nodes[1]->next && NULL == nodes[1]->prev, "chain properly dropped node");
    ok(2 == blocks_freed, "Two blocks were deallocated");
    ok(freed_blocks[0] == nodes[0] && freed_blocks[1] == nodes[2], "first and third allocated nodes were freed");

    blocks_freed = 0;
    bzero(freed_blocks, sizeof(freed_blocks));
    nodes[0] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[0]);
    nodes[2] = bcalloc(1, sizeof(Lnode_t));
    diag("allocated %p", (void*)nodes[2]);
    for (i = 0; i < sizeof(nodes)/sizeof(*nodes); i++) {
        nodes[i]->request = msg;
        nodes[i]->request.client_resource = -1;
    }
    nodes[0]->next = nodes[1];
    nodes[1]->next = nodes[2];
    nodes[1]->prev = nodes[0];
    nodes[2]->prev = nodes[1];
    marriage_broker.head = nodes[0];

    resource = -1;
    diag_nodes("All nodes allocated as I2C resource(-1), all three nodes in chain");
    ok(0 == ll_flush(resource), "No error encountered during flushing attempt (packet in queue for non-I2C)");
    diag_nodes("Expect all nodes in chain to be deallocated");
    ok(0 == errno, "errno untouched");
    ok(NULL == marriage_broker.head, "marriage_broker updated after dropping all nodes");
    ok(3 == blocks_freed, "Three blocks were deallocated");
    ok(freed_blocks[0] == nodes[0] && freed_blocks[1] == nodes[1] && freed_blocks[2] == nodes[2], "all allocated nodes were freed");
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


static void diag_nodes(char const * const __str)
{
    size_t  node;
    size_t  freed_block;

    diag("%s", __str);
    for (node = 0; node < sizeof(nodes)/sizeof(*nodes); node++) {
        for (freed_block = 0; freed_block < sizeof(freed_blocks)/sizeof(*freed_blocks); freed_block++) {
            if (freed_blocks[freed_block] == nodes[node] || NULL == nodes[node]) {
                diag("nodes[%lu] = %p; (deallocated)", node, (void*)nodes[node]);
                break;
            }
        }
        if (freed_block < sizeof(freed_blocks)/sizeof(*freed_blocks))
            continue;
        diag("nodes[%lu] = %p; resource: %d; prev = %p; next = %p",
             node, (void*)nodes[node], nodes[node]->request.client_resource, (void*)nodes[node]->prev, (void*)nodes[node]->next);
    }
}
