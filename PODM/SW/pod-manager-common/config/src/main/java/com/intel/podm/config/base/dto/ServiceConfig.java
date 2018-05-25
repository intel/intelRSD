/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.config.base.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.config.base.ConfigFile;

import java.util.Objects;
import java.util.UUID;

import static java.util.UUID.randomUUID;

@ConfigFile(filename = "service-root-uuid.json", isGeneratedByApplication = true)
public class ServiceConfig extends BaseConfig {
    @JsonProperty("UUID")
    private UUID uuid = randomUUID();

    public UUID getUuid() {
        return uuid;
    }

    @Override
    public boolean configUpdateIsAccepted(BaseConfig updatedConfig) {
        if (!(updatedConfig instanceof ServiceConfig)) {
            return false;
        }

        ServiceConfig newConfig = (ServiceConfig) updatedConfig;
        return Objects.equals(newConfig.getUuid(), getUuid());
    }
}
