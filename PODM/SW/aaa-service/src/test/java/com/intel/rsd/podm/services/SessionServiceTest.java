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

package com.intel.rsd.podm.services;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.podm.AccessVerificationApplication;
import com.intel.rsd.podm.sessions.RedfishSessionsRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.ResponseEntity;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import java.net.URI;

import static java.util.Objects.requireNonNull;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.springframework.http.HttpHeaders.LOCATION;
import static org.springframework.http.HttpMethod.GET;
import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertNull;
import static org.testng.Assert.assertTrue;

@ActiveProfiles({"aaa-service-test"})
@SpringBootTest(classes = {AccessVerificationApplication.class}, webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
public class SessionServiceTest extends AbstractTestNGSpringContextTests {

    @Autowired
    private RedfishSessionsRepository redfishSessionsRepository;

    @Autowired
    private TestRestTemplate restTemplate;

    @Test
    public void sessionListShouldContainSessions() {
        ResponseEntity<ObjectNode> loginResponse = restTemplate.postForEntity("/redfish/v1/SessionService/Sessions",
            new HttpEntity<>(new ObjectMapper().createObjectNode().put("UserName", "admin").put("Password", "admin")), ObjectNode.class);
        ResponseEntity<ObjectNode> sessions = restTemplate.exchange(URI.create("/redfish/v1/SessionService/Sessions"),
            GET, new HttpEntity<>(new HttpHeaders()), ObjectNode.class);

        String sessionId = extractIdFromLocationHeader(loginResponse);
        requireNonNull(sessions.getBody());
        assertThat(sessions.getBody().get("Members"), new ContainsLinkTo("/redfish/v1/SessionService/Sessions/" + sessionId));
    }

    private String extractIdFromLocationHeader(ResponseEntity<ObjectNode> response) {
        String locationHeader = response.getHeaders().getFirst(LOCATION);
        assert locationHeader != null;
        return locationHeader.substring(locationHeader.lastIndexOf('/') + 1);
    }

    @Test
    public void sessionDeletionRemovesSession() {
        ResponseEntity<ObjectNode> loginResponse = restTemplate.postForEntity("/redfish/v1/SessionService/Sessions",
            new HttpEntity<>(new ObjectMapper().createObjectNode().put("UserName", "admin").put("Password", "admin")), ObjectNode.class);
        String sessionId = extractIdFromLocationHeader(loginResponse);
        assertNotNull(redfishSessionsRepository.findById(sessionId));
        restTemplate.delete(URI.create("/redfish/v1/SessionService/Sessions/" + sessionId));
        assertNull(redfishSessionsRepository.findById(sessionId));
    }

    @Test
    public void whenAskedForInvalidSessionReturnSpecificRedfishError() {
        ResponseEntity<ObjectNode> sessions = restTemplate.exchange(URI.create("/redfish/v1/SessionService/Sessions/invalid"),
            GET, new HttpEntity<>(new HttpHeaders()), ObjectNode.class);

        requireNonNull(sessions.getBody());
        assertTrue(sessions.getBody().toString().contains("Invalid endpoint"));
    }

}
