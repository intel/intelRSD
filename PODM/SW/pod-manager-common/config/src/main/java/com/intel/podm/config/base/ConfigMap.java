/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.config.base;

import com.intel.podm.config.base.dto.BaseConfig;

import javax.enterprise.context.Dependent;
import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;

@Dependent
class ConfigMap {
    private final Map<String, BaseConfig> map = new HashMap<>();

    private final Object lock = new Object();

    public boolean update(String fileName, BaseConfig config) {
        requiresNonNull(fileName, "fileName");
        requiresNonNull(config, "config");

        synchronized (lock) {
            if (!canBeUpdatedWith(fileName, config)) {
                return false;
            }

            map.put(fileName, config);
            return true;
        }
    }

    public <T extends BaseConfig> T get(String fileName, Class<T> configClass) {
        requiresNonNull(fileName, "fileName");
        requiresNonNull(configClass, "configClass");

        synchronized (lock) {
            if (map.containsKey(fileName)) {
                BaseConfig config = map.get(fileName);

                if (!configClass.isInstance(config)) {
                    String msg = format("stored config is not instance of '%s'", configClass);
                    throw new IllegalStateException(msg);
                }

                return (T) config;
            }

            T config = newInstance(configClass);
            map.put(fileName, config);
            return config;
        }
    }

    private boolean canBeUpdatedWith(String fileName, BaseConfig config) {
        return !map.containsKey(fileName) || map.get(fileName).configUpdateIsAccepted(config);
    }

    private static <T extends BaseConfig> T newInstance(Class<T> configClass) {
        try {
            return configClass.newInstance();
        } catch (InstantiationException | IllegalAccessException e) {
            throw new RuntimeException("Exception while creating config instance", e);
        }
    }
}
