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

package com.intel.rsd.resourcemanager.layers.filter;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.AllArgsConstructor;
import lombok.val;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Map;
import java.util.Set;

/**
 * Filter provides functionality that allows to remove specified attributes from responses.
 * In case when some information exposed by Resource Services should not be available from Resource Manager, Filter may help to hide them.
 * All currently filtered out attributes are configured in resource-manager/filter/src/main/resources/filters file.
 * @
 * example: /redfish/v1/#/AccountService means that "AccountService" attribute should be removed from /redfish/v1 resource.
 */
@Component
@AllArgsConstructor
public final class Filter extends Layer {
    private final JsonBodyFilter filter;

    @Override
    public Set<ServiceId> getServices() {
        return nextLayer.getServices();
    }

    @Override
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<String, String> requestParams) {
        Response response = passToNextLayer(serviceId, path, method, headers, body, requestParams);
        return filter(path, response);
    }

    private Response filter(String path, Response response) {
        val filteredBody = filter.filterBody(path, response.getBody());
        return new Response(filteredBody, response.getHttpHeaders(), response.getHttpStatus());
    }
}
