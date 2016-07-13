#include "tap/basic.h"
#include "IPMB.h"
#include "shim.h"
#include "debug.h"
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
    Message_t   m_rsp;
    IpmbReq_t*  pReq = (IpmbReq_t*)(msg.Data);
    IpmbResp_t* pResp = (IpmbResp_t*)(m_rsp.Data);
    size_t      data_len = MAX_IPMB_PKT_SIZE - sizeof(IpmbReq_t) - 1UL;
    size_t      i;
    uint8_t     cksum = 0;

    msg.DstAddr = 0x24;
    msg.Length = sizeof(IpmbReq_t) + 1UL;
    pReq->NetFn = 0x6 << 2;
    pReq->MasterAddr = 0x10;
    pReq->SeqNum = 0x30;
    pReq->Command = 1;
    checksum(&msg);
    pReq->Data[1] = 0xAA;   // Bogus test byte
    pReq->Data[2] = 0x55;   // Bogus test byte

    plan(39);

    //message_dump("Message:\n", &msg);
    ok(0 == craft_error_response(&msg, &m_rsp, CC_INV_CMD), "Stock message passes");
    ok(7UL == m_rsp.Length, "Fixed length match");
    ok(m_rsp.DstAddr == pReq->MasterAddr, "Response Destination address match Request Source Address");
    ok(pResp->NetFn == (pReq->NetFn + (1 << 2)), "Response NetFn == Request NetFn + 1");
    ok(pResp->Command == pReq->Command, "Response Command == Request Command");
    ok(pResp->SeqNum == pReq->SeqNum, "Response Sequence Number == Request Sequence Number");
    ok(pResp->SlaveAddr == msg.DstAddr, "Response Source address match Request Destination Address");
    ok(pResp->CompCode == CC_INV_CMD, "Command Code matches parameter");
    cksum = -(m_rsp.DstAddr + pResp->NetFn);
    ok(pResp->Checksum == cksum, "First checksum in Response is valid");
    cksum = -(pResp->SlaveAddr + pResp->Command + pResp->SeqNum + pResp->CompCode);
    ok(pResp->Data[0] == cksum, "Second checksum in Response is valid");
    ok(EINVAL == craft_error_response(pMsg, &m_rsp, CC_NORMAL), "msg NULL pointer fails");
    ok(EINVAL == craft_error_response(&msg, pMsg, CC_NORMAL), "rsp NULL pointer fails");
    ok(EINVAL == craft_error_response(pMsg, pMsg, CC_NORMAL), "both parameters as NULL pointers fails");
    ok(EINVAL == craft_error_response(&msg, &msg, CC_NORMAL), "rsp == msg pointer fails");
    msg.Length = sizeof(IpmbReq_t) - 1UL;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Buffer length too small");
    msg.Length = MAX_IPMB_PKT_SIZE + 1UL;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Buffer length too large");
    msg.Length = sizeof(IpmbReq_t);
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Buffer length too small");
    msg.Length = 0UL;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Buffer length too small");
    msg.Length = 1UL;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Buffer length too small");
    msg.Length = sizeof(IpmbReq_t) + 2UL;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Bad checksum (1 off)");
    msg.Length = sizeof(IpmbReq_t) + 3UL;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Bad checksum (2 off)");

    msg.Length = sizeof(IpmbReq_t) + 1UL;
    pReq->NetFn = 7 << 2;
    checksum(&msg);
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Response NetFn fails");
    pReq->NetFn = 6 << 2;
    msg.DstAddr = 0x39;
    checksum(&msg);
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Invalid 7-bit slave address");
    msg.DstAddr = 0x40;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "First checksum fails (-1)");
    pReq->Checksum += 2;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "First checksum fails (+1)");
    pReq->Checksum = 0;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "First checksum fails (== 0)");
    pReq->Checksum = 0xff;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "First checksum fails (== ff)");
    checksum(&msg);

    for(i = 0; i < data_len - 1UL; i++) {
        pReq->Data[i] = i;
    }
    msg.Length = sizeof(IpmbReq_t) + data_len + 1UL;
    checksum(&msg);
    ok(0 == craft_error_response(&msg, &m_rsp, CC_DEST_UNAVAILABLE), "Large message passes");
    ok(7UL == m_rsp.Length, "Fixed length match");
    ok(m_rsp.DstAddr == pReq->MasterAddr, "Response Destination address match Request Source Address");
    ok(pResp->NetFn == (pReq->NetFn + (1 << 2)), "Response NetFn == Request NetFn + 1");
    ok(pResp->Command == pReq->Command, "Response Command == Request Command");
    ok(pResp->SeqNum == pReq->SeqNum, "Response Sequence Number == Request Sequence Number");
    ok(pResp->SlaveAddr == msg.DstAddr, "Response Source address match Request Destination Address");
    ok(pResp->CompCode == CC_DEST_UNAVAILABLE, "Command Code matches parameter");
    cksum = -(m_rsp.DstAddr + pResp->NetFn);
    ok(pResp->Checksum == cksum, "First checksum in Response is valid");
    cksum = -(pResp->SlaveAddr + pResp->Command + pResp->SeqNum + pResp->CompCode);
    ok(pResp->Data[0] == cksum, "Second checksum in Response is valid");
    pReq->Data[data_len]--;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Large message bad second checksum fails (-1)");
    pReq->Data[data_len] =+ 2;
    ok(EINVAL == craft_error_response(&msg, &m_rsp, CC_NORMAL), "Large message bad second checksum fails (+1)");
}
