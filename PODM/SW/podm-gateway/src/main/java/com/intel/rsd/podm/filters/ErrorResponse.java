/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm.filters;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.netflix.zuul.context.RequestContext;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.HttpStatus;

import static com.intel.rsd.json.JsonUtils.createStringWith;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;
import static org.springframework.http.MediaType.TEXT_PLAIN_VALUE;

@Slf4j
public class ErrorResponse {
    public static void createResponseWithStatus(RequestContext context, HttpStatus httpStatus) {
        context.setSendZuulResponse(false);
        context.getResponse().setContentType(APPLICATION_JSON_VALUE);
        context.setResponseStatusCode(httpStatus.value());
        context.setResponseBody(getBodyInJsonFormat(context, httpStatus));
    }

    private static String getBodyInJsonFormat(RequestContext context, HttpStatus httpStatus) {
        try {
            return createStringWith(createRedfishError(httpStatus));
        } catch (JsonProcessingException e) {
            log.error("Not able to create error message due to error {}", e.getMessage());
            context.getResponse().setContentType(TEXT_PLAIN_VALUE);
            context.setResponseStatusCode(INTERNAL_SERVER_ERROR.value());
            return e.toString();
        }
    }
}
