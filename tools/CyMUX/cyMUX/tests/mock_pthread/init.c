#include "tap/basic.h"
#include "mock_pthread.h"
#include <pthread.h>
#include <errno.h>

int
main(void)
{
    pthread_mutex_t     lock;

    plan(3);

    ok(0 == pthread_mutex_init(&lock, NULL), "(mock) pthread_mutex_init defaults to success");
    mock_pthread_mutex_init_failure = EINVAL;
    ok(EINVAL == pthread_mutex_init(&lock, NULL), "(mock) pthread_mutex_init returned error on mockup");
    mock_pthread_mutex_init_failure = 0;
    ok(0 == pthread_mutex_init(&lock, NULL), "(mock) pthread_mutex_init succeeded on clearing of mockup");
}
