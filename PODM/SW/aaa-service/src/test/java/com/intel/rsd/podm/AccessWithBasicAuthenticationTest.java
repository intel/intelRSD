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

package com.intel.rsd.podm;

import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.http.HttpEntity;
import org.springframework.http.ResponseEntity;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.HttpStatus.UNAUTHORIZED;
import static org.testng.Assert.assertEquals;

@ActiveProfiles({"aaa-service-test"})
@SpringBootTest(classes = AccessVerificationApplication.class, webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
public class AccessWithBasicAuthenticationTest extends AbstractTestNGSpringContextTests {

    @Autowired
    private TestRestTemplate restTemplate;

    @Autowired
    private VerifyAccessRequestFactory requestFactory;

    @Test
    public void given_unrestrictedResource_when_noCredentials_then_grantAccess() {
        ResponseEntity authorizationResponse = verifyAnonymousAccessTo("/redfish/v1");
        assertEquals(OK, authorizationResponse.getStatusCode());
    }

    @Test
    public void given_restrictedResource_when_noCredentials_then_denyAccess() {
        ResponseEntity authorizationResponse = verifyAnonymousAccessTo("/redfish/v1/Nodes");
        assertEquals(UNAUTHORIZED, authorizationResponse.getStatusCode());
    }

    @Test
    public void given_request_when_pathNotNormalized_then_handleProperly() {
        ResponseEntity authorizationResponse = verifyAnonymousAccessTo(
            "/redfish/v1/EthernetSwitches/\\?\\?\\?AA\\?\\?\\?AA\\?\\?\\?AA\\?\\?\\?AA\\?\\?\\?AA\\?\\?\\?AA\\?\\?\\?AA\\?;");
        assertEquals(UNAUTHORIZED, authorizationResponse.getStatusCode());
    }

    @Test
    public void given_restrictedResource_when_validBasicAuthCredentials_then_grantAccess() {
        ResponseEntity authorizationResponseForOperator = verifyAuthorizedAccess();
        assertEquals(OK, authorizationResponseForOperator.getStatusCode());
    }

    @Test
    public void given_restrictedResource_when_invalidBasicAuthCredentials_then_denyAccess() {
        ResponseEntity authorizationResponseForRoot = verifyAccessSpecifyingInvalidCredentials();
        assertEquals(UNAUTHORIZED, authorizationResponseForRoot.getStatusCode());
    }

    private ResponseEntity verifyAnonymousAccessTo(String uri) {
        VerifyAccessRequest accessPayload = requestFactory.create(uri, GET);
        return callVerifyAccess(accessPayload);
    }

    private ResponseEntity verifyAccessSpecifyingInvalidCredentials() {
        VerifyAccessRequest accessPayload = requestFactory.create("/redfish/v1/Nodes", GET, "invalid", "creds");
        return callVerifyAccess(accessPayload);
    }

    private ResponseEntity verifyAuthorizedAccess() {
        VerifyAccessRequest accessPayload = requestFactory.create("/redfish/v1/Nodes", GET, "admin", "admin");
        return callVerifyAccess(accessPayload);
    }

    private ResponseEntity callVerifyAccess(VerifyAccessRequest accessPayload) {
        return restTemplate.postForEntity("/redfish/v1/Actions/Intel.Oem.VerifyAccess",
            new HttpEntity<>(accessPayload), VerifyAccessRequest.class);
    }
}
