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

import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.podm.sessions.RedfishSession;
import com.intel.rsd.podm.sessions.RedfishSessionsRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.stereotype.Component;

import javax.servlet.http.HttpServletRequest;
import java.io.IOException;
import java.util.Optional;

import static com.intel.rsd.json.JsonUtils.readJsonFromStream;
import static java.util.Optional.ofNullable;

@Slf4j
@Component
public class RedfishSessionCreator {

    private AuthenticationManager authenticationManager;

    private RedfishSessionsRepository redfishSessionsRepository;

    @Autowired
    public RedfishSessionCreator(AuthenticationManager authenticationManager, RedfishSessionsRepository redfishSessionsRepository) {
        this.authenticationManager = authenticationManager;
        this.redfishSessionsRepository = redfishSessionsRepository;
    }

    public RedfishSession createSession(HttpServletRequest httpServletRequest) {
        try {
            UsernamePasswordAuthenticationToken authenticationToken = createAuthenticationToken(httpServletRequest);
            if (authenticationToken != null) {
                return startNewSession(authenticationManager.authenticate(authenticationToken));
            }
        } catch (AuthenticationException e) {
            log.error(e.getMessage());
        }
        return null;
    }

    private RedfishSession startNewSession(Authentication authentication) {
        Optional<String> role = authentication.getAuthorities().stream().map(GrantedAuthority::getAuthority).findFirst();
        return role.map(s -> redfishSessionsRepository.createSession(authentication.getName(), s)).orElse(null);
    }

    private UsernamePasswordAuthenticationToken createAuthenticationToken(HttpServletRequest httpServletRequest) {
        try {
            ObjectNode jsonObject = readJsonFromStream(httpServletRequest.getInputStream());
            String username = ofNullable(jsonObject.get("UserName")).orElseThrow(IllegalArgumentException::new).asText();
            String password = ofNullable(jsonObject.get("Password")).orElseThrow(IllegalArgumentException::new).asText();
            return new UsernamePasswordAuthenticationToken(username, password);
        } catch (IOException e) {
            return null;
        }
    }
}
