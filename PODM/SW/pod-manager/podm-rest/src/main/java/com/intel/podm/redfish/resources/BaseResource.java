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

package com.intel.podm.redfish.resources;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.common.types.Id;
import com.intel.podm.redfish.resources.context.ContextBuilder;
import com.intel.podm.redfish.resources.context.ContextBuilderException;

import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.NotAllowedException;
import javax.ws.rs.NotFoundException;
import javax.ws.rs.OPTIONS;
import javax.ws.rs.PATCH;
import javax.ws.rs.POST;
import javax.ws.rs.PUT;
import javax.ws.rs.container.ResourceContext;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;
import java.util.List;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.PathParamConstants.getPathParameterNames;
import static com.intel.podm.common.types.Id.fromString;
import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static java.lang.String.format;
import static org.apache.commons.lang3.StringUtils.removeEnd;
import static org.apache.commons.lang3.StringUtils.removeStart;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public abstract class BaseResource {
    @Context
    private ResourceContext rc;

    @Context
    private UriInfo uriInfo;

    @GET
    public abstract Object get();

    @POST
    public Object post() {
        return throwResourceNotFoundOrMethodNotAllowed();
    }

    @PUT
    public Object put() {
        return throwResourceNotFoundOrMethodNotAllowed();
    }

    @DELETE
    public Object delete() throws TimeoutException, BusinessApiException {
        return throwResourceNotFoundOrMethodNotAllowed();
    }

    @PATCH
    public Object patch() {
        return throwResourceNotFoundOrMethodNotAllowed();
    }

    @OPTIONS
    public Response options() {
        if (!exists()) {
            throw notFound();
        }

        return createOptionsResponse();
    }

    /**
     * Gets DTO based on closure implementation for HTTP GET method.
     *
     * @param closure Expression that provides DTO.
     * @return DTO for GET HTTP call.
     * @throws javax.ws.rs.WebApplicationException when specified entity was not found
     *         or RuntimeException when closure call fails.
     */
    public static <T> T getOrThrow(EntitySupplier<T> closure) {
        try {
            return closure.get();
        } catch (ContextResolvingException e) {
            throw notFound();
        }
    }

    protected boolean exists() {
        try {
            get();
            return true;
        } catch (NotFoundException e) {
            return false;
        }
    }

    protected <T extends BaseResource> T getResource(Class<T> resourceClass) {
        return rc.getResource(resourceClass);
    }

    protected abstract Response createOptionsResponse();

    private String unwrapParam(String wrappedParamName) {
        String param = removeStart(wrappedParamName, "{");
        return removeEnd(param, "}");
    }

    private String getParam(String paramName) {
        if (!hasParam(paramName)) {
            String msg = format("Path parameter '%s' does not exist", paramName);
            throw new IllegalStateException(msg);
        }

        List<String> paramValues = uriInfo.getPathParameters().get(paramName);

        if (paramValues.size() != 1) {
            String msg = format("Path parameter '%s' must be single value parameter", paramName);
            throw new IllegalStateException(msg);
        }

        return paramValues.get(0);
    }

    private boolean hasParam(String paramName) {
        return uriInfo.getPathParameters().containsKey(paramName);
    }

    /**
     * Gets current context from URI.
     *
     * @return current context from URI
     * @throws NotFoundException if URI is incorrect
     */
    protected com.intel.podm.business.services.context.Context getCurrentContext() {
        ContextBuilder builder = new ContextBuilder();

        try {
            for (String paramName : getPathParameterNames()) {
                String param = unwrapParam(paramName);
                if (hasParam(param)) {
                    builder.add(paramName, getId(param));
                }
            }
            return builder.build();
        } catch (NumberFormatException | ContextBuilderException e) {
            throw new NotFoundException(e);
        }
    }

    private Id getId(String paramName) {
        String param = getParam(paramName);
        return fromString(param);
    }

    /**
     * Provides default implementation for all HTTP methods (except GET)
     * which assumes that method is not allowed if resource exists.
     */
    private Object throwResourceNotFoundOrMethodNotAllowed() {
        if (exists()) {
            String dummyString = "";
            throw new NotAllowedException(dummyString);
        } else {
            throw notFound();
        }
    }

    interface EntitySupplier<V> {
        V get() throws ContextResolvingException;
    }
}
