/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish.resources;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.rest.error.NodeComposerWebException;
import com.intel.rsd.nodecomposer.rest.redfish.resources.odataid.ODataIdBuilder;
import com.intel.rsd.nodecomposer.rest.redfish.resources.odataid.ODataIdBuilderException;
import com.intel.rsd.nodecomposer.types.Id;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PatchMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.context.request.RequestContextHolder;
import org.springframework.web.context.request.ServletRequestAttributes;
import org.springframework.web.servlet.HandlerMapping;

import javax.servlet.http.HttpServletRequest;
import java.util.Map;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.PathParamConstants.getPathParameterNames;
import static com.intel.rsd.nodecomposer.rest.error.NodeComposerExceptions.invalidHttpMethod;
import static com.intel.rsd.nodecomposer.rest.error.NodeComposerExceptions.notFound;
import static com.intel.rsd.nodecomposer.types.Id.fromString;
import static java.lang.String.format;
import static org.apache.commons.lang3.StringUtils.removeEnd;
import static org.apache.commons.lang3.StringUtils.removeStart;
import static org.springframework.web.bind.annotation.RequestMethod.OPTIONS;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public abstract class BaseResource {
    /**
     * Gets DTO based on closure implementation for HTTP GET method.
     *
     * @param closure Expression that provides DTO.
     * @return DTO for GET HTTP call.
     * @throws NodeComposerWebException when specified entity was not found
     * or RuntimeException when closure call fails.
     */
    public static <T> T getOrThrow(EntitySupplier<T> closure) {
        try {
            return closure.get();
        } catch (ODataIdResolvingException e) {
            throw notFound();
        }
    }

    @GetMapping
    public abstract Object get();

    @PostMapping
    public Object post() {
        return throwResourceNotFoundOrMethodNotAllowed();
    }

    @PutMapping
    public Object put() {
        return throwResourceNotFoundOrMethodNotAllowed();
    }

    @DeleteMapping
    public Object delete() throws BusinessApiException {
        return throwResourceNotFoundOrMethodNotAllowed();
    }

    @PatchMapping
    public Object patch() {
        return throwResourceNotFoundOrMethodNotAllowed();
    }

    @RequestMapping(method = OPTIONS)
    public ResponseEntity options() {
        if (!exists()) {
            throw notFound();
        }

        return createOptionsResponse();
    }

    protected abstract ResponseEntity createOptionsResponse();

    private boolean exists() {
        try {
            get();
            return true;
        } catch (NodeComposerWebException e) {
            return false;
        }
    }

    private String unwrapParam(String wrappedParamName) {
        String param = removeStart(wrappedParamName, "{");
        return removeEnd(param, "}");
    }

    private String getParam(String paramName) {
        if (!hasParam(paramName)) {
            String msg = format("Path parameter '%s' does not exist", paramName);
            throw new IllegalStateException(msg);
        }

        String paramValue = getPathVariables().get(paramName);
        if (paramValue == null) {
            String msg = format("Parameter '%s' must be single value parameter", paramName);
            throw new IllegalStateException(msg);
        }

        return paramValue;
    }

    private boolean hasParam(String paramName) {
        return getPathVariables().containsKey(paramName);
    }

    @SuppressWarnings({"unchecked"})
    private Map<String, String> getPathVariables() {
        return (Map<String, String>) getCurrentRequest().getAttribute(HandlerMapping.URI_TEMPLATE_VARIABLES_ATTRIBUTE);
    }

    private HttpServletRequest getCurrentRequest() {
        return ((ServletRequestAttributes) RequestContextHolder.currentRequestAttributes()).getRequest();
    }

    /**
     * Gets current context from URI.
     *
     * @return current context from URI
     * @throws NodeComposerWebException if URI is incorrect
     */
    protected ODataId getCurrentODataId() {
        ODataIdBuilder builder = new ODataIdBuilder();

        try {
            for (String paramName : getPathParameterNames()) {
                String param = unwrapParam(paramName);
                if (hasParam(param)) {
                    builder.add(paramName, getId(param));
                }
            }
            return builder.build();
        } catch (NumberFormatException | ODataIdBuilderException e) {
            throw notFound();
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
            throw invalidHttpMethod();
        } else {
            throw notFound();
        }
    }

    public interface EntitySupplier<V> {
        V get() throws ODataIdResolvingException;
    }
}
