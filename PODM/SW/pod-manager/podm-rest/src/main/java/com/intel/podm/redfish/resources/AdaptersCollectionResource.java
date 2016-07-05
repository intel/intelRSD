package com.intel.podm.redfish.resources;

import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.services.redfish.AdapterService;

import javax.inject.Inject;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;

import static com.intel.podm.rest.resources.PathParamConstants.ADAPTER_ID;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Produces(APPLICATION_JSON)
public class AdaptersCollectionResource extends BaseResource {
    @Inject
    private AdapterService adapterService;

    @GET
    @Override
    public CollectionDto get() {
        return getOrThrow(() -> adapterService.getAdapters(getCurrentContext()));
    }

    @Path(ADAPTER_ID)
    public AdapterResource getAdapter() {
        return getResource(AdapterResource.class);
    }
}
