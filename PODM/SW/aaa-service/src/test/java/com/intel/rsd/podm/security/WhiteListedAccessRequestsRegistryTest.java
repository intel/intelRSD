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

import com.intel.rsd.podm.security.whitelist.WhiteListedAccessRequest;
import com.intel.rsd.podm.security.whitelist.WhiteListedAccessRequestsConfiguration;
import com.intel.rsd.podm.security.whitelist.WhiteListedAccessRequestsRegistry;
import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import org.springframework.http.HttpHeaders;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.util.HashSet;
import java.util.List;

import static java.util.Arrays.asList;
import static java.util.Collections.singleton;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.HEAD;
import static org.springframework.http.HttpMethod.OPTIONS;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.HttpMethod.TRACE;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class WhiteListedAccessRequestsRegistryTest {

    private WhiteListedAccessRequestsRegistry sut;

    @Test
    public void givenAccessRequest_whenVerifyAccessNotRequired_thenIndicateWhiteListed() {
        assertTrue(sut.isWhitelisted(get("/redfish/v1")));
        assertTrue(sut.isWhitelisted(post("/redfish/v1")));
        assertTrue(sut.isWhitelisted(head("/redfi")));
        assertTrue(sut.isWhitelisted(head("/redfish/v1/Nodes")));
        assertTrue(sut.isWhitelisted(options("/redfish/v1/Nodes")));
    }


    @Test
    public void givenAccessRequest_whenVerifyAccessRequired_thenIndicateNotWhiteListed() {
        assertFalse(sut.isWhitelisted(get("/redfish/v1/Nodes")));
        assertFalse(sut.isWhitelisted(post("/redfish/v1/Nodes")));
        assertFalse(sut.isWhitelisted(get("/redfish")));
    }

    @BeforeTest
    public void beforeTest() {
        WhiteListedAccessRequestsConfiguration serviceConfig = new WhiteListedAccessRequestsConfiguration();
        List<WhiteListedAccessRequest> whiteListedAccessRequests = asList(
            new WhiteListedAccessRequest("/redfish/v1"),
            new WhiteListedAccessRequest("/actuator/**"),
            new WhiteListedAccessRequest("/**", new HashSet<>(asList(HEAD, TRACE, OPTIONS))),
            new WhiteListedAccessRequest("/redfish/v1/SessionService/Sessions", singleton(POST))
        );
        serviceConfig.setWhiteListedAccessRequests(whiteListedAccessRequests);
        sut = new WhiteListedAccessRequestsRegistry(serviceConfig);
    }

    private VerifyAccessRequest get(String url) {
        return new VerifyAccessRequest(url, GET, new HttpHeaders());
    }

    private VerifyAccessRequest post(String url) {
        return new VerifyAccessRequest(url, POST, new HttpHeaders());
    }

    private VerifyAccessRequest head(String url) {
        return new VerifyAccessRequest(url, HEAD, new HttpHeaders());
    }

    private VerifyAccessRequest options(String url) {
        return new VerifyAccessRequest(url, OPTIONS, new HttpHeaders());
    }
}
