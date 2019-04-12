/*
 * Copyright (c) 2018 Intel Corporation
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

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.http.RsdClient;
import com.intel.rsd.service.detector.data.Service;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.RequestEntity;
import org.springframework.web.client.ResourceAccessException;
import org.springframework.web.client.RestTemplate;

import javax.net.ssl.SSLException;

@Slf4j
class TrustStoreBasedVerifier implements ServiceVerifier {
    private final RestTemplate restTemplate;

    TrustStoreBasedVerifier(@RsdClient RestTemplate restTemplate) {
        this.restTemplate = restTemplate;
    }

    @Override
    @SuppressWarnings("checkstyle:IllegalCatch")
    public boolean verify(Service service) {
        try {
            restTemplate.exchange(RequestEntity.get(service.getServiceUri()).build(), JsonNode.class);
            return true;
        } catch (ResourceAccessException e) {
            return !e.contains(SSLException.class) && service.isTrusted();
        } catch (Exception e) {
            log.trace("Exception during service ({}) verification: {}", service, e.getMessage());
            return service.isTrusted();
        }
    }
}
