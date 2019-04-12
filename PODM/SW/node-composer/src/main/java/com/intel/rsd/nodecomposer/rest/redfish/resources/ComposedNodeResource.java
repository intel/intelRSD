/*
 * Copyright (c) 2015-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.Dirtying;
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.ComposedNodeReader;
import com.intel.rsd.nodecomposer.business.redfish.services.ComposedNodeRemovalService;
import com.intel.rsd.nodecomposer.business.redfish.services.ComposedNodeUpdateService;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.rest.redfish.json.RedfishResourceAmazingWrapper;
import com.intel.rsd.nodecomposer.rest.redfish.json.actions.ComposedNodePartialRepresentation;
import com.intel.rsd.nodecomposer.rest.redfish.json.actions.ComposedNodePatchConstraint;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PatchMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.PathParamConstants.COMPOSED_NODE_ID;
import static com.intel.rsd.nodecomposer.rest.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;
import static org.springframework.http.ResponseEntity.noContent;
import static org.springframework.http.ResponseEntity.ok;

@RestController
@RequestMapping(path = COMPOSED_NODES_RESOURCE_NAME + "/{" + COMPOSED_NODE_ID + "}", produces = APPLICATION_JSON_VALUE)
public class ComposedNodeResource extends BaseResource {
    private final ComposedNodeReader composedNodeReader;
    private final ComposedNodeRemovalService composedNodeRemovalService;
    private final ComposedNodeUpdateService composedNodeUpdateService;

    @Autowired
    public ComposedNodeResource(ComposedNodeReader composedNodeReader, ComposedNodeRemovalService composedNodeRemovalService,
                                ComposedNodeUpdateService composedNodeUpdateService) {
        this.composedNodeReader = composedNodeReader;
        this.composedNodeRemovalService = composedNodeRemovalService;
        this.composedNodeUpdateService = composedNodeUpdateService;
    }

    @GetMapping
    @Override
    public ResponseEntity<RedfishResourceAmazingWrapper> get() {
        ODataId currentODataId = getCurrentODataId();
        RedfishResourceAmazingWrapper redfishResourceAmazingWrapper =
            new RedfishResourceAmazingWrapper(currentODataId, getOrThrow(() -> composedNodeReader.getResource(currentODataId)));

        return ok(redfishResourceAmazingWrapper);
    }

    @DeleteMapping
    @Override
    @Dirtying
    public ResponseEntity delete() throws BusinessApiException {
        composedNodeRemovalService.delete(getCurrentODataId());
        return noContent().build();
    }

    @PatchMapping(consumes = APPLICATION_JSON_VALUE)
    public ResponseEntity overrideBootSource(@ComposedNodePatchConstraint @RequestBody
                                                 ComposedNodePartialRepresentation representation) throws BusinessApiException {
        composedNodeUpdateService.perform(getCurrentODataId(), representation);
        return get();
    }

    @Override
    protected ResponseEntity createOptionsResponse() {
        return newOptionsForResourceBuilder()
            .addDeleteMethod()
            .addPatchMethod()
            .build();
    }
}
