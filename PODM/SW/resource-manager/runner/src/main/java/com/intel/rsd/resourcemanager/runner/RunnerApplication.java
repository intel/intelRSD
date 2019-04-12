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

package com.intel.rsd.resourcemanager.runner;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.intel.rsd.redfish.RedfishErrorController;
import com.intel.rsd.resourcemanager.common.SouthboundConfig;
import com.intel.rsd.resourcemanager.runner.configuration.AppConfig;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.web.servlet.context.AnnotationConfigServletWebServerApplicationContext;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.cloud.client.circuitbreaker.EnableCircuitBreaker;
import org.springframework.cloud.netflix.eureka.EnableEurekaClient;
import org.springframework.context.ApplicationContextInitializer;
import org.springframework.context.annotation.Import;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.util.ReflectionUtils;

import java.io.File;
import java.io.IOException;
import java.util.function.Function;

@EnableScheduling
@SpringBootApplication
@Import({SouthboundConfig.class, RedfishErrorController.class})
@EnableEurekaClient
@EnableCircuitBreaker
@EnableCaching
@SuppressWarnings({"checkstyle:HideUtilityClassConstructor"})
public class RunnerApplication {
    public static void main(String[] args) {
        SpringApplication app = new SpringApplication(RunnerApplication.class);
        app.addInitializers(new ExternalModuleClassPathInitializer());
        app.run(args);
    }

    public static class ExternalModuleClassPathInitializer implements ApplicationContextInitializer<AnnotationConfigServletWebServerApplicationContext> {
        private static final String MODULE_PACKAGE_NAME = "com.intel.rsd.resourcemanager.layers";

        private static final Function<AppConfig.Layer, String> LAYER_TO_PACKAGE_NAME =
            layer -> String.join(".", MODULE_PACKAGE_NAME, layer.getType()).toLowerCase();

        private ObjectMapper mapper = new ObjectMapper(new YAMLFactory());

        @Override
        public void initialize(AnnotationConfigServletWebServerApplicationContext applicationContext) {
            try {
                String appConfigPath = applicationContext.getEnvironment().getProperty("config");
                AppConfig appConfig = mapper.readValue(new File(appConfigPath), AppConfig.class);
                applicationContext.scan(appConfig.getLayers().stream().map(LAYER_TO_PACKAGE_NAME).toArray(String[]::new));
            } catch (IOException e) {
                ReflectionUtils.rethrowRuntimeException(e);
            }
        }
    }
}

