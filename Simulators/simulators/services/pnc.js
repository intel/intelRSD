/*
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const Service = require('./service').Service;
const randomstring = require("randomstring");
const UUID = require('uuid');

class Pnc extends Service {
    constructor(port = '5443', uuid = UUID.v1(), discoveryProtocols = ["dhcp", "ssdp"]) {
        super(uuid, discoveryProtocols, port, 'rss', '/redfish/v1');
        this.ServiceRoot.Name = 'RSS Service Root';

        this.moduleChassis = this.ServiceRoot.AddEnclosureChassis();
        this.moduleChassis.Description = 'PCIe Switch';


        this.system = this.ServiceRoot.AddSystem();
        this.system.SystemType = "Virtual";
        this.system.BootSourceOverrideEnabled = 'Disabled';
        this.system.BootSourceOverrideTarget = 'None';
        this.system.BootSourceOverrideMode = null;
        this.system.Name = "PNC Computer System";
        this.moduleChassis.ComputerSystems = this.system;

        this.fabricManager = this.ServiceRoot.AddManagementControllerManager();
        this.fabricManager.AddManagerForChassis(this.moduleChassis);
        this.fabricManager.AddManagerForServers(this.system);

        this.moduleChassis.AddManagedBy(this.fabricManager);
        this.system.AddManagedBy(this.fabricManager);

        this.fabric = this.ServiceRoot.AddFabric();
        this.fabricSwitch = this.fabric.AddSwitch();

        var fabricDownstreamSwitchPort = this.fabricSwitch.AddPort();
        fabricDownstreamSwitchPort.Name = 'PCIe Downstream Port';
        fabricDownstreamSwitchPort.Description = 'PCIe Downstream Port';
        fabricDownstreamSwitchPort.PortType = 'DownstreamPort';
        this.fabricSwitch.Chassis = this.moduleChassis;

        var fabricZone = this.fabric.AddZone();
        fabricZone.AddToSwitches(this.fabricSwitch);

        var fabricDownstreamEndpoint = this.fabric.AddEndpoint();
        fabricDownstreamEndpoint.Name = 'Downstream Endpoint';
        fabricDownstreamEndpoint.Ports = fabricDownstreamSwitchPort;

        fabricDownstreamSwitchPort.AssociatedEndpoints = fabricDownstreamEndpoint;

        var pcieDrive = this.moduleChassis.AddDrive();
        pcieDrive.Identifiers = [{
            'DurableName': '123e4567-e89b-12d3-a456-426655440000',
            'DurableNameFormat': "UUID"
        }];
        pcieDrive.Protocol = 'NVMe';
        pcieDrive.MediaType = 'SSD';
        pcieDrive.Status = {'State': 'Enabled', 'Health': 'OK'};
        pcieDrive.CapacityBytes = 899527000000;
        pcieDrive.AddEndpoints(fabricDownstreamEndpoint);

        var pcieDrive1 = this.moduleChassis.AddDrive();
        pcieDrive1.Identifiers = [{
            'DurableName': '123e4567-e89b-12d3-a456-426655440000',
            'DurableNameFormat': "UUID"
        }];
        pcieDrive1.Protocol = 'NVMe';
        pcieDrive1.MediaType = 'SSD';
        pcieDrive1.Status = {'State': 'Enabled', 'Health': 'OK'};
        pcieDrive1.CapacityBytes = 555527000000;
        pcieDrive1.AddEndpoints(fabricDownstreamEndpoint);

        var storage = this.system.AddStorage();
        storage.AddDrive(pcieDrive);
        storage.Name = 'PCIe Storage';
        this.moduleChassis.AddStorage(storage);
        pcieDrive.AddStorage(storage);

        var connectedEntity = {
            EntityRole: 'Target',
            EntityType: 'Drive',
            PciFunctionId: 1,
            PciClassCode: '0x010802',
            EntityLink: {
                '@odata.id': pcieDrive.GetBaseUri()
            },
            EntityPciId: {
                'DeviceId': '0xABCD',
                'VendorId': '0xNNBCD',
                'SubsystemId': '0xCCBCD',
                'SubsystemVendorId': '0xBBCD'
            },
            Identifiers: [{
                'DurableName': '123e4567-e89b-12d3-a456-426655440000',
                'DurableNameFormat': "UUID"
            }]
        };

        fabricDownstreamEndpoint.AddConnectedEntity(connectedEntity);

        this.fabricUpstreamSwitchPort = this.fabricSwitch.AddPort();
        this.fabricUpstreamSwitchPort.Name = 'PCIe Upstream Port 1';
        this.fabricUpstreamSwitchPort.PortType = 'UpstreamPort';
        this.fabricUpstreamSwitchPort.AddPcieConnectionId(randomstring.generate(7));

        var fabricUpstreamEndpoint = this.fabric.AddEndpoint();
        fabricUpstreamEndpoint.Ports = this.fabricUpstreamSwitchPort;

        this.fabricUpstreamSwitchPort.AssociatedEndpoints = fabricUpstreamEndpoint;

        var connectedEntity1= {
            EntityType: 'RootComplex',
            EntityRole: 'Initiator',
            EntityPciId: {
                'DeviceId': null,
                'VendorId': null,
                'SubsystemId': null,
                'SubsystemVendorId': null
            }
        };

        fabricUpstreamEndpoint.AddConnectedEntity(connectedEntity1);

        fabricZone.AddToEndpoints(fabricUpstreamEndpoint);

        var pcieDevice = this.moduleChassis.AddPCIeDevice();
        this.system.AddPCIeDevice(pcieDevice);
        var deviceFunction = pcieDevice.AddDeviceFunction();
        deviceFunction.PCIeDevice = pcieDevice;
        deviceFunction.Drives = pcieDrive;
        pcieDrive.AddPcieFunction(deviceFunction);
        this.deviceFunc = deviceFunction;
    }

    get Managers() {
        return this.ServiceRoot.Managers;
    }

    get Fabric() {
        return this.fabric;
    }

    get FabricSwitch() {
        return this.fabricSwitch;
    }

    get FabricUpstreamSwitchPort() {
        return this.fabricUpstreamSwitchPort;
    }

    get FabricModules() {
        return this.ServiceRoot.FabricModules;
    }

    get Chassis() {
        return this.moduleChassis;
    }

    get DeviceFunction() {
        return this.deviceFunc;
    }

    get Systems() {
        return this.ServiceRoot.Systems.GetElements();
    }

    get System() {
        return this.Systems[0];
    }
}

exports.Pnc = Pnc;
