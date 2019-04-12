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
 * @file logger_time.h
 *
 * @brief Logger time interface
 * */

#ifndef LOGGER_TIME_H
#define LOGGER_TIME_H

#include "logger/logger.h"

#include <time.h>

/*!
 * @struct logger_time
 * @brief Logger time object
 *
 * @var logger_time::ts
 * Time primitive
 * */
struct logger_time {
    struct timespec ts;
};

/*!
 * @brief Implemented by user. Update logger time instance with current
 * system time
 *
 * @param[in]   inst    Logger time instance
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_time_update(struct logger_time *inst);

/*!
 * @brief Implemented by user. Check if given time was elapsed compare to
 * current time
 *
 * @param[in]   inst        Logger time instance
 * @param[in]   pelapsed    Return boolean value of elapsed time, true when
 *                          time elapsed otherwise false
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_time_elapsed(struct logger_time *inst, bool *pelapsed);

/*!
 * @brief Implemented by user. Get time value from logger time instance
 *
 * @param[in]   inst    Logger time instance
 * @param[out]  psec    Get seconds from logger time instance
 * @param[out]  pnsec   Get nanoseconds from logger time instance
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_time_get(struct logger_time *inst, time_t *psec, long int *pnsec);

/*!
 * @brief Implemented by user. Add time value to logger time instance
 *
 * @param[in]   inst    Logger time instance
 * @param[in]   sec     Give seconds to add
 * @param[in]   nsec    Give nanoseconds to add
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_time_add(struct logger_time *inst, time_t sec, long int nsec);

/*!
 * @brief Compare time
 *
 * @param[in]   inst1   Logger time instance 1
 * @param[in]   inst2   Logger time instance 2
 * @return      When inst1 < inst2 return  1,
 *              when inst1 = inst2 return  0,
 *              when inst1 > inst2 return -1
 * */
int logger_time_compare(struct logger_time *inst1,
        struct logger_time *inst2);

#endif /* LOGGER_TIME_H */
