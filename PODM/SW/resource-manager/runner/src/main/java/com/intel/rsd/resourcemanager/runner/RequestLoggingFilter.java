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

package com.intel.rsd.resourcemanager.runner;

import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.cloud.netflix.ribbon.support.ResettableServletInputStreamWrapper;
import org.springframework.stereotype.Component;
import org.springframework.web.filter.OncePerRequestFilter;
import org.springframework.web.util.ContentCachingResponseWrapper;

import javax.servlet.FilterChain;
import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletRequestWrapper;
import javax.servlet.http.HttpServletResponse;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import static com.fasterxml.jackson.databind.SerializationFeature.INDENT_OUTPUT;
import static com.google.common.io.ByteStreams.toByteArray;
import static java.lang.String.format;

@Slf4j
@Component
public class RequestLoggingFilter extends OncePerRequestFilter {

    private static final ObjectMapper OBJECT_MAPPER = new ObjectMapper().enable(INDENT_OUTPUT);

    @Override
    protected void doFilterInternal(HttpServletRequest request, HttpServletResponse response, FilterChain filterChain) throws ServletException, IOException {

        if (shouldLog(request)) {
            processWithLogging(request, response, filterChain);
        } else {
            process(request, response, filterChain);
        }
    }

    private void process(HttpServletRequest request, HttpServletResponse response, FilterChain filterChain) throws IOException, ServletException {
        filterChain.doFilter(request, response);
    }

    private boolean shouldLog(HttpServletRequest request) {
        return !"GET".equals(request.getMethod());
    }

    private void processWithLogging(HttpServletRequest request, HttpServletResponse response, FilterChain filterChain) throws IOException, ServletException {
        val wrappedRequest = new ContentCachingRequestWrapper(request);
        val wrappedResponse = new ContentCachingResponseWrapper(response);

        try {
            logServletRequest(wrappedRequest);
            process(wrappedRequest, wrappedResponse, filterChain);
        } finally {
            logServletResponse(wrappedRequest, wrappedResponse);
        }
    }

    private void logServletRequest(HttpServletRequest wrappedRequest) {
        try {
            log.info(
                "{} request to '{}': '{}'",
                wrappedRequest.getMethod().toUpperCase(),
                wrappedRequest.getRequestURI(),
                OBJECT_MAPPER.readTree(wrappedRequest.getInputStream())
            );
        } catch (IOException e) {
            log.info(
                "{} request to '{}': '{}'",
                wrappedRequest.getMethod().toUpperCase(),
                wrappedRequest.getRequestURI(),
                "Request is not in valid json format"
            );
        }
    }

    private void logServletResponse(HttpServletRequest httpServletRequest, ContentCachingResponseWrapper responseWrapper) throws IOException {
        try {
            val location = responseWrapper.getHeader("Location");
            log.info(prepareResponse(location, httpServletRequest, responseWrapper));
        } finally {
            responseWrapper.copyBodyToResponse();
        }
    }

    private String prepareResponse(String location, HttpServletRequest httpServletRequest, ContentCachingResponseWrapper responseWrapper) {
        String response = format("Response for %s request to '%s' (status %s): '%s'",
            httpServletRequest.getMethod().toUpperCase(),
            httpServletRequest.getRequestURI(),
            responseWrapper.getStatus(),
            new String(responseWrapper.getContentAsByteArray()));

        if (location != null) {
            return format("%s, location: '%s'", response, location);
        }

        return response;
    }

    static class ContentCachingRequestWrapper extends HttpServletRequestWrapper {

        private byte[] rawData;

        ContentCachingRequestWrapper(HttpServletRequest request) throws IOException {
            super(request);
            this.rawData = toByteArray(getRequest().getInputStream());
        }

        @Override
        public ServletInputStream getInputStream() {
            return new ResettableServletInputStreamWrapper(rawData);
        }

        @Override
        public BufferedReader getReader() {
            return new BufferedReader(new InputStreamReader(new ResettableServletInputStreamWrapper(rawData)));
        }
    }

}
