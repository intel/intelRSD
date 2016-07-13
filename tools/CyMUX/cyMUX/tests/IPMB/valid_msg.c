#include "tap/basic.h"
#include "IPMB.h"
#include <errno.h>

void checksum(Message_t* msg);
void checksum(Message_t* msg)
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


int
main(void)
{
    Message_t   msg;
    Message_t*  pMsg = NULL;
    IpmbReq_t*  pReq = (IpmbReq_t*)(msg.Data);
    size_t      data_len = MAX_IPMB_PKT_SIZE - sizeof(IpmbReq_t) - 1UL;
    size_t      i;

    msg.DstAddr = 0x24;
    msg.Length = sizeof(IpmbReq_t) + 1UL;
    pReq->NetFn = 0x6 << 2;
    pReq->MasterAddr = 0x10;
    pReq->SeqNum = 0x30;
    pReq->Command = 1;
    checksum(&msg);
    pReq->Data[1] = 0xAA;   // Bogus test byte
    pReq->Data[2] = 0x55;   // Bogus test byte

    plan(18);

    ok(valid_request_msg(&msg), "Stock message passes");
    ok(false == valid_request_msg(pMsg), "NULL pointer fails");
    msg.Length = sizeof(IpmbReq_t) - 1UL;
    ok(false == valid_request_msg(&msg), "Buffer length too small");
    msg.Length = MAX_IPMB_PKT_SIZE + 1UL;
    ok(false == valid_request_msg(&msg), "Buffer length too large");
    msg.Length = sizeof(IpmbReq_t);
    ok(false == valid_request_msg(&msg), "Buffer length too small");
    msg.Length = 0UL;
    ok(false == valid_request_msg(&msg), "Buffer length too small");
    msg.Length = 1UL;
    ok(false == valid_request_msg(&msg), "Buffer length too small");
    msg.Length = sizeof(IpmbReq_t) + 2UL;
    ok(false == valid_request_msg(&msg), "Bad checksum (1 off)");
    msg.Length = sizeof(IpmbReq_t) + 3UL;
    ok(false == valid_request_msg(&msg), "Bad checksum (2 off)");

    msg.Length = sizeof(IpmbReq_t) + 1UL;
    pReq->NetFn = 7 << 2;
    checksum(&msg);
    ok(false == valid_request_msg(&msg), "Response NetFn fails");
    pReq->NetFn = 6 << 2;
    msg.DstAddr = 0x23;
    checksum(&msg);
    ok(false == valid_request_msg(&msg), "Invalid 7-bit slave address");
    msg.DstAddr = 0x22;
    ok(false == valid_request_msg(&msg), "First checksum fails (-1)");
    pReq->Checksum += 2;
    ok(false == valid_request_msg(&msg), "First checksum fails (+1)");
    pReq->Checksum = 0;
    ok(false == valid_request_msg(&msg), "First checksum fails (== 0)");
    pReq->Checksum = 0xff;
    ok(false == valid_request_msg(&msg), "First checksum fails (== ff)");
    checksum(&msg);

    for(i = 0; i < data_len - 1UL; i++) {
        pReq->Data[i] = i;
    }
    msg.Length = sizeof(IpmbReq_t) + data_len + 1UL;
    checksum(&msg);
    ok(valid_request_msg(&msg), "Large message passes");
    pReq->Data[data_len]--;
    ok(false == valid_request_msg(&msg), "Large message bad second checksum fails (-1)");
    pReq->Data[data_len] =+ 2;
    ok(false == valid_request_msg(&msg), "Large message bad second checksum fails (+1)");
}
