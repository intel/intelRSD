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

import com.intel.rsd.nodecomposer.Dirtying;
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ActionInfoDto;
import com.intel.rsd.nodecomposer.business.redfish.services.AssemblyActionService;
import com.intel.rsd.nodecomposer.business.redfish.services.AttachResourceActionService;
import com.intel.rsd.nodecomposer.business.redfish.services.ComposedNodeRemovalService;
import com.intel.rsd.nodecomposer.business.redfish.services.ComposedNodeResetActionService;
import com.intel.rsd.nodecomposer.business.redfish.services.DetachResourceActionService;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.rest.redfish.json.RedfishResourceAmazingWrapper;
import com.intel.rsd.nodecomposer.rest.redfish.json.actions.AttachResourceJson;
import com.intel.rsd.nodecomposer.rest.redfish.json.actions.DetachResourceJson;
import com.intel.rsd.nodecomposer.rest.redfish.json.actions.ResetActionJson;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.PathParamConstants.COMPOSED_NODE_ID;
import static com.intel.rsd.nodecomposer.rest.error.NodeComposerExceptions.invalidHttpMethod;
import static com.intel.rsd.nodecomposer.rest.redfish.OptionsResponseBuilder.newOptionsForResourceActionBuilder;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.ATTACH_RESOURCE_ACTION_INFO;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_ASSEMBLE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_ATTACH_RESOURCE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_DETACH_RESOURCE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_FORCE_DELETE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_RESET;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.DETACH_RESOURCE_ACTION_INFO;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static java.lang.String.format;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;
import static org.springframework.http.ResponseEntity.noContent;
import static org.springframework.http.ResponseEntity.ok;

@RestController
@RequestMapping(path = COMPOSED_NODES_RESOURCE_NAME + "/{" + COMPOSED_NODE_ID + "}/Actions", produces = APPLICATION_JSON_VALUE)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComposedNodeActionsResource extends BaseResource {
    private final ComposedNodeResetActionService composedNodeResetActionService;
    private final AssemblyActionService assemblyActionService;
    private final AttachResourceActionService attachResourceActionService;
    private final DetachResourceActionService detachResourceActionService;
    private final ComposedNodeRemovalService composedNodeRemovalService;

    @Autowired
    public ComposedNodeActionsResource(ComposedNodeResetActionService composedNodeResetActionService, AssemblyActionService assemblyActionService,
                                       AttachResourceActionService attachResourceActionService, DetachResourceActionService detachResourceActionService,
                                       ComposedNodeRemovalService composedNodeRemovalService) {
        this.composedNodeResetActionService = composedNodeResetActionService;
        this.assemblyActionService = assemblyActionService;
        this.attachResourceActionService = attachResourceActionService;
        this.detachResourceActionService = detachResourceActionService;
        this.composedNodeRemovalService = composedNodeRemovalService;
    }

    @Override
    public Object get() {
        throw invalidHttpMethod();
    }

    @PostMapping(value = "/" + COMPOSED_NODE_RESET, consumes = APPLICATION_JSON_VALUE)
    @Dirtying
    public ResponseEntity reset(@RequestBody ResetActionJson resetActionJson) throws BusinessApiException {
        composedNodeResetActionService.perform(getCurrentODataId(), resetActionJson);
        return noContent().build();
    }

    @PostMapping(value = "/" + COMPOSED_NODE_ASSEMBLE)
    @Dirtying
    public ResponseEntity assemble() throws BusinessApiException {
        assemblyActionService.perform(getCurrentODataId());
        return noContent().build();
    }

    @PostMapping(value = "/" + COMPOSED_NODE_ATTACH_RESOURCE, consumes = APPLICATION_JSON_VALUE)
    @Dirtying
    public ResponseEntity attach(@RequestBody AttachResourceJson attachResourceJson) throws BusinessApiException {
        attachResourceActionService.perform(getCurrentODataId(), attachResourceJson);
        return noContent().build();
    }

    @PostMapping(value = "/" + COMPOSED_NODE_DETACH_RESOURCE, consumes = APPLICATION_JSON_VALUE)
    @Dirtying
    public ResponseEntity detachResource(@RequestBody DetachResourceJson detachResourceJson) throws BusinessApiException {
        detachResourceActionService.perform(getCurrentODataId(), detachResourceJson);
        return noContent().build();
    }

    @GetMapping(value = "/" + ATTACH_RESOURCE_ACTION_INFO)
    public ResponseEntity<RedfishResourceAmazingWrapper> getAttachResourceActionInfo() throws BusinessApiException {
        ODataId oDataId = getCurrentODataId();
        RedfishResourceAmazingWrapper redfishResourceAmazingWrapper =
            getRedfishResourceAmazingWrapper(oDataId, attachResourceActionService.getActionInfo(oDataId));

        return ok(redfishResourceAmazingWrapper);
    }

    @GetMapping(value = "/" + DETACH_RESOURCE_ACTION_INFO)
    public ResponseEntity<RedfishResourceAmazingWrapper> getDetachResourceActionInfo() throws BusinessApiException {
        ODataId oDataId = getCurrentODataId();
        RedfishResourceAmazingWrapper redfishResourceAmazingWrapper =
            getRedfishResourceAmazingWrapper(oDataId, detachResourceActionService.getActionInfo(oDataId));

        return ok(redfishResourceAmazingWrapper);
    }

    @PostMapping(value = "/" + COMPOSED_NODE_FORCE_DELETE)
    @Dirtying
    public ResponseEntity<RedfishResourceAmazingWrapper> forceDelete() throws BusinessApiException {
        composedNodeRemovalService.forceDelete(getCurrentODataId());
        return noContent().build();
    }

    @Override
    protected ResponseEntity createOptionsResponse() {
        return newOptionsForResourceActionBuilder().build();
    }

    private RedfishResourceAmazingWrapper getRedfishResourceAmazingWrapper(ODataId oDataId, ActionInfoDto actionInfoDto) {
        String infoServiceType = actionInfoDto.getId();
        return new RedfishResourceAmazingWrapper(oDataId.append(format("/Actions/%s", infoServiceType)), actionInfoDto);
    }
}
