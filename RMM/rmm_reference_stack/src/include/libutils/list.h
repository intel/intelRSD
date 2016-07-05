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


#ifndef __LIBUTILS_LIST_H__
#define __LIBUTILS_LIST_H__

#define offset_of(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({	\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offset_of(type,member) );})


struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define list_entry(ptr, type, member) container_of(ptr, type, member)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline int list_empty(struct list_head *head)
{
	return head->next == head;
}

static inline int list_is_last(struct list_head *list, struct list_head *head)
{
	return list->next == head;
}

static inline void __list_add(struct list_head *item,
				struct list_head *prev,
				struct list_head *next)
{
	next->prev = item;
	item->next = next;

	item->prev = prev;
	prev->next = item;
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

static inline void list_add(struct list_head *item, struct list_head *head)
{
	__list_add(item, head, head->next);
}

static inline void list_add_tail(struct list_head *item, struct list_head *head)
{
	__list_add(item, head->prev, head);
}


#define list_for_each_entry(pos, head, member)	\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
		 (pos != NULL) && (&pos->member != (head));	\
		 pos = list_entry(pos->member.next, typeof(*pos), member))


#define list_for_each_entry_safe(pos, n, head, member)	\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		   n = list_entry(pos->member.next, typeof(*pos), member);	\
		 (pos != NULL) && (&pos->member != (head));	\
		 pos = n, n = list_entry(n->member.next, typeof(*n), member))


#endif

