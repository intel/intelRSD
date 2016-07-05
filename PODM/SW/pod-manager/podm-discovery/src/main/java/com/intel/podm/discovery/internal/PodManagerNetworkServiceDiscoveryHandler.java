/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.discovery.internal;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Status;
import com.intel.podm.config.network.NetworkConfigurationIOException;
import com.intel.podm.config.network.NetworkConfigurationReader;
import com.intel.podm.config.network.dto.NetworkServiceDto;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.common.types.State.DISABLED;

@Dependent
public class PodManagerNetworkServiceDiscoveryHandler {

    private static final String DEFAULT_NAME = "Pod Manager Network Service";

    @Inject
    private Logger logger;

    @Inject
    private NetworkConfigurationReader networkConfigurationReader;

    @Inject
    private GenericDao genericDao;

    public void addNetworkService(Manager manager) {
        try {
            //TODO: fixme in the future - NetworkServiceDto
            NetworkServiceDto networkServiceDto = networkConfigurationReader.readConfiguration("network-service", NetworkServiceDto.class);

            NetworkProtocol networkProtocol = genericDao.create(NetworkProtocol.class);
            networkProtocol.setName(networkServiceDto.getName());
            networkProtocol.setDescription(networkServiceDto.getDescription());
            networkProtocol.setStatus(new Status(networkServiceDto.getState(), networkServiceDto.getHealth(), null));
            networkProtocol.setHostName(networkServiceDto.getHostName());
            networkProtocol.setFqdn(networkServiceDto.getFqdn());
            manager.setNetworkProtocol(networkProtocol);
        } catch (NetworkConfigurationIOException e) {
            logger.e("Pod manager network service configuration is unavailable, using defaults");
            addDefaultDisabledNetworkService(manager);
        }
    }

    private void addDefaultDisabledNetworkService(Manager manager) {
        NetworkProtocol networkProtocol = genericDao.create(NetworkProtocol.class);
        networkProtocol.setName(DEFAULT_NAME);
        networkProtocol.setDescription(DEFAULT_NAME);
        networkProtocol.setStatus(new Status(DISABLED, null, null));
        manager.setNetworkProtocol(networkProtocol);
    }

}
