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

package com.intel.rsd.podm.security;

import lombok.SneakyThrows;
import lombok.val;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Component;

import static com.intel.rsd.json.JsonUtils.createStringWith;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.netflix.zuul.context.RequestContext.getCurrentContext;
import static org.springframework.http.HttpHeaders.CONTENT_TYPE;
import static org.springframework.http.HttpHeaders.WWW_AUTHENTICATE;
import static org.springframework.http.HttpStatus.SERVICE_UNAVAILABLE;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;

@Component
public class VerificationResponder {

    public void respond(HttpStatus statusCode) {
        addAuthenticationMethodHeader();
        zuulResponse(statusCode);
    }

    public void respond(AccessVerificationException e) {
        zuulResponse(SERVICE_UNAVAILABLE, e.getMessage());
    }

    private void addAuthenticationMethodHeader() {
        getCurrentContext().addZuulResponseHeader(WWW_AUTHENTICATE, "Basic realm=\"Pod Manager\", Redfish Sessions realm=\"Pod Manager\"");
    }

    @SneakyThrows
    private void zuulResponse(HttpStatus accessVerifierResponseStatus, String... extendedInfo) {
        val ctx = getCurrentContext();
        ctx.setResponseStatusCode(accessVerifierResponseStatus.value());
        ctx.setResponseBody(createStringWith(createRedfishError(accessVerifierResponseStatus, extendedInfo)));
        ctx.setSendZuulResponse(false);
        ctx.addZuulResponseHeader(CONTENT_TYPE, APPLICATION_JSON_VALUE);
    }
}
