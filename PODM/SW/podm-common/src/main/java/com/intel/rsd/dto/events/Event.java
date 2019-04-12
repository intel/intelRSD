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

package com.intel.rsd.dto.events;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import lombok.Data;

import javax.validation.Valid;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;
import java.util.Collection;

import static com.fasterxml.jackson.annotation.JsonAutoDetect.Visibility.ANY;
import static com.fasterxml.jackson.annotation.JsonAutoDetect.Visibility.NONE;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static java.lang.String.format;
import static java.lang.String.valueOf;

@Data
@JsonAutoDetect(fieldVisibility = ANY, getterVisibility = NONE)
@JsonPropertyOrder({"@odata.context", "@odata.type", "@odata.id"})
public class Event {
    private static final String EVENT_ODATA_ID_PREFIX = "/redfish/v1/EventService/Events/";

    @JsonProperty("@odata.type")
    private final String oDataType = "#Event.v1_2_0.Event";

    @JsonProperty("@odata.context")
    private final String oDataContext = "/redfish/v1/$metadata#Event.Event";

    @JsonProperty("Id")
    private String id;

    @JsonProperty("Name")
    private String name = "Event Array";

    @JsonInclude(NON_NULL)
    @JsonProperty("Description")
    private String description;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @JsonProperty("Events")
    @NotNull
    @Valid
    @Size(min = 1)
    private Collection<EventRecord> eventRecords;

    @JsonProperty("@odata.id")
    public String getODataId() {
        return format("%s%s", EVENT_ODATA_ID_PREFIX, valueOf(id));
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class Oem {
    }
}
