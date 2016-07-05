package com.intel.podm.business.services.redfish;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.MemoryDto;
import com.intel.podm.business.services.context.Context;

public interface DimmConfigService {
    CollectionDto getDimmConfigCollection(Context systemContext) throws EntityNotFoundException;
    MemoryDto getDimmConfig(Context dimmContext) throws EntityNotFoundException;
}
