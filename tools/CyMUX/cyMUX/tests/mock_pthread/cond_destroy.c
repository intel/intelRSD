#include "tap/basic.h"
#include "mock_pthread.h"
#include <pthread.h>
#include <errno.h>

int
main(void)
{
    pthread_cond_t     cond;

    plan(17);

    ok(0 == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy defaults to success");

    mock_pthread_cond_destroy_failure = EINVAL;
    mock_pthread_cond_destroy_count = 0;
    mock_pthread_cond_destroy_failure_mask = 1;    // Fail on first
    ok(EINVAL == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy returned error on mockup");
    ok(1 == mock_pthread_cond_destroy_count, "Counter incremented on call");

    mock_pthread_cond_destroy_failure = EINTR;
    mock_pthread_cond_destroy_count = 0;
    mock_pthread_cond_destroy_failure_mask = 0;    // No failures
    ok(0 == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy succeeded on clearing of mockup");
    ok(1 == mock_pthread_cond_destroy_count, "Counter incremented on call");
    ok(0 == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy succeeded on second calling of mockup");
    ok(2 == mock_pthread_cond_destroy_count, "Counter incremented on call");

    mock_pthread_cond_destroy_failure = EINTR;
    mock_pthread_cond_destroy_count = 0;
    mock_pthread_cond_destroy_failure_mask = 0xA;    // Fail on second and fourth calls
    ok(0 == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy succeeded on first call");
    ok(1 == mock_pthread_cond_destroy_count, "Counter incremented on call");
    ok(EINTR == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy returned error on second call");
    ok(2 == mock_pthread_cond_destroy_count, "Counter incremented on call");
    ok(0 == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy succeeded on third call");
    ok(3 == mock_pthread_cond_destroy_count, "Counter incremented on call");
    ok(EINTR == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy returned error on fourth call");
    ok(4 == mock_pthread_cond_destroy_count, "Counter incremented on call");
    ok(0 == pthread_cond_destroy(&cond), "(mock) pthread_cond_destroy succeeded on fifth call");
    ok(5 == mock_pthread_cond_destroy_count, "Counter incremented on call");
}
