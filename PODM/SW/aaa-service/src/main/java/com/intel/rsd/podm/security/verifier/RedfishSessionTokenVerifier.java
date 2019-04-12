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

import com.intel.rsd.podm.security.PodmUser;
import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import com.intel.rsd.podm.sessions.RedfishSession;
import com.intel.rsd.podm.sessions.RedfishSessionsRepository;
import org.springframework.security.authentication.AbstractAuthenticationToken;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.web.authentication.preauth.PreAuthenticatedAuthenticationToken;
import org.springframework.stereotype.Component;

import static com.intel.rsd.podm.sessions.RedfishSession.X_AUTH_TOKEN;
import static java.util.Collections.singletonList;
import static java.util.Optional.ofNullable;
import static org.springframework.util.StringUtils.hasLength;

@Component
public class RedfishSessionTokenVerifier extends SpringSecurityBasedAccessVerifier {

    private RedfishSessionsRepository redfishSessionsRepository;

    public RedfishSessionTokenVerifier(AuthenticationManager authenticationManager, RedfishSessionsRepository redfishSessionsRepository) {
        super(authenticationManager);
        this.redfishSessionsRepository = redfishSessionsRepository;
    }

    @Override
    protected AbstractAuthenticationToken extractToken(VerifyAccessRequest verifyAccessRequest) {
        String redfishAuthenticationToken = verifyAccessRequest.getHeaders().getFirst(X_AUTH_TOKEN);
        RedfishSession session = getSessionForToken(redfishAuthenticationToken);
        if (session != null) {
            redfishSessionsRepository.prolongSession(redfishAuthenticationToken);
            return createTokenFromExistingSession(session);
        }

        throw new BadCredentialsException("Invalid credentials.");
    }

    private AbstractAuthenticationToken createTokenFromExistingSession(RedfishSession session) {
        PodmUser podmUser = new PodmUser(session.getUserName());
        return new PreAuthenticatedAuthenticationToken(podmUser, podmUser, singletonList(new SimpleGrantedAuthority(session.getRole())));
    }

    private RedfishSession getSessionForToken(String redfishAuthenticationToken) {
        if (hasLength(redfishAuthenticationToken)) {
            return redfishSessionsRepository.findByAuthToken(redfishAuthenticationToken);
        }
        return null;
    }


    @Override
    public boolean isApplicableFor(VerifyAccessRequest request) {
        return ofNullable(request.getHeaders().getFirst(X_AUTH_TOKEN)).isPresent();
    }
}
