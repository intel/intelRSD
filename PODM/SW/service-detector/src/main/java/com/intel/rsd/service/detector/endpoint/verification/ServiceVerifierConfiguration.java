/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.service.detector.endpoint.verification;

import com.intel.rsd.http.RsdClient;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;
import org.springframework.http.client.ClientHttpRequestFactory;
import org.springframework.web.client.RestTemplate;

@Configuration
public class ServiceVerifierConfiguration {

    /**
     * Verification of registered services will be performed against provided trust store.
     * Particular service will be marked as trusted when it presents itself with certificate contained in provided trust store
     * or its certificate is signed by known CA.
     */
    @Profile({"default", "truststore-based-verification"})
    @Bean
    public ServiceVerifier verifyUsingTrustStore(@RsdClient RestTemplate restTemplate) {
        return new TrustStoreBasedVerifier(restTemplate);
    }

    /**
     * Any registered service wil be marked as trusted without verification.
     * This profile is intended to be non production one.
     */
    @Profile("no-verification")
    @Bean
    public ServiceVerifier trustAllServicesVerifier() {
        return new TrustAllServicesVerifier();
    }

    @RsdClient
    @Bean
    public RestTemplate restTemplate(@RsdClient ClientHttpRequestFactory httpComponentsClientHttpRequestFactory) {
        return new RestTemplate(httpComponentsClientHttpRequestFactory);
    }
}
