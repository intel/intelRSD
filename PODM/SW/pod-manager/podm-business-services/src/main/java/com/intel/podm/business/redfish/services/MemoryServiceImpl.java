package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.MemoryDto;
import com.intel.podm.business.dto.redfish.attributes.MemoryLocationDto;
import com.intel.podm.business.dto.redfish.attributes.MemoryRegionDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.embeddables.MemoryLocation;
import com.intel.podm.business.entities.redfish.embeddables.Region;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MEMORY_MODULES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodLength"})
public class MemoryServiceImpl implements ReaderService<MemoryDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context systemContext) throws ContextResolvingException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(systemContext);
        return new CollectionDto(MEMORY_MODULES, getAsIdSet(computerSystem.getMemoryModules()));
    }

    @Override
    public MemoryDto getResource(Context memoryContext) throws ContextResolvingException {
        Memory memory = (Memory) traverser.traverse(memoryContext);
        return map(memory);
    }

    private MemoryDto map(Memory memory) {
        return MemoryDto.newBuilder()
            .id(memory.getId().toString())
            .name(memory.getName())
            .description(memory.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(memory.getService(), memory.getUnknownOems()))
            .memoryType(memory.getMemoryType())
            .memoryDeviceType(memory.getMemoryDeviceType())
            .baseModuleType(memory.getBaseModuleType())
            .memoryMedia(memory.getMemoryMedia())
            .capacityMib(memory.getCapacityMib())
            .dataWidthBits(memory.getDataWidthBits())
            .busWidthBits(memory.getBusWidthBits())
            .manufacturer(memory.getManufacturer())
            .serialNumber(memory.getSerialNumber())
            .partNumber(memory.getPartNumber())
            .allowedSpeedsMhz(memory.getAllowedSpeedsMhz())
            .firmwareRevision(memory.getFirmwareRevision())
            .firmwareApiVersion(memory.getFirmwareApiVersion())
            .functionClasses(memory.getFunctionClasses())
            .vendorId(memory.getVendorId())
            .deviceId(memory.getDeviceId())
            .rankCount(memory.getRankCount())
            .deviceLocator(memory.getDeviceLocator())
            .memoryLocation(buildMemoryLocationDto(memory.getMemoryLocation()))
            .errorCorrection(memory.getErrorCorrection())
            .status(memory.getStatus())
            .operatingSpeedMhz(memory.getOperatingSpeedMhz())
            .regions(processMemoryRegions(memory.getRegions()))
            .operatingMemoryModes(memory.getOperatingMemoryModes())
            .voltageVolt(memory.getVoltageVolt())
            .build();
    }

    private MemoryLocationDto buildMemoryLocationDto(MemoryLocation memoryLocation) {
        if (memoryLocation == null) {
            return null;
        }

        return MemoryLocationDto.newBuilder()
            .locationChannel(memoryLocation.getChannel())
            .locationMemoryController(memoryLocation.getMemoryController())
            .locationSlot(memoryLocation.getSlot())
            .locationSocket(memoryLocation.getSocket())
            .build();
    }

    private Collection<MemoryRegionDto> processMemoryRegions(Collection<Region> regions) {
        Collection<MemoryRegionDto> memoryRegions = regions.stream()
            .map(this::createMemoryRegionDto)
            .collect(toList());
        return memoryRegions;
    }

    private MemoryRegionDto createMemoryRegionDto(Region region) {
        return MemoryRegionDto.newBuilder()
            .regionId(region.getRegionId())
            .memoryClassification(region.getMemoryClassification())
            .offsetMib(region.getOffsetMib())
            .sizeMib(region.getSizeMib())
            .build();
    }
}
