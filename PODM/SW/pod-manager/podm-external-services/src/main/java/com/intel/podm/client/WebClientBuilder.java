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

package com.intel.podm.client;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import com.intel.podm.client.redfish.RedfishClient;
import com.intel.podm.client.redfish.SocketErrorAwareHttpClient;
import com.intel.podm.client.redfish.http.SimpleHttpClient;
import com.intel.podm.common.types.ConnectionParameters;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConnectionConfig;
import com.intel.podm.security.providers.SslContextProvider;
import org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder;
import org.jboss.resteasy.plugins.providers.jackson.ResteasyJackson2Provider;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.ws.rs.client.Client;
import java.net.URI;

import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_NULL_FOR_PRIMITIVES;
import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES;
import static java.util.concurrent.TimeUnit.SECONDS;
import static org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder.HostnameVerificationPolicy.ANY;

@Dependent
public class WebClientBuilder {

    private SslContextProvider sslContextProvider;
    private ConnectionParameters connectionParameters;

    @Inject
    public WebClientBuilder(@Config Holder<ServiceConnectionConfig> configHolder, SslContextProvider sslContextProvider) {
        this.sslContextProvider = sslContextProvider;
        this.connectionParameters = configHolder.get().getConnectionConfiguration().getConnectionParameters();
    }

    public Builder newInstance(URI baseUri) {
        return new Builder(baseUri);
    }

    /**
     * Provides ResteasyClient with configured Jackson
     *
     * @return configured client with jackson provider
     */
    private Client getClientWithJacksonProvider() {
        ResteasyJackson2Provider jackson2Provider = new ResteasyJackson2Provider();
        ObjectMapper mapper = new ObjectMapper()
            .registerModule(new JavaTimeModule())
            .registerModule(new SerializersProvider().getSerializersModule())
            .disable(FAIL_ON_UNKNOWN_PROPERTIES)
            .enable(FAIL_ON_NULL_FOR_PRIMITIVES);
        jackson2Provider.setMapper(mapper);

        ResteasyClientBuilder clientBuilder = ((ResteasyClientBuilder) ResteasyClientBuilder.newBuilder())
            .register(jackson2Provider)
            .sslContext(sslContextProvider.getContext())
            .hostnameVerification(ANY);

        clientBuilder
            .establishConnectionTimeout(connectionParameters.getServiceConnectionTimeout(), SECONDS)
            .socketTimeout(connectionParameters.getServiceSocketTimeout(), SECONDS);

        return clientBuilder.build();
    }

    public class Builder {
        private URI baseUri;
        private boolean retryable;
        private boolean cachable;

        Builder(URI baseUri) {
            this.baseUri = baseUri;
        }

        public Builder retryable() {
            retryable = true;
            return this;
        }

        public Builder cachable() {
            cachable = true;
            return this;
        }

        public WebClient build() {
            Client client = getClientWithJacksonProvider();
            SimpleHttpClient httpClient = new SimpleHttpClient(client);
            SocketErrorAwareHttpClient socketErrorAwareHttpClient = new SocketErrorAwareHttpClient(httpClient);
            RedfishClient redfishClient = new RedfishClient(baseUri, socketErrorAwareHttpClient);
            WebClient webClient = new WebClientBasedOnRedfishClient(redfishClient);

            if (retryable) {
                webClient = new WebClientWithRetrying(webClient);
            }
            if (cachable) {
                webClient = new CachedWebClient(webClient);
            }
            return webClient;
        }
    }
}
