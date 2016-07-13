#include "mock_pthread.h"
#include <errno.h>

int mock_pthread_mutex_init_failure = 0;
int mock_pthread_mutex_init_count = 0;
int pthread_mutex_init (pthread_mutex_t __attribute__((__unused__))*__mutex,
                               const pthread_mutexattr_t __attribute__((unused))*__mutexattr)
{
     mock_pthread_mutex_init_count++;
     return mock_pthread_mutex_init_failure;
}

int mock_pthread_cond_init_failure = 0;
int mock_pthread_cond_init_count = 0;
uint8_t mock_pthread_cond_init_failure_mask = 0;
int pthread_cond_init (pthread_cond_t __attribute__((__unused__))*__restrict __cond,
                              const pthread_condattr_t __attribute__((__unused__))*__restrict __cond_attr)
//     __THROW __nonnull ((1))
{
     // Return failure based on bit set in mask
     // This allows multiple or delayed failure call
     if (0 == (mock_pthread_cond_init_failure_mask & (1U << mock_pthread_cond_init_count++))) {
          return 0;
     }
     return mock_pthread_cond_init_failure;
}

int mock_pthread_cond_destroy_failure = 0;
int mock_pthread_cond_destroy_count = 0;
uint8_t mock_pthread_cond_destroy_failure_mask = 0;
int pthread_cond_destroy (pthread_cond_t __attribute__((__unused__))*__restrict __cond)
{
     // Return failure based on bit set in mask
     // This allows multiple or delayed failure call
     if (0 == (mock_pthread_cond_destroy_failure_mask & (1U << mock_pthread_cond_destroy_count++))) {
          return 0;
     }
     return mock_pthread_cond_destroy_failure;
}

int mock_pthread_mutex_destroy_failure = 0;
int mock_pthread_mutex_destroy_count = 0;
int pthread_mutex_destroy (pthread_mutex_t __attribute__((__unused__))*__mutex)
{
     mock_pthread_mutex_destroy_count++;
     return mock_pthread_mutex_destroy_failure;
}

int mock_pthread_mutex_lock_failure = 0;
int mock_pthread_mutex_lock_count = 0;
int pthread_mutex_lock (pthread_mutex_t __attribute__((__unused__))*__mutex)
{
     mock_pthread_mutex_lock_count++;
     return mock_pthread_mutex_lock_failure;
}

int mock_pthread_mutex_unlock_failure = 0;
int mock_pthread_mutex_unlock_count = 0;
int pthread_mutex_unlock (pthread_mutex_t __attribute__((__unused__))*__mutex)
{
     mock_pthread_mutex_unlock_count++;
     return mock_pthread_mutex_unlock_failure;
}

int mock_pthread_cond_wait_failure = 0;
int mock_pthread_cond_wait_count = 0;
int pthread_cond_wait (pthread_cond_t __attribute__((__unused__))*__restrict __cond,
                     pthread_mutex_t __attribute__((__unused__))*__restrict __mutex)
{
     mock_pthread_cond_wait_count++;
     return mock_pthread_cond_wait_failure;
}

int mock_pthread_cond_signal_failure = 0;
int mock_pthread_cond_signal_count = 0;
int pthread_cond_signal (pthread_cond_t __attribute__((__unused__))*__restrict __cond)
{
     mock_pthread_cond_signal_count++;
     return mock_pthread_cond_signal_failure;
}
