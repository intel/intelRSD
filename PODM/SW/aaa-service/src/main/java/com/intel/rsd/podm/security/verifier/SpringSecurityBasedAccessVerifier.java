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
import lombok.extern.slf4j.Slf4j;
import org.springframework.security.authentication.AbstractAuthenticationToken;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.core.AuthenticationException;

@Slf4j
abstract class SpringSecurityBasedAccessVerifier implements AccessVerifier {

    private final AuthenticationManager authenticationManager;

    SpringSecurityBasedAccessVerifier(AuthenticationManager authenticationManager) {
        this.authenticationManager = authenticationManager;
    }

    public final boolean verifyAccess(VerifyAccessRequest verifyAccessRequest) {
        try {
            AbstractAuthenticationToken token = extractToken(verifyAccessRequest);
            return token.isAuthenticated() || authenticationManager.authenticate(token).isAuthenticated();
        } catch (AuthenticationException e) {
            return false;
        }
    }

    protected abstract AbstractAuthenticationToken extractToken(VerifyAccessRequest verifyAccessRequest);
}
