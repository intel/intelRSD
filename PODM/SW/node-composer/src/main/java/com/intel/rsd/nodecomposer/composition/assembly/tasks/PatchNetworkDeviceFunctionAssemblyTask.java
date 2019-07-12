/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.NetworkDeviceFunctionInvoker;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.dao.ComposedNodeDao;
import com.intel.rsd.nodecomposer.persistence.dao.EndpointDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.IpTransportDetails;
import com.intel.rsd.nodecomposer.persistence.redfish.NetworkDeviceFunction;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import com.intel.rsd.nodecomposer.types.Chap;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.actions.NetworkDeviceFunctionUpdateDefinition;
import com.intel.rsd.nodecomposer.types.actions.NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;

import static com.intel.rsd.nodecomposer.types.AuthenticationMethod.MUTUAL_CHAP;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class PatchNetworkDeviceFunctionAssemblyTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = -8771723636797398276L;

    @Autowired
    private transient ComposedNodeDao composedNodeDao;
    @Autowired
    private transient NetworkDeviceFunctionInvoker networkDeviceFunctionInvoker;
    @Autowired
    private transient EndpointDao endpointDao;

    @Setter
    private Chap chap;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public void run() {
        ComposedNode composedNode = composedNodeDao.find(composedNodeODataId);
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

        val networkDeviceFunction = computerSystem
            .getNetworkDeviceFunction()
            .orElseThrow(() -> new IllegalStateException("Computer System does not have Network Device Function"));
        updateDeviceFunction(networkDeviceFunction, bootDefinition);
    }

    private Ref<String> getPrimaryTargetIpAddress(IpTransportDetails iscsiTransport) {
        return Ref.of(iscsiTransport.getIp().orElseThrow(() ->
            new RuntimeException("IpTransportDetails object do not store any IP")));
    }

    private Ref<String> getOfPrimaryTargetName(Endpoint targetEndpoint) {
        return Ref.of(findTargetIqnIdentifier(targetEndpoint).getDurableName());
    }

    private Ref<String> getInitiatorName(ComputerSystem computerSystem, Fabric fabric) {
        Endpoint initiatorEndpoint = endpointDao.findInitiatorEndpointBySystemAndFabric(computerSystem.getUri(), fabric.getUri());
        return Ref.of(findInitiatorIqnIdentifier(initiatorEndpoint).getDurableName());
    }

    private void updateDeviceFunction(NetworkDeviceFunction networkDeviceFunction, IscsiBootDefinition bootDefinition) {
        try {
            networkDeviceFunctionInvoker.updateNetworkDeviceFunction(networkDeviceFunction, prepareNetworkDeviceFunctionDefinition(bootDefinition));
        } catch (WebClientRequestException e) {
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
        Integer lun = connectedEntity.getLun();
        if (lun == null) {
            throw new RuntimeException("Primary LUN must be provided and must be in integer format");
        }
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

    private NetworkDeviceFunctionUpdateDefinition prepareNetworkDeviceFunctionDefinition(IscsiBootDefinition iscsiBootDefinition) {
        NetworkDeviceFunctionUpdateDefinition updateDefinition = new NetworkDeviceFunctionUpdateDefinition();
        updateDefinition.setIscsiBoot(Ref.of(iscsiBootDefinition));
        return updateDefinition;
    }
}
