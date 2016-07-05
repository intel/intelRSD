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


#include "cooling_ctrl.h"

struct cooling_ctrl* cooling_allocate()
{
	struct cooling_ctrl *ctrl = (struct cooling_ctrl *)malloc(sizeof(struct cooling_ctrl));

	if(ctrl == NULL) {
		rmm_log(ERROR, "cooling_ctrl malloc failed\n");
		return NULL;
	}
	bzero(ctrl, sizeof(struct cooling_ctrl));
	return ctrl;
}

void cooling_init(struct cooling_ctrl *ctrl)
{
	struct cm_attr * attr;
	char policy[128] = {0};
	libdb_attr_get_string(DB_RMM, MC_NODE_ROOT, COOLING_POLICY, policy, 128, LOCK_ID_NULL);

	if(strnlen_s(policy, sizeof(policy)-1) == 0) {
		strncpy_safe(ctrl->policy, POLICY_AGGREGATED_THERMAL_STR, sizeof(ctrl->policy), sizeof(ctrl->policy) - 1);
	}
	else {
		strncpy_safe(ctrl->policy, policy, sizeof(ctrl->policy), sizeof(ctrl->policy) - 1);
	}
	

	ctrl->cm_create_handler = libdb_subscribe_node_create_by_type(DB_RMM, MC_TYPE_CM, LOCK_ID_NULL);
	ctrl->cm_delete_handler = libdb_subscribe_node_delete_by_type(DB_RMM, MC_TYPE_CM, LOCK_ID_NULL);

	ctrl->tmc_create_handler = libdb_subscribe_node_create_by_type(DB_RMM, MC_TYPE_DRAWER, LOCK_ID_NULL);
	ctrl->tmc_delete_handler = libdb_subscribe_node_delete_by_type(DB_RMM, MC_TYPE_DRAWER, LOCK_ID_NULL);

	INIT_LIST_HEAD(&ctrl->cm_head);
	INIT_LIST_HEAD(&ctrl->tmc_head);
	INIT_LIST_HEAD(&ctrl->attr_head);

	attr = malloc(sizeof(*attr));
	if (attr == NULL) {
		rmm_log(ERROR, "attr out of memory\n");
		return;
	}
	attr->prefix_handler = libdb_subscribe_attr_by_prefix(DB_RMM, MC_NODE_ROOT, COOLING_POLICY, LOCK_ID_NULL);
	attr->node_id = MC_NODE_ROOT;
	list_add_tail(&attr->list, &ctrl->attr_head);
}

static int policy_changed(struct cooling_ctrl *ctrl, struct event_info *evt)
{
	char * policy_name = event_attr_name(evt);

	if (strncmp(policy_name, COOLING_POLICY, strnlen_s(COOLING_POLICY, RSIZE_MAX_STR)) == 0) {
		char *policy = event_attr_data(evt);
		if (strncmp(policy, ctrl->policy, strnlen_s(policy, RSIZE_MAX_STR)) != 0) {
			memset(ctrl->policy,0,sizeof(ctrl->policy));
			strncpy_safe(ctrl->policy, policy, sizeof(ctrl->policy), sizeof(ctrl->policy) - 1);
			return 1;
		}
	}
	return 0;
}

static void subscribe_attr(struct cooling_ctrl * ctrl, memdb_integer node_id)
{
	struct cm_attr  * attr;
	
	if (strncmp(ctrl->policy, POLICY_FIXED_PWM_STR, strnlen_s(POLICY_FIXED_PWM_STR, RSIZE_MAX_STR)) != 0) {
		attr = malloc(sizeof(*attr));
		if (attr == NULL) {
			rmm_log(ERROR, "attr out of memory\n");
			return;
		}
		if (strncmp(ctrl->policy, POLICY_AGGREGATED_THERMAL_STR, strnlen_s(POLICY_AGGREGATED_THERMAL_STR, RSIZE_MAX_STR)) == 0) {
			attr->prefix_handler = libdb_subscribe_attr_by_prefix(DB_RMM, node_id, CHASSIS_TEMP_STR, LOCK_ID_NULL);
		}
		else if (strncmp(ctrl->policy, POLICY_AGGREGATED_PWM_STR, strnlen_s(POLICY_AGGREGATED_PWM_STR, RSIZE_MAX_STR)) == 0) {
			attr->prefix_handler = libdb_subscribe_attr_by_prefix(DB_RMM, node_id, FAN_PWM_STR, LOCK_ID_NULL);
		}

		attr->node_id = node_id;
		list_add_tail(&attr->list, &ctrl->attr_head);
	}
}

static int node_delete_action(struct event_info *evt, struct cooling_ctrl * ctrl)
{
	struct cm_node  * cm;
	struct cm_attr  * attr;
	struct tmc_node * tmc;
	
	if (evt->ntype == MC_TYPE_CM) {
		list_for_each_entry(cm, &ctrl->cm_head, list) {
			if(cm->node_id == evt->nnodeid) {
				list_del(&cm->list);
				free(cm);
				break;
			}
		}
		return 0;
	} 
	else if (evt->ntype == MC_TYPE_DRAWER) {
		list_for_each_entry(tmc, &ctrl->tmc_head, list) {
			if(tmc->node_id == evt->nnodeid) {
				list_del(&tmc->list);
				free(tmc);
				break;
			}
		}
		list_for_each_entry(attr, &ctrl->attr_head, list) {
			if(attr->node_id == evt->nnodeid) {
				libdb_unsubscribe_event(DB_RMM, attr->prefix_handler, LOCK_ID_NULL);
				list_del(&attr->list);
				free(attr);
				break;
			}
		}
		return 0;
	}
	
	return 0;
}

static int node_create_action(struct event_info *evt, struct cooling_ctrl * ctrl)
{
	struct cm_node  * cm;
	struct tmc_node * tmc;
	
	if (evt->ntype == MC_TYPE_CM) {
		list_for_each_entry(cm, &ctrl->cm_head, list) {
			if (cm->node_id == evt->nnodeid) {
				rmm_log(ERROR, "subcribed already\n");
				return -1;
			}
		}

		cm = malloc(sizeof(*cm));
		if (cm == NULL) {
			rmm_log(ERROR, "cm out of memory\n");
			return -1;
		}
		cm->node_id = evt->nnodeid;
		list_add_tail(&cm->list, &ctrl->cm_head);
		
		return 0;
	}
	else if (evt->ntype == MC_TYPE_DRAWER) {
		list_for_each_entry(tmc, &ctrl->tmc_head, list) {
			if (tmc->node_id == evt->nnodeid) {
				rmm_log(ERROR, "added already\n");
				return -1;
			}
		}
		
		tmc = malloc(sizeof(*tmc));
		if (tmc == NULL) {
			rmm_log(ERROR, "tmc out of memory\n");
			return -1;
		}
		tmc->node_id = evt->nnodeid;
		list_add_tail(&tmc->list, &ctrl->tmc_head);

		subscribe_attr(ctrl, evt->nnodeid);
		rmm_log(INFO, "current policy is %s\n", ctrl->policy);

		return 0;
	}
	return 0;
}

static int attr_change_action(struct event_info *evt, struct cooling_ctrl * ctrl)
{
	struct tmc_node  * tmc;
	struct cm_attr  * attr, *attr_policy,*attr_safe;
	int aggregated_thermal, aggregated_pwm;
	
	list_for_each_entry(attr, &ctrl->attr_head, list) {
		if (attr->node_id == MC_NODE_ROOT && policy_changed(ctrl, evt)) {
			rmm_log(INFO, "policy changed, new policy is %s\n", ctrl->policy);
			/* unsubscribe old attribute belong to CM and TMC */
			list_for_each_entry_safe(attr_policy, attr_safe, &ctrl->attr_head, list) {
				if (attr_policy->node_id != MC_NODE_ROOT) {
					libdb_unsubscribe_event(DB_RMM, attr_policy->prefix_handler, LOCK_ID_NULL);
					list_del(&attr_policy->list);
					free(attr_policy);
				}
			}
			/* subscribe attribute for new poicy */
			list_for_each_entry(tmc, &ctrl->tmc_head, list) {
				subscribe_attr(ctrl, tmc->node_id);
			}
			return 0;
		}
		else if (attr->node_id == evt->anodeid) {
			if (strncmp(ctrl->policy, POLICY_AGGREGATED_THERMAL_STR, strnlen_s(POLICY_AGGREGATED_THERMAL_STR, RSIZE_MAX_STR)) == 0) {
				libdb_attr_get_int(DB_RMM, evt->anodeid, CHASSIS_TEMP_STR, &aggregated_thermal, LOCK_ID_NULL);

				if(attr->aggregated_thermal != aggregated_thermal && aggregated_thermal != -1) {
					rmm_log(INFO, "aggregated_thermal changed from %d to %d on node %lu\n", attr->aggregated_thermal, aggregated_thermal, attr->node_id);
					attr->aggregated_thermal = aggregated_thermal;
				}
			}
			else if (strncmp(ctrl->policy, POLICY_AGGREGATED_PWM_STR, strnlen_s(POLICY_AGGREGATED_PWM_STR, RSIZE_MAX_STR)) == 0) {
				libdb_attr_get_int(DB_RMM, evt->anodeid, "aggregated_pwm0", &aggregated_pwm, LOCK_ID_NULL);
				
				if(attr->aggregated_pwm[0] != aggregated_pwm && aggregated_pwm != -1) {
					rmm_log(INFO, "aggregated_pwm changed from %d to %d on node %lu\n", attr->aggregated_pwm, aggregated_pwm, attr->node_id);
					attr->aggregated_pwm[0] = aggregated_pwm;
				}
			}
			return 0;
		}
	}
	return 0;
}

void cooling_event_ops(struct event_info *evt, void *cb_data)
{
	struct cooling_ctrl * ctrl = (struct cooling_ctrl *)cb_data;

	if (evt->event == EVENT_NODE_DELETE) {
		rmm_log(INFO, "Node delete, node_id is %lu\n", evt->nnodeid);
		node_delete_action(evt,ctrl);
	} 
	else if (evt->event == EVENT_NODE_CREATE) {
		rmm_log(INFO, "Node create, node_id is %lu\n", evt->nnodeid);
		node_create_action(evt,ctrl);
	}
	else if(evt->event == EVENT_NODE_ATTR) {
		rmm_log(INFO, "Atrribute changed, event node_id is %lu\n", evt->anodeid);
		attr_change_action(evt, ctrl);
	}
	else {
		rmm_log(ERROR, "Unknown event!\n");
	}
}
