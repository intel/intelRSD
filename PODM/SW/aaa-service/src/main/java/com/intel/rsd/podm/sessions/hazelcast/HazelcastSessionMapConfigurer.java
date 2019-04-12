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

import com.hazelcast.config.Config;
import com.hazelcast.config.EntryListenerConfig;
import com.hazelcast.config.MapAttributeConfig;
import com.hazelcast.config.MapIndexConfig;
import com.hazelcast.query.extractor.ValueCollector;
import com.hazelcast.query.extractor.ValueExtractor;
import com.intel.rsd.podm.sessions.RedfishSession;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import java.util.Optional;

@Slf4j
@Component
public class HazelcastSessionMapConfigurer {

    @Value("${aaa-config.session-timeout}")
    private int sessionTimeout;

    public HazelcastSessionMapConfigurer(Config config) {
        MapAttributeConfig sessionIdAttributeConfig = new MapAttributeConfig()
            .setName("xAuthToken")
            .setExtractor(ExtractValueByXAuthToken.class.getName());

        EntryListenerConfig entryListenerConfig = new EntryListenerConfig();
        config.getMapConfig("podm:sessions")
            .addEntryListenerConfig(entryListenerConfig)
            .setMaxIdleSeconds(sessionTimeout)
            .setTimeToLiveSeconds(sessionTimeout)
            .addMapAttributeConfig(sessionIdAttributeConfig)
            .addMapIndexConfig(new MapIndexConfig("xAuthToken", false));
    }

    public static class ExtractValueByXAuthToken extends ValueExtractor<RedfishSession, String> {

        @Override
        @SuppressWarnings("unchecked")
        public void extract(RedfishSession session, String argument, ValueCollector collector) {
            Optional.ofNullable(session.getXAuthToken()).ifPresent(collector::addObject);
        }
    }
}
