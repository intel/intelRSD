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


#ifndef __SNMP_HANDLER_H__
#define __SNMP_HANDLER_H__

extern void send_notification(unsigned int type, const char *msg);

//extern void add_trap_ip(int ip, int port);
//extern void remove_trap_ip(int ip, int port);

void* add_trap_ip(char* sink, const char* sinkport, const char* com);

void remove_trap_sess(void *sess);
#endif