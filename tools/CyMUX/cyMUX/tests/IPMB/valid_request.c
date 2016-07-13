#include "tap/basic.h"
#include "IPMB.h"
#include <errno.h>

void checksum(uint8_t slaveAddr, IpmbReq_t* pReq, size_t data_len);
void checksum(uint8_t slaveAddr, IpmbReq_t* pReq, size_t data_len)
{
    size_t  i;
    uint8_t cksum = 0;

    pReq->Checksum = -(slaveAddr + pReq->NetFn);
    cksum = pReq->MasterAddr + pReq->SeqNum + pReq->Command;

    for(i = 0; i < data_len; i++) {
        cksum += pReq->Data[i];
    }
    pReq->Data[data_len] = -cksum;
}


int
main(void)
{
    uint8_t     buf[MAX_IPMB_PKT_SIZE+2];
    uint8_t*    pBuf = NULL;
    IpmbReq_t*  pReq = (IpmbReq_t*)(buf + 1);
    size_t      buf_len = sizeof(IpmbReq_t) + 2UL; // slave addy, header, cksum
    size_t      data_len = MAX_IPMB_PKT_SIZE - sizeof(IpmbReq_t) - 2UL;
    size_t      i;

    *buf = 0x24;
    pReq->NetFn = 0x6 << 2;
    pReq->MasterAddr = 0x10;
    pReq->SeqNum = 0x30;
    pReq->Command = 1;
    checksum(*buf, pReq, 0UL);
    pReq->Data[1] = 0xAA;   // Bogus test byte
    pReq->Data[2] = 0x55;   // Bogus test byte

    plan(18);

    ok(valid_request(buf, buf_len), "Stock message passes");
    ok(false == valid_request(pBuf, buf_len), "NULL pointer fails");
    ok(false == valid_request(buf, buf_len - 1UL), "Buffer length too small");
    ok(false == valid_request(buf, MAX_IPMB_PKT_SIZE + 2UL), "Buffer length too large");
    ok(false == valid_request(buf, sizeof(IpmbReq_t)), "Buffer length too small");
    ok(false == valid_request(buf, 0UL), "Buffer length too small");
    ok(false == valid_request(buf, 1UL), "Buffer length too small");
    ok(false == valid_request(buf, buf_len + 1UL), "Bad checksum (1 off)");
    ok(false == valid_request(buf, buf_len + 2UL), "Bad checksum (2 off)");

    pReq->NetFn = 7 << 2;
    checksum(*buf, pReq, 0UL);
    ok(false == valid_request(buf, buf_len), "Response NetFn fails");
    pReq->NetFn = 6 << 2;
    *buf = 0x37;
    checksum(*buf, pReq, 0UL);
    ok(false == valid_request(buf, buf_len), "Invalid 7-bit slave address");
    *buf = 0x38;
    ok(false == valid_request(buf, buf_len), "First checksum fails (-1)");
    pReq->Checksum += 2;
    ok(false == valid_request(buf, buf_len), "First checksum fails (+1)");
    pReq->Checksum = 0;
    ok(false == valid_request(buf, buf_len), "First checksum fails (== 0)");
    pReq->Checksum = 0xff;
    ok(false == valid_request(buf, buf_len), "First checksum fails (== ff)");
    checksum(*buf, pReq, 0UL);

    for(i = 0UL; i < data_len; i++) {
        pReq->Data[i] = i;
    }
    checksum(*buf, pReq, data_len);
    ok(valid_request(buf, buf_len + data_len), "Large message passes");
    pReq->Data[data_len]--;
    ok(false == valid_request(buf, buf_len + data_len), "Large message bad second checksum fails (-1)");
    pReq->Data[data_len] =+ 2;
    ok(false == valid_request(buf, buf_len + data_len), "Large message bad second checksum fails (+1)");
}
