#include "../../ll.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


static void checksum(Message_t* msg);


int
main(void)
{
    Lnode_t*        pNode;
    Lnode_t         head;
    Lnode_t         zeroed;
    Message_t       msg;
    Message_t       msg_test;
    Message_t       msg_test2;
    Message_t       msg_rsp;
    IpmbReq_t*      pReq = (IpmbReq_t*)(msg.Data);
    IpmbResp_t*     pRsp = (IpmbResp_t*)(msg_rsp.Data);


    bzero(&zeroed, sizeof(zeroed));
    bzero(&msg, sizeof(msg));
    head = zeroed;

    // Set up a valid request
    msg.DstAddr = 0x24;
    msg.Length = sizeof(IpmbReq_t) + 1UL;
    msg.client_resource = 7;
    pReq->NetFn = 0x6 << 2;
    pReq->MasterAddr = 0x10;
    pReq->SeqNum = 0x30;
    pReq->Command = 1;
    checksum(&msg);
    head.request = msg;
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

    plan(28);

    ok(NULL == ll_find(NULL, &msg, false), "NULL returned when passed null head pointer");
    ok(0 == memcmp(&msg, &msg_test, sizeof(msg_test)), "message remains untouched");

    ok(NULL == ll_find(&head, NULL, false), "NULL returned when passed null message pointer");
    ok(0 == memcmp(&head.request, &msg_test, sizeof(msg_test)), "head node message request untouched");
    ok(NULL == head.next && NULL == head.prev, "head pointers untouched");

    msg_test = msg;
    msg_test2 = msg_rsp;
    pNode = ll_find(&head, &msg_rsp, false);
    ok(NULL != pNode, "Successful find of message");
    ok(pNode == &head, "Pointer return matches expected node");
    ok(0 == memcmp(&head.request, &msg_test, sizeof(msg_test)), "node message untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test2, sizeof(msg_test2)), "response message untouched");

    ok(NULL == ll_find(&head, &msg_rsp, true), "NULL returned for same response looking for I2C message");
    ok(0 == memcmp(&head.request, &msg_test, sizeof(msg_test)), "node message untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test2, sizeof(msg_test2)), "response message untouched");

    head.request.client_resource = -1;
    msg_test = head.request;
    ok(NULL == ll_find(&head, &msg_rsp, false), "NULL returned for same response looking for non-I2C message (w/ node as I2C)");
    ok(0 == memcmp(&head.request, &msg_test, sizeof(msg_test)), "node message untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test2, sizeof(msg_test2)), "response message untouched");

    head.request.client_resource = 7;
    pRsp->SlaveAddr += 2;
    checksum(&msg_rsp);
    msg_test2 = msg_rsp;
    msg_test = head.request;
    ok(NULL == ll_find(&head, &msg_rsp, false), "NULL returned for same response w/ different slave addy");
    ok(0 == memcmp(&head.request, &msg_test, sizeof(msg_test)), "node message untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test2, sizeof(msg_test2)), "response message untouched");

    pRsp->SlaveAddr = head.request.DstAddr;
    pRsp->NetFn = 0x9 << 2;
    checksum(&msg_rsp);
    msg_test2 = msg_rsp;
    msg_test = head.request;
    ok(NULL == ll_find(&head, &msg_rsp, false), "NULL returned for response w/ different NetFn");
    ok(0 == memcmp(&head.request, &msg_test, sizeof(msg_test)), "node message untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test2, sizeof(msg_test2)), "response message untouched");

    pRsp->NetFn = pReq->NetFn;
    pRsp->Command++;
    checksum(&msg_rsp);
    msg_test2 = msg_rsp;
    head.request.client_resource = 7;
    msg_test = head.request;
    ok(NULL == ll_find(&head, &msg_rsp, false), "NULL returned for response w/ different Command");
    ok(0 == memcmp(&head.request, &msg_test, sizeof(msg_test)), "node message untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test2, sizeof(msg_test2)), "response message untouched");

    pRsp->Command = pReq->Command;
    pRsp->SeqNum |= 0x03;
    checksum(&msg_rsp);
    msg_test2 = msg_rsp;
    head.request.client_resource = 7;
    msg_test = head.request;
    pNode = ll_find(&head, &msg_rsp, false);
    ok(NULL != pNode, "Successful find of message on fuzzy seqno match");
    ok(pNode == &head, "Pointer return matches expected node");
    ok(0 == memcmp(&head.request, &msg_test, sizeof(msg_test)), "node message untouched");
    ok(0 == memcmp(&msg_rsp, &msg_test2, sizeof(msg_test2)), "response message untouched");


    // Could be more rigorous in the testing for just the mask of the SeqNo
    // Also, could modify the chain and test for fuzzy or not found
}


/// Stub routines
static void sys_free (void __attribute((__unused__))* __ptr)
{
}


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
