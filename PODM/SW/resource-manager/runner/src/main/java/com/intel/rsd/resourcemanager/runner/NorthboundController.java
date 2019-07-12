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

import static com.intel.rsd.resourcemanager.common.HttpServletRequestUtils.getHttpHeaders;
import static com.intel.rsd.resourcemanager.common.HttpServletRequestUtils.readBody;
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

        val httpMethod = resolve(httpServletRequest.getMethod());

        if (httpMethod == null) {
            return status(NOT_IMPLEMENTED).build();
        }

        val headers = getHttpHeaders(httpServletRequest);
        val body = readBody(httpServletRequest);

        val response = downStreamPipeline.invoke(underlyingServiceId, httpServletRequest.getRequestURI(), httpMethod, headers, body, requestParameters);
        return response.toResponseEntity();
    }

    private ServiceId determineUnderlyingServiceId() {
        Set<ServiceId> services = downStreamPipeline.getServices();

        if (services.size() == 1) {
            return services.iterator().next();
        }

        throw new IllegalArgumentException("serviceId cannot be null");
    }
}
