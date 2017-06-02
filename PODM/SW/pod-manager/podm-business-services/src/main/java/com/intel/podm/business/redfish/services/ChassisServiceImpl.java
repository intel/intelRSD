/*
 * Copyright (c) 2016-2017 Intel Corporation
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
import com.intel.podm.business.dto.redfish.ChassisDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.PowerControlDto;
import com.intel.podm.business.dto.redfish.PowerControlDto.PowerLimitDto;
import com.intel.podm.business.dto.redfish.PowerControlDto.PowerMetricsDto;
import com.intel.podm.business.dto.redfish.PowerDto;
import com.intel.podm.business.dto.redfish.PowerSupplyDto;
import com.intel.podm.business.dto.redfish.PowerVoltageDto;
import com.intel.podm.business.dto.redfish.RedundancyDto;
import com.intel.podm.business.dto.redfish.ThermalDto;
import com.intel.podm.business.dto.redfish.attributes.ThermalFanDto;
import com.intel.podm.business.dto.redfish.attributes.ThermalTemperatureDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.Power;
import com.intel.podm.business.entities.redfish.PowerControl;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerVoltage;
import com.intel.podm.business.entities.redfish.Redundancy;
import com.intel.podm.business.entities.redfish.Thermal;
import com.intel.podm.business.entities.redfish.ThermalFan;
import com.intel.podm.business.entities.redfish.ThermalTemperature;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.PowerLimit;
import com.intel.podm.business.entities.redfish.embeddables.PowerMetrics;
import com.intel.podm.business.entities.redfish.embeddables.RackChassisAttributes;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.InputRangeDtoTranslateHelper;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.CHASSIS;
import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asComputerSystemContexts;
import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetSwitchContexts;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.asStorageContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.Comparator.comparingLong;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodLength", "checkstyle:MethodCount"})
public class ChassisServiceImpl implements ReaderService<ChassisDto> {
    @Inject
    private GenericDao genericDao;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Inject
    private InputRangeDtoTranslateHelper inputRangeDtoTranslateHelper;

    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        Collection<Chassis> chassisCollection = genericDao.findAll(Chassis.class);
        return new CollectionDto(CHASSIS, getAsIdSet(chassisCollection));
    }

    @Override
    public ChassisDto getResource(Context chassisContext) throws ContextResolvingException {
        Chassis chassis = (Chassis) traverser.traverse(chassisContext);
        return map(chassis);
    }

    private ChassisDto map(Chassis chassis) {
        RackChassisAttributes rackChassisAttributes = chassis.getRackChassisAttributes();

        ChassisDto.Builder builder = ChassisDto
            .newBuilder()
            .id(chassis.getId().toString())
            .name(chassis.getName())
            .description(chassis.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(chassis.getService(), chassis.getUnknownOems()))
            .chassisType(chassis.getChassisType())
            .manufacturer(chassis.getManufacturer())
            .model(chassis.getModel())
            .sku(chassis.getSku())
            .serialNumber(chassis.getSerialNumber())
            .partNumber(chassis.getPartNumber())
            .assetTag(chassis.getAssetTag())
            .indicatorLed(chassis.getIndicatorLed())
            .locationId(chassis.getLocationId())
            .locationParentId(getLocationParentId(chassis))
            .status(chassis.getStatus())
            .thermal(toThermalDto(chassis.getThermal()))
            .power(toPowerDto(chassis.getPower()))
            .contains(asChassisContexts(chassis.getContainedChassis()))
            .switches(asEthernetSwitchContexts(chassis.getEthernetSwitches()))
            .containedBy(toContext(chassis.getContainedByChassis()))
            .computerSystems(asComputerSystemContexts(chassis.getComputerSystems()))
            .managedBy(asManagerContexts(chassis.getManagers()))
            .managersInChassis(asManagerContexts(chassis.getInChassisManagers()))
            .storage(asStorageContexts(chassis.getComputerSystems().stream()
                .map(ComputerSystem::getStorages)
                .flatMap(Collection::stream)
                .collect(toSet())))
            .drives(asDriveContexts(chassis.getDrives()))
            .rmmPresent(isRmmPresent(chassis));

        addChassisAttributes(rackChassisAttributes, builder);
        return builder.build();
    }

    private String getLocationParentId(Chassis chassis) {
        Chassis parentChassis = chassis.getContainedByChassis();
        if (chassis.getLocationParentId() == null && parentChassis != null) {
            return parentChassis.getLocationId();
        }
        return chassis.getLocationParentId();
    }

    private ThermalDto toThermalDto(Thermal thermal) {
        if (thermal == null) {
            return null;
        }

        return ThermalDto.newBuilder()
            .id(thermal.getId().toString())
            .name(thermal.getName())
            .description(thermal.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(thermal.getService(), thermal.getUnknownOems()))
            .temperatures(thermal.getTemperatures().stream().sorted(comparingLong(Entity::getPrimaryKey))
                .map(this::toTemperature).collect(toList()))
            .fans(thermal.getFans().stream().sorted(comparingLong(Entity::getPrimaryKey))
                .map(this::toFan).collect(toList()))
            .redundancies(thermal.getOwnedRedundancies().stream().sorted(comparingLong(Entity::getPrimaryKey))
                .map(this::toRedundancy).collect(toList()))
            .build();
    }

    private PowerDto toPowerDto(Power power) {
        if (power == null) {
            return null;
        }

        return PowerDto.newBuilder()
            .id(power.getId().toString())
            .name(power.getName())
            .description(power.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(power.getService(), power.getUnknownOems()))
            .powerControls(power.getPowerControls().stream().map(this::toPowerControl).collect(toList()))
            .voltages(power.getVoltages().stream().sorted(comparingLong(Entity::getPrimaryKey))
                .map(this::toVoltage).collect(toList()))
            .powerSupplies(power.getPowerSupplies().stream().sorted(comparingLong(Entity::getPrimaryKey))
                .map(this::toPowerSupply).collect(toList()))
            .redundancies(power.getOwnedRedundancies().stream().sorted(comparingLong(Entity::getPrimaryKey))
                .map(this::toRedundancy).collect(toList()))
            .build();
    }

    private PowerVoltageDto toVoltage(PowerVoltage voltage) {
        return PowerVoltageDto.newPowerVoltageDto()
            .context(toContext(voltage))
            .memberId(voltage.getMemberId())
            .name(voltage.getName())
            .sensorNumber(voltage.getSensorNumber())
            .status(voltage.getStatus())
            .readingVolts(voltage.getReadingVolts())
            .upperThresholdNonCritical(voltage.getUpperThresholdNonCritical())
            .upperThresholdCritical(voltage.getUpperThresholdCritical())
            .upperThresholdFatal(voltage.getUpperThresholdFatal())
            .lowerThresholdNonCritical(voltage.getLowerThresholdNonCritical())
            .lowerThresholdCritical(voltage.getLowerThresholdCritical())
            .lowerThresholdFatal(voltage.getLowerThresholdFatal())
            .minReadingRange(voltage.getMinReadingRange())
            .maxReadingRange(voltage.getMaxReadingRange())
            .physicalContext(voltage.getPhysicalContext())
            .relatedItem(voltage.getRelatedItems().stream().map(Contexts::toContext).collect(toList()))
            .build();
    }

    private PowerSupplyDto toPowerSupply(PowerSupply powerSupply) {
        return PowerSupplyDto.newPowerSupplyDto()
            .context(toContext(powerSupply))
            .memberId(powerSupply.getMemberId())
            .name(powerSupply.getName())
            .status(powerSupply.getStatus())
            .powerSupplyType(powerSupply.getPowerSupplyType())
            .lineInputVoltageType(powerSupply.getLineInputVoltageType())
            .lineInputVoltage(powerSupply.getLineInputVoltage())
            .powerCapacityWatts(powerSupply.getPowerCapacityWatts())
            .lastPowerOutputWatts(powerSupply.getLastPowerOutputWatts())
            .model(powerSupply.getModel())
            .manufacturer(powerSupply.getManufacturer())
            .firmwareVersion(powerSupply.getFirmwareVersion())
            .serialNumber(powerSupply.getSerialNumber())
            .partNumber(powerSupply.getPartNumber())
            .sparePartNumber(powerSupply.getSparePartNumber())
            .inputRanges(inputRangeDtoTranslateHelper.translateInputRangesToDto(powerSupply.getService(), powerSupply.getInputRanges()))
            .indicatorLed(powerSupply.getIndicatorLed())
            .redundancy(powerSupply.getRedundancies().stream().map(Contexts::toContext).collect(toList()))
            .relatedItem(powerSupply.getRelatedItems().stream().map(Contexts::toContext).collect(toList()))
            .oem(unknownOemTranslator.translateStringOemToDto(powerSupply.getService(), powerSupply.getOem()))
            .build();
    }

    private PowerControlDto toPowerControl(PowerControl powerControl) {
        return PowerControlDto.newPowerControlDto()
            .context(toContext(powerControl))
            .memberId(powerControl.getMemberId())
            .name(powerControl.getName())
            .status(powerControl.getStatus())
            .relatedItems(powerControl.getRelatedItems().stream().map(Contexts::toContext).collect(toList()))
            .powerAllocatedWatts(powerControl.getPowerAllocatedWatts())
            .powerAvailableWatts(powerControl.getPowerAvailableWatts())
            .powerConsumedWatts(powerControl.getPowerConsumedWatts())
            .powerCapacityWatts(powerControl.getPowerCapacityWatts())
            .powerRequestedWatts(powerControl.getPowerRequestedWatts())
            .oem(unknownOemTranslator.translateStringOemToDto(powerControl.getService(), powerControl.getOem()))
            .powerLimit(toPowerLimit(powerControl.getPowerLimit()))
            .powerMetrics(toPowerMetrics(powerControl.getPowerMetrics()))
            .build();
    }

    private PowerMetricsDto toPowerMetrics(PowerMetrics powerMetrics) {
        if (powerMetrics == null) {
            return null;
        }

        return PowerControlDto.newPowerMetricsDto()
            .averageConsumedWatts(powerMetrics.getAverageConsumedWatts())
            .intervalInMin(powerMetrics.getIntervalInMin())
            .maxConsumedWatts(powerMetrics.getMaxConsumedWatts())
            .minConsumedWatts(powerMetrics.getMinConsumedWatts())
            .build();
    }

    private PowerLimitDto toPowerLimit(PowerLimit powerLimit) {
        if (powerLimit == null) {
            return null;
        }

        return PowerControlDto.newPowerLimitDto()
            .correctionInMs(powerLimit.getCorrectionInMs())
            .limitException(powerLimit.getLimitException())
            .limitInWatts(powerLimit.getLimitInWatts())
            .build();
    }

    private RedundancyDto toRedundancy(Redundancy redundancy) {
        return RedundancyDto.newRedundancyDto()
            .context(toContext(redundancy))
            .memberId(redundancy.getMemberId())
            .name(redundancy.getName())
            .status(redundancy.getStatus())
            .maxNumSupported(redundancy.getMaxNumSupported())
            .minNumNeeded(redundancy.getMinNumNeeded())
            .mode(redundancy.getMode())
            .redundancyEnabled(redundancy.getRedundancyEnabled())
            .redundancySet(redundancy.getRedundancyMembers().stream().map(Contexts::toContext).collect(toSet()))
            .build();
    }

    private ThermalTemperatureDto toTemperature(ThermalTemperature temperature) {
        return ThermalTemperatureDto.newThermalTemperatureDto()
            .context(toContext(temperature))
            .lowerThresholdCritical(temperature.getLowerThresholdCritical())
            .lowerThresholdFatal(temperature.getLowerThresholdFatal())
            .lowerThresholdNonCritical(temperature.getLowerThresholdNonCritical())
            .upperThresholdCritical(temperature.getUpperThresholdCritical())
            .upperThresholdFatal(temperature.getUpperThresholdFatal())
            .upperThresholdNonCritical(temperature.getUpperThresholdNonCritical())
            .maxReadingRange(temperature.getMaxReadingRange())
            .memberId(temperature.getMemberId())
            .minReadingRange(temperature.getMinReadingRange())
            .name(temperature.getName())
            .physicalContext(temperature.getPhysicalContext())
            .readingCelsius(temperature.getReadingCelsius())
            .sensorNumber(temperature.getSensorNumber())
            .status(temperature.getStatus())
            .relatedItem(temperature.getRelatedItems().stream().map(Contexts::toContext).collect(toSet()))
            .build();
    }

    private ThermalFanDto toFan(ThermalFan fan) {
        return ThermalFanDto.newThermalFanDto()
            .context(toContext(fan))
            .lowerThresholdCritical(fan.getLowerThresholdCritical())
            .lowerThresholdFatal(fan.getLowerThresholdFatal())
            .lowerThresholdNonCritical(fan.getLowerThresholdNonCritical())
            .upperThresholdNonCritical(fan.getUpperThresholdNonCritical())
            .upperThresholdFatal(fan.getUpperThresholdFatal())
            .upperThresholdCritical(fan.getUpperThresholdCritical())
            .maxReadingRange(fan.getMaxReadingRange())
            .memberId(fan.getMemberId())
            .minReadingRange(fan.getMinReadingRange())
            .name(fan.getName())
            .physicalContext(fan.getPhysicalContext())
            .reading(fan.getReading())
            .readingUnits(fan.getReadingUnits())
            .status(fan.getStatus())
            .maxReadingRange(fan.getMaxReadingRange())
            .redundancy(fan.getRedundancies().stream().map(Contexts::toContext).collect(toSet()))
            .relatedItem(fan.getRelatedItems().stream().map(Contexts::toContext).collect(toSet()))
            .build();
    }

    private void addChassisAttributes(RackChassisAttributes rackChassisAttributes, ChassisDto.Builder builder) {
        if (rackChassisAttributes != null) {
            builder
                .uuid(rackChassisAttributes.getUuid())
                .rackSupportsDisaggregatedPowerCooling(rackChassisAttributes.getRackSupportsDisaggregatedPowerCooling())
                .geoTag(rackChassisAttributes.getGeoTag());
        }
    }

    private boolean isRmmPresent(Chassis chassis) {
        return chassis
            .getManagers()
            .stream()
            .anyMatch(Manager::isPresent);
    }
}
