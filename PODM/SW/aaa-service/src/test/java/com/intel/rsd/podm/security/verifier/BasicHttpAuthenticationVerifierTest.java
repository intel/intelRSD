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

package com.intel.rsd.podm.security.verifier;

import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.util.Base64;

import static java.nio.charset.StandardCharsets.UTF_8;
import static org.mockito.Mockito.mock;
import static org.testng.Assert.assertNotNull;

public class BasicHttpAuthenticationVerifierTest {

    private BasicHttpAuthenticationVerifier basicHttpVerifier;

    @BeforeMethod
    public void beforeMethod() {
        basicHttpVerifier = new BasicHttpAuthenticationVerifier(mock(AuthenticationManager.class));
    }

    @Test(expectedExceptions = BadCredentialsException.class)
    public void givenBasicAuthToken_whenInvalidPrefix_thenIndicateFailure() {
        basicHttpVerifier.getSpringSecurityToken("cisaB " + base64encode("admin:admin"));
    }

    @Test
    public void givenBasicAuthToken_whenExcessiveSpacesWithinToken_thenDealWithThem() {
        basicHttpVerifier.getSpringSecurityToken("Basic   " + base64encode("admin:admin"));
    }

    @Test(expectedExceptions = BadCredentialsException.class)
    public void givenBasicAuthToken_whenNoSeparationInCredentials_thenIndicateFailure() {
        basicHttpVerifier.getSpringSecurityToken("Basic " + base64encode("adminadmin"));
    }

    @Test(expectedExceptions = BadCredentialsException.class)
    public void givenBasicAuthToken_whenInvalidFormatCredentials_thenIndicateFailure() {
        basicHttpVerifier.getSpringSecurityToken("Basic " + base64encode("admin:ad:min"));
    }

    @Test
    public void givenBasicAuthToken_whenValid_thenIndicateSuccess() {
        UsernamePasswordAuthenticationToken token = basicHttpVerifier.getSpringSecurityToken("Basic " + base64encode("admin:admin"));
        assertNotNull(token.getCredentials());
    }

    private String base64encode(String text) {
        return Base64.getEncoder().encodeToString(text.getBytes(UTF_8));
    }
}
