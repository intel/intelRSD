/**
 * Copyright (c)  2014-2018 Intel Corporation.
 *
 * This Software is furnished under license and may only be used or
 * copied in accordance with the terms of that license. No license,
 * express or implied, by estoppel or otherwise, to any intellectual
 * property rights is granted by this document. The Software is
 * subject to change without notice, and should not be construed as
 * a commitment by Intel Corporation to market, license, sell or
 * support any product or technology. Unless otherwise provided for
 * in the * license under which this Software is provided, the
 * Software is provided AS IS, with no warranties of any kind,
 * express or implied. Except as expressly permitted by the Software
 * license, neither Intel Corporation nor its suppliers assumes any
 * responsibility or liability for any errors or inaccuracies that
 * may appear herein. Except as expressly permitted by the Software
 * license, no part of the Software may be reproduced, stored in a
 * retrieval system, transmitted in any form, or distributed by any
 * means without the express written consent of Intel Corporation.
 */


#ifndef __PTAS_SDK_H__
#define __PTAS_SDK_H_

//#include <ptasd_common.h>

#define MAX_IPSTRINGLEN 32

#define PTASMSGSIZ				1024

#define PTAS_APP_PORT			55000


#define PTASDICT_MAXFAN     15  //20U, 5 Fan Shelf, 1Shelf 3 Fan
#define PTASDICT_MAXCOE     PTASDICT_MAXFAN+1
#define PTASDICT_MAXTRAY    10   //tray is 2U
#define SLED_PERTRAY        3
#define PTASDICT_MAXSLED    PTASDICT_MAXTRAY*SLED_PERTRAY  //20U, 2U3N, 10 tray

typedef unsigned char       PTAS_bool;
#define PTAS_true    1
#define PTAS_false   0


typedef enum {
    PTASREQ_INIT = 1,
    PTASREQ_CONFIG,
    PTASREQ_DICTELEM,
    PTASREQ_OFFER,
    PTASREQ_FETCH,
    PTASREQ_RESVD = 0xf
}PTASREQ_TYPE;

typedef enum {
	PTASRSP_OUTPUT = 0x10,
    PTASRSP_CONFIG,
    PTASRSP_DICTELEM,
    PTASRSP_RESVD = 0x1f
}PTASRSP_TYPE;

typedef enum {
    PTASDRC_NORMAL = 0,
    PTASRC_CONFIG_INVALID = 1<<0,
    PTASDRC_HWENUM_NOTSUPORT = 1<<1,     //tray&sled configuration not support
    PTASDRC_DATA_INVALID = 1<<2,         //provided data invalid
    PTASDRC_NO_CONFIG = 1<<3,
    PTASDRC_MULTICONFIG = 1<<4,
    PTASDRC_RESVD = 1<<31
}PTASD_RC;


typedef unsigned int dict_bitmap;
typedef double dict_coeff;
/* ptasd_dict format
*/
typedef struct ptasdict_element
{
    dict_bitmap     fan_map;
    dict_bitmap     sled_map;
    dict_coeff      coe[PTASDICT_MAXCOE];
}ptasdict_element_t;


typedef struct PTAS_config
{
    unsigned short  user_port;
    unsigned char   max_fan;
    unsigned char   max_tray;
}PTAS_config_t;

typedef struct SLED_statistic
{
    PTAS_bool present;
    unsigned char   temperature;
}SLED_statistic_t;

typedef struct fan_statistic
{
    PTAS_bool present;
    unsigned int    rpm;
}fan_statistic_t;


typedef struct TRAY_statistic
{
    PTAS_bool present;
    unsigned int switch_power;
    SLED_statistic_t sled_statis[SLED_PERTRAY];
}TRAY_statistic_t;

typedef struct PTAS_Statistic
{
	unsigned int sequence_num;
	fan_statistic_t fan_rpm[PTASDICT_MAXFAN];
	TRAY_statistic_t tray_statistic[PTASDICT_MAXTRAY];
	int glb_power_sum;
}PTAS_Statistic_t;

typedef struct PTAS_output
{
    unsigned char rc;
    unsigned int sequence_num;
    double Q;
    float outlet_temp;
}PTAS_Output_t;

typedef struct PTAS_rc
{
    PTASD_RC rc;
}PTAS_rc_t;


/* Message exchange format header between app and ptas-d */
struct ptas_msg_hdr {
	unsigned short type;
	unsigned short datalen;

	unsigned char  data[0];
};

union ptas_msg_union {
	struct ptas_msg_hdr hdr;

	unsigned long buff[PTASMSGSIZ/sizeof(long)];
};


typedef int (*ptas_rsp_callback_fn)(PTAS_Output_t * ot);    //ptas_cb_fn should be reenterable

int PTAS_offer(PTAS_Statistic_t * ptas_st);
int PTAS_harvest(PTAS_Output_t * output);
int PTAS_wait_timeout(PTAS_Statistic_t * ptas_st, PTAS_Output_t * output, unsigned int sc);
int libptas_init(PTAS_config_t * p_config, ptas_rsp_callback_fn cb_fn);


#endif
