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

import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.val;
import org.testng.annotations.Test;

import static org.assertj.core.api.Assertions.assertThat;

public class ChassisUuidGeneratorTest {
    @Test
    public void whenDrawerChassisDoesNotHaveUuid_thenUuidShouldBeGeneratedUsingChassisTypeAndLocation() throws Exception {
        val uuidGenerator = new NopUuidGenerator();
        val sut = new ChassisUuidGenerator(uuidGenerator);
        val chassisType = "Drawer";
        val locationId = "1";
        val parentLocationId = "1";

        val location = createObjectNode()
                .put("Id", locationId)
                .put("ParentId", parentLocationId);

        val chassis = createObjectNode();
        chassis.put("ChassisType", chassisType);
        chassis.with("Oem").with("Intel_RackScale").set("Location", location);

        val expectedUuid = uuidGenerator.generateUuid(chassisType + locationId + parentLocationId);
        val actualUuid = sut.generateUuid(new ServiceId("1"), chassis);

        assertThat(actualUuid).isEqualTo(expectedUuid);
    }

    private ObjectNode createObjectNode() {
        return JsonNodeFactory.instance.objectNode();
    }

    @Test
    public void whenNonDrawerChassisDoesNotHaveUuid_thenUuidShouldBeGeneratedUsingServiceIdAndId() throws Exception {
        val uuidGenerator = new NopUuidGenerator();
        val sut = new ChassisUuidGenerator(uuidGenerator);
        val chassisType = "Module";

        val chassisId = "1";
        val chassis = createObjectNode()
                .put("Id", chassisId)
                .put("ChassisType", chassisType);

        val serviceId = new ServiceId("1");
        val expectedUuid = uuidGenerator.generateUuid(serviceId + chassisId);
        val actualUuid = sut.generateUuid(serviceId, chassis);

        assertThat(actualUuid).isEqualTo(expectedUuid);
    }

    class NopUuidGenerator implements UuidGenerator {

        @Override
        public String generateUuid(String payload) {
            return payload;
        }
    }

}
