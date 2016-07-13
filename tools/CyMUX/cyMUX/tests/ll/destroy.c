#include "../../ll.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


/// TAP control variables
void* block_freed = NULL;


static void sys_free (void *__ptr)
{
    block_freed = __ptr;
    free(__ptr);
}


static void *sys_calloc (size_t __nmemb, size_t __size)
{
    return calloc(__nmemb, __size);
}


int
main(void)
{
    LinkedList_t    zeroed;
    void*           block_allocated = NULL;

    bzero(&zeroed, sizeof(zeroed));
    plan(11);

    errno = mock_pthread_mutex_destroy_failure = 0;
    ok(0 == ll_destroy(), "Destroy succeeds with empty list");
    ok(0 == memcmp(&marriage_broker, &zeroed, sizeof(marriage_broker)), "Linked list cleared");
    mock_pthread_mutex_destroy_failure = EINTR;
    errno = 0;
    ok(-1 == ll_destroy(), "Returns error when pthread_mutex_destroy fails");
    ok(EINTR == errno, "errno set to pthread_mutex_destroy return code");

    errno = mock_pthread_mutex_destroy_failure = 0;
    block_allocated =
    marriage_broker.head = bcalloc(1, sizeof(Lnode_t));
    ok(0 == ll_destroy(), "Destroy succeeds with empty list");
    ok(NULL == marriage_broker.head, "List cleaned up");    // Use valgrind to confirm
    ok(block_freed == block_allocated, "Freed up block allocated");

    mock_pthread_mutex_destroy_failure = EINTR;
    errno = 0;
    block_allocated =
    marriage_broker.head = bcalloc(1, sizeof(Lnode_t));
    ok(-1 == ll_destroy(), "Returns error when pthread_mutex_destroy fails");
    ok(EINTR == errno, "errno set to pthread_mutex_destroy return code");
    ok(NULL == marriage_broker.head, "List cleaned up in spite of error");    // Use valgrind to confirm
    ok(block_freed == block_allocated, "Freed up block allocated");
}


/// Stub routines
bool valid_request_msg(Message_t const __attribute((__unused__))* const m)
{
    return true;
}
