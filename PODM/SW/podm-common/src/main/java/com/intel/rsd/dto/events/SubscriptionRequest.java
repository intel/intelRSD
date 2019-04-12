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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.dto.ODataId;
import com.intel.rsd.validator.AbsoluteUri;
import lombok.Data;
import org.hibernate.validator.constraints.Length;

import javax.validation.Valid;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;
import java.net.URI;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@Data
@JsonInclude(NON_NULL)
public class SubscriptionRequest {
    @JsonProperty("Name")
    @NotNull
    @Length(min = 1)
    private String name;

    @JsonProperty(value = "Destination")
    @NotNull
    @AbsoluteUri
    private URI destination;

    @JsonProperty("EventTypes")
    @NotNull
    @Size(min = 1)
    private Set<EventType> eventTypes;

    @JsonProperty("Context")
    @NotNull
    @Length(min = 1)
    private String context;

    @JsonProperty("Protocol")
    @NotNull
    private String protocol = "Redfish";

    @JsonProperty("OriginResources")
    @Valid
    private Set<ODataId> originResources;
}
