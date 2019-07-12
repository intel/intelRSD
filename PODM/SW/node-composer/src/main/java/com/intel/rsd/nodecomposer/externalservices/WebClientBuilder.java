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

package com.intel.rsd.nodecomposer.externalservices;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import com.intel.rsd.http.RsdClient;
import com.intel.rsd.nodecomposer.externalservices.redfish.RedfishClient;
import com.intel.rsd.nodecomposer.externalservices.redfish.SocketErrorAwareHttpClient;
import com.intel.rsd.nodecomposer.externalservices.redfish.http.SimpleHttpClient;
import lombok.SneakyThrows;
import lombok.val;
import org.apache.http.ssl.SSLContextBuilder;
import org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder;
import org.jboss.resteasy.plugins.providers.jackson.ResteasyJackson2Provider;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.cloud.client.loadbalancer.LoadBalancerClient;
import org.springframework.stereotype.Component;

import javax.ws.rs.client.Client;
import javax.ws.rs.core.UriBuilder;
import java.net.URI;

import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_NULL_FOR_PRIMITIVES;
import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES;
import static com.fasterxml.jackson.databind.PropertyNamingStrategy.UPPER_CAMEL_CASE;
import static java.util.concurrent.TimeUnit.SECONDS;
import static org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder.HostnameVerificationPolicy.ANY;

@Component
public class WebClientBuilder {
    private final long serviceConnectionTimeoutInSeconds;
    private final long serviceSocketTimeoutInSeconds;
    private final SSLContextBuilder sslContextBuilder;
    private LoadBalancerClient loadBalancerClient;
    private final ResteasyJackson2Provider jackson2Provider = initializeProvider();

    @Autowired
    public WebClientBuilder(@Value("${connection-configuration.service-connection-timeout-in-seconds:2}") long serviceConnectionTimeoutInSeconds,
                            @Value("${connection-configuration.service-socket-timeout-in-seconds:2}") long serviceSocketTimeoutInSeconds,
                            @RsdClient SSLContextBuilder sslContextBuilder,
                            LoadBalancerClient loadBalancerClient) {
        this.serviceConnectionTimeoutInSeconds = serviceConnectionTimeoutInSeconds;
        this.serviceSocketTimeoutInSeconds = serviceSocketTimeoutInSeconds;
        this.sslContextBuilder = sslContextBuilder;
        this.loadBalancerClient = loadBalancerClient;
    }

    public Builder createResourceManagerInstance() {
        return new Builder(URI.create("http://resource-manager:psme/redfish/v1"), true);
    }

    public Builder newSimpleInstance(URI baseUri) {
        return new Builder(baseUri, false);
    }

    /**
     * Provides ResteasyClient with configured Jackson
     *
     * @return configured client with jackson provider
     */
    @SneakyThrows
    private Client getClientWithJacksonProvider() {
        ResteasyClientBuilder clientBuilder = ((ResteasyClientBuilder) ResteasyClientBuilder.newBuilder())
            .register(jackson2Provider)
            .sslContext(sslContextBuilder.build())
            .hostnameVerification(ANY);

        clientBuilder
            .establishConnectionTimeout(serviceConnectionTimeoutInSeconds, SECONDS)
            .socketTimeout(serviceSocketTimeoutInSeconds, SECONDS);

        return clientBuilder.build();
    }

    private ResteasyJackson2Provider initializeProvider() {
        ResteasyJackson2Provider jackson2Provider = new CustomResteasyJackson2Provider();
        ObjectMapper mapper = new ObjectMapper()
            .setPropertyNamingStrategy(UPPER_CAMEL_CASE)
            .registerModule(new JavaTimeModule())
            .registerModule(new SerializersProvider().getSerializersModule())
            .disable(FAIL_ON_UNKNOWN_PROPERTIES)
            .enable(FAIL_ON_NULL_FOR_PRIMITIVES);
        jackson2Provider.setMapper(mapper);

        return jackson2Provider;
    }

    public class Builder {
        private URI baseUri;
        private boolean retryable;
        private boolean cachable;
        private String basicAuthToken;

        Builder(URI baseUri, boolean loadBalanced) {
            if (loadBalanced) {
                this.baseUri = getLoadBalancedUri(baseUri);
            } else {
                this.baseUri = baseUri;
            }
        }

        public Builder withBasicAuthToken(String basicAuthToken) {
            if (basicAuthToken != null && !basicAuthToken.isEmpty()) {
                this.basicAuthToken = basicAuthToken;
            }
            return this;
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
            supportBasicAuthToken(client);
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

        private void supportBasicAuthToken(Client client) {
            if (basicAuthToken != null) {
                client.register(new BasicAuthenticationToken(basicAuthToken));
            }
        }

        private URI getLoadBalancedUri(URI baseUri) {
            val serviceInstance = loadBalancerClient.choose(baseUri.getRawAuthority());
            if (serviceInstance == null) {
                return null;
            }

            return UriBuilder.fromUri(serviceInstance.getUri()).path(baseUri.getPath()).build();
        }
    }

    private class CustomResteasyJackson2Provider extends ResteasyJackson2Provider {
        /*
         * Workaround for:
         *
         * RESTEASY002155: Provider class org.jboss.resteasy.plugins.providers.jackson.ResteasyJackson2Provider is already registered.
         *     2nd registration is being ignored.
         */
    }
}
