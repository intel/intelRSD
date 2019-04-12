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

package com.intel.rsd.resourcemanager.layers.queryparameter;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.common.QueryParameterType;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_MEMBERS;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_MEMBERS_COUNT;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_ODATA_ID;
import static org.springframework.http.HttpMethod.GET;

@Slf4j
@Component
public class QueryParameter extends Layer {
    private final ResourceCollectionFilter resourceCollectionFilter;

    @Autowired
    public QueryParameter(ResourceCollectionFilter resourceCollectionFilter) {
        this.resourceCollectionFilter = resourceCollectionFilter;
    }

    @Override
    public Set<ServiceId> getServices() {
        return nextLayer.getServices();
    }

    @Override
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<QueryParameterType, String> requestParams) {
        val response = passToNextLayer(serviceId, path, method, headers, body, requestParams);

        if (response.getBody() != null && isCollectionAndParameterRequest(response, requestParams)) {
            val resources = getCollectionResources(response, serviceId);

            return resourceCollectionFilter.filter(response, requestParams, resources);
        }

        return response;
    }

    private Collection<Response> getCollectionResources(Response response, ServiceId serviceId) {
        Set<Response> responses = new HashSet<>();
        for (JsonNode node : response.getBody().get(REDFISH_MEMBERS)) {
            val path = node.get(REDFISH_ODATA_ID).asText();
            responses.add(passToNextLayer(serviceId, path, GET, response.getHttpHeaders(), null, null));
        }

        return responses;
    }

    private boolean isCollectionAndParameterRequest(Response response, Map<QueryParameterType, String> requestParams) {
        val isCollection = response.getBody().get(REDFISH_MEMBERS_COUNT) != null;
        return isCollection && requestParams != null && !requestParams.isEmpty();
    }
}
