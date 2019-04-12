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

import com.intel.rsd.resourcemanager.layers.Chainable;
import com.intel.rsd.resourcemanager.layers.EventHandler;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.runner.configuration.AppConfig;
import lombok.val;
import org.springframework.boot.context.properties.EnableConfigurationProperties;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;

import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;

import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toMap;

@Configuration
@ComponentScan("com.intel.rsd.resourcemanager.runner")
@EnableConfigurationProperties(AppConfig.class)
public class PipelineConfiguration {

    @Bean
    @Pipeline
    Layer inboundDownstream(Collection<Layer> detectedLayers, AppConfig appConfig) {
        val configuredChain = new ArrayList<AppConfig.Layer>(appConfig.getLayers());
        configuredChain.add(createAppConfigLayer("RequiredLayer"));
        return initialize(detectedLayers, configuredChain);
    }

    @Bean
    @Pipeline
    EventHandler eventUpstream(Collection<EventHandler> detectedEventHandlers, AppConfig appConfig) {
        return initialize(detectedEventHandlers, appConfig.getEventsUpstream());
    }

    private <T extends Chainable<T>> T initialize(Collection<T> detectedChain, Collection<AppConfig.Layer> configuredChain) {
        validateConfiguration(configuredChain, detectedChain);
        val sortedChain = sortModules(detectedChain, configuredChain);
        val configurableModules = new LinkedList<T>(sortedChain);
        while (!configurableModules.isEmpty()) {
            configurableModules.poll().setNext(configurableModules.peek());
        }
        return sortedChain.get(0);
    }

    private <T extends Chainable<T>> List<T> sortModules(Collection<T> detectedChain, Collection<AppConfig.Layer> configuredLayers) {
        val detectedChainElementsById = detectedChain.stream().collect(toMap(Chainable::id, identity()));
        return configuredLayers.stream()
            .map(AppConfig.Layer::getType)
            .filter(detectedChainElementsById::containsKey)
            .map(detectedChainElementsById::get)
            .collect(toList());
    }

    private AppConfig.Layer createAppConfigLayer(String name) {
        AppConfig.Layer layer = new AppConfig.Layer();
        layer.setType(name);
        return layer;
    }

    private <T extends Chainable<T>> void validateConfiguration(Collection<AppConfig.Layer> configuredLayers, Collection<T> detectedChainElements) {
        List<String> configuredNames = configuredLayers.stream().map(AppConfig.Layer::getType).collect(toList());
        List<String> detectedNames = detectedChainElements.stream().map(Chainable::id).collect(toList());
        if (!detectedNames.containsAll(configuredNames)) {
            configuredNames.removeAll(detectedNames);
            throw new RuntimeException("Cannot detect configured layers: " + configuredNames);
        }
    }
}
