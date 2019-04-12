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
 *
 * @file logger_time.c
 *
 * @brief Logger time implementation
 * */

#include "logger_time.h"

#include "logger_assert.h"

int logger_time_update(struct logger_time *inst) {
    logger_assert(NULL != inst);

    return clock_gettime(CLOCK_REALTIME, &inst->ts);
}

int logger_time_compare(struct logger_time *inst1,
        struct logger_time *inst2) {
    logger_assert(NULL != inst1);
    logger_assert(NULL != inst2);

    int compare;

    if (inst2->ts.tv_sec == inst1->ts.tv_sec) {
        if (inst2->ts.tv_nsec > inst1->ts.tv_nsec) {
            compare = 1;
        } else if (inst2->ts.tv_nsec == inst1->ts.tv_nsec) {
            compare = 0;
        } else {
            compare = -1;
        }
    } else if (inst2->ts.tv_sec > inst1->ts.tv_sec) {
        compare = 1;
    } else {
        compare = -1;
    }

    return compare;
}

int logger_time_elapsed(struct logger_time *inst, bool *pelapsed) {
    logger_assert(NULL != inst);

    bool elapsed;
    struct timespec current;

    int err = clock_gettime(CLOCK_REALTIME, &current);
    if (err) {
        return err;
    }

    if (current.tv_sec == inst->ts.tv_sec) {
        if (current.tv_nsec > inst->ts.tv_nsec) {
            elapsed = true;
        } else {
            elapsed = false;
        }
    } else if (current.tv_sec > inst->ts.tv_sec) {
        elapsed = true;
    } else {
        elapsed = false;
    }

    if (NULL != pelapsed) {
        *pelapsed = elapsed;
    }

    return LOGGER_SUCCESS;;
}

int logger_time_get(struct logger_time *inst, time_t *psec, long int *pnsec) {
    logger_assert(NULL != inst);

    if (NULL != psec) {
        *psec = inst->ts.tv_sec;
    }

    if (NULL != pnsec) {
        *pnsec = inst->ts.tv_nsec;
    }

    return LOGGER_SUCCESS;
}

int logger_time_add(struct logger_time *inst, time_t sec, long int nsec) {
    logger_assert(NULL != inst);

    inst->ts.tv_sec += sec;
    inst->ts.tv_nsec += nsec;

    return LOGGER_SUCCESS;
}
