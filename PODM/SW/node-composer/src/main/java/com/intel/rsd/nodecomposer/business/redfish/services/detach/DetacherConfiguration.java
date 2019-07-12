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

import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import static com.intel.rsd.nodecomposer.types.AttachableType.Types.DRIVE;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.ENDPOINT;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.PROCESSOR;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.VOLUME;

@Configuration
public class DetacherConfiguration {

    @Bean(VOLUME)
    public Detacher<Volume> volumeDetacher(DetachRequestValidator<Volume> requestValidator,
                                           DetachContextProvider<Volume> contextProvider,
                                           LocalDetacher<Volume> volumeDetacher,
                                           RemoteDetacher<Volume> remoteDetacher) {

        return new Detacher<>(requestValidator, remoteDetacher, volumeDetacher, contextProvider);
    }

    @Bean(DRIVE)
    public Detacher<Drive> driveDetacher(DetachRequestValidator<Drive> requestValidator,
                                         DetachContextProvider<Drive> contextProvider,
                                         LocalDetacher<Drive> localDetacher,
                                         RemoteDetacher<Drive> remoteDetacher) {

        return new Detacher<>(requestValidator, remoteDetacher, localDetacher, contextProvider);
    }

    @Bean(ENDPOINT)
    public Detacher<Endpoint> endpointDetacher(DetachRequestValidator<Endpoint> requestValidator,
                                               DetachContextProvider<Endpoint> contextProvider,
                                               LocalDetacher<Endpoint> localDetacher,
                                               RemoteDetacher<Endpoint> remoteDetacher) {

        return new Detacher<>(requestValidator, remoteDetacher, localDetacher, contextProvider);
    }

    @Bean(PROCESSOR)
    public Detacher<Processor> processorDetacher(DetachRequestValidator<Processor> requestValidator,
                                                 DetachContextProvider<Processor> contextProvider,
                                                 LocalDetacher<Processor> localDetacher,
                                                 RemoteDetacher<Processor> remoteDetacher) {

        return new Detacher<>(requestValidator, remoteDetacher, localDetacher, contextProvider);
    }
}
