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

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.podm.security.RedfishSessionCreator;
import com.intel.rsd.podm.sessions.RedfishSession;
import com.intel.rsd.podm.sessions.RedfishSessionsRepository;
import com.intel.rsd.podm.sessions.SessionResource;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.PostConstruct;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static com.intel.rsd.podm.sessions.RedfishSession.X_AUTH_TOKEN;
import static com.intel.rsd.redfish.RedfishUtils.toODataIdArrayNode;
import static java.util.stream.Collectors.toList;
import static org.springframework.http.HttpHeaders.LOCATION;
import static org.springframework.http.HttpStatus.CREATED;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.ResponseEntity.noContent;

@RestController
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
@RequestMapping("/redfish/v1")
public class SessionService {

    @Value("${aaa-config.session-timeout}")
    private Long sessionTimeout;

    private ObjectNode sessionServiceResource;
    private ObjectNode sessionsCollectionResource;

    private RedfishSessionsRepository sessionsRepository;
    private RedfishSessionCreator sessionCreator;

    @Autowired
    public SessionService(RedfishSessionsRepository sessionsRepository, RedfishSessionCreator tokenGranter) {
        this.sessionsRepository = sessionsRepository;
        this.sessionCreator = tokenGranter;
    }

    @PostConstruct
    public void postConstruct() throws IOException {
        ObjectNode sessionRootTemplate = readJsonFromFile(this, "SessionServiceTemplate.json");
        sessionServiceResource = sessionRootTemplate.put("SessionTimeout", sessionTimeout);
        sessionsCollectionResource = readJsonFromFile(this, "SessionsCollectionTemplate.json");
    }

    @GetMapping("SessionService")
    public ObjectNode getSessionService() {
        return sessionServiceResource;
    }

    @PostMapping("SessionService/Sessions")
    public ResponseEntity createSession(HttpServletRequest httpServletRequest, HttpServletResponse httpServletResponse) {
        RedfishSession redfishSession = sessionCreator.createSession(httpServletRequest);
        if (redfishSession == null) {
            throw new SessionCreationException("Unable to create session for provided credentials.");
        }
        httpServletResponse.setHeader(X_AUTH_TOKEN, redfishSession.getXAuthToken());
        httpServletResponse.setHeader(LOCATION, "/redfish/v1/SessionService/Sessions/" + redfishSession.getId());
        return ResponseEntity.status(CREATED).body(new SessionResource(redfishSession.getId(), redfishSession.getUserName()));
    }

    @GetMapping("SessionService/Sessions")
    public JsonNode getSessions() {
        val ids = sessionsRepository.getSessionIds().stream().map(id -> "/redfish/v1/SessionService/Sessions/" + id).collect(toList());
        val idsColl = toODataIdArrayNode(ids);
        return sessionsCollectionResource.deepCopy()
            .put("Members@odata.count", idsColl.size())
            .set("Members", idsColl);
    }

    @GetMapping("SessionService/Sessions/{id}")
    public ResponseEntity getSession(@PathVariable String id) {
        RedfishSession redfishSession = sessionsRepository.findById(id);
        if (redfishSession == null) {
            throw new ResourceNotFoundException("Unable to find session for given id.");
        }

        SessionResource session = new SessionResource(id, redfishSession.getUserName());
        return new ResponseEntity<>(session, OK);
    }

    @DeleteMapping("SessionService/Sessions/{id}")
    public ResponseEntity deleteSession(@PathVariable("id") String id) {
        boolean sessionRemoved = sessionsRepository.removeSession(id);
        if (sessionRemoved) {
            return noContent().build();
        }
        throw new ResourceNotFoundException("Unable to find session for given id.");
    }
}
