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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.embeddables.Fpga;
import com.intel.podm.client.resources.redfish.FpgaObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class FpgaMapper extends SubresourceMapper<FpgaObject, Fpga> {
    @Inject
    public FpgaMapper(EmbeddableCleanAndCreateStrategy<Fpga> embeddableCleanAndCreateStrategy) {
        super(Fpga.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(FpgaObject fpgaResource, Fpga fpgaTarget) {
        fpgaResource.getReconfigurationSlots().ifAssigned(fpgaTarget::setReconfigurationSlots);
        fpgaResource.getBitStreamVersion().ifAssigned(fpgaTarget::setBitStreamVersion);
        fpgaResource.getHssiConfiguration().ifAssigned(fpgaTarget::setHssiConfiguration);
        fpgaResource.getHssiSideband().ifAssigned(fpgaTarget::setHssiSideband);
        fpgaResource.getType().ifAssigned(fpgaTarget::setType);
    }

    @Override
    public boolean equals(FpgaObject fpgaResource, Fpga fpgaTarget) {
        return hssiEquals(fpgaResource, fpgaTarget)
            && Objects.equals(fpgaResource.getReconfigurationSlots().orElse(fpgaTarget.getReconfigurationSlots()), fpgaTarget.getReconfigurationSlots())
            && Objects.equals(fpgaResource.getBitStreamVersion().orElse(fpgaTarget.getBitStreamVersion()), fpgaTarget.getBitStreamVersion())
            && Objects.equals(fpgaResource.getType().orElse(fpgaTarget.getType()), fpgaTarget.getType());
    }

    public boolean hssiEquals(FpgaObject fpgaResource, Fpga fpgaTarget) {
        return Objects.equals(fpgaResource.getHssiConfiguration().orElse(fpgaTarget.getHssiConfiguration()), fpgaTarget.getHssiConfiguration())
            && Objects.equals(fpgaResource.getHssiSideband().orElse(fpgaTarget.getHssiSideband()), fpgaTarget.getHssiSideband());
    }
}
