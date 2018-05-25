/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.IpTransportDetails;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.business.redfish.services.actions.NetworkDeviceFunctionInvoker;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.types.Chap;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition;
import com.intel.podm.discovery.external.partial.EndpointObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static com.intel.podm.common.types.AuthenticationMethod.MUTUAL_CHAP;
import static com.intel.podm.common.types.DurableNameFormat.LUN;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class PatchNetworkDeviceFunctionAssemblyTask extends NodeTask {
    @Inject
    private GenericDao genericDao;

    @Inject
    private NetworkDeviceFunctionInvoker networkDeviceFunctionInvoker;

    @Inject
    private EndpointObtainer endpointObtainer;

    private Chap chap;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public void run() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        ComputerSystem computerSystem = getComputerSystemFromNode(composedNode);
        ConnectedEntity connectedEntity = findConnectedEntityWithBootableVolume(composedNode);
        Endpoint targetEndpoint = findTargetEndpoint(connectedEntity);
        IpTransportDetails iscsiTransport = findIscsiTransportWithExistingIp(targetEndpoint);

        IscsiBootDefinition bootDefinition = createBootDefinition();
        bootDefinition.setPrimaryTargetName(getOfPrimaryTargetName(targetEndpoint));
        bootDefinition.setInitiatorName(getInitiatorName(computerSystem, targetEndpoint.getFabric()));
        bootDefinition.setPrimaryTargetIpAddress(getPrimaryTargetIpAddress(iscsiTransport));
        bootDefinition.setPrimaryTargetTcpPort(Ref.of(iscsiTransport.getPort()));
        bootDefinition.setPrimaryLun(getLunFromConnectedEntity(connectedEntity));

        updateDeviceFunction(computerSystem.getNetworkDeviceFunction(), bootDefinition);
    }

    private Ref<String> getPrimaryTargetIpAddress(IpTransportDetails iscsiTransport) {
        return Ref.of(iscsiTransport.getIp().orElseThrow(() ->
            new RuntimeException("IpTransportDetails object do not store any IP")));
    }

    private Ref<String> getOfPrimaryTargetName(Endpoint targetEndpoint) {
        return Ref.of(findTargetIqnIdentifier(targetEndpoint).getDurableName());
    }

    private Ref<String> getInitiatorName(ComputerSystem computerSystem, Fabric fabric) {
        Endpoint initiatorEndpoint = endpointObtainer.getInitiatorEndpoint(computerSystem, fabric);
        return Ref.of(findInitiatorIqnIdentifier(initiatorEndpoint).getDurableName());
    }

    public void setChap(Chap chap) {
        this.chap = chap;
    }

    private void updateDeviceFunction(NetworkDeviceFunction networkDeviceFunction, IscsiBootDefinition bootDefinition) {
        try {
            networkDeviceFunctionInvoker.updateNetworkDeviceFunction(networkDeviceFunction, prepareNetworkDeviceFunctionDefinition(bootDefinition));
        } catch (EntityOperationException e) {
            throw new RuntimeException("NetworkDeviceFunction update failed: " + e.getMessage(), e);
        }
    }

    private IscsiBootDefinition createBootDefinition() {
        IscsiBootDefinition bootDefinition = new IscsiBootDefinition();
        bootDefinition.setTargetInfoViaDhcp(Ref.of(false));
        bootDefinition.setIpMaskDnsViaDhcp(Ref.of(true));
        bootDefinition.setAuthenticationMethod(Ref.of(MUTUAL_CHAP));
        bootDefinition.setMutualChapUsername(chap.getMutualUsername());
        bootDefinition.setMutualChapSecret(chap.getMutualSecret());
        bootDefinition.setChapUsername(chap.getUsername());
        bootDefinition.setChapSecret(chap.getSecret());

        return bootDefinition;
    }

    private IpTransportDetails findIscsiTransportWithExistingIp(Endpoint targetEndpoint) {
        return targetEndpoint.findIscsiTransportDetailsWithDefinedIp()
            .orElseThrow(() -> new RuntimeException("Cannot find transport for iSCSI protocol"));
    }

    private Ref<Integer> getLunFromConnectedEntity(ConnectedEntity connectedEntity) {
        Integer lun = connectedEntity.getIdentifiers().stream()
            .filter(ce -> LUN.equals(ce.getDurableNameFormat()))
            .map(Identifier::getDurableName)
            .mapToInt(Integer::parseInt)
            .findFirst()
            .orElseThrow(() -> new RuntimeException("Primary LUN must be provided and must be in integer format"));
        return Ref.of(lun);
    }

    private Identifier findTargetIqnIdentifier(Endpoint targetEndpoint) {
        return targetEndpoint.findIqnIdentifier()
            .orElseThrow(() -> new RuntimeException("IQN Identifier doesn't exist on endpoint (target) related with bootable volume"));
    }

    private Identifier findInitiatorIqnIdentifier(Endpoint initiatorEndpoint) {
        return initiatorEndpoint.findIqnIdentifier()
            .orElseThrow(() -> new RuntimeException("IQN Identifier doesn't exist on endpoint (initiator) related with bootable volume"));
    }

    private Endpoint findTargetEndpoint(ConnectedEntity connectedEntity) {
        return ofNullable(connectedEntity.getEndpoint())
            .orElseThrow(() -> new RuntimeException("Connection between bootable volume and target endpoint doesn't exist"));
    }

    private ConnectedEntity findConnectedEntityWithBootableVolume(ComposedNode composedNode) {
        return composedNode.findAnyConnectedEntityWithBootableVolume()
            .orElseThrow(() -> new RuntimeException("Composed node is not connected with any bootable volume"));
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        return getAssociatedComputeServiceUuid(genericDao.find(ComposedNode.class, nodeId));
    }

    private NetworkDeviceFunctionUpdateDefinition prepareNetworkDeviceFunctionDefinition(IscsiBootDefinition iscsiBootDefinition) {
        NetworkDeviceFunctionUpdateDefinition updateDefinition = new NetworkDeviceFunctionUpdateDefinition();
        updateDefinition.setIscsiBoot(Ref.of(iscsiBootDefinition));
        return updateDefinition;
    }
}
