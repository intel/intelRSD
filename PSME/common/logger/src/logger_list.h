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
 * @file logger_list.h
 *
 * @brief Logger list interface
 * */

#ifndef LOGGER_LIST_H
#define LOGGER_LIST_H

#include "logger/logger.h"

/*!
 * @def LOGGER_LIST_INIT
 * Logger list object initialization
 * */
#define LOGGER_LIST_INIT            (struct logger_list){NULL, NULL}

/*! Logger compare function */
typedef bool (*logger_list_compare_t)(
        void *obj1, int id1,
        void *obj2, int id2
    );

/*!
 * @struct logger_list_node
 * @brief Logger list node object
 *
 * @var logger_list_node::next
 * Next list node
 *
 * @var logger_list_node::prev
 * Previous list node
 *
 * @var logger_list_node::object
 * Object
 *
 * @var logger_list_node::id
 * Object id
 * */
struct logger_list_node {
    struct logger_list_node *next;
    struct logger_list_node *prev;
    void *object;
    int id;
};

/*!
 * @struct logger_list
 * @brief Logger list object
 *
 * @var logger_list::first
 * First node in the list
 *
 * @var logger_list::last
 * Last node in the list
 * */
struct logger_list {
    struct logger_list_node *first;
    struct logger_list_node *last;
};

/*!
 * @brief Initialize logger list instance
 *
 * @param[in]   inst Logger list instance
 * */
void logger_list_init(struct logger_list *inst);

/*!
 * @brief Check if logger list is empty
 *
 * @param[in]   inst Logger list instance
 * @return      When logger list is empty return true otherwise return false
 * */
bool logger_list_empty(struct logger_list *inst);

/*!
 * @brief Check if object already exist in logger list
 *
 * @param[in]   inst    Logger list instance
 * @param[in]   object  Object to find and compare in the list
 * @return      When object already exist in the list return true otherwise
 *              return false
 * */
bool logger_list_exist(struct logger_list *inst, void *object);

/*!
 * @brief Push new object to list (FIFO)
 *
 * @param[in]   inst    Logger list instance
 * @param[in]   id      Object ID
 * @param[in]   object  Object to push
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_list_push(struct logger_list *inst, void *object, int id);

/*!
 * @brief Pop object from list (FIFO)
 *
 * @param[in]   inst    Logger list instance
 * @param[out]  id      Return object ID
 * @return      When list is not empty return object otherwise return NULL
 * */
void *logger_list_pop(struct logger_list *inst, int *id);

/*!
 * @brief Clear list. Doesn't destroy object!
 *
 * @param[in]   inst    Logger list instance to clear
 * */
void logger_list_clear(struct logger_list *inst);

/*!
 * @brief Move given list to new list
 *
 * @param[in]   inst            Logger list instance to add new moved list
 * @param[in]   list_to_move    Logger list to move. Given list will be empty
 * */
void logger_list_move(struct logger_list *inst,
        struct logger_list *list_to_move);

/*!
 * @brief Sort list
 *
 * @param[in]   inst            Logger list instance to sort
 * @param[in]   compare         Compare function
 * */
void logger_list_sort(struct logger_list *inst,
        logger_list_compare_t compare);

/*!
 * @brief Remove node in the list
 *
 * @param[in]   inst    Logger list instance
 * @param[in]   node    List node to remove
 * */
void logger_list_remove_node(struct logger_list *inst,
        struct logger_list_node *node);

#endif /* LOGGER_LIST_H */
