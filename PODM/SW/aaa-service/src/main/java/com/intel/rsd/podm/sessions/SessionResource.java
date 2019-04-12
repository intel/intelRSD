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

package com.intel.rsd.podm.sessions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import lombok.Data;
import lombok.EqualsAndHashCode;

import javax.validation.constraints.NotNull;

@Data
@EqualsAndHashCode
@JsonPropertyOrder({"@odata.type", "@odata.id", "@odata.context", "Id", "UserName"})
public class SessionResource {

    @JsonProperty("@odata.context")
    private final String odataContext = "/redfish/v1/$metadata#Session.Session";

    @JsonProperty("@odata.id")
    private String odataId = "/redfish/v1/SessionService/Sessions/";

    @JsonProperty("@odata.type")
    private final String odataType = "#Session.v1_0_0.Session";

    @JsonProperty("UserName")
    private final String username;

    @JsonProperty("Id")
    private String id;

    public SessionResource(@NotNull String id, @NotNull String username) {
        this.id = id;
        this.username = username;
        this.odataId += this.id;
    }
}
