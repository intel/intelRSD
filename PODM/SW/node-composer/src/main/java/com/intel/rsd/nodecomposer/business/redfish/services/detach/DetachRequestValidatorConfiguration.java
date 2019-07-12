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

import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static java.lang.String.format;

@Configuration
public class DetachRequestValidatorConfiguration {

    @Bean
    public DetachRequestValidator<Drive> driveDetachRequestValidator() {
        return (composedNode, drive) -> {
            if (!composedNode.getDrives().contains(drive)) {
                throw new RequestValidationException(createWithViolations(format("Provided drive(%s) is not attached to this Node.", drive)));
            }
        };
    }

    @Bean
    public DetachRequestValidator<Volume> volumeDetachRequestValidator(VolumeHelper volumeHelper) {
        return (composedNode, volume) -> {
            if (!composedNode.getVolumes().contains(volume)) {
                throw new RequestValidationException(createWithViolations(format("Provided volume(%s) is not attached to this Node.", volume)));
            }

            if (ISCSI.equals(volumeHelper.retrieveProtocolFromVolume(volume))) {
                throw new RequestValidationException(createWithViolations(format("Detach volume(%s) with iSCSI protocol is not allowed.", volume)));
            }
        };
    }

    @Bean
    public DetachRequestValidator<Endpoint> endpointDetachRequestValidator() {
        return (composedNode, endpoint) -> {
            if (!composedNode.getEndpoints().contains(endpoint)) {
                throw new RequestValidationException(createWithViolations(format("Provided endpoint(%s) is not attached to this Node.", endpoint)));
            }

            if (ISCSI.equals(endpoint.getProtocol())) {
                throw new RequestValidationException(createWithViolations(format("Detach endpoint(%s) with iSCSI protocol is not allowed.", endpoint)));
            }
        };
    }

    @Bean
    public DetachRequestValidator<Processor> processorDetachRequestValidator() {
        return (composedNode, processor) -> {
            if (!composedNode.getProcessors().contains(processor)) {
                throw new RequestValidationException(createWithViolations(format("Provided processor(%s) is not attached to this Node.", processor)));
            }
        };
    }
}
