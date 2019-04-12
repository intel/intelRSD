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
 * @file logger_list.c
 *
 * @brief Logger list implementation
 * */

#include "logger_list.h"

#include "logger_assert.h"
#include "logger_memory.h"

void logger_list_init(struct logger_list *inst) {
    logger_assert(NULL != inst);

    *inst = LOGGER_LIST_INIT;
}

bool logger_list_empty(struct logger_list *inst) {
    logger_assert(NULL != inst);

    return (inst->first == NULL) ? true : false;
}

int logger_list_push(struct logger_list *inst, void *object, int id) {
    logger_assert(NULL != inst);

    struct logger_list_node *node =
        logger_memory_alloc(sizeof(struct logger_list_node));

    if (NULL == node) {
        return LOGGER_ERROR_NULL;
    }

    node->next = NULL;
    node->prev = NULL;
    node->object = object;
    node->id = id;

    if (NULL == inst->first) {
        inst->first = node;
        inst->last = node;
    } else {
        inst->last->next = node;
        node->prev = inst->last;
        inst->last = node;
    }

    return LOGGER_SUCCESS;
}

void *logger_list_pop(struct logger_list *inst, int *id) {
    logger_assert(NULL != inst);

    void *object = NULL;
    struct logger_list_node *node = inst->first;

    if (NULL != node) {
        object = node->object;
        if (NULL != id) {
            *id = node->id;
        }
        logger_list_remove_node(inst, node);
    }

    return object;
}

void logger_list_sort(struct logger_list *inst,
        logger_list_compare_t compare) {
    logger_assert(NULL != inst);

    if (inst->first == inst->last) {
        return;
    }

    struct logger_list_node *const last = inst->last->prev;
    struct logger_list_node *iit;
    struct logger_list_node *it;

    bool swap = true;
    void *object;
    int id;

    for (iit = inst->first; (NULL != iit) && (true == swap); iit = iit->next) {
        swap = false;
        for (it = inst->first; last != it; it = it->next) {
            if (true == compare(it->object, it->id,
                        it->next->object, it->next->id)) {
                /* Store */
                object = it->object;
                id = it->id;
                /* Left swap */
                it->object = it->next->object;
                it->id = it->next->id;
                /* Right swap */
                it->next->object = object;
                it->next->id = id;
                swap = true;
            }
        }
    }
}

void logger_list_clear(struct logger_list *inst) {
    logger_assert(NULL != inst);

    struct logger_list_node *it;
    struct logger_list_node *next;

    for (it = inst->first; it != NULL; it = next) {
        next = it->next;
        logger_memory_free(it);
    }

    *inst = LOGGER_LIST_INIT;
}

void logger_list_move(struct logger_list *inst,
        struct logger_list *list_to_move) {
    logger_assert(NULL != inst);
    logger_assert(NULL != list_to_move);

    if (NULL != list_to_move->first) {
        if (NULL != inst->first) {
            inst->last->next = list_to_move->first;
            inst->last = list_to_move->last;
        } else {
            inst->first = list_to_move->first;
            inst->last = list_to_move->last;
        }
        *list_to_move = LOGGER_LIST_INIT;
    }
}

void logger_list_remove_node(struct logger_list *inst,
        struct logger_list_node *node) {
    logger_assert(NULL != inst);

    if ((NULL == inst->first) || (NULL == node)) return;

    if ((node == inst->first) && (node == inst->last)) {
        inst->first = NULL;
        inst->last = NULL;
    } else if (node == inst->first) {
        inst->first = node->next;
    } else if (node == inst->last) {
        inst->last = node->prev;
    } else {
        struct logger_list_node *after = node->next;
        struct logger_list_node *before = node->prev;

        after->prev = before;
        before->next = after;
    }

    logger_memory_free(node);
}

bool logger_list_exist(struct logger_list *inst, void *object) {
    logger_assert(NULL != inst);

    struct logger_list_node *it;

    for (it = inst->first; it != NULL; it = it->next) {
        if (it->object == object) {
            return true;
        }
    }

    return false;
}
