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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import com.hazelcast.core.HazelcastInstance;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Map;

import static com.intel.rsd.resourcemanager.layers.unifier.unification.UnifierMappingsProvider.Mappings.CONSTANTS;
import static com.intel.rsd.resourcemanager.layers.unifier.unification.UnifierMappingsProvider.Mappings.MAPPING;
import static com.intel.rsd.resourcemanager.layers.unifier.unification.UnifierMappingsProvider.Mappings.REVERSE_MAPPING;
import static java.lang.String.format;

@Component
class UnifierMappingsProvider {

    enum Mappings {
        MAPPING,
        REVERSE_MAPPING,
        CONSTANTS
    }

    private final HazelcastInstance hazelcastInstance;

    @Autowired
    UnifierMappingsProvider(HazelcastInstance hazelcastInstance) {
        this.hazelcastInstance = hazelcastInstance;
    }

    public Map<String, String> getMappingMap(String mapId, String serviceId) {
        return hazelcastInstance.getMap(createComplexMapId(mapId, serviceId, MAPPING));
    }

    public Map<String, String> getReverseMappingMap(String mapId, String serviceId) {
        return hazelcastInstance.getMap(createComplexMapId(mapId, serviceId, REVERSE_MAPPING));
    }

    public Map<String, String> getConstantsMappingMap(String mapId, String serviceId) {
        return hazelcastInstance.getMap(createComplexMapId(mapId, serviceId, CONSTANTS));
    }

    private String createComplexMapId(Object prefix, Object id, Object suffix) {
        return format("%s.%s.%s", prefix, id, suffix);
    }
}
