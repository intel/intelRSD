/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm.security;

import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.cloud.client.loadbalancer.LoadBalancerClient;
import org.springframework.context.annotation.Profile;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.http.client.ClientHttpResponse;
import org.springframework.http.client.HttpComponentsClientHttpRequestFactory;
import org.springframework.stereotype.Component;
import org.springframework.web.client.DefaultResponseErrorHandler;
import org.springframework.web.client.ResourceAccessException;
import org.springframework.web.client.RestTemplate;

import java.io.IOException;
import java.net.URI;

import static org.springframework.http.HttpMethod.POST;

@Slf4j
@Profile("!no-auth")
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
@Component
public class AccessVerifier {
    private final LoadBalancerClient loadBalancerClient;
    private final String accessVerifierServiceId;
    private final RestTemplate restTemplate;

    @Autowired
    public AccessVerifier(RestTemplate restTemplate, LoadBalancerClient loadBalancerClient,
                          @Value("${access-verification-provider.service-id}") @NonNull String accessVerifierServiceName) {
        this.restTemplate = restTemplate;
        this.restTemplate.setErrorHandler(new NoRethrowErrorHandler());
        this.restTemplate.setRequestFactory(new HttpComponentsClientHttpRequestFactory());
        this.loadBalancerClient = loadBalancerClient;
        this.accessVerifierServiceId = accessVerifierServiceName;
    }

    public ResponseEntity verifyAccess(@NonNull VerifyAccessRequest verifyAccessRequest) throws AccessVerificationException {
        URI accessVerificationUri = createVerifierServiceUri(this.accessVerifierServiceId);
        try {
            return exchange(accessVerificationUri, POST, verifyAccessRequest, new HttpHeaders());
        } catch (ResourceAccessException e) {
            throw new AccessVerificationException("Unable to process authorization request.", e);
        }
    }

    private URI createVerifierServiceUri(String serviceName) {
        val serviceInstance = loadBalancerClient.choose(serviceName);
        if (serviceInstance == null) {
            throw new AccessVerificationException("Unable to process authorization request.");
        }
        URI serviceUri = serviceInstance.getUri();
        String servicePath = serviceUri.getPath() + "/redfish/v1/Actions/Intel.Oem.VerifyAccess";
        return serviceUri.resolve(servicePath);
    }


    private <T> ResponseEntity<T> exchange(URI uri, HttpMethod httpMethod, T body, HttpHeaders headers) {
        HttpEntity<T> requestEntity = new HttpEntity<>(body, headers);
        @SuppressWarnings({"rawtypes", "unchecked"})
        ResponseEntity<T> response = restTemplate.exchange(uri, httpMethod, requestEntity, (Class<T>) Object.class);
        return response;
    }

    class NoRethrowErrorHandler extends DefaultResponseErrorHandler {
        @Override
        public boolean hasError(ClientHttpResponse response) throws IOException {
            return response.getStatusCode().is5xxServerError();
        }
    }
}
