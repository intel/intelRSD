/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.hazelcast;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.services.common.Shared;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Map;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class HazelcastHelper {
    private final Map<ODataId, List<NodeTask>> assemblyTaskMap;

    @Autowired
    @SuppressWarnings("unchecked")
    public HazelcastHelper(@Shared("HazelcastHelper.assemblyTaskMap") Map<?, ?> assemblyTaskMap) {
        this.assemblyTaskMap = (Map<ODataId, List<NodeTask>>) assemblyTaskMap;
    }

    public Map<ODataId, List<NodeTask>> assemblyTaskMap() {
        return assemblyTaskMap;
    }
}
