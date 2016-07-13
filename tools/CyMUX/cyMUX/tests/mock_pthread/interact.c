#include "tap/basic.h"
#include "mock_pthread.h"
#include <pthread.h>
#include <errno.h>

int
main(void)
{
    pthread_mutex_t     lock;
    pthread_cond_t      cond;

    plan(20);

    mock_pthread_mutex_destroy_failure = EINVAL;
    ok(0 == pthread_mutex_init(&lock, NULL), "(mock) *destroy_failure doesn't impede pthread_mutex_init success");
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) *destroy_failure doesn't impede pthread_cond_init success");
    ok(0 == pthread_mutex_lock(&lock), "(mock) *destroy_failure doesn't impede pthread_mutex_lock success");
    ok(0 == pthread_mutex_unlock(&lock), "(mock) *destroy_failure doesn't impede pthread_mutex_unlock success");

    mock_pthread_mutex_destroy_failure = 0;
    mock_pthread_mutex_lock_failure = EINVAL;
    ok(0 == pthread_mutex_init(&lock, NULL), "(mock) *lock_failure doesn't impede pthread_mutex_init success");
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) *lock_failure doesn't impede pthread_cond_init success");
    ok(0 == pthread_mutex_destroy(&lock), "(mock) *lock_failure doesn't impede pthread_mutex_destroy success");
    ok(0 == pthread_mutex_unlock(&lock), "(mock) *lock_failure doesn't impede pthread_mutex_unlock success");

    mock_pthread_mutex_lock_failure = 0;
    mock_pthread_mutex_unlock_failure = EINVAL;
    ok(0 == pthread_mutex_init(&lock, NULL), "(mock) *unlock_failure doesn't impede pthread_mutex_init success");
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) *unlock_failure doesn't impede pthread_cond_init success");
    ok(0 == pthread_mutex_lock(&lock), "(mock) *unlock_failure doesn't impede pthread_mutex_lock success");
    ok(0 == pthread_mutex_destroy(&lock), "(mock) *unlock_failure doesn't impede pthread_mutex_destroy success");

    mock_pthread_mutex_unlock_failure = 0;
    mock_pthread_mutex_init_failure = EINVAL;
    ok(0 == pthread_cond_init(&cond, NULL), "(mock) *unlock_failure doesn't impede pthread_cond_init success");
    ok(0 == pthread_mutex_destroy(&lock), "(mock) *unlock_failure doesn't impede pthread_mutex_destroy success");
    ok(0 == pthread_mutex_lock(&lock), "(mock) *unlock_failure doesn't impede pthread_mutex_lock success");
    ok(0 == pthread_mutex_unlock(&lock), "(mock) *unlock_failure doesn't impede pthread_mutex_unlock success");

    mock_pthread_mutex_init_failure = 0;
    mock_pthread_cond_init_failure = EINVAL;
    mock_pthread_cond_init_failure_mask = -1;
    ok(0 == pthread_mutex_init(&lock, NULL), "(mock) *cond_init_falure doesn't impede pthread_mutex_init success");
    ok(0 == pthread_mutex_destroy(&lock), "(mock) *cond_init_falure doesn't impede pthread_mutex_destroy success");
    ok(0 == pthread_mutex_lock(&lock), "(mock) *cond_init_falure doesn't impede pthread_mutex_lock success");
    ok(0 == pthread_mutex_unlock(&lock), "(mock) *cond_init_falure doesn't impede pthread_mutex_unlock success");
}
