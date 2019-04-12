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
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.service.detector.data.ServiceRepository;
import com.intel.rsd.service.detector.endpoint.registration.ServiceRegistrar;
import lombok.NonNull;
import lombok.SneakyThrows;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.ResponseEntity.badRequest;
import static org.springframework.http.ResponseEntity.created;
import static org.springframework.http.ResponseEntity.ok;

@RestController
@RequestMapping("/redfish/v1/Managers")
public class ManagerCollectionController {
    private enum Expand {
        LEVEL_1(".($levels=1)");

        private String value;

        Expand(String value) {
            this.value = value;
        }
    }

    private final ServiceRegistrar serviceRegistrar;
    private final ServiceRepository serviceRepository;

    @Autowired
    public ManagerCollectionController(ServiceRegistrar serviceRegistrar, ServiceRepository serviceRepository) {
        this.serviceRegistrar = serviceRegistrar;
        this.serviceRepository = serviceRepository;
    }

    /**
     * Sample request:
     * <pre>GET http://host/redfish/v1/Managers?$expand=.($levels=1)</pre>
     * <p>
     * Sample output of expanded query:
     * <pre>
     * {
     *   "@odata.context": "/redfish/v1/$metadata#ManagerCollection.ManagerCollection",
     *   "@odata.id": "/redfish/v1/Managers",
     *   "@odata.type": "#ManagerCollection.ManagerCollection",
     *   "Name": "ManagerCollection",
     *   "Members@odata.count": 1,
     *   "Members": [
     *     {
     *       "@odata.id": "/redfish/v1/Managers/0cb6c421-7614-4db1-9456-4d6464bd5baa",
     *       "@odata.type": "#Manager.v1_5_0.Manager",
     *       "Id": "0cb6c421-7614-4db1-9456-4d6464bd5baa",
     *       "ServiceEntryPointUUID": "0cb6c421-7614-4db1-9456-4d6464bd5baa",
     *       "RemoteRedfishServiceUri": "https://ipaddress/redfish/v1",
     *       "Oem":{
     *         "Intel_RackScale": {
     *           "Trusted": true"
     *         }
     *       }
     *     }
     *   ]
     * }
     * </pre>
     */
    @GetMapping
    public ResponseEntity<?> getExpandedManagers(@RequestParam(value = "$expand", required = false) String expand) {
        if (expand == null) {
            return createManagersCollection(
                stream(serviceRepository.findAll().spliterator(), false)
                    .map(ServiceToManagerConverter::toOdataId)
                    .collect(toList())
            );
        }

        if (!Expand.LEVEL_1.value.equals(expand)) {
            return badRequest().build();
        }

        return createManagersCollection(findRegisteredManagers());
    }

    @PostMapping
    public ResponseEntity<?> createManager(@RequestBody ObjectNode requestBody) {
        val serviceUriProperty = requestBody.at("/RemoteRedfishServiceUri");
        if (serviceUriProperty.isMissingNode() || serviceUriProperty.isNull()) {
            val redfishError = createRedfishError(BAD_REQUEST, "Required property is missing: /RemoteRedfishServiceUri");
            return badRequest().body(redfishError);
        }

        val registeredManagerUri = serviceRegistrar.register(serviceUriProperty.asText());
        return created(registeredManagerUri).build();
    }

    private List<JsonNode> findRegisteredManagers() {
        return stream(serviceRepository.findAll().spliterator(), false)
            .map(ServiceToManagerConverter::toManager)
            .collect(toList());
    }

    @SneakyThrows
    private ResponseEntity<JsonNode> createManagersCollection(@NonNull List<JsonNode> managersCollectionMembers) {
        val managerCollection = readJsonFromFile(this, "ManagersCollection.json");
        managerCollection.withArray("Members").addAll(managersCollectionMembers);
        managerCollection.put("Members@odata.count", managersCollectionMembers.size());
        return ok(managerCollection);
    }
}
