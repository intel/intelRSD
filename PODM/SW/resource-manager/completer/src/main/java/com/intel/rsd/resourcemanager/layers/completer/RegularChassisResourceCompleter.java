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

import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.crawler.Resource;
import lombok.val;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Set;
import java.util.regex.Pattern;

import static java.util.Collections.emptyList;

@Component
public class RegularChassisResourceCompleter implements ResourceCompleter {
    @Override
    public boolean canCompleteResource(ObjectNode objectNode) {
        String odataType = objectNode.path("@odata.type").asText();
        String chassisType = objectNode.path("ChassisType").asText();

        return odataType.contains("#Chassis.") && !chassisType.equals("Pod");
    }

    @Override
    public ObjectNode completeResource(ObjectNode objectNode) {
        val containedBy = objectNode.with("Links").get("ContainedBy");
        if (containedBy == null || containedBy.isNull()) {
            objectNode
                .with("Links")
                .set("ContainedBy", JsonNodeFactory.instance.objectNode().put("@odata.id", "/redfish/v1/Chassis/pod"));
        }

        if (chassisHasNullParentId(objectNode)) {
            objectNode
                .with("Oem")
                .with("Intel_RackScale")
                .with("Location")
                .set("ParentId", JsonNodeFactory.instance.textNode("pod"));
        }

        return objectNode;
    }

    @Override
    public List<Pattern> interestedInPatterns() {
        return emptyList();
    }

    @Override
    public void onResourcesCrawled(Set<Resource> resources) {
    }

    private boolean chassisHasNullParentId(ObjectNode objectNode) {
        val parentId = objectNode.at("/Oem/Intel_RackScale/Location/ParentId");
        return parentId.isNull() || parentId.asText().equals("");
    }
}
