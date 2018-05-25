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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.PcieDeviceDto;
import com.intel.podm.business.entities.redfish.PcieDevice;

import javax.enterprise.context.Dependent;

import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asPcieDeviceFunctionContexts;

@Dependent
class PcieDeviceDtoMapper extends DtoMapper<PcieDevice, PcieDeviceDto> {
    PcieDeviceDtoMapper() {
        super(PcieDevice.class, PcieDeviceDto.class);
        this.ignoredProperties("links", "oem");
    }

    @Override
    protected void performNotAutomatedMapping(PcieDevice source, PcieDeviceDto target) {
        super.performNotAutomatedMapping(source, target);

        target.getLinks().getChassis().addAll(asChassisContexts(source.getChassis()));
        target.getLinks().getPcieFunctions().addAll(asPcieDeviceFunctionContexts(source.getPcieDeviceFunctions()));
    }
}
