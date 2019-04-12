/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm.services.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import lombok.EqualsAndHashCode;
import lombok.Getter;

import java.util.ArrayList;
import java.util.List;

@EqualsAndHashCode
@JsonPropertyOrder({"@odata.type", "@odata.id", "@odata.context"})
public class Role {

    @JsonProperty("@odata.context")
    private final String odataContext = "/redfish/v1/$metadata#Role.Role";

    @JsonProperty("@odata.id")
    private String odataId = "/redfish/v1/AccountService/Roles/";

    @JsonProperty("@odata.type")
    private final String odataType = "#Role.v1_1_0.Role";

    @JsonProperty("Id")
    @Getter
    private final String name;

    @JsonProperty("OemPrivileges")
    @Getter
    private final List<String> oemPrivileges = new ArrayList<>();

    @JsonProperty("AssignedPrivileges")
    @Getter
    private final List<String> assignedPrivileges = new ArrayList<>();

    public Role(String name) {
        this.name = name;
        this.odataId += this.name;
    }
}
