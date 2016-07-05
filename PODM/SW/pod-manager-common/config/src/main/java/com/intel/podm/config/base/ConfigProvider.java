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

import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.dto.BaseConfig;

import javax.ejb.Singleton;
import javax.inject.Inject;
import java.io.FileNotFoundException;
import java.io.IOException;

import static com.google.common.base.Preconditions.checkArgument;
import static com.google.common.base.Preconditions.checkNotNull;
import static java.lang.String.format;
import static java.util.Objects.nonNull;

@Singleton
public class ConfigProvider {
    @Inject
    private Logger logger;

    @Inject
    private ConfigMap configMap;

    @Inject
    private ConfigAccessor configAccessor;

    public <T extends BaseConfig> T get(Class<T> configClass) {
        checkNotNull(configClass, "configClass should not be null");

        String filePath = getConfigFilePath(configClass);
        try {
            readFileAndUpdateMap(filePath, configClass);
        } catch (FileNotFoundException e) {
            T config = configMap.get(filePath, configClass);
            if (configCanBeGeneratedByApplication(configClass)) {
                write(filePath, config);
            } else {
                logger.w("Problem while reading config file '{}'. PODM will use defaults for this file.", filePath);
            }
        } catch (IOException e) {
            logger.e("Problem while reading config file '{}'. Please check if file has correct permissions and valid content.", filePath);
        }

        return configMap.get(filePath, configClass);
    }

    private <T extends BaseConfig> void readFileAndUpdateMap(String fileName, Class<T> configClass) throws IOException {
        T config = configAccessor.read(fileName, configClass);
        if (!configMap.update(fileName, config)) {
            logger.e("Config file '{}' has not been updated correctly.", fileName);
        }
    }

    private void write(String fileName, Object config) {
        try {
            configAccessor.write(fileName, config);
        } catch (IOException e) {
            logger.e("Problem while writing default config to '{}'", fileName);
        }
    }

    private static <T extends BaseConfig> boolean configCanBeGeneratedByApplication(Class<T> configClass) {
        return getConfigFileAnnotation(configClass).isGeneratedByApplication();
    }

    private static String getConfigFilePath(Class configClass) {
        ConfigFile annotation = getConfigFileAnnotation(configClass);

        String baseDirectory = annotation.isGeneratedByApplication() ? ConfigPaths.WRITABLE_CONFIG_DIR : ConfigPaths.READONLY_CONFIG_DIR;
        return baseDirectory + annotation.filename();
    }

    private static <T extends BaseConfig> ConfigFile getConfigFileAnnotation(Class<T> configClass) {
        ConfigFile annotation = configClass.getDeclaredAnnotation(ConfigFile.class);
        checkArgument(nonNull(annotation), format("%s should be annotated with ConfigFile", configClass));
        return annotation;
    }
}
