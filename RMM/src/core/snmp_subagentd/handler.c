/**
 * Copyright (c)  2015-2017 Intel Corporation.
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


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/session_api.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "libsecurec/safe_lib.h"
#include <stdlib.h>

void *add_trap_ip(char* sink, const char* sinkport, char *com)
{
    netsnmp_transport *t = NULL;
    struct snmp_session session, *sesp = NULL;
    int pdutype = SNMP_MSG_TRAP2;
    int version = SNMP_VERSION_2c;
    memset(&session, 0, sizeof(netsnmp_session));

    //snmp_sess_init( &session );
    session.version = version;
    session.community = (u_char *)com;
    session.community_len = strnlen_s(com, RSIZE_MAX_STR);
    if ((0 == strcmp("localhost",sink)) || (0 == strcmp("127.0.0.1",sink)))
        session.localname = strdup("localhost");

    t = netsnmp_tdomain_transport_full("snmptrap", sink, 0, NULL, sinkport);
    if (t != NULL) {
        sesp = snmp_add(&session, t, NULL, NULL);
    }
    add_trap_session(sesp, pdutype, (pdutype == SNMP_MSG_INFORM), version);

    // snmp_add copy session.
    free(session.localname);
    return sesp;
}


void send_notification(unsigned int type, const char *msg)
{
    oid             notification_oid[] =  { 1, 3, 6, 1, 4, 1, 343, 1, 1, 1, 0, 1 };
    size_t          notification_oid_len = OID_LENGTH(notification_oid);
    static u_long count = 0;


    oid      objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
    size_t   objid_snmptrap_len = OID_LENGTH(objid_snmptrap);


    oid      type_oid[]   = { 1, 3, 6, 1, 4, 1, 343, 1, 1, 1, 2, 1, 0 };
    size_t   type_oid_len = OID_LENGTH(type_oid);
    oid      msg_oid[]   = { 1, 3, 6, 1, 4, 1, 343, 1, 1, 1, 2, 2, 0 };
    size_t   msg_oid_len = OID_LENGTH(msg_oid);

    /*
     * here is where we store the variables to be sent in the trap 
     */
    netsnmp_variable_list *notification_vars = NULL;


    /*
     * add in the trap definition object 
     */
    snmp_varlist_add_variable(&notification_vars,
                              /*
                               * the snmpTrapOID.0 variable 
                               */
                              objid_snmptrap, objid_snmptrap_len,
                              /*
                               * value type is an OID 
                               */
                              ASN_OBJECT_ID,
                              /*
                               * value contents is our notification OID 
                               */
                              (u_char *) notification_oid,
                              /*
                               * size in bytes = oid length * sizeof(oid) 
                               */
                              notification_oid_len * sizeof(oid));

    /*
     * add in the additional objects defined as part of the trap
     */

    snmp_varlist_add_variable(&notification_vars,
                               type_oid, type_oid_len,
                               ASN_INTEGER,
                              (u_char *)&type,
                                  sizeof(type));

    /*
     * if we want to insert additional objects, we do it here 
     */

    snmp_varlist_add_variable(&notification_vars,
                           msg_oid, msg_oid_len,
                           ASN_OCTET_STR,
                           msg, strnlen_s(msg, RSIZE_MAX_STR));


    /*
     * send the trap out.  This will send it to all registered
     * receivers (see the "SETTING UP TRAP AND/OR INFORM DESTINATIONS"
     * section of the snmpd.conf manual page. 
     */
    ++count;
    DEBUGMSGTL(("send_notification", "sending trap %ld\n",count));
    send_v2trap(notification_vars);

    /*
     * free the created notification variable list 
     */
    DEBUGMSGTL(("send_notification", "cleaning up\n"));
    snmp_free_varbind(notification_vars);
}

void remove_trap_sess(void *sess)
{
    remove_trap_session((struct snmp_session *)sess);
    return;
}
