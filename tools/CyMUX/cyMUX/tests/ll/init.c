#include "../../ll.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


int
main(void)
{
    plan(4);

    errno = mock_pthread_mutex_init_failure = 0;
    ok(0 == ll_init(), "Successful initialization with pthread_mutex_init");
    ok(NULL == marriage_broker.head, "Linked List Head initialized");
    errno = 0;
    mock_pthread_mutex_init_failure = EINTR;
    ok(-1 == ll_init(), "Returns error when pthread_mutex_init fails");
    ok(EINTR == errno, "errno set to pthread_mutex_init return code");
}


/// Stub routines
bool valid_request_msg(Message_t const __attribute((__unused__))* const m)
{
    return true;
}


static void sys_free (void __attribute((__unused__))* __ptr)
{
}


static void *sys_calloc (size_t __attribute__((__unused__))__nmemb, size_t __attribute__((__unused__))__size)
{
    return NULL;
}
