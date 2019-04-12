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

package com.intel.rsd.nodecomposer.rest;

import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;
import org.springframework.web.filter.OncePerRequestFilter;
import org.springframework.web.util.ContentCachingRequestWrapper;

import javax.servlet.FilterChain;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

import static org.apache.commons.lang.StringUtils.EMPTY;
import static org.springframework.web.util.WebUtils.getNativeRequest;

@Slf4j
@Component
public class RequestLoggingFilterConfig extends OncePerRequestFilter {
    @Override
    protected void doFilterInternal(HttpServletRequest request, HttpServletResponse response, FilterChain filterChain) throws ServletException, IOException {
        HttpServletRequest wrappedRequest = request;
        if (!isAsyncDispatch(request) && !(request instanceof ContentCachingRequestWrapper)) {
            logRequest(request);
            wrappedRequest = new ContentCachingRequestWrapper(request);
        }
        try {
            filterChain.doFilter(wrappedRequest, response);
        } finally {
            if (!isAsyncStarted(wrappedRequest)) {
                logResponse(wrappedRequest, response);
            }
        }
    }

    private void logRequest(HttpServletRequest request) {
        log.debug("Request: [uri: {}, method: {}]", request.getRequestURI(), request.getMethod());
    }

    private void logResponse(HttpServletRequest request, HttpServletResponse response) throws IOException {
        log.debug("Response: [uri: {}, method: {}, request payload: {}, response: {}]", request.getRequestURI(), request.getMethod(),
            getRequestPayload(request), response.getStatus());
    }

    private String getRequestPayload(HttpServletRequest request) throws IOException {
        ContentCachingRequestWrapper wrapper = getNativeRequest(request, ContentCachingRequestWrapper.class);

        if (wrapper != null) {
            byte[] buf = wrapper.getContentAsByteArray();
            if (buf.length > 0) {
                return new String(buf, 0, buf.length, wrapper.getCharacterEncoding())
                    .replace("\n", "").replace("\t", "");
            }
        }

        return EMPTY;
    }
}
