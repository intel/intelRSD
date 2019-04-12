/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file rmm/ptas_reader/ptas_sdk/ptas_sdk.c
 */

#include "ptas_reader/ptas_sdk/ptas_sdk.h"

int PTAS_offer(PTAS_Statistic_t * ptas_st) {
    return -1;
}

int PTAS_harvest(PTAS_Output_t * output) {
    return -1;
}

int PTAS_wait_timeout(PTAS_Statistic_t * ptas_st, PTAS_Output_t * output, unsigned int sc) {
    output->rc = 0;
    output->sequence_num = ptas_st->sequence_num;
    output->Q = 0;
    output->outlet_temp = 0;
    return -1;
}

int libptas_init(PTAS_config_t * p_config, ptas_rsp_callback_fn cb_fn) {
    return -1;
}
