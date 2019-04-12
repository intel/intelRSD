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

package com.intel.rsd.resourcemanager.runner;

import com.intel.rsd.resourcemanager.common.QueryParameterType;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;
import java.io.IOException;
import java.util.Map;
import java.util.Set;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.intel.rsd.resourcemanager.common.HttpServletRequestUtils.getHttpHeaders;
import static com.intel.rsd.resourcemanager.common.HttpServletRequestUtils.readBody;
import static com.intel.rsd.resourcemanager.common.QueryParameterType.getParameterTypeByString;
import static com.intel.rsd.resourcemanager.common.QueryParameterType.getSupportedQueryParams;
import static java.lang.String.format;
import static java.util.stream.Collectors.joining;
import static java.util.stream.Collectors.toMap;
import static org.springframework.http.HttpMethod.resolve;
import static org.springframework.http.HttpStatus.NOT_IMPLEMENTED;
import static org.springframework.http.ResponseEntity.status;

@RestController("/")
@Slf4j
public class NorthboundController {

    private final Layer downStreamPipeline;
    private final ServiceId underlyingServiceId;

    @Autowired
    public NorthboundController(@Pipeline Layer downStreamPipeline) {
        this.downStreamPipeline = downStreamPipeline;
        this.underlyingServiceId = determineUnderlyingServiceId();
    }

    @RequestMapping("/**")
    public Object invoke(@RequestParam Map<String, String> requestParameters, HttpServletRequest httpServletRequest) throws IOException {
        log.debug("URI: {}", httpServletRequest.getRequestURI());

        val queryParameters = extractSupportedParamsFrom(requestParameters);
        if (isThereAnyUnsupportedParameters(requestParameters, queryParameters)) {
            val redfishError = createRedfishError(NOT_IMPLEMENTED, getUnsupportedParametersMessage(requestParameters));
            return status(redfishError.getHttpStatus()).body(redfishError);
        }

        val httpMethod = resolve(httpServletRequest.getMethod());

        if (httpMethod == null) {
            return status(NOT_IMPLEMENTED).build();
        }

        val headers = getHttpHeaders(httpServletRequest);
        val body = readBody(httpServletRequest);

        val response = downStreamPipeline.invoke(underlyingServiceId, httpServletRequest.getRequestURI(), httpMethod, headers, body, queryParameters);
        return response.toResponseEntity();
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

    private ServiceId determineUnderlyingServiceId() {
        Set<ServiceId> services = downStreamPipeline.getServices();

        if (services.size() == 1) {
            return services.iterator().next();
        }

        throw new IllegalArgumentException("serviceId cannot be null");
    }

    private String getUnsupportedParametersMessage(Map<String, String> requestParameters) {
        String unsupportedParameters = requestParameters.entrySet().stream()
            .filter(entry -> getParameterTypeByString(entry.getKey()) != null)
            .map(Map.Entry::getKey)
            .collect(joining(", "));
        String message = "These parameters are not supported";

        return format("%s: %s", message, unsupportedParameters);
    }
}
