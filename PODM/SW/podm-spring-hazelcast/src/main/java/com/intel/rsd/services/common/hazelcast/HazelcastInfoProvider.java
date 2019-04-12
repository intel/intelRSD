/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.services.common.hazelcast;

import com.hazelcast.config.Config;
import com.netflix.appinfo.ApplicationInfoManager;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.HashMap;
import java.util.Map;

@Slf4j
@Component
public class HazelcastInfoProvider {

    @Autowired
    public HazelcastInfoProvider(Config hazelcastConfig, ApplicationInfoManager applicationInfoManager) {
        Integer hazelcastPort = hazelcastConfig.getNetworkConfig().getPort();
        String hazelcastHost = hazelcastConfig.getNetworkConfig().getInterfaces().getInterfaces().iterator().next();
        log.info("Registering {} as hazelcast host and {} as hazelcast port info in service metadata", hazelcastHost, hazelcastPort);
        Map<String, String> metadata = new HashMap<>();
        metadata.put("HAZELCAST_HOST", hazelcastHost);
        metadata.put("HAZELCAST_PORT", hazelcastPort.toString());
        applicationInfoManager.registerAppMetadata(metadata);
    }
}
