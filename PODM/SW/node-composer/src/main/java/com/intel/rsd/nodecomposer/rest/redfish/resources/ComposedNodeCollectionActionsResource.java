/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish.resources;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationRequestProcessingException;
import com.intel.rsd.nodecomposer.composition.allocation.CompositionException;
import com.intel.rsd.nodecomposer.composition.allocation.NodeAllocator;
import com.intel.rsd.nodecomposer.rest.redfish.json.assembly.RequestedNodeJson;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import static com.intel.rsd.nodecomposer.rest.error.NodeComposerExceptions.invalidHttpMethod;
import static com.intel.rsd.nodecomposer.rest.error.NodeComposerExceptions.resourcesStateMismatch;
import static com.intel.rsd.nodecomposer.rest.error.NodeComposerExceptions.unsupportedCreationRequest;
import static com.intel.rsd.nodecomposer.rest.redfish.OptionsResponseBuilder.newOptionsForResourceActionBuilder;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;
import static org.springframework.http.ResponseEntity.created;

@RestController
@RequestMapping(path = COMPOSED_NODES_RESOURCE_NAME + "/Actions", produces = APPLICATION_JSON_VALUE)
public class ComposedNodeCollectionActionsResource extends BaseResource {
    private final NodeAllocator nodeAllocator;

    @Autowired
    public ComposedNodeCollectionActionsResource(NodeAllocator nodeAllocator) {
        this.nodeAllocator = nodeAllocator;
    }

    @Override
    public Object get() {
        throw invalidHttpMethod();
    }

    @PostMapping(path = "/Allocate", consumes = APPLICATION_JSON_VALUE)
    public ResponseEntity allocate(@RequestBody RequestedNodeJson requestedNode) {
        try {
            ODataId composedNodeODataId = nodeAllocator.allocate(requestedNode);
            return created(composedNodeODataId.toUri()).build();
        } catch (AllocationRequestProcessingException e) {
            throw unsupportedCreationRequest(e.getViolations());
        } catch (CompositionException e) {
            throw resourcesStateMismatch("Conflict during allocation", new BusinessApiException(e.getMessage(), e));
        }
    }

    @Override
    protected ResponseEntity createOptionsResponse() {
        return newOptionsForResourceActionBuilder().build();
    }
}
