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

import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import com.netflix.zuul.ZuulFilter;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Profile;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;

import javax.annotation.Nonnull;
import javax.servlet.http.HttpServletRequest;
import java.util.Enumeration;

import static com.netflix.zuul.context.RequestContext.getCurrentContext;
import static org.springframework.cloud.netflix.zuul.filters.support.FilterConstants.PRE_TYPE;
import static org.springframework.cloud.netflix.zuul.filters.support.FilterConstants.SERVLET_DETECTION_FILTER_ORDER;
import static org.springframework.http.MediaType.APPLICATION_JSON;

@Slf4j(topic = "security")
@Component
@Profile("!no-auth")
public class AccessVerifierFilter extends ZuulFilter {

    private final AccessVerifier accessVerifier;

    private final VerificationResponder responder;

    @Autowired
    public AccessVerifierFilter(AccessVerifier accessVerifier, VerificationResponder responder) {
        this.accessVerifier = accessVerifier;
        this.responder = responder;
    }

    @Override
    public Object run() {
        verifyAccess();
        return null;
    }

    private void verifyAccess() {
        VerifyAccessRequest request = createVerificationRequest(getCurrentContext().getRequest());
        try {
            ResponseEntity responseEntity = accessVerifier.verifyAccess(request);
            HttpStatus statusCode = responseEntity.getStatusCode();
            if (statusCode.isError()) {
                log.warn("Unauthorized access attempt : {}", request);
                responder.respond(statusCode);
            }
        } catch (AccessVerificationException e) {
            responder.respond(e);
        }
    }

    private VerifyAccessRequest createVerificationRequest(HttpServletRequest request) {
        return new VerifyAccessRequest(request.getRequestURI(), HttpMethod.valueOf(request.getMethod()), extractHeaders(request));
    }

    private HttpHeaders extractHeaders(@Nonnull HttpServletRequest httpServletRequest) {
        val httpHeaders = convertToHttpHeaders(httpServletRequest);
        httpHeaders.setContentType(APPLICATION_JSON);
        return httpHeaders;
    }

    private HttpHeaders convertToHttpHeaders(HttpServletRequest httpServletRequest) {
        val httpHeaders = new HttpHeaders();
        Enumeration<String> headerNames = httpServletRequest.getHeaderNames();
        while (headerNames.hasMoreElements()) {
            val headerName = headerNames.nextElement();
            httpHeaders.set(headerName, httpServletRequest.getHeader(headerName));
        }
        return httpHeaders;
    }

    @Override
    public String filterType() {
        return PRE_TYPE;
    }

    @Override
    public int filterOrder() {
        return SERVLET_DETECTION_FILTER_ORDER - 1;
    }

    @Override
    public boolean shouldFilter() {
        return true;
    }
}
