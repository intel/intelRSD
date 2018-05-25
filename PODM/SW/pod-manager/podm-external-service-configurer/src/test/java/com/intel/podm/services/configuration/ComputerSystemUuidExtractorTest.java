/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.services.configuration;

import com.intel.podm.client.resources.redfish.EndpointResource;
import org.testng.annotations.Test;

import java.util.UUID;

import static com.intel.podm.common.types.DurableNameFormat.IQN;
import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static com.intel.podm.services.configuration.ComputerSystemUuidExtractor.IQN_PREFIX;
import static com.intel.podm.services.configuration.ComputerSystemUuidExtractor.NQN_PREFIX;
import static com.intel.podm.services.configuration.difference.factory.MockedEndpointResourcesBuilder.createEmptyEndpointResourceBuilder;
import static java.lang.String.format;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MethodName"})
public class ComputerSystemUuidExtractorTest {
    private static final String CS_UUID = "a6e60010-26d9-11e8-9870-433de91ef1cb";
    private static final String NVME_DURABLE_NAME = format("%s:%s", NQN_PREFIX, CS_UUID);
    private static final String ISCSI_DURABLE_NAME = format("%s:%s", IQN_PREFIX, CS_UUID);
    private static final String WRONG_DURABLE_NAME = format("%s%s", "wrong_durable_name", CS_UUID);

    private ComputerSystemUuidExtractor computerSystemUuidExtractor = new ComputerSystemUuidExtractor();

    @Test
    public void extractUuidFromInitiatorEndpoint_shouldReturnCorrectUuid() {
        EndpointResource nvmeResource = createEmptyEndpointResourceBuilder()
            .addIdentifier(NVME_DURABLE_NAME, NQN).addConnectedEntityWithRole(INITIATOR).build();
        EndpointResource iscsiResource = createEmptyEndpointResourceBuilder()
            .addIdentifier(ISCSI_DURABLE_NAME, IQN).addConnectedEntityWithRole(INITIATOR).build();
        UUID extractedUuidFromIscsi = computerSystemUuidExtractor.extractFromInitiatorEndpointResource(iscsiResource);
        UUID extractedUuidFromNvme = computerSystemUuidExtractor.extractFromInitiatorEndpointResource(nvmeResource);

        assertEquals(extractedUuidFromNvme.toString(), CS_UUID, "UUID extracted from NVMe Initiator Endpoint does not match given UUID");
        assertEquals(extractedUuidFromIscsi.toString(), CS_UUID, "UUID extracted from ISCSI Initiator Endpoint does not match given UUID");
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void extractUuidFromTargetEndpoint_shouldFailWithException() {
        EndpointResource resource = createEmptyEndpointResourceBuilder().addIdentifier(NVME_DURABLE_NAME, NQN).addConnectedEntityWithRole(TARGET).build();
        computerSystemUuidExtractor.extractFromInitiatorEndpointResource(resource);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void extractUuidFromInitiatorEndpointWithWrongDurableName_shouldFailWithException() {
        EndpointResource resource = createEmptyEndpointResourceBuilder()
            .addIdentifier(WRONG_DURABLE_NAME, NQN).addConnectedEntityWithRole(INITIATOR).build();
        computerSystemUuidExtractor.extractFromInitiatorEndpointResource(resource);
    }
}
