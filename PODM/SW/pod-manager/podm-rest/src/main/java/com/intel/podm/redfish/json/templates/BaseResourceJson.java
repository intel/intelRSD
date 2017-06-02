/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonAnyGetter;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.redfish.OemType;
import org.atteo.classindex.IndexSubclasses;

import java.util.Map;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_ACTIONS;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static com.intel.podm.redfish.UnknownOemsJsonHelper.createUnknownsOemsMapToAddFromOemPathAndAllUnknownOems;
import static java.util.Collections.emptyMap;

@SuppressWarnings({"checkstyle:VisibilityModifier"})
public abstract class BaseResourceJson extends BaseJson {
    private static final Logger LOGGER = getLogger(BaseResourceJson.class);

    @JsonProperty("Id")
    public String id;
    @JsonProperty("Name")
    public String name;
    @JsonProperty("Description")
    public String description;
    @JsonProperty("Oem")
    public DefaultTopLevelOemJson oem = new DefaultTopLevelOemJson();

    protected BaseResourceJson(String oDataType) {
        super(oDataType);
    }

    @IndexSubclasses
    public abstract class RedfishOemJson {
        @JsonAnyGetter
        public Map<String, Object> handleUnknownOems() throws JsonProcessingException {
            OemType oemType = getClass().getAnnotation(OemType.class);
            if (oemType == null) {
                LOGGER.e("Mandatory OemType annotation is not defined for: " + getClass().getName());
                return emptyMap();
            }

            return createUnknownsOemsMapToAddFromOemPathAndAllUnknownOems(oemType.value().getPath(), unknownOems);
        }
    }

    public abstract class RedfishLinksJson {
        @JsonProperty("Oem")
        public DefaultLinksOemJson oem = new DefaultLinksOemJson();
    }

    public abstract class RedfishActionsJson {
        @JsonProperty("Oem")
        public DefaultActionsOemJson oem = new DefaultActionsOemJson();
    }

    @OemType(TOP_LEVEL_OEM)
    public class DefaultTopLevelOemJson extends RedfishOemJson {
    }

    @OemType(OEM_IN_LINKS)
    public class DefaultLinksOemJson extends RedfishOemJson {
    }

    @OemType(OEM_IN_ACTIONS)
    public class DefaultActionsOemJson extends RedfishOemJson {
    }
}
