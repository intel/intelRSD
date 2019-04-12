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
import com.intel.rsd.podm.sessions.RedfishSession;
import com.intel.rsd.podm.sessions.RedfishSessionsRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import java.util.Objects;

import static com.intel.rsd.podm.sessions.RedfishSession.X_AUTH_TOKEN;
import static org.springframework.http.HttpHeaders.LOCATION;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNotEquals;
import static org.testng.Assert.assertTrue;

@ActiveProfiles({"aaa-service-test"})
@SpringBootTest(classes = AccessVerificationApplication.class,
    webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT,
    properties = {
        "aaa-config.session-timeout=1"
    })
@EnableDiscoveryClient(autoRegister = false)
public class SessionLifecycleTest extends AbstractTestNGSpringContextTests {

    @Autowired
    private RedfishSessionsRepository sessionsRepository;

    @Autowired
    private TestRestTemplate restTemplate;

    @Autowired
    private VerifyAccessRequestFactory requestFactory;

    @Test
    public void givenCredentials_whenValid_thenSessionIsCreated() {
        HttpHeaders headers = getSessionToken();
        String location = headers.getFirst(LOCATION);
        Objects.requireNonNull(location);
        assertTrue(sessionsRepository.getSessionIds().stream().anyMatch(location::contains));
    }

    @Test
    public void givenSession_whenUserActivity_thenSessionProlonged() {
        final HttpHeaders headers = getSessionToken();
        final String xAuthToken = headers.getFirst(X_AUTH_TOKEN);
        String sessionId = extractIdFromLocationHeader(headers);
        RedfishSession sessionBeforeUpdate = sessionsRepository.findById(sessionId);

        VerifyAccessRequest accessPayload = requestFactory.create("/redfish/v1/Nodes", HttpMethod.GET, xAuthToken);
        restTemplate.postForEntity("/redfish/v1/Actions/Intel.Oem.VerifyAccess", new HttpEntity<>(accessPayload), ObjectNode.class);

        RedfishSession sessionAfterUpdate = sessionsRepository.findById(sessionId);

        assertNotEquals(sessionBeforeUpdate.getLastAccessedTime(), sessionAfterUpdate.getLastAccessedTime());
    }

    private String extractIdFromLocationHeader(HttpHeaders headers) {
        String locationHeader = headers.getFirst(LOCATION);
        assert locationHeader != null;
        return locationHeader.substring(locationHeader.lastIndexOf('/') + 1);
    }

    @Test
    public void givenSessionExists_whenSearchedByIndexOrTokenValue_thenRepositoryReturnsSession() {
        HttpHeaders headers = getSessionToken();
        String xAuthToken = headers.getFirst(X_AUTH_TOKEN);
        RedfishSession session = sessionsRepository.findByAuthToken(xAuthToken);
        RedfishSession sessionById = sessionsRepository.findById(session.getId());
        assertEquals(session.getId(), sessionById.getId());
    }

    @Test
    public void givenInvalidSessionId_whenProlonging_thenExitGracefully() {
        sessionsRepository.prolongSession("invalid");
    }

    @Test
    public void givenInvalidSessionId_whenRemoving_thenNothingRemoved() {
        assertFalse(sessionsRepository.removeSession("invalid"));
    }

    private HttpHeaders getSessionToken() {
        ResponseEntity<ObjectNode> tokenAcquisitionResponse = restTemplate.postForEntity("/redfish/v1/SessionService/Sessions",
            new HttpEntity<>(new ObjectMapper().createObjectNode().put("UserName", "admin").put("Password", "admin")), ObjectNode.class);

        return tokenAcquisitionResponse.getHeaders();
    }
}
