#include "../../ll.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


/// TAP control variables
bool mock_valid_msg_failure = true;
void* block_allocated = NULL;
size_t block_size = 0;
bool mock_calloc_failure = false;


bool valid_request_msg(Message_t const __attribute((__unused__))* const m)
{
    return !mock_valid_msg_failure;
}


static void *sys_calloc (size_t __nmemb, size_t __size)
{
    block_size = __nmemb * __size;
    if (mock_calloc_failure) {
        block_allocated = NULL;
    }
    else {
        block_allocated = calloc(__nmemb, __size);
    }
    return block_allocated;
}


int
main(void)
{
    Lnode_t         zeroed;
    Message_t       msg;
    Message_t       msg_test;

    bzero(&marriage_broker, sizeof(&marriage_broker));
    bzero(&zeroed, sizeof(zeroed));

    memset(&msg_test, 1, sizeof(msg_test));
    msg = msg_test;

    plan(31);

    diag("Test for expected error with NULL node pointer passed");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = mock_valid_msg_failure = false;
    mock_pthread_mutex_lock_failure = mock_pthread_mutex_unlock_failure = 0;
    ok(-1 == ll_post(NULL), "Error called with NULL Pointer");
    ok(EINVAL == errno, "errno set to EINVAL with NULL Pointer");
    ok(NULL == marriage_broker.head, "Linked list pointers untouched");
    ok(NULL == block_allocated && 0 == block_size, "calloc not called");

    diag("Test for expected error with invalid message");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = false;
    mock_valid_msg_failure = true;
    mock_pthread_mutex_lock_failure = mock_pthread_mutex_unlock_failure = 0;
    ok(-1 == ll_post(&msg), "Error with invalid message");
    ok(EINVAL == errno, "errno set to EINVAL with NULL Pointer");
    ok(NULL == marriage_broker.head, "Linked list pointers untouched");
    ok(NULL == block_allocated && 0 == block_size, "calloc not called");
    ok(0 == memcmp(&msg, &msg_test, sizeof(msg_test)), "Message untouched");

    diag("Test for expected error with lock failure");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = mock_valid_msg_failure = false;
    mock_pthread_mutex_lock_failure = EINTR;
    mock_pthread_mutex_unlock_failure = 0;
    ok(-1 == ll_post(&msg), "Error with lock failure");
    ok(EINTR == errno, "errno set to EINTR with lock failure");
    ok(NULL == marriage_broker.head, "Linked list pointers untouched");
    ok(NULL == block_allocated && 0 == block_size, "calloc not called");
    ok(0 == memcmp(&msg, &msg_test, sizeof(msg_test)), "Message untouched");

    diag("Test for expected error with memory allocation failure (ll_add failure)");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = true;
    mock_valid_msg_failure = false;
    mock_pthread_mutex_lock_failure = mock_pthread_mutex_unlock_failure = 0;
    ok(-1 == ll_post(&msg), "Error with memory allocation failure");
    ok(ENOMEM == errno, "errno set to ENOMEM with memory allocation failure");
    ok(NULL == marriage_broker.head, "Linked list pointers untouched");
    ok(NULL == block_allocated, "calloc sanity check");
    ok(0 == memcmp(&msg, &msg_test, sizeof(msg_test)), "Message untouched");

    diag("Test for expected error with memory allocation failure (ll_add failure) and unlock failure");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = true;
    mock_valid_msg_failure = false;
    mock_pthread_mutex_lock_failure = 0;
    mock_pthread_mutex_unlock_failure = EINTR;
    ok(-1 == ll_post(&msg), "Error with memory allocation failure");
    ok(ENOMEM == errno, "errno set to ENOMEM with memory allocation failure");
    ok(NULL == marriage_broker.head, "Linked list pointers untouched");
    ok(NULL == block_allocated, "calloc sanity check");
    ok(0 == memcmp(&msg, &msg_test, sizeof(msg_test)), "Message untouched");

    diag("Test for expected success");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = mock_valid_msg_failure = false;
    mock_pthread_mutex_lock_failure = mock_pthread_mutex_unlock_failure = 0;
    memset(&msg, 0x55, sizeof(msg));
    bzero(&marriage_broker, sizeof(marriage_broker));
    msg_test = msg;

    ok(0 == ll_post(&msg), "Success adding message");
    ok(0 == errno, "errno not set");
    ok(NULL != block_allocated, "Node allocated");
    ok(sizeof(Lnode_t) == block_size, "Lnode_t block allocated");
    ok(marriage_broker.head == block_allocated, "New node assigned to marriage_broker chain");
    ok(0 == memcmp(&marriage_broker.head->request, &msg_test, sizeof(msg_test)),
       "message copied");
    ok(NULL == marriage_broker.head->next && NULL == marriage_broker.head->prev,
       "node pointers are initialized");
    free(marriage_broker.head);
}


/// Stub routines
static void sys_free (void __attribute((__unused__))* __ptr)
{
}
