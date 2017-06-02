/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.rest;


import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.rest.HttpServletRequestValidator.HttpServletRequestValidationException;
import com.intel.podm.rest.representation.json.errors.ErrorType;
import org.jboss.resteasy.plugins.server.servlet.FilterDispatcher;

import javax.servlet.FilterChain;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.core.Response;
import java.io.IOException;
import java.io.PrintWriter;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static com.intel.podm.redfish.RedfishResponseHeadersProvider.getRedfishResponseHeaders;
import static com.intel.podm.rest.error.ErrorResponseCreator.from;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

/**
 * Request URL Validator Filter
 * Extends RESTEasy FilterDispatcher and checks if provided URL request met basic requirements and is properly built.
 *
 * @see org.jboss.resteasy.plugins.server.servlet.FilterDispatcher
 */
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class RequestValidationFilter extends FilterDispatcher {

    private static final Logger LOGGER = getLogger(RequestValidationFilter.class);
    private static final String DEFAULT_ENCODING_HTTP = "utf8";

    @Override
    public void doFilter(ServletRequest servletRequest, ServletResponse servletResponse, FilterChain filterChain) throws IOException, ServletException {
        ServletCommunicationCoordinator servletCoordinator =
                new ServletCommunicationCoordinator((HttpServletRequest) servletRequest, (HttpServletResponse) servletResponse);

        HttpServletRequestValidator requestValidator = new HttpServletRequestValidator();

        servletCoordinator.logServletRequest();
        try {
            requestValidator.validateServletRequest(servletCoordinator.getHttpServletRequest());
            super.doFilter(servletCoordinator.getHttpServletRequest(), servletCoordinator.getHttpServletResponse(), filterChain);
        } catch (HttpServletRequestValidationException e) {
            setErrorResponse(servletCoordinator.getHttpServletResponse(), e.getErrorType());
        }
        servletCoordinator.logServletResponse();
    }

    public void setErrorResponse(HttpServletResponse httpServletResponse, ErrorType error) {
        httpServletResponse.reset();
        httpServletResponse.resetBuffer();

        /**
         * Since we are outside our application we need to provide Redfish headers additionally
         */
        getRedfishResponseHeaders().forEach(httpServletResponse::addHeader);

        Response response = from(error).create();
        httpServletResponse.setCharacterEncoding(DEFAULT_ENCODING_HTTP);
        httpServletResponse.setContentType(APPLICATION_JSON);
        httpServletResponse.setStatus(response.getStatus());

        try (PrintWriter responseWriter = httpServletResponse.getWriter()) {
            responseWriter.print(serializeToJsonString(response.getEntity()));
            responseWriter.flush();
        } catch (IOException e) {
            LOGGER.e("Unable to write response. Sending empty body response.", e);
        }
    }

    private String serializeToJsonString(Object entity) throws JsonProcessingException {
        return new ObjectMapper().writeValueAsString(entity);
    }
}
