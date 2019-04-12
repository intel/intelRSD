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

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.http.HttpEntity;
import org.springframework.http.ResponseEntity;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import static com.intel.rsd.podm.sessions.RedfishSession.X_AUTH_TOKEN;
import static java.util.Objects.requireNonNull;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CREATED;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.HttpStatus.UNAUTHORIZED;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNull;

@ActiveProfiles({"aaa-service-test"})
@SpringBootTest(classes = {AccessVerificationApplication.class}, webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
public class AccessWithRedfishSessionTokenTest extends AbstractTestNGSpringContextTests {

    @Autowired
    private TestRestTemplate restTemplate;

    @Autowired
    private VerifyAccessRequestFactory requestFactory;

    @Test
    public void givenCredentials_whenValid_thenReturnXAuthToken() {
        ResponseEntity<ObjectNode> response = restTemplate.postForEntity("/redfish/v1/SessionService/Sessions",
            new HttpEntity<>(createLoginPayloadBody("admin", "admin")), ObjectNode.class);
        assertEquals(CREATED, response.getStatusCode());
        assertFalse(requireNonNull(response.getHeaders().get(X_AUTH_TOKEN)).isEmpty());
    }

    @Test
    public void givenMalformedPayload_thenFail() {
        ResponseEntity<ObjectNode> response = restTemplate.postForEntity("/redfish/v1/SessionService/Sessions",
            new HttpEntity<>(new ObjectMapper().createObjectNode().put("Uname", "admin").put("Password", "admin")), ObjectNode.class);
        assertEquals(BAD_REQUEST, response.getStatusCode());
        assertNull(response.getHeaders().get(X_AUTH_TOKEN));
    }

    @Test
    public void givenCredentials_whenInvalid_thenStatusUnauthorized() {
        ResponseEntity<ObjectNode> response = restTemplate.postForEntity("/redfish/v1/SessionService/Sessions",
            new HttpEntity<>(createLoginPayloadBody("invalid", "credentials")), ObjectNode.class);
        assertEquals(UNAUTHORIZED, response.getStatusCode());
        assertNull(response.getHeaders().get(X_AUTH_TOKEN));
    }

    @Test
    public void givenSessionsToken_whenValid_thenGrantAccess() {
        ResponseEntity<ObjectNode> tokenAcquisitionResponse = restTemplate.postForEntity("/redfish/v1/SessionService/Sessions",
            new HttpEntity<>(createLoginPayloadBody("admin", "admin")), ObjectNode.class);

        VerifyAccessRequest accessPayload = requestFactory.create("/redfish/v1/Nodes",
            GET, tokenAcquisitionResponse.getHeaders().getFirst(X_AUTH_TOKEN));
        ResponseEntity<ObjectNode> accessResponse = restTemplate.postForEntity("/redfish/v1/Actions/Intel.Oem.VerifyAccess",
            new HttpEntity<>(accessPayload), ObjectNode.class);
        assertEquals(OK, accessResponse.getStatusCode());
    }

    @Test
    public void givenSessionsToken_whenInvalid_thenDenyAccess() {
        VerifyAccessRequest accessPayload = requestFactory.create("/redfish/v1/Nodes",
            GET, "FabricatedToken");
        ResponseEntity<ObjectNode> response = restTemplate.postForEntity("/redfish/v1/Actions/Intel.Oem.VerifyAccess",
            new HttpEntity<>(accessPayload), ObjectNode.class);
        assertEquals(UNAUTHORIZED, response.getStatusCode());
        assertNull(response.getHeaders().get(X_AUTH_TOKEN));
    }

    private ObjectNode createLoginPayloadBody(String login, String password) {
        return new ObjectMapper().createObjectNode().put("UserName", login).put("Password", password);
    }
}
