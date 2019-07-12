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

package com.intel.rsd.nodecomposer.business.redfish.services.detach;

import com.intel.rsd.nodecomposer.business.redfish.services.helpers.DriveHelper;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import lombok.val;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachableContextBuilder.createDetachOperationContext;

@Configuration
public class DetachContextProviderConfiguration {

    @Bean
    public DetachContextProvider<Drive> detachDriveContextProvider(DriveHelper driveHelper) {
        return (assetToBeDetached, composedNode) -> {
            val relatedFabric = driveHelper.retrieveFabricFromDrive(assetToBeDetached);
            return createDetachOperationContext(relatedFabric, composedNode, assetToBeDetached);
        };
    }

    @Bean
    public DetachContextProvider<Volume> detachVolumeContextProvider(VolumeHelper volumeHelper) {
        return (assetToBeDetached, composedNode) -> {
            val relatedFabric = volumeHelper.retrieveFabricFromVolume(assetToBeDetached);
            return createDetachOperationContext(relatedFabric, composedNode, assetToBeDetached);
        };
    }

    @Bean
    public DetachContextProvider<Endpoint> detachEndpointContextProvider() {
        return (assetToBeDetached, composedNode) -> createDetachOperationContext(assetToBeDetached.getFabric(), composedNode, assetToBeDetached);
    }

    @Bean
    public DetachContextProvider<Processor> detachProcessorContextProvider(ProcessorHelper processorHelper) {
        return (assetToBeDetached, composedNode) -> {
            val relatedFabric = processorHelper.retrieveFabricFromProcessor(assetToBeDetached);
            return createDetachOperationContext(relatedFabric, composedNode, assetToBeDetached);
        };
    }
}
