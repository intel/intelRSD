#include "../../msgq.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


/// TAP control variables
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
    MsgQ_t  Q;
    MsgQ_t* pQ = NULL;
    MsgQ_t  zeroed;
    MsgQ_t  bogus;
    void*   block_allocated = NULL;

    bzero(&zeroed, sizeof(zeroed));
    memset(&bogus, 1, sizeof(bogus));
    Q = bogus;

    plan(19);

    mock_pthread_mutex_destroy_failure = mock_pthread_cond_destroy_failure = 0;
    freed_block = NULL;
    ok(EINVAL == msgq_destroy(pQ), "destroy returns error when passed NULL pointer");
    ok(NULL == freed_block, "free not called with NULL Node Pointer");
    ok(0 == mock_pthread_mutex_destroy_count, "pthread_mutex_destroy not called");
    ok(0 == mock_pthread_cond_destroy_count, "pthread_cond_destroy not called");

    freed_block = Q.queue = NULL;
    ok(EINVAL == msgq_destroy(&Q), "destroy returns error when Queue is unassigned");
    ok(NULL == freed_block, "free not called");

    Q.queue = block_allocated = bcalloc(2, sizeof(Message_t));
    freed_block = NULL;
    mock_pthread_mutex_destroy_failure = EINTR;
    ok(EINTR == msgq_destroy(&Q), "destroy returns error on pthread_mutex_destroy");
    ok((freed_block == block_allocated && NULL == Q.queue) || (NULL == freed_block && block_allocated == Q.queue),
       "destroy clears queue pointer if deallocated");

    if (freed_block == block_allocated) {
        // Reallocate for next test
        Q.queue = block_allocated = bcalloc(2, sizeof(Message_t));
    }
    freed_block = NULL;
    mock_pthread_mutex_destroy_failure = 0;
    mock_pthread_cond_destroy_count = 0;
    mock_pthread_cond_destroy_failure = EIO;
    mock_pthread_cond_destroy_failure_mask = 1; // Fail on first call
    ok(EIO == msgq_destroy(&Q), "destroy returns error on detecting queue pointer not NULL");
    ok(1 == mock_pthread_cond_destroy_count, "pthread_cond_destroy was called");
    ok((freed_block == block_allocated && NULL == Q.queue) || (NULL == freed_block && block_allocated == Q.queue),
       "destroy clears queue pointer if deallocated");

    if (freed_block == block_allocated) {
        // Reallocate for next test
        Q.queue = block_allocated = bcalloc(2, sizeof(Message_t));
    }
    freed_block = NULL;
    mock_pthread_cond_destroy_count = 0;
    mock_pthread_cond_destroy_failure = EIO;
    mock_pthread_cond_destroy_failure_mask = 2; // Fail on second call
    ok(EIO == msgq_destroy(&Q), "destroy returns error on detecting queue pointer not NULL");
    ok(2 == mock_pthread_cond_destroy_count, "pthread_cond_destroy was called twice");
    ok((freed_block == block_allocated && NULL == Q.queue) || (NULL == freed_block && block_allocated == Q.queue),
       "destroy clears queue pointer if deallocated");

    if (freed_block == block_allocated) {
        // Reallocate for next test
        Q.queue = block_allocated = bcalloc(2, sizeof(Message_t));
    }
    freed_block = NULL;
    mock_pthread_cond_destroy_count = mock_pthread_mutex_destroy_count = 0;
    mock_pthread_cond_destroy_failure = 0;
    mock_pthread_cond_destroy_failure_mask = 0; // Fail on second call
    ok(0 == msgq_destroy(&Q), "destroy returns success");
    ok(2 == mock_pthread_cond_destroy_count, "pthread_cond_destroy was called twice");
    ok(freed_block == block_allocated, "queue was deallocated");
    ok(1 == mock_pthread_mutex_destroy_count, "pthread_mutex_destroy was called");
    ok(0 == memcmp(&Q, &zeroed, sizeof(zeroed)), "Q structure zeroed out");

}


static void *sys_calloc (size_t __attribute__((__unused__))__nmemb, size_t __attribute__((__unused__))__size)
{
    return NULL;
}
