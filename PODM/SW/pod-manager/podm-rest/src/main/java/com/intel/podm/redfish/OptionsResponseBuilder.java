/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.redfish;

import com.google.common.base.Joiner;

import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.HEAD;
import javax.ws.rs.HttpMethod;
import javax.ws.rs.OPTIONS;
import javax.ws.rs.PATCH;
import javax.ws.rs.POST;
import javax.ws.rs.PUT;
import javax.ws.rs.core.Response;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static java.util.Collections.sort;
import static javax.ws.rs.core.HttpHeaders.ALLOW;

public final class OptionsResponseBuilder {

    private Set<Class> supportedHttpMethods = new HashSet<Class>() {
        {
            add(OPTIONS.class);
        }
    };

    private OptionsResponseBuilder() {
    }

    /**
     * Builder for creating default OPTIONS response with default OPTIONS method allowed in ALLOW header.
     * <p>
     * Response for OPTIONS shall be 200 OK (according to RFC2616 document)
     *
     * @see <a href="https://www.ietf.org/rfc/rfc2616.txt">RFC2616 Documentation</a>
     */
    public static OptionsResponseBuilder newOptionsResponseBuilder() {
        return new OptionsResponseBuilder();
    }

    /**
     * Builder for creating default OPTIONS response with default OPTIONS, GET, HEAD methods allowed in ALLOW header.
     * <p>
     * Response for OPTIONS shall be 200 OK (according to RFC2616 document)
     *
     * @see <a href="https://www.ietf.org/rfc/rfc2616.txt">RFC2616 Documentation</a>
     */
    public static OptionsResponseBuilder newOptionsForResourceBuilder() {
        return new OptionsResponseBuilder().addGetMethod().addHeadMethod();
    }

    /**
     * Builder for creating default OPTIONS response with default OPTIONS, HEAD methods allowed in ALLOW header.
     * <p>
     * Response for OPTIONS shall be 200 OK (according to RFC2616 document)
     *
     * @see <a href="https://www.ietf.org/rfc/rfc2616.txt">RFC2616 Documentation</a>
     */
    public static OptionsResponseBuilder newOptionsForResourceActionBuilder() {
        return new OptionsResponseBuilder().addHeadMethod();
    }

    /**
     * Adds DELETE method to ALLOW header
     */
    public OptionsResponseBuilder addDeleteMethod() {
        this.supportedHttpMethods.add(DELETE.class);
        return this;
    }

    /**
     * Adds GET method to ALLOW header
     */
    public OptionsResponseBuilder addGetMethod() {
        this.supportedHttpMethods.add(GET.class);
        return this;
    }

    /**
     * Adds HEAD method to ALLOW header
     */
    public OptionsResponseBuilder addHeadMethod() {
        this.supportedHttpMethods.add(HEAD.class);
        return this;
    }

    /**
     * Adds PATCH method to ALLOW header
     */
    public OptionsResponseBuilder addPatchMethod() {
        this.supportedHttpMethods.add(PATCH.class);
        return this;
    }

    /**
     * Adds POST method to ALLOW header
     */
    public OptionsResponseBuilder addPostMethod() {
        this.supportedHttpMethods.add(POST.class);
        return this;
    }

    /**
     * Adds PUT method to ALLOW header
     */
    public OptionsResponseBuilder addPutMethod() {
        this.supportedHttpMethods.add(PUT.class);
        return this;
    }

    /**
     * Create proper OPTIONS response
     *
     * @return response with fulfilled Allow header
     */
    public Response build() {
        return Response
            .ok()
            .header(ALLOW, stringifyAllowedMethods())
            .build();
    }

    private String stringifyAllowedMethods() {
        List<String> allowedMethods = new ArrayList<>();

        for (Class httpMethod : supportedHttpMethods) {
            HttpMethod httpMethodAnnotation = (HttpMethod) httpMethod.getAnnotation(HttpMethod.class);
            allowedMethods.add(httpMethodAnnotation.value());
        }

        sort(allowedMethods);

        return Joiner.on(", ").join(allowedMethods);
    }
}
