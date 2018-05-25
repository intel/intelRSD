/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.EthernetSwitchStaticMac;
import com.intel.podm.client.resources.redfish.EthernetSwitchStaticMacResource;
import com.intel.podm.mappers.EntityMapper;

import javax.enterprise.context.Dependent;

@Dependent
public class EthernetSwitchStaticMacMapper extends EntityMapper<EthernetSwitchStaticMacResource, EthernetSwitchStaticMac> {
    public EthernetSwitchStaticMacMapper() {
        super(EthernetSwitchStaticMacResource.class, EthernetSwitchStaticMac.class);
    }
}
