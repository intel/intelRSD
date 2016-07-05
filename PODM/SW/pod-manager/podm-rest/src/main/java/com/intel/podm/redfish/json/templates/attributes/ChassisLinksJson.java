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
import com.intel.podm.rest.odataid.ODataId;

import java.util.ArrayList;
import java.util.Collection;

@JsonPropertyOrder({"contains", "containedBy", "computerSystems", "switches", "managedBy", "managersInChassis", "oem"})
public final class ChassisLinksJson {
    public Collection<ODataId> contains = new ArrayList<>();
    public ODataId containedBy;
    public Collection<ODataId> computerSystems = new ArrayList<>();
    public Collection<ODataId> switches = new ArrayList<>();
    public Collection<ODataId> managedBy = new ArrayList<>();
    public Collection<ODataId> managersInChassis = new ArrayList<>();
    public final Object oem = new Object();
}
