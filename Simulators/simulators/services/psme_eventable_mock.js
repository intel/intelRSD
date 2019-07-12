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

const EventableMock = require('../api/common/eventableMock').EventableMock;
const printEventsArray = require('./mock_utils').PrintEventsArray;
const encapsulatedMock = require('../mock_entry_points/psme_mock').psmeMock;

class PsmeEventableMock extends EventableMock {
    constructor() {
        let prompt = 'PSME';
        let menuOptions = [
            '1. Insert Module into Drawer',
            '2. Insert System into Module',
            '3. Remove Chassis',
            '4. Remove System',
            '5. Change System\'s Status',
            '6. Change RackID',
            '0. Exit'
        ];

        super(prompt, menuOptions);

        this.MockBehavior = this.SetupMockBehavior;
    }

    SetupMockBehavior(line) {
        let rl = this.rl;

        switch (line.trim()) {
            case '1':
                var drawer = encapsulatedMock.ServiceRoot.GetChassisWithId('drawer1');
                var module = encapsulatedMock.ServiceRoot.AddModule();
                drawer.AddToContains([module]);
                console.log('Adding Chassis(module): ' + module.Id);

                var sentEvents = encapsulatedMock.FireResourceAddedEvent(module.GetBaseUri());
                printEventsArray(sentEvents);
                rl.prompt();
                break;
            case '2':
                rl.question('To which module should I add system? ', function (id) {
                    var chassis = encapsulatedMock.ServiceRoot.GetChassisWithId(id);

                    if (chassis === undefined) {
                        console.log('There is no module with id: ' + id);
                    } else if (chassis.ChassisType != 'Module') {
                        console.log(`Chassis: ${id} is not a module`);
                    } else {
                        console.log(chassis.ComputerSystems.length)
                        var system = encapsulatedMock.ServiceRoot.AddSystem();
                        system.TotalSystemMemoryGiB = 8;
                        var processor = system.AddProcessor();
                        processor.Socket = '1';
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

                        chassis.AddToComputerSystems([system]);

                        console.log(`Adding system: ${system.Id} to module: ${id}`);

                        var sentEvents = encapsulatedMock.FireResourceAddedEvent(system.GetBaseUri());
                        printEventsArray(sentEvents);
                    }
                    rl.prompt();
                });
                break;
            case '3':

                rl.question('Which chassis should I remove? ', function (id) {
                    var chassisUri = encapsulatedMock.ServiceRoot.RemoveChassisWithId(id);

                    if (chassisUri === undefined) {
                        console.log(`There is no chassis with id: ` + id);
                    } else {
                        console.log(`Removing chassis: ` + chassisUri);
                        var sentEvents = encapsulatedMock.FireResourceRemovedEvent(chassisUri);
                        printEventsArray(sentEvents);
                    }

                    rl.prompt();
                });
                break;
            case '4':
                rl.question(`Which system should I remove? `, function (id) {
                    const system = encapsulatedMock.ServiceRoot.GetSystemById(id);

                    if (system === undefined) {
                        console.log(`There is no system with id: ` + id);
                    } else {
                        const systemUri = system.GetBaseUri();
                        console.log(`Removing system: ` + systemUri);
                        encapsulatedMock.ServiceRoot.RemoveSystem(system);

                        const sentEvents = encapsulatedMock.FireResourceRemovedEvent(systemUri);
                        printEventsArray(sentEvents);
                    }

                    rl.prompt();
                });

                break;
            case '5':
                rl.question('Which system? ', function (id) {
                    var system = encapsulatedMock.ServiceRoot.Systems.GetById(id);

                    if (system === undefined) {
                        console.log('There is no system with id: ' + id);
                    } else {
                        var currentStatus = system.Status;
                        var newStatus = {State: 'Disabled', Health: 'Critical', HealthRollup: 'Critical'};
                        console.log('Changing blade\'s status (' + system.GetBaseUri() + ') from: '
                            + JSON.stringify(currentStatus) + ' to: ' + JSON.stringify(newStatus));

                        system.Status = newStatus;
                        var sentEvents = encapsulatedMock.FireStatusChangedEvent(system.GetBaseUri());
                        printEventsArray(sentEvents);
                    }
                    rl.prompt();
                });
                break;
            case '6':
                rl.question('Change RackID to: ', function (id) {
                    encapsulatedMock.Drawer.ParentId = id;
                    var drawerUri = encapsulatedMock.Drawer.GetBaseUri();
                    encapsulatedMock.EventTrigger.FireEvent(drawerUri, 'ResourceUpdated');
                    console.log('Changed');
                    rl.prompt();
                });
                break;
            case '0':
                console.log('psme service will be removed from /tmp/services.list');
                encapsulatedMock.Stop();
                setTimeout(function () {
                    process.exit();
                }, 2000);
                break;
        }
    };
}

exports.PsmeEventableMock = PsmeEventableMock;
