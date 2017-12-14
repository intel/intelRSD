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

package com.intel.podm.discovery;

import com.intel.podm.business.entities.dao.ComposedNodeDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ComputerSystemMetadata;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.RemoteTargetMetadata;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import org.testng.annotations.Test;

import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.ComposedNode.OFFLINE_CRITICAL_STATUS;
import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLED;
import static com.intel.podm.common.types.ComposedNodeState.FAILED;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.PowerState.OFF;
import static com.intel.podm.common.types.PowerState.ON;
import static com.intel.podm.common.types.State.ENABLED;
import static java.util.Collections.emptySet;
import static java.util.Collections.singleton;
import static java.util.Collections.singletonList;
import static java.util.UUID.fromString;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNull;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class ComposedNodeUpdaterTest {

    private static final Status ENABLED_OK_STATUS = new Status(ENABLED, OK, OK);
    private static final UUID COMPUTE_SERVICE_UUID = fromString("71f7888a-fec8-44f5-8c0f-c15312ad4f9b");
    private static final UUID COMPUTER_SYSTEM_UUID = fromString("81f7888a-fec8-44f5-8c0f-c15312ad4f9b");
    private static final UUID STORAGE_SERVICE_UUID = fromString("91f7888a-fec8-44f5-8c0f-c15312ad4f9b");
    private static final String REMOTE_TARGET_IQN = "remote_target_iqn";

    private static final UUID ANOTHER_COMPUTER_SYSTEM_UUID = fromString("11111111-fec8-44f5-8c0f-c15312ad4f9b");
    private static final UUID ANOTHER_COMPUTE_SERVICE_UUID = fromString("22222222-fec8-44f5-8c0f-c15312ad4f9b");
    private static final UUID ANOTHER_STORAGE_SERVICE_UUID = fromString("33333333-fec8-44f5-8c0f-c15312ad4f9b");
    private static final String ANOTHER_REMOTE_TARGET_IQN = "another_remote_target_iqn";

    @Test
    public void disabledNodeWithoutRemoteTarget_whenComputerSystemIsBack_shouldRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, OFF);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, null, null);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));

        assertFalse(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getStatus(), ENABLED_OK_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        verify(computerSystem.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithoutRemoteTarget_whenComputerSystemIsBackAndPoweredOff_shouldRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, OFF);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, null, null);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));

        assertFalse(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), ASSEMBLED);
        assertEquals(composedNode.getStatus(), ENABLED_OK_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        verify(computerSystem.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithoutRemoteTarget_whenComputerSystemIsBackAndPoweredOn_shouldRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, ON);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, null, null);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));

        assertFalse(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), ASSEMBLED);
        assertEquals(composedNode.getStatus(), ENABLED_OK_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        verify(computerSystem.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithoutRemoteWithMissingComputerSystem_whenComputerSystemIsNotBack_shouldNotRecover() {
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(emptySet());

        assertTrue(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), FAILED);
        assertEquals(composedNode.getStatus(), OFFLINE_CRITICAL_STATUS);
        assertNull(composedNode.getComputerSystem());
    }

    @Test
    public void disabledNodeWithoutRemoteTargetWithMissingComputerSystem_whenImproperComputerSystemIsBack_shouldNotRecover() {
        ComputerSystem computerSystem = mockComputerSystem(ANOTHER_COMPUTE_SERVICE_UUID, ANOTHER_COMPUTER_SYSTEM_UUID, ON);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));

        assertTrue(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), FAILED);
        assertEquals(composedNode.getStatus(), OFFLINE_CRITICAL_STATUS);
        assertNull(composedNode.getComputerSystem());
        verify(computerSystem.getMetadata(), never()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingSystemAndTarget_whenOnlyComputerSystemIsBack_nodeShouldContainComputerSystemButNotRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, ON);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));

        assertTrue(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), FAILED);
        assertEquals(composedNode.getStatus(), OFFLINE_CRITICAL_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        verify(computerSystem.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingSystemAndTarget_whenOnlyTargetIsBack_nodeShouldContainTargetButNotRecover() {
        RemoteTarget remoteTarget = mockRemoteTarget(STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(remoteTarget));

        assertTrue(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), FAILED);
        assertEquals(composedNode.getStatus(), OFFLINE_CRITICAL_STATUS);
        assertEquals(composedNode.getRemoteTargets().size(), 1);
        assertEquals(composedNode.getRemoteTargets().iterator().next(), remoteTarget);
        verify(remoteTarget.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingSystemAndTarget_whenSystemAndTargetIsBack_shouldRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, ON);
        RemoteTarget remoteTarget = mockRemoteTarget(STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));
        composedNodeUpdater.updateRelatedComposedNodes(singleton(remoteTarget));

        assertFalse(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), ASSEMBLED);
        assertEquals(composedNode.getStatus(), ENABLED_OK_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        assertEquals(composedNode.getRemoteTargets().size(), 1);
        assertEquals(composedNode.getRemoteTargets().iterator().next(), remoteTarget);
        verify(remoteTarget.getMetadata()).setAllocated(true);
        verify(computerSystem.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingSystemAndTarget_whenProperSystemAndImproperTargetIsBack_shouldNotRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, ON);
        RemoteTarget remoteTarget = mockRemoteTarget(ANOTHER_STORAGE_SERVICE_UUID, ANOTHER_REMOTE_TARGET_IQN);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));
        composedNodeUpdater.updateRelatedComposedNodes(singleton(remoteTarget));

        assertTrue(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), FAILED);
        assertEquals(composedNode.getStatus(), OFFLINE_CRITICAL_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        assertEquals(composedNode.getRemoteTargets().size(), 0);
        verify(remoteTarget.getMetadata(), never()).setAllocated(true);
        verify(computerSystem.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingSystemAndTarget_whenImproperSystemAndProperTargetIsBack_shouldNotRecover() {
        ComputerSystem computerSystem = mockComputerSystem(ANOTHER_COMPUTE_SERVICE_UUID, ANOTHER_COMPUTER_SYSTEM_UUID, ON);
        RemoteTarget remoteTarget = mockRemoteTarget(STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));
        composedNodeUpdater.updateRelatedComposedNodes(singleton(remoteTarget));

        assertTrue(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), FAILED);
        assertEquals(composedNode.getStatus(), OFFLINE_CRITICAL_STATUS);
        assertNull(composedNode.getComputerSystem());
        assertEquals(composedNode.getRemoteTargets().size(), 1);
        assertEquals(composedNode.getRemoteTargets().iterator().next(), remoteTarget);
        verify(remoteTarget.getMetadata()).setAllocated(true);
        verify(computerSystem.getMetadata(), never()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingSystem_whenSystemIsBack_shouldRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, ON);
        RemoteTarget remoteTarget = mockRemoteTarget(STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        composedNode.addRemoteTarget(remoteTarget);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));
        composedNodeUpdater.updateRelatedComposedNodes(singleton(computerSystem));

        assertFalse(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), ASSEMBLED);
        assertEquals(composedNode.getStatus(), ENABLED_OK_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        assertEquals(composedNode.getRemoteTargets().size(), 1);
        assertEquals(composedNode.getRemoteTargets().iterator().next(), remoteTarget);
        verify(remoteTarget.getMetadata(), never()).setAllocated(true);
        verify(computerSystem.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingTarget_whenTargetIsBack_shouldRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, ON);
        RemoteTarget remoteTarget = mockRemoteTarget(STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        composedNode.setComputerSystem(computerSystem);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));
        composedNodeUpdater.updateRelatedComposedNodes(singleton(remoteTarget));

        assertFalse(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), ASSEMBLED);
        assertEquals(composedNode.getStatus(), ENABLED_OK_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        assertEquals(composedNode.getRemoteTargets().size(), 1);
        assertEquals(composedNode.getRemoteTargets().iterator().next(), remoteTarget);
        verify(remoteTarget.getMetadata()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingTarget_whenImproperTargetIsBack_shouldNotRecover() {
        ComputerSystem computerSystem = mockComputerSystem(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID, ON);
        RemoteTarget remoteTarget = mockRemoteTarget(ANOTHER_STORAGE_SERVICE_UUID, ANOTHER_REMOTE_TARGET_IQN);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        composedNode.setComputerSystem(computerSystem);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(remoteTarget));

        assertTrue(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), FAILED);
        assertEquals(composedNode.getStatus(), OFFLINE_CRITICAL_STATUS);
        assertEquals(composedNode.getComputerSystem(), computerSystem);
        assertEquals(composedNode.getRemoteTargets().size(), 0);
        verify(remoteTarget.getMetadata(), never()).setAllocated(true);
    }

    @Test
    public void disabledNodeWithRemoteWithMissingComputerSystem_whenTargetIsRediscovered_shouldContainOnlyOneTarget() {
        RemoteTarget remoteTarget = mockRemoteTarget(STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);
        ComposedNode composedNode = mockDisabledComposedNode(COMPUTE_SERVICE_UUID, COMPUTER_SYSTEM_UUID,
                STORAGE_SERVICE_UUID, REMOTE_TARGET_IQN);

        composedNode.addRemoteTarget(remoteTarget);

        ComposedNodeUpdater composedNodeUpdater = getComposedNodeUpdater(singletonList(composedNode));

        composedNodeUpdater.updateRelatedComposedNodes(singleton(remoteTarget));

        assertTrue(composedNode.isEligibleForRecovery());
        assertEquals(composedNode.getComposedNodeState(), FAILED);
        assertEquals(composedNode.getStatus(), OFFLINE_CRITICAL_STATUS);
        assertEquals(composedNode.getRemoteTargets().size(), 1);
        assertEquals(composedNode.getRemoteTargets().iterator().next(), remoteTarget);
    }


    private RemoteTarget mockRemoteTarget(UUID storageServiceUuid, String remoteTargetIqn) {
        ExternalService externalService = mock(ExternalService.class);
        when(externalService.getUuid()).thenReturn(storageServiceUuid);

        RemoteTarget remoteTarget = mock(RemoteTarget.class);

        RemoteTargetMetadata metadata = mock(RemoteTargetMetadata.class);
        when(remoteTarget.getMetadata()).thenReturn(metadata);
        when(remoteTarget.getTargetIqn()).thenReturn(remoteTargetIqn);
        when(remoteTarget.getService()).thenReturn(externalService);
        when(remoteTarget.isEnabledAndHealthy()).thenReturn(true);
        when(remoteTarget.canBeAllocated()).thenReturn(true);
        when(remoteTarget.getMetadata().isAllocated()).thenReturn(false);

        return remoteTarget;
    }

    private ComputerSystem mockComputerSystem(UUID computeServiceUuid, UUID computerSystemUuid, PowerState powerState) {
        ComputerSystem computerSystem = mock(ComputerSystem.class);

        ExternalService externalService = mock(ExternalService.class);
        when(externalService.getUuid()).thenReturn(computeServiceUuid);

        ComputerSystemMetadata metadata = mock(ComputerSystemMetadata.class);
        when(computerSystem.getMetadata()).thenReturn(metadata);

        when(computerSystem.getUuid()).thenReturn(computerSystemUuid);
        when(computerSystem.isEnabledAndHealthy()).thenReturn(true);
        when(computerSystem.canBeAllocated()).thenReturn(true);
        when(computerSystem.getService()).thenReturn(externalService);
        when(computerSystem.getPowerState()).thenReturn(powerState);
        when(computerSystem.getMetadata().isAllocated()).thenReturn(false);

        return computerSystem;
    }

    private ComposedNodeDao mockComposedNodeDao(List<ComposedNode> composedNodes) {
        ComposedNodeDao composedNodeDao = mock(ComposedNodeDao.class);
        when(composedNodeDao.getComposedNodesEligibleForRecovery()).thenReturn(composedNodes);

        return composedNodeDao;
    }

    private ComposedNode mockDisabledComposedNode(UUID computeServiceUuid, UUID computerSystemUuid,
                                                  UUID storageServiceUuid, String targetIqn) {
        ComposedNodeMock composedNode = new ComposedNodeMock();
        composedNode.setEligibleForRecovery(true);
        composedNode.setStatus(OFFLINE_CRITICAL_STATUS);
        composedNode.setComposedNodeState(FAILED);

        composedNode.setAssociatedComputeServiceUuid(computeServiceUuid);
        composedNode.setAssociatedComputerSystemUuid(computerSystemUuid);
        composedNode.setAssociatedStorageServiceUuid(storageServiceUuid);
        composedNode.setAssociatedRemoteTargetIqn(targetIqn);

        return composedNode;
    }

    private ComposedNodeUpdater getComposedNodeUpdater(List<ComposedNode> composedNodes) {
        ComposedNodeUpdater composedNodeUpdater = new ComposedNodeUpdater();
        composedNodeUpdater.logger = mock(Logger.class);
        composedNodeUpdater.composedNodeDao = mockComposedNodeDao(composedNodes);
        return composedNodeUpdater;
    }

    private static class ComposedNodeMock extends ComposedNode {
        private Status status;
        private ComposedNodeState composedNodeState;
        private UUID associatedComputeServiceUuid;
        private UUID associatedComputerSystemUuid;
        private String associatedRemoteTargetIqn;
        private UUID associatedStorageServiceUuid;
        private Boolean eligibleForRecovery;
        private ComputerSystem computerSystem;
        private Set<RemoteTarget> remoteDrives = new HashSet<>();

        @Override
        public void setStatus(Status status) {
            this.status = status;
        }

        @Override
        public Status getStatus() {
            return status;
        }

        @Override
        public ComposedNodeState getComposedNodeState() {
            return composedNodeState;
        }

        @Override
        public void setComposedNodeState(ComposedNodeState composedNodeState) {
            this.composedNodeState = composedNodeState;
        }

        @Override
        public UUID getAssociatedComputeServiceUuid() {
            return associatedComputeServiceUuid;
        }

        @Override
        public void setAssociatedComputeServiceUuid(UUID associatedComputeServiceUuid) {
            this.associatedComputeServiceUuid = associatedComputeServiceUuid;
        }

        @Override
        public UUID getAssociatedComputerSystemUuid() {
            return associatedComputerSystemUuid;
        }

        @Override
        public void setAssociatedComputerSystemUuid(UUID associatedComputerSystemUuid) {
            this.associatedComputerSystemUuid = associatedComputerSystemUuid;
        }

        @Override
        public String getAssociatedRemoteTargetIqn() {
            return associatedRemoteTargetIqn;
        }

        @Override
        public void setAssociatedRemoteTargetIqn(String associatedRemoteTargetIqn) {
            this.associatedRemoteTargetIqn = associatedRemoteTargetIqn;
        }

        @Override
        public UUID getAssociatedStorageServiceUuid() {
            return associatedStorageServiceUuid;
        }

        @Override
        public void setAssociatedStorageServiceUuid(UUID associatedStorageServiceUuid) {
            this.associatedStorageServiceUuid = associatedStorageServiceUuid;
        }

        @Override
        public boolean isEligibleForRecovery() {
            return eligibleForRecovery;
        }

        @Override
        public void setEligibleForRecovery(boolean eligibleForRecovery) {
            this.eligibleForRecovery = eligibleForRecovery;
        }

        @Override
        public ComputerSystem getComputerSystem() {
            return computerSystem;
        }

        @Override
        public void setComputerSystem(ComputerSystem computerSystem) {
            this.computerSystem = computerSystem;
        }

        @Override
        public Set<RemoteTarget> getRemoteTargets() {
            return remoteDrives;
        }

        @Override
        public void addRemoteTarget(RemoteTarget remoteTarget) {
            remoteDrives.add(remoteTarget);
        }
    }
}
