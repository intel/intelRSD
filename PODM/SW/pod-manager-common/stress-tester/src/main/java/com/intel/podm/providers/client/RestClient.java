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
package com.intel.podm.providers.client;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.intel.podm.providers.ssl.SslContextProvider;
import org.jboss.resteasy.client.jaxrs.ResteasyClient;
import org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder;
import org.jboss.resteasy.plugins.providers.jackson.ResteasyJackson2Provider;

import javax.net.ssl.SSLContext;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.Invocation;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.Response;
import java.net.URI;

import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_NULL_FOR_PRIMITIVES;
import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES;
import static org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder.HostnameVerificationPolicy.ANY;

public class RestClient {
    private final ResteasyClient client;
    private String encodedPodmUserPassword;

    public RestClient(String encodedPodmUserPassword) {
        client = getResteasyClient();
        this.encodedPodmUserPassword = encodedPodmUserPassword;
    }

    public Response get(URI url) {
        Invocation.Builder invocation = getInvocation(url);
        return invocation.get();
    }

    public Response post(URI url, Object content) {
        Invocation.Builder invocation = getInvocation(url);
        return invocation.post(Entity.json(content));
    }

    public Response delete(URI url) {
        Invocation.Builder invocation = getInvocation(url);
        return invocation.delete();
    }

    private Invocation.Builder getInvocation(URI url) {
        WebTarget target = client.target(url);
        return target.request().header("Authorization", "Basic "
                + encodedPodmUserPassword);
    }

    private ResteasyClient getResteasyClient() {
        SSLContext sslContext = new SslContextProvider().getContext();
        ResteasyJackson2Provider jackson2Provider = new ResteasyJackson2Provider();

        ObjectMapper mapper = new ObjectMapper()
                .disable(FAIL_ON_UNKNOWN_PROPERTIES)
                .enable(FAIL_ON_NULL_FOR_PRIMITIVES);
        jackson2Provider.setMapper(mapper);

        return ((ResteasyClientBuilder) ResteasyClientBuilder.newBuilder())
                .register(jackson2Provider)
                .sslContext(sslContext)
                .hostnameVerification(ANY)
                .build();
    }

    public void closeClient() {
        client.close();
    }
}
