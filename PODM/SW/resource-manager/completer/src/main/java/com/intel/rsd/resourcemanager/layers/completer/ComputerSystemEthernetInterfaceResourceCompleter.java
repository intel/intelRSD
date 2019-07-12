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

import static com.intel.rsd.ResourcePatterns.ETHERNET_SWITCH_PORT_PATTERN;
import static com.intel.rsd.collections.IterableHelper.single;
import static com.intel.rsd.redfish.ExtendedInfo.ExtendedInfoBuilder.newExtendedInfoBuilder;
import static com.intel.rsd.resourcemanager.layers.completer.CollectionContentsUpdater.updateMapContents;
import static java.util.Collections.emptySet;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.mapping;
import static java.util.stream.Collectors.toSet;

@Component
public class ComputerSystemEthernetInterfaceResourceCompleter implements ResourceCompleter {
    private final Map<String, Set<String>> neighborMacToSwitchPortUriMap;

    private final ObjectMapper mapper = new ObjectMapper();

    @SuppressWarnings("unchecked")
    public ComputerSystemEthernetInterfaceResourceCompleter(@Shared("ComputerSystemEthernetInterfaceResourceCompleter.neighborMacToSwitchPortUri")
                                                                Map<?, ?> neighborMacToSwitchPortUri) {
        this.neighborMacToSwitchPortUriMap = (Map<String, Set<String>>) neighborMacToSwitchPortUri;
    }

    @Override
    public boolean canCompleteResource(ObjectNode objectNode) {
        String odataType = objectNode.path("@odata.type").asText();
        String odataId = objectNode.path("@odata.id").asText();

        return odataType.contains("#EthernetInterface.") && odataId.contains("Systems");
    }

    @Override
    public ObjectNode completeResource(ObjectNode objectNode) {
        String macAddress = objectNode.path("MACAddress").asText("");
        if (macAddress.isEmpty()) {
            return objectNode;
        }
        val resources = neighborMacToSwitchPortUriMap.getOrDefault(macAddress, emptySet());

        if (resources.isEmpty()) {
            return objectNode;
        } else if (resources.size() > 1) {
            return completeWithExtendedInfo(macAddress, objectNode, resources);
        }

        val switchPortOdataId = single(resources);

        objectNode
            .with("Links")
            .with("Oem")
            .with("Intel_RackScale")
            .putObject("NeighborPort")
            .put("@odata.id", switchPortOdataId);

        return objectNode;
    }

    @Override
    public List<Pattern> interestedInPatterns() {
        return singletonList(ETHERNET_SWITCH_PORT_PATTERN);
    }

    @Override
    public void onResourcesCrawled(Set<Resource> resources) {
        val freshMapping = resources.stream()
            .filter(resource -> !resource.get("/NeighborMAC").asText("").equals(""))
            .collect(
                groupingBy(
                    r -> r.get("/NeighborMAC").asText(),
                    mapping(r -> r.get("/@odata.id").asText(), toSet())
                )
            );
        // TODO: 12/14/17 do this in transaction?
        updateMapContents(neighborMacToSwitchPortUriMap, freshMapping);
    }

    private ObjectNode completeWithExtendedInfo(String macAddress, ObjectNode objectNode, Set<String> resources) {
        String message = String.format(
            "Couldn't match neighbor Ethernet Switch Port. "
                + "Couldn't find single, enabled and healthy Ethernet Switch Port with neighbor MAC Address: '%s'. Found %d Ethernet Switch Ports.",
            macAddress,
            resources.size()
        );

        ArrayNode extendedInfos = JsonNodeFactory.instance.arrayNode()
            .add(mapper.valueToTree(newExtendedInfoBuilder(message).build()));

        objectNode
            .with("Links")
            .with("Oem")
            .with("Intel_RackScale")
            .set("NeighborPort@Message.ExtendedInfo", extendedInfos);

        return objectNode;
    }
}
