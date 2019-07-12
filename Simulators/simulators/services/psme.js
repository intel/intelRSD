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

class Psme extends Service {
    constructor(port = '9443', uuid = UUID.v1(), discoveryProtocols = ["dhcp", "ssdp"], settings = undefined) {
        super(uuid, discoveryProtocols, port, 'psme', '/redfish/v1');

        this.BuildFromSettings(settings);
        this.ServiceRoot.Name = 'PSME Service Root';

        var drawer = this.ServiceRoot.AddDrawer();
        var sledModule = this.ServiceRoot.AddModule();
        var fabricModule = this.ServiceRoot.AddModule();

        drawer.Contains = [sledModule, fabricModule];

        var system = this.ServiceRoot.AddSystem();
        system.TotalSystemMemoryGiB = 8;
        system.BootSourceOverrideMode = 'Legacy';
        system.Name = randomstring.generate(7);
        system.AddPcieConnectionId(randomstring.generate(7));

        sledModule.ComputerSystems = system;
        var processor = system.AddProcessor();
        processor.Socket = 'SOCKET 1';
        var memory = system.AddMemory();

        var region = {
            'RegionId': '1',
            'MemoryClassification': 'Volatile',
            'OffsetMiB': 0,
            'SizeMiB': 8192
        };

        memory.Regions = [region];
        memory.DeviceLocator = 'A0';
        memory.OperatingMemoryModes = ['Volatile'];
        memory.VendorId = 'id.89';
        memory.DeviceId = 'id.55';
        memory.OperatingSpeedsMhz = 3600;
        memory.AllowedSpeedsMhz = [];

        var ethernetInterfaceMac = 'E9:47:D3:60:64:66';
        var ethernetInterface = system.AddEthernetInterface(ethernetInterfaceMac);
        var vlan99 = ethernetInterface.AddVlan();
        vlan99.VlanId = 99;
        vlan99.Description = 'Ethernet Interface VLAN ' + vlan99.vlanId;
        vlan99.Tagged = true;

        var networkInterface = system.AddNetworkInterface();
        networkInterface.AddNetworkDeviceFunction();

        this.drawerManager = this.ServiceRoot.AddEnclosureManager()
            .AddManagerForChassis(drawer);
        this.drawerManager.ManagerInChassis = drawer;
        this.drawerManager.Name = 'PSME Manager';
        drawer.AddManagedBy(this.drawerManager);

        this.sledManager = this.ServiceRoot.AddManagementControllerManager()
            .AddManagerForServers(system)
            .AddManagerForChassis(sledModule);

        this.sledManager.ManagerInChassis = sledModule;
        sledModule.AddManagedBy(this.sledManager);
        sledModule.Name = "Module chassis";
        system.AddManagedBy(this.sledManager);

        this.ethernetSwitch = this.ServiceRoot.AddEthernetSwitch();
        var fabricModuleManager = this.ServiceRoot.AddManagementControllerManager()
            .AddManagerForChassis(fabricModule)
            .AddManagerForSwitches(this.ethernetSwitch);
        fabricModuleManager.ManagerInChassis = fabricModule;
        fabricModule.AddManagedBy(fabricModuleManager);

        fabricModuleManager.AddEthernetInterface('E9:22:D3:11:64:66');

        this.ethernetSwitch.AddManagedBy(fabricModuleManager);

        this.ethernetSwitch.ContainedBy = fabricModule;

        var ethernetSwitchPort = this.ethernetSwitch.AddPhysicalPort('11:22:33:44:55:66');
        ethernetSwitchPort.PortId = "sw1p1";
        ethernetSwitchPort.PortType = 'Upstream';
        ethernetSwitchPort.AdministrativeState = 'Down';

        var vlan666 = ethernetSwitchPort.AddVlan();
        vlan666.VlanId = 666;
        vlan666.Description = 'Switch Port VLAN ' + vlan666.vlanId;
        vlan666.Tagged = true;

        var vlan10 = ethernetSwitchPort.AddVlan();
        vlan10.VlanId = 10;
        vlan10.Description = 'Switch Port VLAN ' + vlan10.vlanId;
        ethernetSwitchPort.PrimaryVlan = vlan10.GetBaseUri();

        var secondPort = this.ethernetSwitch.AddPhysicalPort('66:66:66:66:66:66');
        secondPort.PortId = "sw1p2";
        secondPort.PortType = "Downstream";
        secondPort.NeighborMAC = ethernetInterfaceMac;
        secondPort.AdministrativeState = 'Down';

        var thirdPort = this.ethernetSwitch.AddPhysicalPort('A7:47:B5:60:64:99');
        thirdPort.PortId = "sw1p3";
        thirdPort.PortType = "Downstream";
        thirdPort.NeighborMAC = "E9:47:D3:60:64:77";

        var drive = sledModule.AddDrive();
        drive.CapacityBytes = 899527000000;
        drive.Model = 'Model string';
        drive.CapacityBytes = 4096;

        var storage = system.AddStorage();
        storage.AddDrive(drive);
        storage.AddEnclosure(sledModule);

        sledModule.AddStorage(storage);

        this.power = drawer.Power;
        var powerControl = this.power.AddPowerControl();
        var powerVoltage = this.power.AddVoltage();
        var powerSupply = this.power.AddPowerSupply();
        var powerRedundancy = this.power.AddRedundancy();

        powerSupply.AddToRelatedItems(drawer);
        powerVoltage.AddToRelatedItems(system);
        powerControl.AddToRelatedItems(system);
        system.AddComputerSystemRelatedItem(powerControl);
        system.AddComputerSystemRelatedItem(powerVoltage);

        powerControl.AddToRelatedItems(drawer);
        powerSupply.AddToRedundancySet(powerRedundancy);
        powerRedundancy.AddToRedundancySet(powerSupply);
    }

    get Drawers() {
        return this.ServiceRoot.Drawers;
    }

    get Systems() {
        return this.ServiceRoot.Systems.GetElements();
    }

    get Chassis() {
        return this.ServiceRoot.Chassis;
    }

    get Drawer() {
        return this.Drawers[0];
    }

    get System() {
        return this.Systems[0];
    }

    get DrawerManager() {
        return this.drawerManager;
    }

    get Blades() {
        return this.ServiceRoot.Blades;
    }

    get Sleds() {
        return this.ServiceRoot.Sleds;
    }

    get FabricModules() {
        return this.ServiceRoot.FabricModules;
    }

    get Managers() {
        return this.ServiceRoot.Managers;
    }

    get EthernetSwitch() {
        return this.ethernetSwitch;
    }

    ManagerBMC() {
        return this.sledManager;
    }

    ManagerPsme() {
        return this.drawerManager;
    }

    BuildFromSettings(settings) {
        if (!settings) {
            return;
        }

        let drawers = settings.drawers;
        let sleds = settings.sleds;
        let blades = settings.blades;

        for (let drawerNum = 0; drawerNum < drawers; drawerNum++) {
            let drawer = this.ServiceRoot.AddDrawer();

            for (let sledNum = 0; sledNum < sleds; sledNum++) {
                let sled = this.ServiceRoot.AddSled();
                drawer.Contains = [sled];

                for (let bladeNum = 0; bladeNum < blades; bladeNum++) {
                    let blade = this.ServiceRoot.AddBlade();
                    sled.Contains = [blade];

                    let system = this.ServiceRoot.AddSystem();
                    system.TotalSystemMemoryGiB = 8;
                    blade.ComputerSystems = system;

                    let processor = system.AddProcessor();
                    processor.Socket = '1';

                    let memory = system.AddMemory();

                    let region = {
                        'RegionId': '1',
                        'MemoryType': 'Volatile',
                        'OffsetMiB': 0,
                        'SizeMiB': 8192
                    };

                    memory.Regions = [region];
                    memory.DeviceLocator = 'A0';
                    memory.OperatingMemoryModes = ['Volatile'];

                    let ethernetInterfaceMac = 'E9:47:D3:60:64:66';
                    let ethernetInterface = system.AddEthernetInterface(ethernetInterfaceMac);
                    let vlan = ethernetInterface.AddVlan();

                    vlan.VlanId = 1000 + drawerNum * sledNum + bladeNum; //prevent duplicates
                    vlan.Description = 'Ethernet Interface VLAN ' + vlan.vlanId;
                    vlan.Tagged = true;
                }
            }
        }
    }
}

exports.Psme = Psme;
