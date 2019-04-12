/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish;

import com.google.common.base.Joiner;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.RequestMethod;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static java.util.Collections.sort;
import static org.springframework.http.HttpHeaders.ALLOW;
import static org.springframework.web.bind.annotation.RequestMethod.DELETE;
import static org.springframework.web.bind.annotation.RequestMethod.GET;
import static org.springframework.web.bind.annotation.RequestMethod.HEAD;
import static org.springframework.web.bind.annotation.RequestMethod.OPTIONS;
import static org.springframework.web.bind.annotation.RequestMethod.PATCH;
import static org.springframework.web.bind.annotation.RequestMethod.POST;
import static org.springframework.web.bind.annotation.RequestMethod.PUT;

public final class OptionsResponseBuilder {
    private final Set<RequestMethod> supportedHttpMethods = new HashSet<RequestMethod>() {
        private static final long serialVersionUID = -2465944028633933148L;

        {
            add(OPTIONS);
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
        this.supportedHttpMethods.add(DELETE);
        return this;
    }

    /**
     * Adds GET method to ALLOW header
     */
    public OptionsResponseBuilder addGetMethod() {
        this.supportedHttpMethods.add(GET);
        return this;
    }

    /**
     * Adds HEAD method to ALLOW header
     */
    public OptionsResponseBuilder addHeadMethod() {
        this.supportedHttpMethods.add(HEAD);
        return this;
    }

    /**
     * Adds PATCH method to ALLOW header
     */
    public OptionsResponseBuilder addPatchMethod() {
        this.supportedHttpMethods.add(PATCH);
        return this;
    }

    /**
     * Adds POST method to ALLOW header
     */
    public OptionsResponseBuilder addPostMethod() {
        this.supportedHttpMethods.add(POST);
        return this;
    }

    /**
     * Adds PUT method to ALLOW header
     */
    public OptionsResponseBuilder addPutMethod() {
        this.supportedHttpMethods.add(PUT);
        return this;
    }

    /**
     * Create proper OPTIONS response
     *
     * @return response with fulfilled Allow header
     */
    public ResponseEntity build() {
        return ResponseEntity
            .ok()
            .header(ALLOW, stringifyAllowedMethods())
            .build();
    }

    private String stringifyAllowedMethods() {
        List<String> allowedMethods = new ArrayList<>();

        for (RequestMethod httpMethod : supportedHttpMethods) {
            allowedMethods.add(httpMethod.name());
        }

        sort(allowedMethods);

        return Joiner.on(", ").join(allowedMethods);
    }
}
