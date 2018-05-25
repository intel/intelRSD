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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.EndpointDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.client.actions.EndpointCreationRequest;
import com.intel.podm.client.resources.ODataId;
import org.mockito.InjectMocks;
import org.modelmapper.Converter;
import org.modelmapper.spi.MappingContext;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import javax.inject.Inject;
import java.util.Collection;

import static com.intel.podm.common.types.DurableNameFormat.NQN;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static com.intel.podm.common.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.podm.common.types.Protocol.ROCE;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.util.Collections.singletonList;
import static org.mockito.Mockito.spy;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EndpointCreationServiceTest {

    @InjectMocks
    private ContextAwareMapper<EndpointDto, EndpointCreationRequest> underTest;

    @Inject
    private Converter<Context, ODataId> contextODataIdConverter;

    @BeforeMethod
    public void setUp() throws Exception {
        this.contextODataIdConverter = spy(new Converter<Context, ODataId>() {
            @Override
            public ODataId convert(MappingContext<Context, ODataId> context) {
                return new ODataId(context.getSource().asOdataId().toUri());
            }
        });
        initMocks(this);
        this.underTest.init();
    }

    @SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
    @Test
    public void testIfValuesAreCorrectlyMapped() throws ContextResolvingException {
        EndpointDto dto = createCompleteEndpointCreationRequest();
        EndpointCreationRequest result = underTest.map(dto, EndpointCreationRequest.class);

        assertEquals(result.getName(), dto.getName());
        assertEquals(result.getDescription(), dto.getDescription());
        assertEquals(result.getEndpointProtocol(), dto.getProtocol());
        assertEquals(result.getEndpointProtocol(), dto.getProtocol());

        Collection<EndpointCreationRequest.Identifier> targetIdentifiers = result.getIdentifiers();
        Collection<EndpointDto.IdentifierDto> sourceIdentifiers = dto.getIdentifiers();
        assertEquals(targetIdentifiers.size(), sourceIdentifiers.size());
        EndpointDto.IdentifierDto sourceIdentifier = single(sourceIdentifiers);
        EndpointCreationRequest.Identifier targetIdentifier = single(targetIdentifiers);
        assertEquals(targetIdentifier.getDurableName(), sourceIdentifier.getDurableName());
        assertEquals(targetIdentifier.getDurableNameFormat(), sourceIdentifier.getDurableNameFormat());

        assertEquals(result.getConnectedEntities().size(), dto.getConnectedEntities().size());
        EndpointDto.ConnectedEntityDto sourceConnectedEntity = single(dto.getConnectedEntities());
        EndpointCreationRequest.ConnectedEntity targetConnectedEntity = single(result.getConnectedEntities());
        assertEquals(targetConnectedEntity.getEntityLink().toUri(), sourceConnectedEntity.getEntityLink().asOdataId().toUri());
        assertEquals(targetConnectedEntity.getEntityRole(), sourceConnectedEntity.getEntityRole());

        assertEquals(result.getIpTransportDetails().size(), dto.getIpTransportDetails().size());
        EndpointDto.IpTransportDetailsDto sourceTransport = single(dto.getIpTransportDetails());
        EndpointCreationRequest.IpTransportDetails targetTransport = single(result.getIpTransportDetails());
        assertEquals(targetTransport.getTransportProtocol(), sourceTransport.getTransportProtocol());
    }

    @SuppressWarnings({"checkstyle:ExecutableStatementCount"})
    private EndpointDto createCompleteEndpointCreationRequest() {
        EndpointDto endpointDto = new EndpointDto();
        endpointDto.setProtocol(NVME_OVER_FABRICS);
        EndpointDto.IdentifierDto identifierDto = new EndpointDto.IdentifierDto();
        identifierDto.setDurableNameFormat(NQN);
        identifierDto.setDurableName("someName");
        endpointDto.setIdentifiers(singletonList(identifierDto));
        EndpointDto.ConnectedEntityDto connectedEntityDto = new EndpointDto.ConnectedEntityDto();
        connectedEntityDto.setEntityRole(TARGET);
        connectedEntityDto.setEntityLink(new com.intel.podm.business.services.redfish.odataid.ODataId("/redfish/v1/StorageServices/1/Volumes/1"));
        endpointDto.addConnectedEntity(connectedEntityDto);
        EndpointDto.IpTransportDetailsDto ipTransportDetailsDto = new EndpointDto.IpTransportDetailsDto();
        ipTransportDetailsDto.setTransportProtocol(ROCE);
        endpointDto.setIpTransportDetails(singletonList(ipTransportDetailsDto));
        return endpointDto;
    }

}
