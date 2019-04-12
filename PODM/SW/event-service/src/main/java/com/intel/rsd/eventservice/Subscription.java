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

package com.intel.rsd.eventservice;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.rsd.dto.events.SubscriptionRequest;
import lombok.Data;

@Data
public class Subscription {
    @JsonProperty("@odata.context")
    private final String odataContext = "/redfish/v1/$metadata#EventDestination.EventDestination";

    @JsonProperty("@odata.id")
    private final String odataId;

    @JsonProperty("@odata.type")
    private final String odataType = "#EventDestination.v1_0_0.EventDestination";

    private final Integer id;

    @JsonUnwrapped
    private final SubscriptionRequest subscriptionRequest;

    @JsonProperty("Id")
    public String idAsString() {
        return String.valueOf(id);
    }
}
