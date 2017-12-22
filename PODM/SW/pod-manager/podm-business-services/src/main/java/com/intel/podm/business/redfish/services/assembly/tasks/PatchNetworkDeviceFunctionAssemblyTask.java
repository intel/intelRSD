/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.entities.redfish.NetworkInterface;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.RemoteTargetIscsiAddress;
import com.intel.podm.business.entities.redfish.embeddables.Chap;
import com.intel.podm.business.redfish.services.actions.NetworkDeviceFunctionInvoker;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.ChapType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.UUID;

import static com.intel.podm.common.types.AuthenticationMethod.CHAP;
import static com.intel.podm.common.types.AuthenticationMethod.MUTUAL_CHAP;
import static com.intel.podm.common.types.AuthenticationMethod.NONE;
import static com.intel.podm.common.utils.Collector.toSingle;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.apache.commons.lang3.StringUtils.isEmpty;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PatchNetworkDeviceFunctionAssemblyTask extends NodeTask {
    @Inject
    private GenericDao genericDao;

    @Inject
    private NetworkDeviceFunctionInvoker networkDeviceFunctionInvoker;

    @Inject
    private Logger logger;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public void run() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        ComputerSystem computerSystem = getComputerSystemFromNode(composedNode);
        NetworkDeviceFunction deviceFunction = retrieveNetworkDeviceFunction(computerSystem);
        RemoteTarget remoteTarget = retrieveRemoteTarget(composedNode);

        try {
            networkDeviceFunctionInvoker.updateNetworkDeviceFunction(deviceFunction, prepareNetworkDeviceFunctionDefinition(remoteTarget));
        } catch (EntityOperationException e) {
            throw new RuntimeException("NetworkDeviceFunction update failed: " + e.getMessage(), e);
        }
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        return getAssociatedComputeServiceUuid(genericDao.find(ComposedNode.class, nodeId));
    }

    private NetworkDeviceFunction retrieveNetworkDeviceFunction(ComputerSystem computerSystem) {
        return computerSystem.getNetworkInterfaces().stream()
            .map(NetworkInterface::getNetworkDeviceFunctions)
            .flatMap(Collection::stream)
            .collect(toSingle());
    }

    private RemoteTarget retrieveRemoteTarget(ComposedNode composedNode) {
        return composedNode.getRemoteTargets().stream()
            .collect(toSingle());
    }

    private NetworkDeviceFunctionUpdateDefinition prepareNetworkDeviceFunctionDefinition(RemoteTarget remoteTarget) {
        NetworkDeviceFunctionUpdateDefinition updateDefinition = new NetworkDeviceFunctionUpdateDefinition();
        updateDefinition.setIscsiBoot(Ref.of(prepareIscsiBoot(remoteTarget)));

        return updateDefinition;
    }

    private IscsiBootDefinition prepareIscsiBoot(RemoteTarget remoteTarget) {
        IscsiBootDefinition bootDefinition = new IscsiBootDefinition();
        RemoteTargetIscsiAddress iscsiAddress = retrieveIscsiAddress(remoteTarget);

        bootDefinition.setPrimaryTargetIpAddress(Ref.of(iscsiAddress.getTargetPortalIp()));
        bootDefinition.setPrimaryTargetName(Ref.of(iscsiAddress.getTargetIqn()));
        bootDefinition.setPrimaryTargetTcpPort(Ref.of(iscsiAddress.getTargetPortalPort()));
        bootDefinition.setTargetInfoViaDhcp(Ref.of(false));
        bootDefinition.setIpMaskDnsViaDhcp(Ref.of(true));
        bootDefinition.setInitiatorName(Ref.of(prepareInitiatorName(remoteTarget)));
        prepareChap(bootDefinition, iscsiAddress.getChap());

        try {
            bootDefinition.setPrimaryLun(Ref.of(iscsiAddress.getTargetLuns().get(0)));
        } catch (IndexOutOfBoundsException e) {
            throw new IllegalArgumentException("iSCSI address on RemoteTarget is missing TargetLUN");
        }

        return bootDefinition;
    }

    private void prepareChap(IscsiBootDefinition bootDefinition, Chap chap) {
        if (chap == null) {
            prepareNullChap(bootDefinition);
        } else {
            mapChap(bootDefinition, chap);
        }
    }

    private void prepareNullChap(IscsiBootDefinition bootDefinition) {
        bootDefinition.setAuthenticationMethod(Ref.of(NONE));
        bootDefinition.setChapUsername(Ref.of(null));
        bootDefinition.setMutualChapUsername(Ref.of(null));
    }

    private void mapChap(IscsiBootDefinition bootDefinition, Chap chap) {
        bootDefinition.setAuthenticationMethod(Ref.of(mapChapTypeToAuthenticationMethod(chap.getType())));
        bootDefinition.setChapUsername(Ref.of(chap.getUsername()));
        bootDefinition.setMutualChapUsername(Ref.of(chap.getMutualUsername()));
    }

    private String prepareInitiatorName(RemoteTarget remoteTarget) {
        return isEmpty(remoteTarget.getIscsiInitiatorIqn())
            ? "iqn.podm-" + UUID.randomUUID().toString()
            : remoteTarget.getIscsiInitiatorIqn();
    }

    private AuthenticationMethod mapChapTypeToAuthenticationMethod(ChapType chapType) {
        if (chapType == null) {
            return NONE;
        }

        switch (chapType) {
            case ONE_WAY:
                return CHAP;
            case MUTUAL:
                return MUTUAL_CHAP;
            default:
                throw new IllegalArgumentException("Not supported chap type: " + chapType + " in remote target");
        }
    }

    private RemoteTargetIscsiAddress retrieveIscsiAddress(RemoteTarget remoteTarget) {
        return remoteTarget.getRemoteTargetIscsiAddresses().stream()
            .collect(toSingle());
    }
}
