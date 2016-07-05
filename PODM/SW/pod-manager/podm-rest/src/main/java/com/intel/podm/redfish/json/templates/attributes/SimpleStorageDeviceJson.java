/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.redfish.json.templates.attributes;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;

@JsonPropertyOrder({"name", "manufacturer", "model", "status", "capacityBytes"})
public class SimpleStorageDeviceJson {
    public String name;
    public String manufacturer;
    public String model;
    public Status status;
    public BigDecimal capacityBytes;

    public SimpleStorageDeviceJson(String name, String manufacturer, String model, Status status, BigDecimal capacityBytes) {
        this.name = name;
        this.manufacturer = manufacturer;
        this.model = model;
        this.status = status;
        this.capacityBytes = capacityBytes;
    }
}
