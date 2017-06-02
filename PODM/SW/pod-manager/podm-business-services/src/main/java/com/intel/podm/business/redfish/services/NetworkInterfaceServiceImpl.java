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

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.NetworkInterfaceDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.NetworkInterface;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.NETWORK_INTERFACES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class NetworkInterfaceServiceImpl implements ReaderService<NetworkInterfaceDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        ComputerSystem system = (ComputerSystem) traverser.traverse(context);
        return new CollectionDto(NETWORK_INTERFACES, getAsIdSet(system.getNetworkInterfaces()));
    }

    @Override
    public NetworkInterfaceDto getResource(Context context) throws ContextResolvingException {
        NetworkInterface networkInterface = (NetworkInterface) traverser.traverse(context);
        return NetworkInterfaceDto.newBuilder()
            .id(networkInterface.getId().toString())
            .name(networkInterface.getName())
            .description(networkInterface.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(networkInterface.getService(), networkInterface.getUnknownOems()))
            .status(networkInterface.getStatus())
            .build();
    }
}
