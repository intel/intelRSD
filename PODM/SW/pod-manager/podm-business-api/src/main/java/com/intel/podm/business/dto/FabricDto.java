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

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "fabricType", "maxZones", "status", "zones", "endpoints", "switches",
    "fabricType", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class FabricDto extends RedfishDto {
    private final Links links = new Links();
    private final Actions actions = new Actions();
    private Protocol fabricType;
    private Status status;
    private Integer maxZones;
    private SingletonContext zones;
    private SingletonContext endpoints;
    private SingletonContext switches;

    public FabricDto() {
        super("#Fabric.v1_0_0.Fabric");
    }

    public Protocol getFabricType() {
        return fabricType;
    }

    public void setFabricType(Protocol fabricType) {
        this.fabricType = fabricType;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Integer getMaxZones() {
        return maxZones;
    }

    public void setMaxZones(Integer maxZones) {
        this.maxZones = maxZones;
    }

    public SingletonContext getZones() {
        return zones;
    }

    public void setZones(SingletonContext zones) {
        this.zones = zones;
    }

    public SingletonContext getEndpoints() {
        return endpoints;
    }

    public void setEndpoints(SingletonContext endpoints) {
        this.endpoints = endpoints;
    }

    public SingletonContext getSwitches() {
        return switches;
    }

    public void setSwitches(SingletonContext switches) {
        this.switches = switches;
    }

    public Links getLinks() {
        return links;
    }

    public Actions getActions() {
        return actions;
    }

    public final class Links extends RedfishLinksDto {
    }

    public final class Actions extends RedfishActionsDto {
    }
}
