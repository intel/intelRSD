/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.mappers.lui;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.properties.ComputerSystemDevice;
import com.intel.podm.client.api.resources.redfish.DeviceObject;
import com.intel.podm.client.api.resources.redfish.LuiComputerSystemResource;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.function.Supplier;

import static com.intel.podm.common.types.DiscoveryState.BASIC;
import static com.intel.podm.common.types.DiscoveryState.DEEP;

@Dependent
public class LuiComputerSystemMapper extends LuiMapper<LuiComputerSystemResource, ComputerSystem> {
    @Inject
    private GenericDao genericDao;

    @Inject @Config
    private Holder<DiscoveryConfig> config;

    public LuiComputerSystemMapper() {
        super(LuiComputerSystemResource.class, ComputerSystem.class);
    }

    @Override
    protected void performNotAutomatedMapping(LuiComputerSystemResource source, ComputerSystem target) {
        target.setDiscoveryState(config.get().isDeepDiscoveryEnabled() ? DEEP : BASIC);

        clearDevices(target.getPciDevices());
        addDevices(source.getPciDevices(), target::addPciDevice);
    }

    private void addDevices(Iterable<DeviceObject> computerSystemDevices, Supplier<ComputerSystemDevice> deviceSupplier) {
        if (computerSystemDevices == null) {
            return;
        }

        for (DeviceObject deviceObject : computerSystemDevices) {
            ComputerSystemDevice computerSystemDevice = deviceSupplier.get();
            computerSystemDevice.setDeviceId(deviceObject.getDeviceId());
            computerSystemDevice.setVendorId(deviceObject.getVendorId());
        }
    }

    private void clearDevices(Collection<ComputerSystemDevice> computerSystemDevices) {
        computerSystemDevices.forEach(genericDao::remove);
    }
}
