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

import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class LocalDetacherConfiguration {

    @Bean
    public LocalDetacher<Volume> volumeLocalDetacher(GenericDao genericDao, LocalDetachStrategy<Volume> localDetachStrategy) {
        return new LocalDetacher<>(genericDao, localDetachStrategy);
    }

    @Bean
    public LocalDetacher<Drive> driveLocalDetacher(GenericDao genericDao, LocalDetachStrategy<Drive> localDetachStrategy) {
        return new LocalDetacher<>(genericDao, localDetachStrategy);
    }

    @Bean
    public LocalDetacher<Endpoint> endpointLocalDetacher(GenericDao genericDao, LocalDetachStrategy<Endpoint> localDetachStrategy) {
        return new LocalDetacher<>(genericDao, localDetachStrategy);
    }

    @Bean
    public LocalDetacher<Processor> processorLocalDetacher(GenericDao genericDao, LocalDetachStrategy<Processor> localDetachStrategy) {
        return new LocalDetacher<>(genericDao, localDetachStrategy);
    }
}
