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

package com.intel.rsd.resourcemanager.layers.adapter;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Objects;

@Component
public class ChassisUuidGenerator {

    private final UuidGenerator uuidGenerator;

    @Autowired
    public ChassisUuidGenerator(UuidGenerator uuidGenerator) {
        this.uuidGenerator = uuidGenerator;
    }

    public String generateUuid(ServiceId serviceId, JsonNode body) {
        //Chassis with ChassisType Drawer may exist on different RSD services
        // in case of other Chassis types (e.g Module), different chassis could have the same Location Id and ParentId
        if (rackScaleOemLocationExists(body) && chassisTypeEqualsDrawer(body)) {
            return generateFromRackScaleOemLocationAndChassisType(body);
        }

        val resourceId = body.at("/Id").asText();
        String payload = String.format("%s%s", serviceId, resourceId);
        return uuidGenerator.generateUuid(payload);
    }

    private boolean chassisTypeEqualsDrawer(JsonNode body) {
        return Objects.equals(body.at("/ChassisType").asText(""), "Drawer");
    }

    private String generateFromRackScaleOemLocationAndChassisType(JsonNode body) {
        val chassisType = body.at("/ChassisType").asText();
        val locationId = body.at("/Oem/Intel_RackScale/Location/Id").asText();
        val parentLocationId = body.at("/Oem/Intel_RackScale/Location/ParentId").asText();

        return uuidGenerator.generateUuid(String.format("%s%s%s", chassisType, locationId, parentLocationId));
    }

    private boolean rackScaleOemLocationExists(JsonNode body) {
        return !body.at("/Oem/Intel_RackScale/Location/Id").asText("").isEmpty()
               && !body.at("/Oem/Intel_RackScale/Location/ParentId").asText("").isEmpty();
    }

}
