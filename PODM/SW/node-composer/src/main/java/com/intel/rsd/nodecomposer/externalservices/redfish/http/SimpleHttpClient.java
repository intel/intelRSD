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

package com.intel.rsd.nodecomposer.externalservices.redfish.http;

import com.intel.rsd.nodecomposer.types.net.HttpMethod;

import javax.ws.rs.client.Client;
import javax.ws.rs.client.Invocation;
import javax.ws.rs.core.Response;
import java.net.URI;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.ws.rs.client.Entity.entity;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON_TYPE;

public class SimpleHttpClient implements AutoCloseable {
    private final Client client;

    public SimpleHttpClient(Client client) {
        this.client = requiresNonNull(client, "com/intel/rsd/nodecomposer/externalservices");
    }

    /**
     * @throws javax.ws.rs.ProcessingException on javax.ws.rs.client.Invocation.invoke()
     */
    public HttpResponse call(HttpMethod method, URI uri, Object requestEntity, Class responseEntityClass) {
        requiresNonNull(method, "method");
        requiresNonNull(uri, "uri");
        requiresNonNull(responseEntityClass, "responseEntityClass");

        Response response = buildInvocation(uri, method, requestEntity).invoke();

        try {
            return toHttpResponse(responseEntityClass, response);
        } finally {
            response.close();
        }
    }

    @Override
    public void close() {
        client.close();
    }

    @SuppressWarnings({"unchecked"})
    private HttpResponse toHttpResponse(Class responseEntityClass, Response response) {
        Object responseEntity = response.hasEntity()
            ? response.readEntity(responseEntityClass)
            : null;

        return new HttpResponse(response.getStatus(), responseEntity, response.getLocation());
    }

    private Invocation buildInvocation(URI uri, HttpMethod method, Object requestEntity) {
        Invocation.Builder request = client.target(uri).request(APPLICATION_JSON_TYPE);

        return requestEntity != null
            ? request.build(method.name(), entity(requestEntity, APPLICATION_JSON_TYPE))
            : request.build(method.name());
    }
}
