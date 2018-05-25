/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.services.context;

import com.intel.podm.business.services.context.UriToContextConverter.UriPatternDetail;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM_METRICS;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static com.intel.podm.business.services.context.ContextType.DRIVE_METRICS;
import static com.intel.podm.business.services.context.ContextType.ENDPOINT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_ACL;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_ACL_RULE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_METRICS;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_METRICS;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_STATIC_MAC;
import static com.intel.podm.business.services.context.ContextType.EVENT_SERVICE;
import static com.intel.podm.business.services.context.ContextType.EVENT_SUBSCRIPTION;
import static com.intel.podm.business.services.context.ContextType.FABRIC;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.MEMORY_METRICS;
import static com.intel.podm.business.services.context.ContextType.METRIC_DEFINITION;
import static com.intel.podm.business.services.context.ContextType.METRIC_REPORT_DEFINITION;
import static com.intel.podm.business.services.context.ContextType.NETWORK_DEVICE_FUNCTION;
import static com.intel.podm.business.services.context.ContextType.NETWORK_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.NETWORK_PROTOCOL;
import static com.intel.podm.business.services.context.ContextType.PCIE_DEVICE;
import static com.intel.podm.business.services.context.ContextType.PCIE_DEVICE_FUNCTION;
import static com.intel.podm.business.services.context.ContextType.PORT;
import static com.intel.podm.business.services.context.ContextType.PORT_METRICS;
import static com.intel.podm.business.services.context.ContextType.POWER;
import static com.intel.podm.business.services.context.ContextType.POWER_CONTROL;
import static com.intel.podm.business.services.context.ContextType.POWER_SUPPLY;
import static com.intel.podm.business.services.context.ContextType.POWER_VOLTAGE;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR_METRICS;
import static com.intel.podm.business.services.context.ContextType.REDUNDANCY;
import static com.intel.podm.business.services.context.ContextType.SIMPLE_STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE_POOL;
import static com.intel.podm.business.services.context.ContextType.STORAGE_SERVICE;
import static com.intel.podm.business.services.context.ContextType.SWITCH;
import static com.intel.podm.business.services.context.ContextType.TELEMETRY_SERVICE;
import static com.intel.podm.business.services.context.ContextType.THERMAL;
import static com.intel.podm.business.services.context.ContextType.THERMAL_FAN;
import static com.intel.podm.business.services.context.ContextType.THERMAL_TEMPERATURE;
import static com.intel.podm.business.services.context.ContextType.VOLUME;
import static com.intel.podm.business.services.context.ContextType.VOLUME_METRICS;
import static com.intel.podm.business.services.context.ContextType.ZONE;
import static com.intel.podm.business.services.context.UriToContextConverter.CONTEXT_TYPE_TO_PATTERN;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUriWithPossibleParent;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.utils.FailManager.failWithMessageIfAnyError;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MethodLength", "checkstyle:MethodName"})
public class UriToContextConverterTest {
    @Test(dataProvider = "validUrisWithValidContexts")
    public void whenConvertingValidUriToContext_shouldReturnCorrectContext(URI uriToParse, Context expectedContext) {
        Context context = getContextFromUri(uriToParse);
        assertEquals(expectedContext, context);
    }

    @Test(dataProvider = "validUrisWithValidContexts")
    public void whenConvertingValidUriAndGivenContextTypeToContext_shouldReturnCorrectContext(URI uriToParse, Context expectedContext) {
        Context context = getContextFromUri(uriToParse, expectedContext.getType());
        assertEquals(expectedContext, context);
    }

    @Test(dependsOnMethods = {
        "whenConvertingValidUriToContext_shouldReturnCorrectContext",
        "whenConvertingValidUriAndGivenContextTypeToContext_shouldReturnCorrectContext"})
    public void checkWhetherAllUrisAreTested() {
        List<String> errors = new ArrayList<>();
        Map<ContextType, Integer> testedContextTypes = new HashMap<>();

        for (Object[] objects : validUrisWithValidContexts()) {
            Context context = (Context) objects[1];
            Integer numberOfContextTypes = testedContextTypes.getOrDefault(context.getType(), 0);
            testedContextTypes.put(context.getType(), ++numberOfContextTypes);
        }

        for (Map.Entry<ContextType, List<UriPatternDetail>> contextTypesPatterns : CONTEXT_TYPE_TO_PATTERN.entrySet()) {
            ContextType contextType = contextTypesPatterns.getKey();
            int numberOfUriPatterns = contextTypesPatterns.getValue().size();

            int numberOfTestedUris = testedContextTypes.getOrDefault(contextType, 0);

            if (numberOfTestedUris != numberOfUriPatterns) {
                errors.add(contextType + " has " + numberOfUriPatterns + " URI pattern(s), " + numberOfTestedUris + " was tested.");
            }
        }

        failWithMessageIfAnyError(errors, "URI pattern(s). Please, adjust tests!");
    }

    @Test(dataProvider = "invalidUris", expectedExceptions = UriConversionException.class)
    public void whenConvertingInvalidUriToContext_shouldThrowUriConversionException(URI invalidUriToParse) {
        getContextFromUri(invalidUriToParse);
    }

    @Test(dataProvider = "validUrisWithInvalidContexts", expectedExceptions = UriConversionException.class)
    public void whenConvertingValidUriButInvalidContextTypeToContext_shouldThrowUriConversionException(URI uriToParse, Context expectedContext) {
        getContextFromUri(uriToParse, expectedContext.getType());
    }

    @Test(dataProvider = "validUrisWithValidContextsAndExcludedIncludedStrings")
    public void whenConvertingValidUriGivenContextTypeAndExcludedIncludedStringsToContext_shouldReturnCorrectContext(
        URI uriToParse, Context expectedContext, ContextType requiredParent) {

        Context context = getContextFromUriWithPossibleParent(uriToParse, requiredParent, expectedContext.getType());
        assertEquals(expectedContext, context);
    }

    @DataProvider
    public Object[][] validUrisWithValidContexts() {
        return new Object[][]{
            {
                URI.create("/redfish/v1/Managers/1-m-10"),
                contextOf(id("1-m-10"), MANAGER)
            },
            {
                URI.create("/redfish/v1/StorageServices/4-sv-1"),
                contextOf(id("4-sv-1"), STORAGE_SERVICE)
            },
            {
                URI.create("/redfish/v1/Nodes/3"),
                contextOf(id("3"), COMPOSED_NODE)
            },
            {
                URI.create("/redfish/v1/StorageServices/4-sv-1/Volumes/4-sv-1-vl-10"),
                contextOf(id("4-sv-1"), STORAGE_SERVICE).child(id("4-sv-1-vl-10"), VOLUME)
            },
            {
                URI.create("/redfish/v1/StorageServices/4-sv-1/Volumes/4-sv-1-vl-10/Metrics"),
                contextOf(id("4-sv-1"), STORAGE_SERVICE).child(id("4-sv-1-vl-10"), VOLUME).child(id(""), VOLUME_METRICS)
            },
            {
                URI.create("/redfish/v1/StorageServices/4-sv-1/StoragePools/4-sv-1-t-10"),
                contextOf(id("4-sv-1"), STORAGE_SERVICE).child(id("4-sv-1-t-10"), STORAGE_POOL)
            },
            {
                URI.create("/redfish/v1/Chassis/1-c-10"),
                contextOf(id("1-c-10"), CHASSIS)
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Thermal"),
                contextOf(id("3-c-drawer1"), CHASSIS).child(id(""), THERMAL)
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power"),
                contextOf(id("3-c-drawer1"), CHASSIS).child(id(""), POWER)
            },
            {
                URI.create("/redfish/v1/Chassis/5-c-rack1/Thermal#/Redundancy/0"),
                contextOf(id("5-c-rack1"), CHASSIS).child(id(""), THERMAL).child(id("0"), REDUNDANCY)
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power#/Redundancy/0"),
                contextOf(id("3-c-drawer1"), CHASSIS).child(id(""), POWER).child(id("0"), REDUNDANCY)
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power#/Voltages/0"),
                contextOf(id("3-c-drawer1"), CHASSIS).child(id(""), POWER).child(id("0"), POWER_VOLTAGE)
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power#/PowerControl/0"),
                contextOf(id("3-c-drawer1"), CHASSIS).child(id(""), POWER).child(id("0"), POWER_CONTROL)
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power#/PowerSupplies/0"),
                contextOf(id("3-c-drawer1"), CHASSIS).child(id(""), POWER).child(id("0"), POWER_SUPPLY)
            },
            {
                URI.create("/redfish/v1/Chassis/5-c-rack1/Thermal#/Fans/1"),
                contextOf(id("5-c-rack1"), CHASSIS).child(id(""), THERMAL).child(id("1"), THERMAL_FAN)
            },
            {
                URI.create("/redfish/v1/Chassis/5-c-rack1/Thermal#/Temperatures/1"),
                contextOf(id("5-c-rack1"), CHASSIS).child(id(""), THERMAL).child(id("1"), THERMAL_TEMPERATURE)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM)
            },
            {
                URI.create("/redfish/v1/Systems/3-s-1/SimpleStorage/3-s-1-ss-1"),
                contextOf(id("3-s-1"), COMPUTER_SYSTEM).child(id("3-s-1-ss-1"), SIMPLE_STORAGE)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Storage/1-s-1-sr-1"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-sr-1"), STORAGE)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Memory/1-s-1-mm-1"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-mm-1"), MEMORY)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Memory/1-s-1-mm-1/Metrics"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM)
                    .child(id("1-s-1-mm-1"), MEMORY)
                    .child(id(""), MEMORY_METRICS)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Processors/1-s-1-p-2"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-p-2"), PROCESSOR)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/EthernetInterfaces/1-s-1-ei-1"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-ei-1"), ETHERNET_INTERFACE)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Metrics"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id(""), COMPUTER_SYSTEM_METRICS)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Processors/1-s-1-p-1/Metrics"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-p-1"), PROCESSOR).child(id(""), PROCESSOR_METRICS)
            },
            {
                URI.create("/redfish/v1/Managers/1-m-10/EthernetInterfaces/1-m-10-ei-1"),
                contextOf(id("1-m-10"), MANAGER).child(id("1-m-10-ei-1"), ETHERNET_INTERFACE)
            },
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH)
            },
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1/Metrics"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH).child(id(""), ETHERNET_SWITCH_METRICS)
            },
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1/ACLs/1-es-1-acl-1"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH).child(id("1-es-1-acl-1"), ETHERNET_SWITCH_ACL)
            },
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1/ACLs/1-es-1-acl-1/Rules/1-es-1-acl-1-rl-1"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH)
                    .child(id("1-es-1-acl-1"), ETHERNET_SWITCH_ACL)
                    .child(id("1-es-1-acl-1-rl-1"), ETHERNET_SWITCH_ACL_RULE)
            },
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1/Ports/1-es-1-pt-1"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH).child(id("1-es-1-pt-1"), ETHERNET_SWITCH_PORT)
            },
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1/Ports/1-es-1-pt-1/Metrics"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH).child(id("1-es-1-pt-1"), ETHERNET_SWITCH_PORT).child(id(""), ETHERNET_SWITCH_PORT_METRICS)
            },
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1/Ports/1-es-1-pt-1/VLANs/1-es-1-pt-1-vlan-1"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH).child(id("1-es-1-pt-1"), ETHERNET_SWITCH_PORT)
                    .child(id("1-es-1-pt-1-vlan-1"), ETHERNET_SWITCH_PORT_VLAN)
            },
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1/Ports/1-es-1-pt-1/StaticMACs/1-es-1-pt-1-smac-1"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH).child(id("1-es-1-pt-1"), ETHERNET_SWITCH_PORT)
                    .child(id("1-es-1-pt-1-smac-1"), ETHERNET_SWITCH_STATIC_MAC)
            },
            {
                URI.create("/redfish/v1/Managers/3-m-rmc/EthernetInterfaces/3-m-rmc-ei-1/VLANs/3-m-rmc-ei-1-vlan-1"),
                contextOf(id("3-m-rmc"), MANAGER).child(id("3-m-rmc-ei-1"), ETHERNET_INTERFACE).child(id("3-m-rmc-ei-1-vlan-1"), ETHERNET_SWITCH_PORT_VLAN)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/EthernetInterfaces/1-s-1-ei-1/VLANs/1-s-1-ei-1-vlan-1"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-ei-1"), ETHERNET_INTERFACE).child(id("1-s-1-ei-1-vlan-1"), ETHERNET_SWITCH_PORT_VLAN)
            },
            {
                URI.create("/redfish/v1/Managers/1-m-10/NetworkProtocol"),
                contextOf(id("1-m-10"), MANAGER).child(id(""), NETWORK_PROTOCOL)
            },
            {
                URI.create("/redfish/v1/Chassis/1-c-10/Drives/1-c-10-d-1"),
                contextOf(id("1-c-10"), CHASSIS).child(id("1-c-10-d-1"), DRIVE)
            },
            {
                URI.create("/redfish/v1/Chassis/1-c-10/Drives/1-c-10-d-1/Metrics"),
                contextOf(id("1-c-10"), CHASSIS).child(id("1-c-10-d-1"), DRIVE).child(id(""), DRIVE_METRICS)
            },
            {
                URI.create("/redfish/v1/Chassis/5-c-1/PCIeDevices/5-c-1-dv-1"),
                contextOf(id("5-c-1"), CHASSIS).child(id("5-c-1-dv-1"), PCIE_DEVICE)
            },
            {
                URI.create("/redfish/v1/Chassis/5-c-1/PCIeDevices/5-c-1-dv-1/Functions/5-c-1-dv-1-fn-1"),
                contextOf(id("5-c-1"), CHASSIS).child(id("5-c-1-dv-1"), PCIE_DEVICE).child(id("5-c-1-dv-1-fn-1"), PCIE_DEVICE_FUNCTION)
            },
            {
                URI.create("/redfish/v1/Fabrics/5-ff-1"),
                contextOf(id("5-ff-1"), FABRIC)
            },
            {
                URI.create("/redfish/v1/Fabrics/5-ff-1/Zones/5-ff-1-z-1"),
                contextOf(id("5-ff-1"), FABRIC).child(id("5-ff-1-z-1"), ZONE)
            },
            {
                URI.create("/redfish/v1/Fabrics/5-ff-1/Switches/5-ff-1-fs-1"),
                contextOf(id("5-ff-1"), FABRIC).child(id("5-ff-1-fs-1"), SWITCH)
            },
            {
                URI.create("/redfish/v1/Fabrics/5-ff-1/Switches/5-ff-1-fs-1/Ports/5-ff-1-fs-1-pt-1"),
                contextOf(id("5-ff-1"), FABRIC).child(id("5-ff-1-fs-1"), SWITCH).child(id("5-ff-1-fs-1-pt-1"), PORT)
            },
            {
                URI.create("/redfish/v1/Fabrics/5-ff-1/Switches/5-ff-1-fs-1/Ports/5-ff-1-fs-1-pt-1/Metrics"),
                contextOf(id("5-ff-1"), FABRIC).child(id("5-ff-1-fs-1"), SWITCH).child(id("5-ff-1-fs-1-pt-1"), PORT).child(id(""), PORT_METRICS)
            },
            {
                URI.create("/redfish/v1/Fabrics/5-ff-1/Endpoints/5-ff-1-e-1"),
                contextOf(id("5-ff-1"), FABRIC).child(id("5-ff-1-e-1"), ENDPOINT)
            },
            {
                URI.create("/redfish/v1/EventService"),
                contextOf(id(""), EVENT_SERVICE)
            },
            {
                URI.create("/redfish/v1/EventService/Subscriptions/1"),
                contextOf(id(""), EVENT_SERVICE).child(id("1"), EVENT_SUBSCRIPTION)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/NetworkInterfaces/1-s-1-ni-1"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-ni-1"), NETWORK_INTERFACE)
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/NetworkInterfaces/1-s-1-ni-1/NetworkDeviceFunctions/1-s-1-ni-1-ndf-1"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-ni-1"), NETWORK_INTERFACE).child(id("1-s-1-ni-1-ndf-1"), NETWORK_DEVICE_FUNCTION)
            },
            {
                URI.create("/redfish/v1/TelemetryService"),
                contextOf(id(""), TELEMETRY_SERVICE)
            },
            {
                URI.create("/redfish/v1/TelemetryService/MetricDefinitions/1"),
                contextOf(id(""), TELEMETRY_SERVICE).child(id("1"), METRIC_DEFINITION)
            },
            {
                URI.create("/redfish/v1/TelemetryService/MetricReportDefinitions/1"),
                contextOf(id(""), TELEMETRY_SERVICE).child(id("1"), METRIC_REPORT_DEFINITION)
            }
        };
    }

    @DataProvider
    public Object[][] invalidUris() {
        return new Object[][]{
            {
                URI.create("/redfish/v1/Chassis")
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Thermal/3-c-drawer1-th-23")
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power/3-c-drawer1-po-23")
            },
            {
                URI.create("/redfish/v1/Chassis/5-c-rack1/Thermal/Redundancy/0")
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power/Redundancy/0")
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power/Voltages/0")
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power/PowerControl/0")
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power/PowerSupplies/0")
            },
            {
                URI.create("/redfish/v1/Chassis/5-c-rack1/Thermal/Fans/1")
            },
            {
                URI.create("/redfish/v1/Chassis/5-c-rack1/Thermal/Temperatures/1")
            },
            {
                URI.create("/redfish/v1/Managers/1-m-10/NetworkProtocol/1-m-10-np-1")
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Metrics/1-s-1-met-1")
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Processors/1-s-1-p-1/Metrics/1-s-1-p-met-1")
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/Memory/1-s-1-mm-1/Metrics/1-s-1-mm-1-met-1")
            }
        };
    }

    @DataProvider
    public Object[][] validUrisWithInvalidContexts() {
        return new Object[][]{
            {
                URI.create("/redfish/v1/Managers/1-m-10"),
                contextOf(id("1-m-10"), STORAGE_SERVICE)
            },
            {
                URI.create("/redfish/v1/Services/4-sv-1"),
                contextOf(id("4-sv-1"), COMPOSED_NODE)
            },
            {
                URI.create("/redfish/v1/Nodes/3"),
                contextOf(id("3"), STORAGE_SERVICE)
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Thermal"),
                contextOf(id("3-c-drawer1"), CHASSIS).child(id(""), THERMAL_FAN)
            },
            {
                URI.create("/redfish/v1/Chassis/3-c-drawer1/Power"),
                contextOf(id("3-c-drawer1"), CHASSIS).child(id(""), POWER_VOLTAGE)
            }
        };
    }

    @DataProvider
    public Object[][] validUrisWithValidContextsAndExcludedIncludedStrings() {
        return new Object[][]{
            {
                URI.create("/redfish/v1/EthernetSwitches/1-es-1/Ports/1-es-1-pt-1/VLANs/1-es-1-pt-1-vlan-1"),
                contextOf(id("1-es-1"), ETHERNET_SWITCH).child(id("1-es-1-pt-1"), ETHERNET_SWITCH_PORT)
                    .child(id("1-es-1-pt-1-vlan-1"), ETHERNET_SWITCH_PORT_VLAN),
                ETHERNET_SWITCH
            },
            {
                URI.create("/redfish/v1/Systems/1-s-1/EthernetInterfaces/1-s-1-ei-1"),
                contextOf(id("1-s-1"), COMPUTER_SYSTEM).child(id("1-s-1-ei-1"), ETHERNET_INTERFACE),
                COMPUTER_SYSTEM
            }
        };
    }
}
