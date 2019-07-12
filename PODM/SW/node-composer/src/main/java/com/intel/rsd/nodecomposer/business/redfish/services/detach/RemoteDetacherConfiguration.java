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

import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class RemoteDetacherConfiguration {

    @Bean
    public RemoteDetacher<Volume> volumeRemoteDetacher(EntityTreeTraverser entityTreeTraverser, RemoteDetachStrategy<Volume> remoteDetachStrategy) {
        return new RemoteDetacher<>(entityTreeTraverser, remoteDetachStrategy);
    }

    @Bean
    public RemoteDetacher<Drive> driveRemoteDetacher(EntityTreeTraverser entityTreeTraverser, RemoteDetachStrategy<Drive> remoteDetachStrategy) {
        return new RemoteDetacher<>(entityTreeTraverser, remoteDetachStrategy);
    }


    @Bean
    public RemoteDetacher<Endpoint> endpointRemoteDetacher(EntityTreeTraverser entityTreeTraverser, RemoteDetachStrategy<Endpoint> remoteDetachStrategy) {
        return new RemoteDetacher<>(entityTreeTraverser, remoteDetachStrategy);
    }


    @Bean
    public RemoteDetacher<Processor> processorRemoteDetacher(EntityTreeTraverser entityTreeTraverser, RemoteDetachStrategy<Processor> remoteDetachStrategy) {
        return new RemoteDetacher<>(entityTreeTraverser, remoteDetachStrategy);
    }
}
