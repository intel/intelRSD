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

import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.podm.AccessVerificationApplication;
import com.intel.rsd.podm.security.jpa.AccountDetailsRepository;
import com.intel.rsd.podm.security.jpa.ManagerAccountEntity;
import com.intel.rsd.podm.services.dto.ManagerAccount;
import com.intel.rsd.podm.services.dto.ManagerAccountUpdateRequest;
import com.intel.rsd.podm.services.dto.Role;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.http.HttpEntity;
import org.springframework.http.ResponseEntity;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.UUID;

import static java.util.Objects.requireNonNull;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.not;
import static org.springframework.http.HttpMethod.DELETE;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.PATCH;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CREATED;
import static org.springframework.http.HttpStatus.METHOD_NOT_ALLOWED;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertTrue;

@ActiveProfiles("aaa-service-test")
@SpringBootTest(classes = {AccessVerificationApplication.class}, webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT,
    properties = {"aaa-config.password-policy.uppercaseCharactersAmount=1",
        "aaa-config.password-policy.digitCharactersAmount=1",
        "aaa-config.password-policy.specialCharactersAmount=1",
        "aaa-config.default-user.password=Admin!2"
    })
public class AccountServiceTest extends AbstractTestNGSpringContextTests {

    @Autowired
    private TestRestTemplate testRestTemplate;

    @Autowired
    private AccountDetailsRepository accountDetailsRepository;

    @Autowired
    private PasswordEncoder passwordEncoder;

    @Test
    public void givenCreateManager_whenDataOk_thenCreated() {
        ManagerAccount user = createUserDto();
        ResponseEntity<ObjectNode> response = doPost("/redfish/v1/AccountService/Accounts", user);
        assertNotNull(response.getBody());
        assertEquals(response.getHeaders().getFirst("Location"), "/redfish/v1/AccountService/Accounts/" + user.getUsername());
        assertEquals(response.getStatusCode(), CREATED);
    }

    @Test
    public void givenCreateManager_whenUsernameAlreadyExists_thenFail() {
        ManagerAccount user = createUserDto();
        doPost("/redfish/v1/AccountService/Accounts", user);
        ResponseEntity<ObjectNode> failingResponse = doPost("/redfish/v1/AccountService/Accounts", user);

        assertNotNull(failingResponse);
        assertEquals(failingResponse.getStatusCode(), BAD_REQUEST);
    }

    @Test
    public void givenCreateManager_whenEmptyUsername_thenFail() {
        ResponseEntity<ObjectNode> response = doPost("/redfish/v1/AccountService/Accounts",
            new ManagerAccount("", "Password", "Administrator"));
        assertEquals(response.getStatusCode(), BAD_REQUEST);
    }

    @Test
    public void givenCreateManager_whenEmptyPassword_thenFail() {
        ResponseEntity<ObjectNode> response = doPost("/redfish/v1/AccountService/Accounts",
            new ManagerAccount("User", "", "Administrator"));
        assertEquals(response.getStatusCode(), BAD_REQUEST);
    }

    @Test
    public void givenUpdateOnManagerPassword_whenPasswordValid_thenStoreNewPassword() {
        ManagerAccount user = createUserDto();
        doPost("/redfish/v1/AccountService/Accounts", user);
        ResponseEntity<ObjectNode> patchResponse = doPatch("/redfish/v1/AccountService/Accounts/" + user.getUsername(),
            createUpdateForPassword("NewPassword!1"));

        ManagerAccountEntity rootUser = accountDetailsRepository.findByUsername(user.getUsername());
        assertTrue(passwordEncoder.matches("NewPassword!1", rootUser.getPassword()));
        assertNotNull(patchResponse.getBody());
    }

    @Test
    public void givenUpdateOnManagerPassword_whenPasswordEmpty_thenFail() {
        ManagerAccount user = createUserDto();
        doPost("/redfish/v1/AccountService/Accounts", user);
        ResponseEntity<ObjectNode> patchResponse = doPatch("/redfish/v1/AccountService/Accounts/" + user.getUsername(), createUpdateForPassword(""));
        assertEquals(patchResponse.getStatusCode(), BAD_REQUEST);
    }

    @Test
    public void givenUpdateOnManagerPassword_whenPasswordBreaksLengthPolicy_thenFail() {
        ManagerAccount user = createUserDto();
        doPost("/redfish/v1/AccountService/Accounts", user);
        ResponseEntity<ObjectNode> patchResponse = doPatch("/redfish/v1/AccountService/Accounts/" + user.getUsername(), createUpdateForPassword("Nd3"));
        assertEquals(patchResponse.getStatusCode(), BAD_REQUEST);
    }

    @Test
    public void givenCreateRole_thenIndicateNotImplemented() {
        ResponseEntity<ObjectNode> response = doPost("/redfish/v1/AccountService/Roles", new Role("Viewer"));
        assertEquals(response.getStatusCode(), METHOD_NOT_ALLOWED);
    }

    @Test
    public void givenUserExists_whenDelete_thenRemove() {
        ManagerAccount user = createUserDto();
        ResponseEntity<ObjectNode> response = doPost("/redfish/v1/AccountService/Accounts", user);
        String userLink = response.getHeaders().getFirst("Location");
        doDelete(userLink);

        ResponseEntity<ObjectNode> users = testRestTemplate.exchange(URI.create("/redfish/v1/AccountService/Accounts"), GET, null, ObjectNode.class);
        requireNonNull(users.getBody());
        assertThat(users.getBody().get("Members"), not(new ContainsLinkTo(userLink)));
    }

    @Test
    public void givenNoUser_whenDelete_thenFail() {
        assertEquals(doDelete("/redfish/v1/AccountService/Accounts/NonExistingUser").getStatusCode(), NOT_FOUND);
    }

    private ManagerAccountUpdateRequest createUpdateForPassword(String newPassword) {
        ManagerAccountUpdateRequest request = new ManagerAccountUpdateRequest();
        request.setPassword(newPassword);
        return request;
    }

    private <T> ResponseEntity<ObjectNode> doDelete(String path) {
        return testRestTemplate.exchange(URI.create(path), DELETE, null, ObjectNode.class);
    }

    private <T> ResponseEntity<ObjectNode> doPost(String path, T requestBody) {
        return testRestTemplate.exchange(URI.create(path), POST, new HttpEntity<>(requestBody), ObjectNode.class);
    }

    private <T> ResponseEntity<ObjectNode> doPatch(String path, T requestBody) {
        return testRestTemplate.exchange(URI.create(path), PATCH, new HttpEntity<>(requestBody), ObjectNode.class);
    }

    private ManagerAccount createUserDto() {
        return createUserDto("Pswd!5");
    }

    private ManagerAccount createUserDto(String password) {
        return new ManagerAccount("Ps!1" + UUID.randomUUID().toString(), password, "Administrator");
    }
}
