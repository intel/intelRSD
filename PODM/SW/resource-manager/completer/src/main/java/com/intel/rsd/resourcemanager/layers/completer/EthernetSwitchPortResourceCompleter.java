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

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.services.common.Shared;
import lombok.val;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import static com.intel.rsd.ResourcePatterns.COMPUTER_SYSTEM_ETHERNET_INTERFACE_PATTERN;
import static com.intel.rsd.redfish.ExtendedInfo.ExtendedInfoBuilder.newExtendedInfoBuilder;
import static com.intel.rsd.resourcemanager.layers.completer.CollectionContentsUpdater.updateMapContents;
import static java.util.Collections.emptySet;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.mapping;
import static java.util.stream.Collectors.toSet;

@Component
public class EthernetSwitchPortResourceCompleter implements ResourceCompleter {
    private final Map<String, Set<String>> neighborInterfaceToEthernetInterfaceUri;
    private final ObjectMapper mapper = new ObjectMapper();

    @SuppressWarnings("unchecked")
    public EthernetSwitchPortResourceCompleter(@Shared("EthernetSwitchPortResourceCompleter.neighborInterfaceToEthernetInterfaceUri")
                                                   Map<?, ?> neighborInterfaceToEthernetInterfaceUri) {
        this.neighborInterfaceToEthernetInterfaceUri = (Map<String, Set<String>>) neighborInterfaceToEthernetInterfaceUri;
    }

    @Override
    public boolean canCompleteResource(ObjectNode objectNode) {
        return objectNode.path("@odata.type").asText().contains("#EthernetSwitchPort.");
    }

    @Override
    public ObjectNode completeResource(ObjectNode objectNode) {
        String neighborMacAddress = objectNode.path("NeighborMAC").asText("");

        if (neighborMacAddress.isEmpty()) {
            return objectNode;
        }
        val resources = neighborInterfaceToEthernetInterfaceUri.getOrDefault(neighborMacAddress, emptySet());

        if (resources.isEmpty()) {
            return objectNode;
        } else if (resources.size() > 1) {
            return completeWithExtendedInfo(neighborMacAddress, objectNode, resources);
        }

        val ethernetInterfaceOdataId = resources.iterator().next();

        objectNode
            .putObject("NeighborInterface")
            .put("@odata.id", ethernetInterfaceOdataId);

        return objectNode;
    }

    @Override
    public List<Pattern> interestedInPatterns() {
        return singletonList(COMPUTER_SYSTEM_ETHERNET_INTERFACE_PATTERN);
    }

    @Override
    public void onResourcesCrawled(Set<Resource> resources) {
        val freshMap = resources.stream()
            .filter(resource -> !resource.get("/MACAddress").asText("").equals(""))
            .collect(
                groupingBy(
                    r -> r.get("/MACAddress").asText(),
                    mapping(r -> r.get("/@odata.id").asText(), toSet())
                )
            );
        // TODO: 12/15/17 do this in transaction?
        updateMapContents(neighborInterfaceToEthernetInterfaceUri, freshMap);
    }

    private ObjectNode completeWithExtendedInfo(String macAddress, ObjectNode objectNode, Set<String> resources) {
        String message = String.format(
            "Couldn't match neighbor Ethernet Interface. "
                + "Couldn't find single, enabled and healthy Ethernet Interface with MAC Address: '%s'. Found %d Ethernet Interfaces.",
            macAddress,
            resources.size()
        );

        ArrayNode extendedInfos = JsonNodeFactory.instance.arrayNode()
            .add(mapper.valueToTree(newExtendedInfoBuilder(message).build()));

        objectNode
            .set("NeighborInterface@Message.ExtendedInfo", extendedInfos);

        return objectNode;
    }
}
