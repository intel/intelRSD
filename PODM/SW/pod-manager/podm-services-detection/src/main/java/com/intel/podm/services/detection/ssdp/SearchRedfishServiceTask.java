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

package com.intel.podm.services.detection.ssdp;

import com.intel.podm.common.enterprise.utils.tasks.DefaultManagedTask;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;
import org.fourthline.cling.controlpoint.event.Search;
import org.fourthline.cling.model.message.header.ServiceTypeHeader;
import org.fourthline.cling.model.types.ServiceType;

import javax.annotation.PostConstruct;
import javax.enterprise.context.Dependent;
import javax.enterprise.event.Event;
import javax.inject.Inject;

import static org.fourthline.cling.model.types.ServiceType.valueOf;

@Dependent
public class SearchRedfishServiceTask extends DefaultManagedTask implements Runnable {

    private static final ServiceType REDFISH_SERVICE_TYPE = valueOf("urn:dmtf-org:service:redfish-rest:1");

    @Inject
    @Config
    private Holder<ServiceDetectionConfig> configuration;

    @Inject
    private Event<Search> msearchEvent;
    private Search searchRequest;

    @PostConstruct
    private void init() {
        searchRequest = new Search(
                new ServiceTypeHeader(REDFISH_SERVICE_TYPE),
                configuration.get().getProtocols().getSsdp().getMx()
        );
    }

    @Override
    public void run() {
        msearchEvent.fire(searchRequest);
    }
}
