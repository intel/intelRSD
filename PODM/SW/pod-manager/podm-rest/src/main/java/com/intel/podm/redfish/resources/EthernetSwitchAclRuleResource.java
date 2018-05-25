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

package com.intel.podm.redfish.resources;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.dto.EthernetSwitchAclRuleDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;
import com.intel.podm.redfish.json.templates.actions.AclRuleModificationJson;
import com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraint;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.PATCH;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraint.Action.Update;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;
import static javax.ws.rs.core.Response.ok;

@RequestScoped
@Produces(APPLICATION_JSON)
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class EthernetSwitchAclRuleResource extends BaseResource {
    @Inject
    ReaderService<EthernetSwitchAclRuleDto> readerService;

    @Inject
    private RemovalService<RedfishEthernetSwitchAclRule> removalService;

    @Inject
    private UpdateService<RedfishEthernetSwitchAclRule> updateService;

    @GET
    @Override
    public RedfishResourceAmazingWrapper get() {
        Context context = getCurrentContext();
        EthernetSwitchAclRuleDto dto = getOrThrow(() -> readerService.getResource(context));
        return new RedfishResourceAmazingWrapper(context, dto);
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    public Response update(@EthernetSwitchAclRuleConstraint(actionType = Update) AclRuleModificationJson update)
        throws BusinessApiException, TimeoutException {

        updateService.perform(getCurrentContext(), update);
        return ok(get()).build();
    }

    @DELETE
    @Override
    public Response delete() throws TimeoutException, BusinessApiException {
        removalService.perform(getCurrentContext());
        return noContent().build();
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder()
            .addPatchMethod()
            .addDeleteMethod()
            .build();
    }
}
