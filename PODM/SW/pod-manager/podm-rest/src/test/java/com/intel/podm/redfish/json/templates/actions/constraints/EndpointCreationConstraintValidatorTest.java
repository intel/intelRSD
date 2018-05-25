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

package com.intel.podm.redfish.json.templates.actions.constraints;

import com.intel.podm.business.dto.EndpointDto;
import com.intel.podm.business.dto.EndpointDto.ConnectedEntityDto;
import com.intel.podm.business.dto.EndpointDto.IdentifierDto;
import com.intel.podm.business.dto.EndpointDto.IpTransportDetailsDto;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import javax.validation.ConstraintValidatorContext;
import javax.validation.ConstraintValidatorContext.ConstraintViolationBuilder;

import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static com.intel.podm.common.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.podm.common.types.Protocol.ROCEV2;
import static java.util.Collections.EMPTY_LIST;
import static java.util.Collections.singletonList;
import static org.mockito.Matchers.anyString;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity", "checkstyle:ExecutableStatementCount", "checkstyle:MethodName"})
public class EndpointCreationConstraintValidatorTest {

    private EndpointCreationConstraintValidator underTest = new EndpointCreationConstraintValidator();

    private ConstraintValidatorContext context;
    private ConstraintViolationBuilder builder;

    @BeforeMethod
    public void setUp() throws Exception {
        this.context = mock(ConstraintValidatorContext.class);
        this.builder = mock(ConstraintViolationBuilder.class);
        when(context.buildConstraintViolationWithTemplate(anyString())).thenReturn(builder);
    }

    @Test
    public void checkWithEmptyRequest_shouldReturnFalse() throws Exception {
        assertFalse(underTest.isValid(null, context), "Null request should not be accepted");
    }

    @Test
    public void checkWithCorrectRequest_shouldReturnTrue() throws Exception {
        assertTrue(underTest.isValid(createCompleteEndpointCreationRequest(), context), "Correct request should be accepted");
    }

    @Test
    public void checkWithEmptyConnectedEntityList_shouldReturnFalse() throws Exception {
        EndpointDto dto = createCompleteEndpointCreationRequest();
        dto.getConnectedEntities().clear();
        dto = spy(dto);
        assertFalse(underTest.isValid(dto, context), "Empty ConnectedEntity list should NOT be accepted");
    }

    @Test
    public void checkWithEmptyIdentifiersList_shouldReturnFalse() throws Exception {
        EndpointDto dto = spy(createCompleteEndpointCreationRequest());
        dto.getIdentifiers().clear();
        assertFalse(underTest.isValid(dto, context), "Empty Identifiers list should NOT be accepted");
    }

    @Test
    public void checkWithCorrectProtocolAndDurableNameFormat_shouldReturnTrue() throws Exception {
        EndpointDto dto = spy(createCompleteEndpointCreationRequest());
        assertTrue(underTest.isValid(dto, context), "Protocol.ISCSI and DurableNameFormat.iQN should be accepted");
    }

    @Test
    public void checkWithMissingDurableNameFormat_shouldReturnFalse() {
        EndpointDto dto = createCompleteEndpointCreationRequest();
        dto.getIdentifiers().clear();
        dto.addIdentifier(new IdentifierDto());
        assertFalse(underTest.isValid(dto, context));
    }

    @Test
    public void checkWithEmptyTransportsList_shouldReturnTrue() {
        EndpointDto dto = spy(createCompleteEndpointCreationRequest());
        dto.setIpTransportDetails(EMPTY_LIST);
        assertTrue(underTest.isValid(dto, context));
    }

    @Test
    public void checkWithMissingTransportProtocol_shouldReturnFalse() {
        EndpointDto dto = spy(createCompleteEndpointCreationRequest());
        dto.getIpTransportDetails().forEach(transport -> transport.setTransportProtocol(null));
        assertFalse(underTest.isValid(dto, context));
    }

    private EndpointDto createCompleteEndpointCreationRequest() {
        EndpointDto endpointDto = new EndpointDto();
        endpointDto.setProtocol(NVME_OVER_FABRICS);
        IdentifierDto identifierDto = new IdentifierDto();
        identifierDto.setDurableNameFormat(NQN);
        identifierDto.setDurableName("someName");
        endpointDto.addIdentifier(identifierDto);
        ConnectedEntityDto connectedEntityDto = new ConnectedEntityDto();
        connectedEntityDto.setEntityRole(TARGET);
        connectedEntityDto.setEntityLink(new ODataId("/redfish/v1/StorageServices/1/Volumes/1"));
        endpointDto.addConnectedEntity(connectedEntityDto);
        IpTransportDetailsDto ipTransportDetailsDto = new IpTransportDetailsDto();
        ipTransportDetailsDto.setTransportProtocol(ROCEV2);
        endpointDto.setIpTransportDetails(singletonList(ipTransportDetailsDto));
        return endpointDto;
    }
}
