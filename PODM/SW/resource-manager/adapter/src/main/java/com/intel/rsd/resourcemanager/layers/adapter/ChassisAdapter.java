/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.adapter;

import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.AllArgsConstructor;
import org.springframework.stereotype.Component;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@Component
@AllArgsConstructor
public class ChassisAdapter implements ResourceAdapter {
    private final ChassisUuidGenerator chassisUuidGenerator;

    @Override
    public boolean shouldAdaptResource(ObjectNode resource) {
        return isChassis(resource)
            && resource.at("/UUID").isMissingNode();
    }

    @Override
    public void adapt(ServiceId serviceId, ObjectNode resource) {
        resource.put("UUID", chassisUuidGenerator.generateUuid(serviceId, resource));
    }

    private boolean isChassis(ObjectNode resource) {
        return resource.at("/@odata.type").asText().matches("#Chassis" + VERSION_PATTERN + "Chassis");
    }
}
