#ifndef _MOCK_PTHREAD
    #define _MOCK_PTHREAD
    #include <stdbool.h>
    #include <pthread.h>
    #include <stdint.h>

    extern int mock_pthread_mutex_init_failure;
    extern int mock_pthread_mutex_init_count;

    extern int mock_pthread_mutex_destroy_failure;
    extern int mock_pthread_mutex_destroy_count;

    extern int mock_pthread_mutex_lock_failure;
    extern int mock_pthread_mutex_lock_count;

    extern int mock_pthread_mutex_unlock_failure;
    extern int mock_pthread_mutex_unlock_count;

    extern int mock_pthread_cond_init_failure;
    extern int mock_pthread_cond_init_count;
    extern uint8_t mock_pthread_cond_init_failure_mask;

    extern int mock_pthread_cond_wait_failure;
    extern int mock_pthread_cond_wait_count;

    extern int mock_pthread_cond_destroy_failure;
    extern int mock_pthread_cond_destroy_count;
    extern uint8_t mock_pthread_cond_destroy_failure_mask;

    extern int mock_pthread_cond_signal_failure;
    extern int mock_pthread_cond_signal_count;
#endif
