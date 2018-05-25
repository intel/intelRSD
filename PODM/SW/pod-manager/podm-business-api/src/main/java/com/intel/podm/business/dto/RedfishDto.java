/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonAnyGetter;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.common.types.redfish.RedfishResource;
import org.atteo.classindex.IndexSubclasses;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static com.intel.podm.business.services.OemSerializeHelper.oemDtosToUnknownOems;
import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static com.intel.podm.common.types.redfish.OemType.OEM_ELEMENT_SEPARATOR;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_ACTIONS;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static com.intel.podm.common.utils.StringRepresentation.fromIterable;
import static java.util.Collections.emptyMap;
import static java.util.stream.Collectors.toMap;

public abstract class RedfishDto implements RedfishResource {
    private static final Logger LOGGER = getLogger(RedfishDto.class);
    @JsonProperty("@odata.type")
    private final String oDataType;
    @JsonProperty("Id")
    private String id;
    @JsonProperty("Name")
    private String name;
    @JsonProperty("Description")
    private String description;
    @JsonProperty("Oem")
    private DefaultTopLevelOemDto oem = new DefaultTopLevelOemDto();
    @JsonIgnore
    private List<UnknownOemDto> unknownOems = new ArrayList<>();

    protected RedfishDto(String oDataType) {
        this.oDataType = oDataType;
    }

    @Override
    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getoDataType() {
        return oDataType;
    }

    @Override
    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    @JsonIgnore
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    public void setUnknownOems(List<UnknownOemDto> unknownOems) {
        this.unknownOems = unknownOems;
    }

    @IndexSubclasses
    public abstract class RedfishOemDto {
        @JsonAnyGetter
        public Map<String, Object> handleUnknownOems() throws JsonProcessingException {
            OemType oemType = getClass().getAnnotation(OemType.class);
            if (oemType == null) {
                LOGGER.e("Mandatory OemType annotation is not defined for: " + getClass().getName());
                return emptyMap();
            }

            return createUnknownsOemsMapToAddFromOemPathAndAllUnknownOems(oemType.value().getPath(), oemDtosToUnknownOems(unknownOems));
        }

        private Map<String, Object> createUnknownsOemsMapToAddFromOemPathAndAllUnknownOems(List<String> oemNodePath, Map<String, JsonNode> unknownOems) {
            String oemNodePathBeginning = fromIterable(oemNodePath, OEM_ELEMENT_SEPARATOR);
            String prefix = oemNodePathBeginning + OEM_ELEMENT_SEPARATOR;

            return unknownOems.entrySet().stream()
                .filter(entry -> entry.getKey().startsWith(prefix))
                .collect(toMap(e -> e.getKey().replaceFirst(prefix, ""), Map.Entry::getValue));
        }
    }

    public abstract class RedfishLinksDto {
        @JsonProperty("Oem")
        private DefaultLinksOemDto oem = new DefaultLinksOemDto();
    }

    public abstract class RedfishActionsDto {
        @JsonProperty("Oem")
        private DefaultActionsOemDto oem = new DefaultActionsOemDto();
    }

    @OemType(TOP_LEVEL_OEM)
    public class DefaultTopLevelOemDto extends RedfishOemDto {
    }

    @OemType(OEM_IN_LINKS)
    public class DefaultLinksOemDto extends RedfishOemDto {
    }

    @OemType(OEM_IN_ACTIONS)
    public class DefaultActionsOemDto extends RedfishOemDto {
    }
}
