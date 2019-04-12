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

package com.intel.rsd.resourcemanager.layers.completer;

import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.services.common.Shared;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Set;
import java.util.regex.Pattern;

import static com.intel.rsd.ResourcePatterns.CHASSIS_PATTERN;
import static com.intel.rsd.resourcemanager.layers.completer.CollectionContentsUpdater.updateSetContents;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toSet;

@Component
public class PodChassisResourceCompleter implements ResourceCompleter {
    private Set<String> chassisContainedByPod;

    @SuppressWarnings("unchecked")
    public PodChassisResourceCompleter(@Shared("PodChassisResourceCompleter.chassisContainedByPod") Set<?> chassisContainedByPod) {
        this.chassisContainedByPod = (Set<String>) chassisContainedByPod;
    }

    @Override
    public boolean canCompleteResource(ObjectNode objectNode) {
        String odataType = objectNode.path("@odata.type").asText();
        String chassisType = objectNode.path("ChassisType").asText();

        return odataType.contains("#Chassis.") && chassisType.equals("Pod");
    }

    @Override
    public ObjectNode completeResource(ObjectNode objectNode) {
        ArrayNode contains = JsonNodeFactory.instance.arrayNode();

        chassisContainedByPod
            .stream()
            .sorted()
            .forEach(odataId -> contains.addObject().put("@odata.id", odataId));

        objectNode
            .with("Links")
            .set("Contains", contains);

        return objectNode;
    }

    @Override
    public List<Pattern> interestedInPatterns() {
        return singletonList(CHASSIS_PATTERN);
    }

    @Override
    public void onResourcesCrawled(Set<Resource> resources) {
        Set<String> freshData = resources.stream()
            .filter(resource -> !resource.get("/ChassisType").asText().equals("Pod"))
            .filter(resource -> resource.get("/Links/ContainedBy").isNull())
            .map(Resource::getOdataId)
            .collect(toSet());

        updateSetContents(chassisContainedByPod, freshData);
    }
}
