/*
 * Copyright (c) 2015 Intel Corporation
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

import com.fasterxml.jackson.databind.ObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.inject.Named;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import static com.google.common.base.Preconditions.checkNotNull;

@Dependent
class ConfigAccessor {
    @Inject @Named("ConfigMapper")
    private ObjectMapper mapper;

    public <T> T read(String fileName, Class<T> configClass) throws IOException {
        checkNotNull(fileName, "fileName should not be null");
        checkNotNull(configClass, "configClass should not be null");

        try (FileInputStream stream = new FileInputStream(fileName)) {
            return mapper.readValue(stream, configClass);
        }
    }

    public void write(String fileName, Object config) throws IOException {
        checkNotNull(fileName, "fileName should not be null");
        checkNotNull(config, "config should not be null");

        try (FileOutputStream stream = new FileOutputStream(fileName)) {
            mapper.writeValue(stream, config);
        }
    }
}
