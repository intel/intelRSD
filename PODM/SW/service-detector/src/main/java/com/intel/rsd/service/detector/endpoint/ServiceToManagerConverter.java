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

package com.intel.rsd.service.detector.endpoint;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.intel.rsd.service.detector.data.Service;
import lombok.NonNull;
import lombok.SneakyThrows;
import lombok.experimental.UtilityClass;
import lombok.val;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;

@UtilityClass
class ServiceToManagerConverter {

    @SneakyThrows
    JsonNode toManager(@NonNull Service service) {
        val managerBody = readJsonFromFile(ServiceToManagerConverter.class, "Manager.json");
        val endpointUuid = service.getId();

        managerBody.put("@odata.id", "/redfish/v1/Managers/".concat(endpointUuid));
        managerBody.put("Id", endpointUuid);
        managerBody.put("ServiceEntryPointUUID", endpointUuid);
        managerBody.put("RemoteRedfishServiceUri", service.getServiceUri().toString());
        managerBody.put("RemoteRedfishServiceType", service.getRemoteServiceType().name());
        managerBody.with("Status")
            .put("State", service.getStatus().toString());
        managerBody.with("Oem").with("Intel_RackScale")
            .put("Trusted", service.isTrusted());

        return managerBody;
    }

    JsonNode toOdataId(@NonNull Service service) {
        return JsonNodeFactory.instance.objectNode().put("@odata.id", "/redfish/v1/Managers/".concat(service.getId()));
    }
}
