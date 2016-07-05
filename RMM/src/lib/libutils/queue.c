/**
 * Copyright (c)  2015, Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "queue.h"

#include <stdio.h>

static msg_list_t *pool_get(queue_t *q)
{
	msg_list_t *msg_list = NULL;

	if (q->msg_pool != NULL) {
		msg_list = q->msg_pool;
		q->msg_pool = msg_list->next;
		q->msg_pool_nr--;
	} else
		msg_list = (msg_list_t *)malloc(sizeof(msg_list_t));

	assert(msg_list);
	return msg_list;
}

static void pool_add(queue_t *q, msg_list_t *msg_list)
{
	if (q->msg_pool_nr > MSG_POOL_MAX) {
		free(msg_list);
	} else {
		bzero(msg_list, sizeof(msg_list_t));
		msg_list->next = q->msg_pool;
		q->msg_pool = msg_list;
		q->msg_pool_nr++;
	}
}

queue_t *queue_init(void)
{
	queue_t *q = NULL;

	q = (queue_t *)malloc(sizeof(queue_t));
	assert(q);
	bzero(q, sizeof(queue_t));

	if (pthread_mutex_init(&q->mutex, NULL) != 0) {
		free(q);
		return NULL;
	}

	if (pthread_cond_init(&q->cond, NULL) != 0) {
		pthread_mutex_destroy(&q->mutex);
		free(q);
		return NULL;
	}

	return q;
}

void queue_add(queue_t *q, void *data, int len, int type)
{
	msg_list_t *msg_list = NULL;

	assert(q);
	assert(data);

	pthread_mutex_lock(&q->mutex);
	msg_list = pool_get(q);
	assert(msg_list);

	msg_list->msg.data = data;
	msg_list->msg.type = type;
	msg_list->msg.len = len;
	msg_list->next = NULL;

	if (q->first == NULL)
		q->first = q->last = msg_list;
	else {
		q->last->next = msg_list;
		q->last = msg_list;
	}

	if (q->msg_nr == 0)
		pthread_cond_broadcast(&q->cond);
	q->msg_nr++;

	pthread_mutex_unlock(&q->mutex);
	return;
}

void queue_get(queue_t *q, msg_t *msg)
{
	msg_list_t *first;

	assert(q); assert(msg);
	pthread_mutex_lock(&q->mutex);
	while (q->msg_nr == 0)
		pthread_cond_wait(&q->cond, &q->mutex);

	first = q->first;
	q->first = first->next;
	q->msg_nr--;

	msg->data = first->msg.data;
	msg->type = first->msg.type;
	msg->len = first->msg.len;

	pool_add(q, first);
	pthread_mutex_unlock(&q->mutex);
	return;
}
