#include "../../msgq.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


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


int
main(void)
{
    MsgQ_t  Q;
    MsgQ_t* pQ = NULL;
    MsgQ_t  zeroed;
    MsgQ_t  bogus;

    bzero(&zeroed, sizeof(zeroed));
    memset(&bogus, 1, sizeof(bogus));
    Q = bogus;

    plan(41);

    mock_pthread_mutex_init_failure = mock_pthread_cond_init_failure = 0;
    block_size = 0;
    block_allocated = freed_block = NULL;
    mock_calloc_failure = false;
    ok(EINVAL == msgq_init(pQ, 1), "init returns error when passed NULL pointer");
    ok(NULL == block_allocated && 0 == num_members && 0 == block_size,
       "calloc not called with NULL Node Pointer");
    ok(NULL == freed_block, "free not called");

    ok(EINVAL == msgq_init(&Q, 0), "init returns error when passed 0 slots");
    ok(NULL == block_allocated && 0 == num_members && 0 == block_size,
       "calloc not called with NULL Node Pointer");
    ok(0 == memcmp(&Q, &bogus, sizeof(bogus)), "Q untouched");
    ok(NULL == freed_block, "free not called");

    ok(EINVAL == msgq_init(&Q, 1), "init returns error when passed 0 slots");
    ok(NULL == block_allocated && 0 == num_members && 0 == block_size,
       "calloc not called with NULL Node Pointer");
    ok(0 == memcmp(&Q, &bogus, sizeof(bogus)), "Q untouched");
    ok(NULL == freed_block, "free not called");

    Q = bogus;
    ok(EINVAL == msgq_init(&Q, 1), "init returns error on detecting queue pointer not NULL");
    ok(0 == memcmp(&Q, &bogus, sizeof(bogus)), "Q untouched");
    ok(NULL == freed_block, "free not called");

    bogus.queue = NULL;
    Q = bogus;
    mock_calloc_failure = true;
    ok(ENOMEM == msgq_init(&Q, 1), "init returns error on memory allocation error");
    ok(NULL == block_allocated, "No memory was allocated");
    ok(2 == num_members && sizeof(Message_t) == block_size,
       "attempted to allocate +1 Message_t blocks");
    // Note on the following:  The logic would be
    // (NULL != block_allocated  && freed_block == block_allocated) OR
    // (NULL == block_allocated && NULL == freed_block_allocated)
    // which essentially reduces to freed_block == block_allocated
    ok(freed_block == block_allocated, "Memory was has a net result zero of allocation");
    ok(mock_pthread_cond_init_count == mock_pthread_cond_destroy_count,
       "Conditional initialization has a net result of zero");
    ok(0 == memcmp(&Q, &bogus, sizeof(bogus)), "Q untouched");

    mock_calloc_failure = false;
    freed_block = block_allocated = NULL;
    mock_pthread_mutex_init_failure = EINTR;
    mock_pthread_mutex_init_count = mock_pthread_mutex_destroy_count = 0;
    mock_pthread_cond_init_count = mock_pthread_cond_destroy_count = 0;
    Q = bogus;
    ok(EINTR == msgq_init(&Q, 1), "init returns error on pthread_mutex_init error");
    ok(1 == mock_pthread_mutex_init_count, "pthread_mutex_init called");
    ok(freed_block == block_allocated, "Memory was has a net result zero of allocation");
    ok(mock_pthread_cond_init_count == mock_pthread_cond_destroy_count,
       "Conditional initialization has a net result of zero");
    ok(0 == memcmp(&Q, &bogus, sizeof(bogus)), "Q restored");

    mock_calloc_failure = false;
    freed_block = block_allocated = NULL;
    mock_pthread_mutex_init_failure = 0;
    mock_pthread_mutex_init_count = mock_pthread_mutex_destroy_count = 0;
    mock_pthread_cond_init_count = mock_pthread_cond_destroy_count = 0;
    mock_pthread_cond_init_failure = EINTR;
    mock_pthread_cond_init_failure_mask = 1;    // First error
    Q = bogus;
    ok(EINTR == msgq_init(&Q, 1), "init returns error on pthread_cond_init error");
    ok(1 == mock_pthread_cond_init_count, "pthread_cond_init called");
    ok(freed_block == block_allocated, "Memory was has a net result zero of allocation");
    ok(0 == mock_pthread_cond_destroy_count,
       "Conditional initialization has a net result of zero");
    ok(0 == memcmp(&Q, &bogus, sizeof(bogus)), "Q restored");

    mock_calloc_failure = false;
    freed_block = block_allocated = NULL;
    mock_pthread_mutex_init_failure = 0;
    mock_pthread_mutex_init_count = mock_pthread_mutex_destroy_count = 0;
    mock_pthread_cond_init_count = mock_pthread_cond_destroy_count = 0;
    mock_pthread_cond_init_failure = EINTR;
    mock_pthread_cond_init_failure_mask = 2;    // Second error
    Q = bogus;
    ok(EINTR == msgq_init(&Q, 1), "init returns error on pthread_cond_init error");
    ok(2 == mock_pthread_cond_init_count, "pthread_cond_init called");
    ok(freed_block == block_allocated, "Memory was has a net result zero of allocation");
    ok((mock_pthread_cond_init_count-1) == mock_pthread_cond_destroy_count,
       "Conditional initialization has a net result of zero");
    ok(0 == memcmp(&Q, &bogus, sizeof(bogus)), "Q restored");

    mock_calloc_failure = false;
    freed_block = block_allocated = NULL;
    mock_pthread_mutex_init_failure = 0;
    mock_pthread_mutex_init_count = mock_pthread_mutex_destroy_count = 0;
    mock_pthread_cond_init_count = mock_pthread_cond_destroy_count = 0;
    mock_pthread_cond_init_failure = 0;
    mock_pthread_cond_init_failure_mask = 0;    // No error
    Q = bogus;
    ok(0 == msgq_init(&Q, 1), "init returns error on pthread_cond_init error");
    ok(NULL != block_allocated && 2 == num_members && sizeof(Message_t) == block_size,
       "calloc called and allocated memory");
    ok(Q.queue == block_allocated, "Queue block assigned allocation");
    ok(2 == mock_pthread_cond_init_count, "pthread_cond_init called twice");
    ok(0 == mock_pthread_cond_destroy_count, "Conditional destroy not called");
    ok(0 == Q.head && 0 == Q.tail && 2 == Q.size, "Message queue initialized properly");

    free(Q.queue);
}
