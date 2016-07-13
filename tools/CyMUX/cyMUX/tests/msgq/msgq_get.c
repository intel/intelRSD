#include "../../msgq.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"
#include "debug.h"


/// TAP control variables
void* block_allocated = NULL;
size_t block_size = 0;
size_t num_members = 0;
bool mock_calloc_failure = false;
void* freed_block = NULL;


static void sys_free (void* __ptr)
{
    freed_block = __ptr;
    diag("freeing block %p", __ptr);
    free(__ptr);
}


static void *sys_calloc (size_t __nmemb, size_t __size)
{
    num_members = __nmemb;
    block_size = __size;
    if (mock_calloc_failure) {
        block_allocated = NULL;
    }
    else {
        block_allocated = calloc(__nmemb, __size);
        diag("allocated block %p", block_allocated);
    }
    return block_allocated;
}


void checksum(Message_t* msg);
void checksum(Message_t* msg)
{
    size_t     i;
    uint8_t    cksum    = 0;
    IpmbReq_t* pReq     = NULL;
    size_t     data_len = 0;

    if (NULL == msg) return;

    pReq = (IpmbReq_t*)msg->Data;
    data_len = msg->Length - sizeof(IpmbReq_t) - 1UL;

    pReq->Checksum = -(msg->DstAddr + pReq->NetFn);
    cksum = pReq->MasterAddr + pReq->SeqNum + pReq->Command;

    for (i = 0; i < data_len; i++) {
        cksum += pReq->Data[i];
    }
    pReq->Data[data_len] = -cksum;
}


int
main(void)
{
    MsgQ_t      Q;
    MsgQ_t*     pQ = NULL;
    MsgQ_t      zeroed_q;
    MsgQ_t      bogus_q;
    MsgQ_t      valid_q;
    Message_t   Msg;
    Message_t   NullMsg;
    Message_t   Req_m;
    Message_t*  pMsg = NULL;
    Message_t*  pNULLMsg = NULL;
    Message_t   zeroed_m;
    Message_t   bogus_m;
    Message_t   array_m[3];
    IpmbReq_t*  pReq     = (IpmbReq_t*)(Req_m.Data);
    size_t      old_head;
    size_t      old_tail;

    bzero(&zeroed_q, sizeof(zeroed_q));
    bzero(&zeroed_m, sizeof(zeroed_m));
    bzero(&Req_m, sizeof(Req_m));
    bzero(&array_m, sizeof(array_m));
    bzero(&NullMsg, sizeof(NullMsg));
    memset(&bogus_m, 1, sizeof(bogus_m));
    memset(&bogus_q, 1, sizeof(bogus_q));
    Q = bogus_q;
    valid_q = zeroed_q;

    Req_m.DstAddr = 0x24;
    Req_m.Length = sizeof(IpmbReq_t) + 1UL;
    pReq->NetFn = 0x6 << 2;
    pReq->MasterAddr = 0x10;
    pReq->SeqNum = 0x30;
    pReq->Command = 1;
    checksum(&Req_m);


    plan(56);

    Msg = NullMsg;
    ok(EINVAL == msgq_get(pQ, &Msg), "get returns error when passed NULL Q pointer");
    ok(0 == memcmp(&Msg, &NullMsg, sizeof(NullMsg)), "Message block untouched");
    ok(0 == mock_pthread_mutex_lock_count, "pthread_mutex_lock not called");
    ok(0 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock not called");

    ok(0 == msgq_init(&valid_q, 2), "Successfully initialized Message Q");
    Msg = Req_m;
    ok(0 == msgq_post(&valid_q, &Msg), "Successfully posted single valid message");
    pMsg = valid_q.queue;
    memcpy(array_m, pMsg, sizeof(array_m));
    Q = valid_q;
    mock_pthread_mutex_lock_count = mock_pthread_mutex_unlock_count = 0;
    ok(EINVAL == msgq_get(&Q, pNULLMsg), "get returns error when passed NULL message pointer");
    ok(valid_q.queue == pMsg, "queue pointer didn't change");
    ok(0 == memcmp(&Q, &valid_q, sizeof(valid_q)), "Q block untouched");
    ok(0 == memcmp(pMsg, array_m, sizeof(array_m)), "queue untouched");
    ok(0 == mock_pthread_mutex_lock_count, "pthread_mutex_lock not called");
    ok(0 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock not called");

    mock_pthread_mutex_lock_count = mock_pthread_mutex_unlock_count = 0;
    ok(EINVAL == msgq_get(pQ, pNULLMsg), "get returns error when passed NULL message & Q pointers");
    ok(0 == mock_pthread_mutex_lock_count, "pthread_mutex_lock not called");
    ok(0 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock not called");

    valid_q.queue = pMsg;
    Q = valid_q;
    Msg = NullMsg;
    memcpy(pMsg, array_m, sizeof(array_m));
    // corrupt by removing queue
    Q.queue = NULL;
    ok(EINVAL == msgq_get(&Q, &Msg), "get returns error when Q's queue is NULL");
    ok(0 == memcmp(&Msg, &NullMsg, sizeof(NullMsg)), "Message was untouched");
    Q.queue = pMsg;
    ok(0 == memcmp(&Q, &valid_q, sizeof(valid_q)), "Q was untouched");
    ok(0 == memcmp(pMsg, array_m, sizeof(array_m)), "queue untouched");
    ok(0 == mock_pthread_mutex_lock_count, "pthread_mutex_lock not called");
    ok(0 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock not called");

    valid_q.queue = pMsg;
    Q = valid_q;
    Msg = NullMsg;
    memcpy(pMsg, array_m, sizeof(array_m));
    // corrupt size
    Q.size = 1;
    ok(EINVAL == msgq_get(&Q, &Msg), "get returns error when Q's size is < 2");
    ok(0 == memcmp(&Msg, &NullMsg, sizeof(NullMsg)), "Message was untouched");
    Q.size = valid_q.size;
    ok(0 == memcmp(&Q, &valid_q, sizeof(valid_q)), "Q was untouched");
    ok(0 == memcmp(pMsg, array_m, sizeof(array_m)), "queue untouched");
    ok(0 == mock_pthread_mutex_lock_count, "pthread_mutex_lock not called");
    ok(0 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock not called");

    valid_q.queue = pMsg;
    Q = valid_q;
    Msg = NullMsg;
    memcpy(pMsg, array_m, sizeof(array_m));
    mock_pthread_mutex_lock_failure = EIO;
    mock_pthread_mutex_lock_count = 0;
    ok(EIO == msgq_get(&Q, &Msg), "get returns error on pthread_mutex_lock failure");
    ok(0 == memcmp(&Q, &valid_q, sizeof(valid_q)), "Q was untouched");
    ok(0 == memcmp(pMsg, array_m, sizeof(array_m)), "queue untouched");
    ok(0 == memcmp(&Msg, &NullMsg, sizeof(NullMsg)), "Message was untouched");
    ok(1 == mock_pthread_mutex_lock_count, "pthread_mutex_lock was called");
    ok(0 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock not called");

    valid_q.queue = pMsg;
    Q = valid_q;
    Msg = NullMsg;
    old_head = Q.head;
    old_tail = Q.tail;
    Q.head = Q.tail;
    memcpy(pMsg, array_m, sizeof(array_m));
    mock_pthread_mutex_lock_failure = 0;
    mock_pthread_cond_wait_failure = EINTR;
    mock_pthread_mutex_lock_count =
    mock_pthread_mutex_unlock_count =
    mock_pthread_cond_wait_count = 0;
    ok(EINTR == msgq_get(&Q, &Msg), "get returns error on pthread_cond_wait failure");
    ok(Q.head == old_tail, "Head pointer untouched");
    Q.head = old_head;
    ok(0 == memcmp(&Q, &valid_q, sizeof(valid_q)), "Q was untouched");
    ok(0 == memcmp(pMsg, array_m, sizeof(array_m)), "queue untouched");
    ok(0 == memcmp(&Msg, &NullMsg, sizeof(NullMsg)), "Message was untouched");
    ok(1 == mock_pthread_mutex_lock_count, "pthread_mutex_lock was called");
    ok(1 == mock_pthread_cond_wait_count, "pthread_cond_wait_count was called");
    ok(1 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock was called");

    valid_q.queue = pMsg;
    Q = valid_q;
    old_head = Q.head;
    old_tail = Q.tail;
    Q.head = Q.tail;
    memcpy(pMsg, array_m, sizeof(array_m));
    Msg = NullMsg;
    mock_pthread_mutex_lock_failure = 0;
    mock_pthread_cond_wait_failure = EINTR;
    mock_pthread_mutex_unlock_failure = EIO;
    mock_pthread_mutex_lock_count =
    mock_pthread_mutex_unlock_count =
    mock_pthread_cond_wait_count = 0;
    ok(EINTR == msgq_get(&Q, &Msg), "get returns error on pthread_cond_wait failure (not unlock)");
    ok(Q.head == old_tail, "Head pointer untouched");
    Q.head = old_head;
    ok(0 == memcmp(&Q, &valid_q, sizeof(valid_q)), "Q was untouched");
    ok(0 == memcmp(pMsg, array_m, sizeof(array_m)), "queue untouched");
    ok(0 == memcmp(&Msg, &NullMsg, sizeof(NullMsg)), "Message was untouched");
    ok(1 == mock_pthread_mutex_lock_count, "pthread_mutex_lock was called");
    ok(1 == mock_pthread_cond_wait_count, "pthread_cond_wait was called");
    ok(1 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock was called");

    valid_q.queue = pMsg;
    Q = valid_q;
    memcpy(pMsg, array_m, sizeof(array_m));
    Msg = NullMsg;
    mock_pthread_mutex_lock_failure = 0;
    mock_pthread_mutex_lock_count = 0;
    mock_pthread_cond_wait_failure = 0;
    mock_pthread_mutex_unlock_failure = 0;
    mock_pthread_mutex_unlock_count =
    mock_pthread_cond_signal_count =
    mock_pthread_cond_wait_count = 0;
    ok(0 == msgq_get(&Q, &Msg), "get succeeds");
    ok(Q.head == Q.tail, "Pointers updated reasonably");
    ok(0 == memcmp(&Msg, &Req_m, sizeof(Req_m)), "Message was updated");
    ok(1 == mock_pthread_mutex_lock_count, "pthread_mutex_lock was called");
    ok(0 == mock_pthread_cond_wait_count, "pthread_cond_wait_count was called");
    ok(1 == mock_pthread_cond_signal_count, "pthread_cond_signal was called");
    ok(1 == mock_pthread_mutex_unlock_count, "pthread_mutex_unlock was called");

    free(valid_q.queue);
}
