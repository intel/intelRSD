package com.intel.podm.business.redfish.services;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.MemoryDto;
import com.intel.podm.business.dto.redfish.attributes.MemoryLocationDto;
import com.intel.podm.business.dto.redfish.attributes.MemoryRegionDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.properties.MemoryLocation;
import com.intel.podm.business.entities.redfish.properties.Region;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.DimmConfigService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MEMORY_MODULES;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class MemoryServiceImpl implements DimmConfigService {

    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getDimmConfigCollection(Context systemContext) throws EntityNotFoundException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(systemContext);
        return new CollectionDto(MEMORY_MODULES, Contexts.getAsIdList(computerSystem.getMemoryModules()));
    }

    @Override
    public MemoryDto getDimmConfig(Context dimmContext) throws EntityNotFoundException {
        Memory dimmConfig = (Memory) traverser.traverse(dimmContext);
        return map(dimmContext, dimmConfig);
    }

    private MemoryDto map(Context context, Memory dimmConfig) {
        return MemoryDto.newBuilder()
                .name(dimmConfig.getName())
                .id(dimmConfig.getId().toString())
                .description(dimmConfig.getDescription())
                .memoryType(dimmConfig.getMemoryType())
                .memoryDeviceType(dimmConfig.getMemoryDeviceType())
                .baseModuleType(dimmConfig.getBaseModuleType())
                .memoryMedia(dimmConfig.getMemoryMedia())
                .capacityMib(dimmConfig.getCapacityMib())
                .dataWidthBits(dimmConfig.getDataWidthBits())
                .busWidthBits(dimmConfig.getBusWidthBits())
                .manufacturer(dimmConfig.getManufacturer())
                .serialNumber(dimmConfig.getSerialNumber())
                .partNumber(dimmConfig.getPartNumber())
                .allowedSpeedsMhz(dimmConfig.getAllowedSpeedsMhz())
                .firmwareRevision(dimmConfig.getFirmwareRevision())
                .firmwareApiVersion(dimmConfig.getFirmwareApiVersion())
                .functionClasses(dimmConfig.getFunctionClasses())
                .vendorId(dimmConfig.getVendorId())
                .deviceId(dimmConfig.getDeviceId())
                .rankCount(dimmConfig.getRankCount())
                .deviceLocator(dimmConfig.getDeviceLocator())
                .memoryLocation(buildDimmLocationDto(dimmConfig.getMemoryLocation()))
                .errorCorrection(dimmConfig.getErrorCorrection())
                .status(dimmConfig.getStatus())
                .operatingSpeedMhz(dimmConfig.getOperatingSpeedMhz())
                .regions(processDimmRegions(dimmConfig.getRegions()))
                .operatingMemoryModes(dimmConfig.getOperatingMemoryModes())
                .voltageVolt(dimmConfig.getVoltageVolt())
                .context(context)
                .build();
    }

    private MemoryLocationDto buildDimmLocationDto(MemoryLocation dimmLocation) {
        if (dimmLocation == null) {
            return null;
        }

        return MemoryLocationDto.newBuilder()
                .locationChannel(dimmLocation.getChannel())
                .locationMemoryController(dimmLocation.getMemoryController())
                .locationSlot(dimmLocation.getSlot())
                .locationSocket(dimmLocation.getSocket())
                .build();
    }

    private Collection<MemoryRegionDto> processDimmRegions(Collection<Region> regions) {
        Collection<MemoryRegionDto> dimmRegions = new ArrayList<>();
        for (Region reg : regions) {
            dimmRegions.add(createDimmRegionDto(reg));
        }

        return dimmRegions;
    }

    private MemoryRegionDto createDimmRegionDto(Region region) {
        return MemoryRegionDto.newBuilder()
                .regionId(region.getRegionId())
                .memoryClassification(region.getMemoryClassification())
                .offsetMib(region.getOffsetMib())
                .sizeMib(region.getSizeMib())
                .build();
    }
}
