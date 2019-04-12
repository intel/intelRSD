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

package com.intel.rsd.podm.sessions.hazelcast;

import com.hazelcast.core.IMap;
import com.hazelcast.core.Offloadable;
import com.hazelcast.map.AbstractEntryProcessor;
import com.hazelcast.query.Predicates;
import com.intel.rsd.podm.sessions.RedfishSession;
import com.intel.rsd.podm.sessions.RedfishSessionsRepository;
import com.intel.rsd.services.common.Shared;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;
import java.util.Optional;

import static java.time.Instant.now;
import static java.util.concurrent.TimeUnit.SECONDS;
import static java.util.stream.Collectors.toList;

@Component
public class HazelcastSessionsRepository implements RedfishSessionsRepository {

    @Value("${aaa-config.session-timeout}")
    private int sessionTimeout;

    private IMap<String, RedfishSession> sessionMap;

    @SuppressWarnings("unchecked")
    public HazelcastSessionsRepository(@Shared("podm:sessions") Map<?, ?> sessionMap) {
        this.sessionMap = (IMap<String, RedfishSession>) sessionMap;
    }

    @Override
    public Collection<String> getSessionIds() {
        return sessionMap.values().stream().map(RedfishSession::getId).collect(toList());
    }

    @Override
    public RedfishSession findByAuthToken(String xAuthToken) {
        return findByXAuthTokenValue(xAuthToken).orElse(null);
    }

    @Override
    public RedfishSession findById(String id) {
        return this.sessionMap.get(id);
    }

    private Optional<RedfishSession> findByXAuthTokenValue(String token) {
        return this.sessionMap.values(Predicates.equal("xAuthToken", token)).stream().findFirst();
    }

    @Override
    public boolean removeSession(String sessionId) {
        RedfishSession existingSession = sessionMap.get(sessionId);
        if (existingSession != null) {
            sessionMap.remove(existingSession.getId());
            return true;
        }
        return false;
    }

    @Override
    public RedfishSession createSession(String userName, String roleName) {
        RedfishSession newSession = new RedfishSession();
        newSession.setUserName(userName);
        newSession.setRole(roleName);
        sessionMap.set(newSession.getId(), newSession, sessionTimeout, SECONDS);
        return newSession;
    }

    @Override
    public void prolongSession(String sessionId) {
        findByXAuthTokenValue(sessionId).ifPresent(redfishSession -> sessionMap.executeOnKey(redfishSession.getId(), new UpdateSessionEntryProcessor()));
    }

    final class UpdateSessionEntryProcessor extends AbstractEntryProcessor<String, RedfishSession> implements Offloadable {

        private static final long serialVersionUID = -2920658312805450732L;

        @Override
        public Object process(Map.Entry<String, RedfishSession> entry) {
            RedfishSession value = entry.getValue();
            if (value != null) {
                value.setLastAccessedTime(now());
            }
            entry.setValue(value);
            return entry;
        }

        @Override
        public String getExecutorName() {
            return OFFLOADABLE_EXECUTOR;
        }
    }
}
