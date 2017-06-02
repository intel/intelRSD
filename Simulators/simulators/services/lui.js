/*
 * Copyright (c) 2016-2017 Intel Corporation
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
const randomMac = require('./../utils/random').randomMac;
const UUID = require('uuid');

class Lui extends Service {
    constructor(port = '6443', uuid = UUID.v1(), discoveryProtocols = ["dhcp", "ssdp"]) {
        super(uuid, discoveryProtocols, port, 'lui', '/redfish/v1');

        this.ServiceRoot.Name = 'LUI Service Root';

        this.computerSystem = this.ServiceRoot.AddSystem();
        configureSystem(this.computerSystem);

        this.sled = this.ServiceRoot.AddSled();
        this.sled.AddToComputerSystems(this.computerSystem);
        this.sled.Manufacturer = 'Unknown manufacturer';
        this.sled.Model = 'Intel Xeon';

        var drive = this.sled.AddDrive();
        drive.SKU = 'sku';
        drive.Protocol = 'SATA';
        drive.AssetTag = 'AssetTag';
        drive.Revision = 'revision string';
        drive.MediaType = 'SSD';
        drive.PartNumber = 'SG0GP8811253178M02GJA01';
        drive.HotspareType = 'Dedicated';
        drive.SerialNumber = '72D0A037FRD28';
        drive.Manufacturer = 'Intel';
        drive.BlockSizeBytes = 1024;
        drive.CapableSpeedGbs = 1234;
        drive.StatusIndicator = 'OK';
        drive.EncryptionStatus = 'Locked';
        drive.EncryptionAbility = 'Other';
        drive.PredictedMediaLifeLeftPercent = 80;
        drive.RotationSpeedRPM = 460;
        drive.IndicatorLED = 'Lit';
        drive.Location = [{
                'Info': '4',
                'InfoFormat': 'Hdd index'
            }];
        drive.Identifiers = [{
                'DurableName': '123e4567-e89b-12d3-a456-426655440000',
                'DurableNameFormat': 'UUID'
            }];
        drive.Status = {
            'State': 'Enabled',
            'Health': 'OK'
        };

        var sledManager = this.ServiceRoot.AddManagementControllerManager()
            .AddManagerForServers(this.computerSystem)
            .AddManagerForChassis(this.sled);

        sledManager.AddEthernetInterface(randomMac());

        sledManager.ManagerInChassis = this.sled;
        this.computerSystem.AddManagedBy(sledManager);
        this.sled.AddManagedBy(sledManager);

        var storage = this.computerSystem.AddStorage();
        storage.AddDrive(drive);
        storage.AddEnclosure(this.sled);
        storage.AddEnclosure(this.sled);
        storage.AddStorageController();
        storage.AddStorageController();

        this.sled.AddStorage(storage);

        function configureSystem(system) {
            system.TotalSystemMemoryGiB = 8;
            system.Manufacturer = 'Intel';
            system.Model = 'Xeon';
            system.Sku = 'sku';
            system.SerialNumber = '123';
            system.PartNumber = 'part number';

            system.ProcessorModel = 'Multi-Core Intel(R) Xeon(R) processor 7xxx Series';

            system.PciDevices = [{'VendorId': '0x1111', 'DeviceId': '0x1111'}];


            var processor = system.AddProcessor();
            processor.Socket = 'SOCKET 1';
            processor.ProcessorType = 'CPU';
            processor.ProcessorArchitecture = 'x86';
            processor.InstructionSet = 'x86';
            processor.Manufacturer = 'Intel';
            processor.Model = 'Xeon';
            processor.ProcessorId = {
                'VendorId': 'GenuineIntel',
                'IdentificationRegisters': '0x34AC34DC8901274A',
                'EffectiveFamily': '0x42',
                'EffectiveModel': '0x61',
                'Step': '0x1',
                'MicrocodeInfo': '0x429943'
            };
            processor.TotalCores = 8;
            processor.TotalThreads = 16;
            processor.Brand = 'E5';
            processor.Capabilities = ['sse', 'sse2', 'sse3'];

            var memory = system.AddMemory();
            memory.MemoryDeviceType = 'DDR4';
            memory.BaseModuleType = 'LRDIMM';
            memory.DataWidthBits = 64;
            memory.BusWidthBits = 72;
            memory.Manufacturer = 'Contoso';
            memory.SerialNumber = '1A2B3B';
            memory.PartNumber = '1A2V3D';
            memory.AllowedSpeedsMhz = [
                2133,
                2400,
                2667
            ];
            memory.RankCount = 1;
            memory.DeviceLocator = 'A0';
            memory.MemoryLocation = {
                    'Socket': 1,
                    'MemoryController': 1,
                    'Channel': 1,
                    'Slot': 1
                };
            memory.ErrorCorrection = 'MultiBitECC';
            memory.Regions = [
                    {
                        'RegionId': '1',
                        'MemoryClassification': 'Volatile',
                        'OffsetMiB': 0,
                        'SizeMiB': 8192
                    }
                ];
            memory.OperatingMemoryModes = [
                    'Volatile'
                ];
            memory.VoltageVolt = 1.35;

            var networkInterfaceMac = 'E9:47:D3:60:64:66';
            var ethernetInterface = system.AddEthernetInterface(networkInterfaceMac);
            ethernetInterface.SpeedMbps = 100;
            ethernetInterface.FullDuplex = true;

            var networkInterface2Mac = 'E9:47:D3:60:64:77';
            var ethernetInterface2 = system.AddEthernetInterface(networkInterface2Mac);
            ethernetInterface2.SpeedMbps = 100;
            ethernetInterface2.FullDuplex = true;

            return system;
        }
    }

    get ComputerSystem() {
        return this.computerSystem;
    }

    get Sled() {
        return this.sled;
    }

}

exports.Lui = Lui;
