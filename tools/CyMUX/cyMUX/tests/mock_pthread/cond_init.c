#include "tap/basic.h"
#include "mock_pthread.h"
#include <pthread.h>
#include <errno.h>

int
main(void)
{
    pthread_cond_t     cond;

    plan(17);

    ok(0 == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init defaults to success");

    mock_pthread_cond_init_failure = EINVAL;
    mock_pthread_cond_init_count = 0;
    mock_pthread_cond_init_failure_mask = 1;    // Fail on first
    ok(EINVAL == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init returned error on mockup");
    ok(1 == mock_pthread_cond_init_count, "Counter incremented on call");

    mock_pthread_cond_init_failure = EINTR;
    mock_pthread_cond_init_count = 0;
    mock_pthread_cond_init_failure_mask = 0;    // No failures
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init succeeded on clearing of mockup");
    ok(1 == mock_pthread_cond_init_count, "Counter incremented on call");
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init succeeded on second calling of mockup");
    ok(2 == mock_pthread_cond_init_count, "Counter incremented on call");

    mock_pthread_cond_init_failure = EINTR;
    mock_pthread_cond_init_count = 0;
    mock_pthread_cond_init_failure_mask = 0xA;    // Fail on second and fourth calls
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init succeeded on first call");
    ok(1 == mock_pthread_cond_init_count, "Counter incremented on call");
    ok(EINTR == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init returned error on second call");
    ok(2 == mock_pthread_cond_init_count, "Counter incremented on call");
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init succeeded on third call");
    ok(3 == mock_pthread_cond_init_count, "Counter incremented on call");
    ok(EINTR == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init returned error on fourth call");
    ok(4 == mock_pthread_cond_init_count, "Counter incremented on call");
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) pthread_cond_init succeeded on fifth call");
    ok(5 == mock_pthread_cond_init_count, "Counter incremented on call");
}
