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


#ifndef __ASSETD_ATTRIBUTE_H__
#define __ASSETD_ATTRIBUTE_H__

#include <stdio.h>
typedef void (*reset_zone_fn)(const memdb_integer *node_id, int cm_lid, int zone_lid);
typedef void (*reset_item_fn)(const memdb_integer *node_id, int cm_lid, int item_lid, int zone_lid);


/**
 * @brief initalize rack attribute.
 *
 */
extern void init_rack_attr(void);

/**
 * @brief initalize rack attribute.
 *
 */
extern void init_root_service_attr(const memdb_integer *node_id, int32 type);

/**
 * @brief initalize rack attribute.
 *
 */
extern void init_rmc_attr(const memdb_integer *node_id, int32 type);

/**
 * @brief initalize rack attribute.
 *
 */
extern void init_mbpc_attr(const memdb_integer *node_id, int cm_lid, int32 type);

/**
 * @brief initalize drawer zone attribute when drawer zone added.
 *
 * @param  node_id		node_id of the drawer zone.
 * @param  dzone_idx	drawer zone index.
 * @param  snap			snapshot or not.
 *
 */
extern void init_dzone_attr(const memdb_integer *node_id, int dzone_idx, int cm_idx, int snap);

/**
 * @brief initalize drawer attribute when drawer added.
 *
 * @param  node_id		node_id of the drawer.
 * @param  drawer_idx	drawer index.
 * @param  dzone_idx	dower zone index of this drawer.
 * @param  snap			snapshot or not.
 *
 */
extern void init_drawer_attr(const memdb_integer *node_id, int drawer_idx, int dzone_idx, int cm_idx, int snap);

/**
 * @brief initalize sled attribute when sled added.
 *
 * @param  node_id		node_id of the sled.
  * @param  sled_idx		sled index.
 * @param  drawer_idx	drawer index.
 * @param  dzone_idx	dower zone index of this drawer.
 * @param  snap			snapshot or not.
 *
 */
extern void init_sled_attr(const memdb_integer *node_id, int sled_idx, int drawer_idx, int dzone_idx, int cm_idx, int snap);


/**
 * @brief initalize mbp attribute when mbp added.
 *
 * @param  node_id		node_id of the mbp.
 * @param  mbp_idx		mbp index.
 * @param  cm_idx		cm index of this mbp.
 * @param  snap			snapshot or not.
 *
 */
extern void init_mbp_attr(const memdb_integer *node_id, int cm_idx, int snap);

/**
 * @brief initalize power zone attribute when power zone added.
 *
 * @param  node_id		node_id of the power zone.
 * @param  pzone_idx	power zone index.
 * @param  snap			snapshot or not.
 *
 */
extern void init_pzone_attr(const memdb_integer *node_id, int pzone_idx, int cm_idx, int snap);

/**
 * @brief initalize psu attribute when psu added.
 *
 * @param  node_id		node_id of the psu.
 * @param  psu_idx		psu index.
 * @param  pzone_idx	power zone index of this psu.
 * @param  snap			snapshot or not.
 *
 */
extern void init_psu_attr(const memdb_integer *node_id, int psu_idx, int pzone_idx, int cm_idx, int snap);

/**
 * @brief initalize thermal zone attribute when thermal zone added.
 *
 * @param  node_id		node_id of the thermal zone.
 * @param  tzone_idx	thermal zone index.
 * @param  snap			snapshot or not.
 *
 */
extern void init_tzone_attr(const memdb_integer *node_id, int tzone_idx, int cm_idx, int snap);

/**
 * @brief initalize fan attribute when fan added.
 *
 * @param  node_id		node_id of the fan.
 * @param  fan_idx		fan index.
 * @param  tzone_idx	thermal zone index of this fan.
 * @param  snap			snapshot or not.
 *
 */
extern void init_fan_attr(const memdb_integer *node_id, int fan_idx, int tzone_idx, int cm_idx, int snap);



/**
 * @brief reset rack attribute.
 *
 */
extern void reset_rack_attr(void);

/**
 * @brief reset drawer zone attribute.
 *
 * @param  node_id		node_id of the drawer zone.
 * @param  dzone_idx	drawer zone index.
 * @param  snap			snapshot or not.
 *
 */
extern void reset_dzone_attr(const memdb_integer *node_id, int cm_lid, int dzone_lid);

/**
 * @brief reset drawer attribute.
 *
 * @param  node_id		node_id of the drawer.
 * @param  drawer_idx	drawer index.
 * @param  dzone_idx	dower zone index of this drawer.
 * @param  snap			snapshot or not.
 *
 */
extern void reset_drawer_attr(const memdb_integer *node_id, int cm_lid, int drawer_lid, int dzone_lid);

/**
 * @brief reset mbp attribute.
 *
 * @param  node_id		node_id of the mbp.
 * @param  mbp_idx		mbp index.
 * @param  cm_idx		cm index of this mbp.
 * @param  snap			snapshot or not.
 *
 */
extern void reset_mbp_attr(const memdb_integer *node_id, int cm_lid);

/**
 * @brief reset power zone attribute.
 *
 * @param  node_id		node_id of the power zone.
 * @param  pzone_idx	power zone index.
 * @param  snap			snapshot or not.
 *
 */
extern void reset_pzone_attr(const memdb_integer *node_id, int cm_lid, int pzone_lid);

/**
 * @brief reset psu attribute.
 *
 * @param  node_id		node_id of the psu.
 * @param  psu_idx		psu index.
 * @param  pzone_idx	power zone index of this psu.
 * @param  snap			snapshot or not.
 *
 */
extern void reset_psu_attr(const memdb_integer *node_id, int cm_lid, int psu_lid, int pzone_lid);

/**
 * @brief reset thermal zone attribute.
 *
 * @param  node_id		node_id of the thermal zone.
 * @param  tzone_idx	thermal zone index.
 * @param  snap			snapshot or not.
 *
 */
extern void reset_tzone_attr(const memdb_integer *node_id, int cm_lid, int tzone_lid);

/**
 * @brief reset fan attribute.
 *
 * @param  node_id		node_id of the fan.
 * @param  fan_idx		fan index.
 * @param  tzone_idx	thermal zone index of this fan.
 * @param  snap			snapshot or not.
 *
 */
extern void reset_fan_attr(const memdb_integer *node_id, int cm_lid, int fan_lid, int tzone_lid);

#define PERSISTENT_N       (1<<0)
#define PERSISTENT_Y       (1<<1)
#define PERSISTENT_ALL     (PERSISTENT_N | PERSISTENT_Y)
#define RESET_N            (0)
#define RESET_Y            (1)

#define DEFAULT_FAN_PWM    40

#endif
