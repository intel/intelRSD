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

import static com.intel.rsd.json.JsonUtils.objectToJsonNode;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterType.getParameterTypeByString;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterType.getSupportedQueryParams;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_MEMBERS;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_MEMBERS_COUNT;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_ODATA_ID;
import static java.lang.String.format;
import static java.util.stream.Collectors.joining;
import static java.util.stream.Collectors.toMap;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpStatus.NOT_IMPLEMENTED;
import static org.springframework.http.ResponseEntity.status;

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
                                  Map<String, String> requestParams) {


        val queryParameters = extractSupportedParamsFrom(requestParams);
        if (isThereAnyUnsupportedParameters(requestParams, queryParameters)) {
            val redfishError = createRedfishError(NOT_IMPLEMENTED, getUnsupportedParametersMessage(requestParams));
            return new Response(status(redfishError.getHttpStatus()).body(objectToJsonNode(redfishError)));
        }

        val response = passToNextLayer(serviceId, path, method, headers, body, requestParams);

        if (response.getBody() != null && isCollectionAndParameterRequest(response, queryParameters)) {
            val resources = getCollectionResources(response, serviceId);

            return resourceCollectionFilter.filter(response, queryParameters, resources);
        }

        return response;
    }


    private Map<QueryParameterType, String> extractSupportedParamsFrom(Map<String, String> requestParameters) {
        return requestParameters.entrySet().stream()
            .filter(entry -> getSupportedQueryParams().contains(getParameterTypeByString(entry.getKey())))
            .collect(toMap(entry -> getParameterTypeByString(entry.getKey()), Map.Entry::getValue));
    }

    private boolean isThereAnyUnsupportedParameters(Map<String, String> requestParameters, Map<QueryParameterType, String> queryParams) {
        queryParams.forEach((key, value) -> requestParameters.remove(key.getValue()));

        return requestParameters.entrySet().stream()
            .anyMatch(entry -> getParameterTypeByString(entry.getKey()) != null);
    }

    private String getUnsupportedParametersMessage(Map<String, String> requestParameters) {
        String unsupportedParameters = requestParameters.entrySet().stream()
            .filter(entry -> getParameterTypeByString(entry.getKey()) != null)
            .map(Map.Entry::getKey)
            .collect(joining(", "));
        String message = "These parameters are not supported";

        return format("%s: %s", message, unsupportedParameters);
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
