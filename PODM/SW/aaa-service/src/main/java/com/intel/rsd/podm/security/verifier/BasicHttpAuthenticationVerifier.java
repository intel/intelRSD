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

import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.AbstractAuthenticationToken;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.stereotype.Component;

import java.util.Base64;

import static java.nio.charset.StandardCharsets.UTF_8;
import static java.util.Optional.ofNullable;
import static org.springframework.http.HttpHeaders.AUTHORIZATION;
import static org.springframework.util.StringUtils.countOccurrencesOf;
import static org.springframework.util.StringUtils.hasLength;

@Component
class BasicHttpAuthenticationVerifier extends SpringSecurityBasedAccessVerifier {

    @Autowired
    BasicHttpAuthenticationVerifier(AuthenticationManager authenticationManager) {
        super(authenticationManager);
    }

    @Override
    protected AbstractAuthenticationToken extractToken(VerifyAccessRequest verifyAccessRequest) {
        String authorizationHeader = verifyAccessRequest.getHeaders().getFirst(AUTHORIZATION);
        if (hasLength(authorizationHeader)) {
            return getSpringSecurityToken(authorizationHeader);
        }

        throw new BadCredentialsException("Invalid credentials.");
    }

    UsernamePasswordAuthenticationToken getSpringSecurityToken(String authorizationHeader) {
        if (containsBasicAuthScheme(authorizationHeader)) {
            String token = decodeToken(authorizationHeader);
            rejectInvalidCredentials(token);
            String[] loginAndPassword = token.split(":", 2);
            return new UsernamePasswordAuthenticationToken(loginAndPassword[0], loginAndPassword[1]);
        }
        throw new BadCredentialsException("Invalid Basic Authorization header format.");
    }

    private boolean containsBasicAuthScheme(String authenticationHeader) {
        return authenticationHeader.startsWith("Basic");
    }

    private String decodeToken(String headerValue) {
        try {
            return new String(Base64.getDecoder().decode(headerValue.substring("Basic".length()).trim().getBytes(UTF_8)), UTF_8);
        } catch (IllegalArgumentException e) {
            throw new BadCredentialsException("Failed to decode basic authentication token");
        }
    }

    private void rejectInvalidCredentials(String token) {
        if (countOccurrencesOf(token, ":") == 1) {
            return;
        }
        throw new BadCredentialsException("Invalid basic authentication token");
    }

    @Override
    public boolean isApplicableFor(VerifyAccessRequest request) {
        return ofNullable(request.getHeaders().getFirst(AUTHORIZATION)).isPresent();
    }
}
