/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.common.logger.Logger;
import org.apache.commons.io.IOUtils;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static javax.ws.rs.HttpMethod.GET;
import static javax.ws.rs.core.Response.Status.Family.SUCCESSFUL;
import static javax.ws.rs.core.Response.Status.Family.familyOf;

public class ServletCommunicationCoordinator {
    private static final Logger LOGGER = getLogger(ServletCommunicationCoordinator.class);

    private final HttpServletRequest httpServletRequest;
    private final HttpServletResponse httpServletResponse;
    private final SensitiveParametersRequestFilter filterForHazardousParameters = new SensitiveParametersRequestFilter();

    private final boolean requestLoggable;

    public ServletCommunicationCoordinator(HttpServletRequest servletRequest, HttpServletResponse servletResponse) {
        requestLoggable = !isGetRequest(servletRequest);

        httpServletRequest = requestLoggable
            ? new ResettableStreamHttpServletRequest(servletRequest)
            : servletRequest;

        httpServletResponse = new ReplicatedStreamHttpServletResponse(servletResponse);
    }

    private boolean isGetRequest(HttpServletRequest servletRequest) {
        return GET.equalsIgnoreCase(servletRequest.getMethod());
    }

    public HttpServletRequest getHttpServletRequest() {
        return httpServletRequest;
    }

    public HttpServletResponse getHttpServletResponse() {
        return httpServletResponse;
    }

    public void logServletRequest() throws IOException {
        if (requestLoggable) {
            String request = IOUtils.toString(httpServletRequest.getReader());
            request = filterForHazardousParameters.filterSecretPropertiesFromRequest(request);

            LOGGER.i("{} request to '{}': '{}'",
                httpServletRequest.getMethod().toUpperCase(),
                httpServletRequest.getRequestURI(),
                request);

            ((ResettableStreamHttpServletRequest) httpServletRequest).resetInputStream();
        }
    }

    public void logServletResponse() throws IOException {
        if (!isSuccessfulGetResponse(httpServletRequest, httpServletResponse)) {
            LOGGER.i("Response for {} request to '{}' (status {}): '{}'",
                httpServletRequest.getMethod().toUpperCase(),
                httpServletRequest.getRequestURI(),
                httpServletResponse.getStatus(),
                httpServletResponse.toString());
        }
    }

    private boolean isSuccessfulGetResponse(HttpServletRequest servletRequest, HttpServletResponse servletResponse) {
        return isGetRequest(servletRequest) && familyOf(servletResponse.getStatus()) == SUCCESSFUL;
    }
}
