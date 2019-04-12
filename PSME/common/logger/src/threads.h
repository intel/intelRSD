/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 */

#ifndef THREADS_H
#define THREADS_H

#include "pthread.h"

#include <time.h>
#include <errno.h>

/* @see Documentation reference: http://en.cppreference.com/w/c/thread */

enum {
    thrd_success,
    thrd_timedout,
    thrd_busy,
    thrd_nomem,
    thrd_error
};

enum {
    mtx_plain,
    mtx_recursive,
    mtx_timed
};

typedef pthread_t thrd_t;

typedef pthread_mutex_t mtx_t;

typedef pthread_cond_t cnd_t;

typedef void* (*thrd_start_t)(void*);

/* Threads */
static inline int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
    return !pthread_create(thr, NULL, func, arg) ? thrd_success : thrd_error;
}

static inline int thrd_equal(thrd_t lhs, thrd_t rhs) {
    return pthread_equal(lhs, rhs);
}

static inline thrd_t thrd_current(void) {
    return pthread_self();
}

static inline int thrd_sleep(const struct timespec* time_point,
        struct timespec* remaining) {
    return nanosleep(time_point, remaining);
}

static inline void thrd_yield(void) {
    //pthread_yield();
}

_Noreturn static inline void thrd_exit(int res) {
    pthread_exit(&res);
}

static inline int thrd_detach(thrd_t thr) {
    return !pthread_detach(thr) ? thrd_success : thrd_error;
}

static inline int thrd_join(thrd_t thr, int *res) {
    return !pthread_join(thr, (void*)&res) ? thrd_success : thrd_error;
}

/* Mutex */
static inline int mtx_init(mtx_t *mutex, int type) {
    (void)type;
    return !pthread_mutex_init(mutex, NULL) ? thrd_success : thrd_error;
}

static inline int mtx_lock(mtx_t *mutex) {
    return !pthread_mutex_lock(mutex) ? thrd_success : thrd_error;
}

static inline int mtx_timedlock(mtx_t *mutex,
        const struct timespec *restrict time_point) {
    return !pthread_mutex_timedlock(mutex, time_point) ?
        thrd_success : thrd_error;
}

static inline int mtx_trylock(mtx_t *mutex) {
    return !pthread_mutex_trylock(mutex) ? thrd_success : thrd_error;
}

static inline int mtx_unlock(mtx_t *mutex) {
    return !pthread_mutex_unlock(mutex) ? thrd_success : thrd_error;
}

static inline void mtx_destroy(mtx_t *mutex) {
    pthread_mutex_destroy(mutex);
}

/* Conditional variables */
static inline int cnd_init(cnd_t *cond) {
    return !pthread_cond_init(cond, NULL) ? thrd_success : thrd_error;
}

static inline int cnd_signal(cnd_t *cond) {
    return !pthread_cond_signal(cond) ? thrd_success : thrd_error;
}

static inline int cnd_broadcast(cnd_t *cond) {
    return !pthread_cond_broadcast(cond) ? thrd_success : thrd_error;
}

static inline int cnd_wait(cnd_t *cond, mtx_t *mutex) {
    return !pthread_cond_wait(cond, mutex) ? thrd_success : thrd_error;
}

static inline int cnd_timedwait(cnd_t *cond, mtx_t* restrict mutex,
        const struct timespec* restrict time_point) {
    int err = pthread_cond_timedwait(cond, mutex, time_point);

    if (ETIMEDOUT == err) {
        return thrd_timedout;
    }
    if (err) {
        return thrd_error;
    }
    return thrd_success;
}

static inline void cnd_destroy(cnd_t *cond) {
    pthread_cond_destroy(cond);
}

#endif /* THREADS_H */
