/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.dto.redfish.ServiceRootDto;
import com.intel.podm.business.services.redfish.ServiceRootService;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.ejb.Stateless;
import javax.inject.Inject;

import static java.util.Collections.emptyList;

@Stateless
public class ServiceRootServiceImpl implements ServiceRootService {
    private static final String SERVICE_VERSION = "1.0.2";

    @Inject
    @Config
    Holder<ServiceConfig> config;

    @Override
    public ServiceRootDto getServiceRoot() {
        return ServiceRootDto.newBuilder()
            .id("ServiceRoot")
            .name("Service root")
            .description("POD Manager Service root")
            .unknownOems(emptyList())
            .redfishVersion(SERVICE_VERSION)
            .uuid(config.get().getUuid())
            .build();
    }
}
