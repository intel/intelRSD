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


#ifndef __LIBUTILS_RMM_H__
#define __LIBUTILS_RMM_H__

#include "libutils/rack.h"
#include "libredfish/rf_event.h"
#include "libsecurec/safe_lib.h"

#define PLATFORM_BDC_R
#define PLATFORM_NAME_BDC_R     "bdc-r"
#define PLATFORM_NAME_BDC_A     "bdc-a"

#define MAX_URL                     1024
#define PREFIX_LEN                  32
#define UUID_MAX_LEN	            37

#define IPMI_COOLING_CTRL_PORT		50050
#define IPMI_POWER_MGMT_PORT		(IPMI_COOLING_CTRL_PORT + 1)
#define IPMI_ASSET_MON_PORT			(IPMI_POWER_MGMT_PORT + 1)
#define IPMI_MEMDB_TEST_PORT		(IPMI_ASSET_MON_PORT + 1)
#define IPMI_TEST_PORT				(IPMI_MEMDB_TEST_PORT + 1)
#define IPMI_UART_TGT_CFG_PORT		(IPMI_TEST_PORT + 1)
#define IPMI_WEB_SOCKET_PORT		(IPMI_UART_TGT_CFG_PORT + 1)
#define IPMI_JSON_RPC_SVR_PORT		(IPMI_WEB_SOCKET_PORT + 1)
#define IPMI_JSON_RPC_IPMID_PORT	(IPMI_JSON_RPC_SVR_PORT + 1)

#define RACK_HTTPD_PORT         8090
#define RACK_HTTPD_PORT_STR     "8090"
#define RACK_HTTPSD_PORT        8091
#define RACK_HTTPSD_PORT_STR    "8091"
#define RACK_API_VERSION        "1.0"

#define IPMI_APP_NETFN          0x06
#define IPMI_LANCFG_NETFN       0x0C
#define IPMI_CM_NETFN           0x38

#define WARM_RESET_CMD			0x03
#define UPDATE_FIRMWARE_NETFN   0x30
#define UPDATE_FM_SEND_IP_CMD   0x23
#define UPDATE_FM_SEND_FILE_CMD 0x23
#define UPDATE_FM_START_CMD     0x24
#define CHASSIS_RESET_CMD       0x01
#define CHASSIS_PRESENCE_CMD    0x02
#define FAN_PRESENCE_CMD        0x05
#define PSU_PRESENCE_CMD        0x0B
#define SEND_PSU_CMD            0x0E
#define PZONE_PRESENCE_CMD      0x0A
#define GET_ALERT_CMD           0x03
#define GET_FAN_PWM_CMD         0x06
#define SET_DEFAULT_PWM_CMD     0x07
#define FAN_TACH_METER_CMD      0x08
#define UART_SWITCH_CMD         0x09
#define PSU_READ_PIN_CMD        0x25
#define PSU_READ_IOUT_CMD       0x21
#define CHASSIS_BR_CHAN0        0x00
#define CHASSIS_BR_CHAN1        0x01
#define CHASSIS_BR_CHAN2        0x02
#define CHASSIS_BR_CHAN3        0x03
#define CHASSIS_BR_CHAN4        0x04
#define CHASSIS_BR_CHAN5        0x05
#define CHASSIS_BR_CHAN6        0x06
#define CHASSIS_BR_CHAN7        0x07
#define CHASSIS_BR_ADDR0        0x20
#define CHASSIS_BR_ADDR1        0x20
#define CHASSIS_BR_ADDR2        0x20
#define CHASSIS_BR_ADDR3        0x20
#define CHASSIS_BR_ADDR4        0x20
#define CHASSIS_BR_ADDR5        0x20
#define CHASSIS_BR_ADDR6        0x20
#define CHASSIS_BR_ADDR7        0x20

#define DRAWER_IP_CMD           0x90
#define SET_ID_FIELD_CMD        0x06
#define GET_ID_FIELD_CMD        0x07

#define GET_AGGREGATED_THERMAL_SENSOR	0xC1
#define GET_AGGREGATED_HEALTH_SENSOR	0xC2
#define GET_TRAY_POWER					0xC3

#define DRAWER_SET_CERT		0xC4
#define DRAWER_GET_CERT		0xC5
#define DRAWER_GET_CERT_HASH	0xC6

#define DRAWER_CERT_TYPE_PODM	0x00
#define DRAWER_CERT_TYPE_RMM	0x01

#define FIELD_TYPE_DEFAULT      0x00

#define INSTANCE_RACK_PUID      0x01
#define INSTANCE_RACK_BPID      0x02
#define INSTANCE_DRAWER_RUID    0x03
#define INSTANCE_RACK_ID    0x04

#define CHASSIS_TEMP_CMD             0x08
#define CHASSIS_SET_RACKID_CMD       0xFF
#define CHASSIS_SET_BACKPLANEID_CMD  0xFF
#define FAN_GET_PWM_CMD              0x01
#define CPP_DEBUG_CHASSIS_PRESENT    0x10

#define CURRENT_OUT_CMD          0x8C
#define POWER_IN_CMD             0x97

#define PSU_READ_BYTE            0x00
#define PSU_READ_WORD            0x01
#define PSU_READ_BLK             0x02
#define PSU_WRITE_BYTE           0x03
#define PSU_WRITE_WORD           0x04
#define PSU_WRITE_BLK            0x05

#define PSU_OFFSET_STATUS        0x79
#define PSU_OFFSET_POWER_OUT     0x96
#define PSU_OFFSET_MANUFACTURE   0x99
#define PSU_OFFSET_MODEL         0x9A
#define PSU_OFFSET_REVISION      0x9B
#define PSU_OFFSET_SERIAL        0x9E
#define PSU_OFFSET_POWER_CAP     0xA7

#define PSU_OPERATION            0x01
#define GET_DEVID_CMD            0x01

#define GET_LANCFG_CMD           0x02
#define GET_LANCFG_DATA0         0x01
#define GET_LANCFG_DATA1         0x03
#define GET_LANCFG_DATA2         0x00
#define GET_LANCFG_DATA3         0x00
#define GET_LANCFG_DATA_LEN      0x04
#define GET_LANCFG_IPSRC_DATA1			0x04
#define GET_LANCFG_SUBNET_MASK_DATA1	0x06



#define OEM_IPMI_NETFN           0x30
#define OEM_SERIAL_OPEN_CMD      0x01
#define OEM_SERIAL_CLOSE_CMD     0x02

#define INVALID_IP               0x00
#define INVALID_DEVID            0x00

#define IP_RSP_OFFSET            2
#define PWM_RSP_OFFSET           3

#define GAMI_MAX_CAP_NUM         4
#define GAMI_MAX_NAME_LEN        32
#define GAMI_MAX_CAP_STR_LEN     128

#define GAMI_CAP_SPLIT_STR       ";"

#define	HTTPD_IPMI_DEBUG
#ifdef	HTTPD_IPMI_DEBUG
#define	HTTPD_IPMI_PRINT	printf
#endif

extern char *insert_str(char *dest, const char *source, unsigned int first, const char *ins_str);
extern int hexstr2buf(const char *hex_str, int size, char *buf);
extern int buf2hexstr(const char *buf, int size, char *hex_str);

extern int fill_string_from_str(char *output, char *input);
extern int fill_string_from_int(char *output, int input);

extern void reg_sigterm_handler(void (*handler)(int s));

#if 0
#define FILL_STR(output, input) fill_string_from_str(output, input)
#define FILL_INT(output, input) fill_string_from_int(output, input)
#else
#define FILL_STR(output, input) \
	{	\
		if ((output) != NULL) {	\
			snprintf_s_s((output), sizeof((output)), "%s", (input));	\
		}	\
	}

#define FILL_INT(output, input) \
        {       \
                if ((output) != NULL) { \
                	snprintf_s_i((output), sizeof((output)), "%d", (int32)(input));    \
		}	\
        }
#endif

#if 1
#define NDISC_DEBUG(format, args...)		printf(format, ##args)
#else
#define NDISC_DEBUG(format, args...)
#endif

typedef struct gami_cap {
	char cap_mbp[GAMI_MAX_CAP_NUM][GAMI_MAX_NAME_LEN];
	char cap_psu[GAMI_MAX_CAP_NUM][GAMI_MAX_NAME_LEN];
	char cap_fan[GAMI_MAX_CAP_NUM][GAMI_MAX_NAME_LEN];
	char cap_drawer[GAMI_MAX_CAP_NUM][GAMI_MAX_NAME_LEN];
} gami_cap_t;

typedef struct gami_reg {
	char name[MAX_NAME_LEN];
	int  ip_addr;
	int  port;
	gami_cap_t capability;
} gami_reg_t;


#define GETU8(c, cp) ({ \
	(c) = *(cp)++; \
})
#define PUTU8(c, cp) ({ \
	*(cp)++ = (unsigned char) (c); \
})

#define GETU16(s, cp) ({ \
	(s)  = *(cp)++ << 8; \
	(s) |= *(cp)++; \
})
#define PUTU16(s, cp) ({ \
	*(cp)++ = (unsigned char) ((s) >> 8); \
	*(cp)++ = (unsigned char) ((s)); \
})

#define GETU32(l, cp) ({ \
	(l)  = *(cp)++ << 24; \
	(l) |= *(cp)++ << 16; \
	(l) |= *(cp)++ <<  8; \
	(l) |= *(cp)++; \
})
#define PUTU32(l, cp) ({ \
	*(cp)++ = (unsigned char) ((l) >> 24); \
	*(cp)++ = (unsigned char) ((l) >> 16); \
	*(cp)++ = (unsigned char) ((l) >>  8); \
	*(cp)++ = (unsigned char) ((l)); \
})

#define INCPTR(n, cp)	((cp) += (n))
#define DECPTR(n, cp)	((cp) -= (n))

#define rf_snmp_evt(level, msg_sn, args...)  rf_msg_send(__func__, level, RF_EVENT, msg_sn, ##args); snmp_subagentd_msg_send(__func__, level, RF_EVENT, msg_sn, ##args)


#define INIT_STEP_CHECKER()	char step_checker[128] = {}
#define SET_STEP_CHECKER(str)	memcpy_s(step_checker, 128-1, str, 128-1)
#define PRINT_STEP_CHECKER(level)	rmm_log(level, "Function %s failed at %s\n", __FUNCTION__, step_checker)

#endif

