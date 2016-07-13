#include "tap/basic.h"
#include "mock_pthread.h"
#include <pthread.h>
#include <errno.h>

int
main(void)
{
    pthread_mutex_t     lock;

    plan(3);

    ok(0 == pthread_mutex_lock(&lock), "(mock) pthread_mutex_lock defaults to success");
    mock_pthread_mutex_lock_failure = EINVAL;
    ok(EINVAL == pthread_mutex_lock(&lock), "(mock) pthread_mutex_lock returned error on mockup");
    mock_pthread_mutex_lock_failure = 0;
    ok(0 == pthread_mutex_lock(&lock), "(mock) pthread_mutex_lock succeeded on clearing of mockup");
}
