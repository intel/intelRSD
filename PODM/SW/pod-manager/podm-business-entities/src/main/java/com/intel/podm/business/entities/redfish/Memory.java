package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.MemoryModule;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.properties.MemoryLocation;
import com.intel.podm.business.entities.redfish.properties.Region;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.helpers.EnumeratedTypeListHolder;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.math.BigDecimal;
import java.net.URI;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.REGIONS;
import static com.intel.podm.business.entities.base.DomainObjectProperties.decimalProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumeratedTypeListProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.listProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.types.helpers.EnumeratedTypeListHolder.enumeratedTypeListHolder;
import static com.intel.podm.common.types.helpers.EnumeratedTypeListHolder.toList;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class Memory extends DomainObject implements Discoverable, StatusPossessor, Descriptable, MemoryModule {
    public static final DomainObjectProperty<MemoryType> MEMORY_TYPE = enumProperty("memoryType", MemoryType.class);
    public static final DomainObjectProperty<MemoryDeviceType> MEMORY_DEVICE_TYPE = enumProperty("memoryDeviceType", MemoryDeviceType.class);
    public static final DomainObjectProperty<BaseModuleType> BASE_MODULE_TYPE = enumProperty("baseModuleType", BaseModuleType.class);
    public static final DomainObjectProperty<EnumeratedTypeListHolder<MemoryMedia>> MEMORY_MEDIA
            = enumeratedTypeListProperty("memoryMedia", MemoryMedia.class);
    public static final DomainObjectProperty<Integer> CAPACITY_MIB = integerProperty("capacityMib");
    public static final DomainObjectProperty<Integer> DATA_WIDTH_BITS = integerProperty("dataWidthBits");
    public static final DomainObjectProperty<Integer> BUS_WIDTH_BITS = integerProperty("busWidthBits");
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> SERIAL_NUMBER = stringProperty("serialNumber");
    public static final DomainObjectProperty<String> PART_NUMBER = stringProperty("partNumber");
    public static final DomainObjectProperty<List<Integer>> ALLOWED_SPEEDS_MHZ = listProperty("allowedSpeedMHz");
    public static final DomainObjectProperty<String> FIRMWARE_REVISION = stringProperty("firmwareRevision");
    public static final DomainObjectProperty<String> FIRMWARE_API_VERSION = stringProperty("firmwareApiVersion");
    public static final DomainObjectProperty<List<String>> FUNCTION_CLASSES = listProperty("functionClasses");
    public static final DomainObjectProperty<String> VENDOR_ID = stringProperty("vendorId");
    public static final DomainObjectProperty<String> DEVICE_ID = stringProperty("deviceId");
    public static final DomainObjectProperty<Integer> RANK_COUNT = integerProperty("rankCount");
    public static final DomainObjectProperty<String> DEVICE_LOCATOR = stringProperty("deviceLocator");
    public static final DomainObjectProperty<ErrorCorrection> ERROR_CORRECTION = enumProperty("errorCorrection", ErrorCorrection.class);
    public static final DomainObjectProperty<Integer> OPERATING_SPEED_MHZ = integerProperty("operatingSpeedMHz");
    public static final DomainObjectProperty<EnumeratedTypeListHolder<OperatingMemoryMode>> OPERATING_MEMORY_MODES
            = enumeratedTypeListProperty("operatingMemoryModes", OperatingMemoryMode.class);
    public static final DomainObjectProperty<BigDecimal> VOLTAGE_VOLT = decimalProperty("voltageVolt");

    @Override
    public String getName() {
        return getProperty(NAME);
    }

    @Override
    public void setName(String name) {
        setProperty(NAME, name);
    }

    @Override
    public String getDescription() {
        return getProperty(DESCRIPTION);
    }

    @Override
    public void setDescription(String description) {
        setProperty(DESCRIPTION, description);
    }

    public Region addRegion() {
        return addDomainObject(REGIONS, Region.class);
    }

    public Collection<Region> getRegions() {
        return getLinked(REGIONS, Region.class);
    }

    public MemoryType getMemoryType() {
        return getProperty(MEMORY_TYPE);
    }

    public void setMemoryType(MemoryType memoryType) {
        setProperty(MEMORY_TYPE, memoryType);
    }

    @Override
    public MemoryDeviceType getMemoryDeviceType() {
        return getProperty(MEMORY_DEVICE_TYPE);
    }

    public void setMemoryDeviceType(MemoryDeviceType memoryDeviceType) {
        setProperty(MEMORY_DEVICE_TYPE, memoryDeviceType);
    }

    public BaseModuleType getBaseModuleType() {
        return getProperty(BASE_MODULE_TYPE);
    }

    public void setBaseModuleType(BaseModuleType baseModuleType) {
        setProperty(BASE_MODULE_TYPE, baseModuleType);
    }

    public List<MemoryMedia> getMemoryMedia() {
        return toList(MemoryMedia.class, getProperty(MEMORY_MEDIA));
    }

    public void setMemoryMedia(List<MemoryMedia> memoryMedia) {
        setProperty(MEMORY_MEDIA, enumeratedTypeListHolder(MemoryMedia.class, memoryMedia));
    }

    @Override
    public Integer getCapacityMib() {
        return getProperty(CAPACITY_MIB);
    }

    public void setCapacityMib(Integer capacityMib) {
        setProperty(CAPACITY_MIB, capacityMib);
    }

    @Override
    public Integer getDataWidthBits() {
        return getProperty(DATA_WIDTH_BITS);
    }

    public void setDataWidthBits(Integer dataWidthBits) {
        setProperty(DATA_WIDTH_BITS, dataWidthBits);
    }

    public Integer getBusWidthBits() {
        return getProperty(BUS_WIDTH_BITS);
    }

    public void setBusWidthBits(Integer capacityMib) {
        setProperty(BUS_WIDTH_BITS, capacityMib);
    }

    @Override
    public String getManufacturer() {
        return getProperty(MANUFACTURER);
    }

    public void setManufacturer(String manufacturer) {
        setProperty(MANUFACTURER, manufacturer);
    }

    public String getSerialNumber() {
        return getProperty(SERIAL_NUMBER);
    }

    public void setSerialNumber(String serialNumber) {
        setProperty(SERIAL_NUMBER, serialNumber);
    }

    public String getPartNumber() {
        return getProperty(PART_NUMBER);
    }

    public void setPartNumber(String partNumber) {
        setProperty(PART_NUMBER, partNumber);
    }

    public List<Integer> getAllowedSpeedsMhz() {
        return getProperty(ALLOWED_SPEEDS_MHZ);
    }

    public void setAllowedSpeedsMhz(List<Integer> allowedSpeeds) {
        setProperty(ALLOWED_SPEEDS_MHZ, allowedSpeeds);
    }

    public String getFirmwareRevision() {
        return getProperty(FIRMWARE_REVISION);
    }

    public void setFirmwareRevision(String firmwareRevision) {
        setProperty(FIRMWARE_REVISION, firmwareRevision);
    }

    public String getFirmwareApiVersion() {
        return getProperty(FIRMWARE_API_VERSION);
    }

    public void setFirmwareApiVersion(String firmwareApiVersion) {
        setProperty(FIRMWARE_API_VERSION, firmwareApiVersion);
    }

    public List<String> getFunctionClasses() {
        return getProperty(FUNCTION_CLASSES);
    }

    public void setFunctionClasses(List<String> functionClasses) {
        setProperty(FUNCTION_CLASSES, functionClasses);
    }

    public String getVendorId() {
        return getProperty(VENDOR_ID);
    }

    public void setVendorId(String vendorId) {
        setProperty(VENDOR_ID, vendorId);
    }

    public String getDeviceId() {
        return getProperty(DEVICE_ID);
    }

    public void setDeviceId(String deviceId) {
        setProperty(DEVICE_ID, deviceId);
    }

    public Integer getRankCount() {
        return getProperty(RANK_COUNT);
    }

    public void setRankCount(Integer rankCount) {
        setProperty(RANK_COUNT, rankCount);
    }

    public String getDeviceLocator() {
        return getProperty(DEVICE_LOCATOR);
    }

    public void setDeviceLocator(String deviceLocator) {
        setProperty(DEVICE_LOCATOR, deviceLocator);
    }

    public ErrorCorrection getErrorCorrection() {
        return getProperty(ERROR_CORRECTION);
    }

    public void setErrorCorrection(ErrorCorrection errorCorrection) {
        setProperty(ERROR_CORRECTION, errorCorrection);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    @Override
    public Integer getOperatingSpeedMhz() {
        return getProperty(OPERATING_SPEED_MHZ);
    }

    public void setOperatingSpeedMhz(Integer operatingSpeedMhz) {
        setProperty(OPERATING_SPEED_MHZ, operatingSpeedMhz);
    }

    public List<OperatingMemoryMode> getOperatingMemoryModes() {
        return toList(OperatingMemoryMode.class, getProperty(OPERATING_MEMORY_MODES));
    }

    public void setOperatingMemoryModes(List<OperatingMemoryMode> operatingMemoryModes) {
        setProperty(OPERATING_MEMORY_MODES, enumeratedTypeListHolder(OperatingMemoryMode.class, operatingMemoryModes));
    }

    public BigDecimal getVoltageVolt() {
        return getProperty(VOLTAGE_VOLT);
    }

    public void setVoltageVolt(BigDecimal voltageVolt) {
        setProperty(VOLTAGE_VOLT, voltageVolt);
    }

    public ComputerSystem getComputerSystem() {
        return single(getLinked(CONTAINED_BY, ComputerSystem.class));
    }

    @Override
    public URI getSourceUri() {
        return getProperty(SOURCE_URI);
    }

    @Override
    public void setSourceUri(URI sourceUri) {
        setProperty(SOURCE_URI, sourceUri);
    }

    @Override
    public ExternalService getService() {
        return singleOrNull(getLinked(OWNED_BY, ExternalService.class));
    }

    public void setMemoryLocation(MemoryLocation memoryLocation) {
        link(CONTAINS, memoryLocation);
    }

    public MemoryLocation getMemoryLocation() {
        return singleOrNull(getLinked(CONTAINS, MemoryLocation.class));
    }
}
