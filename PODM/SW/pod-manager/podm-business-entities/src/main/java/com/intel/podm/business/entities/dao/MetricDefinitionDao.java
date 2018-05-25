/*
 * Copyright (c) 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.business.entities.dao;

import com.intel.podm.business.entities.redfish.MetricDefinition;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.ApplicationScoped;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.entities.redfish.MetricDefinition.GET_ALL_METRIC_DEFINITIONS_IDS;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class MetricDefinitionDao extends Dao<MetricDefinition> {
    @Transactional(MANDATORY)
    public List<Id> getAllMetricDefinitionsIds() {
        return entityManager.createNamedQuery(GET_ALL_METRIC_DEFINITIONS_IDS, Id.class).getResultList();
    }
}

