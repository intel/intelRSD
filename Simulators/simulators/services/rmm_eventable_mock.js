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
const encapsulatedMock = require('../mock_entry_points/rmm_mock').rmmMock;

class RmmEventableMock extends EventableMock {
    constructor() {
        const prompt = 'RMM';
        const menuOptions = [
            '1. Insert Drawer',
            '2. Remove Drawer',
            '3. Insert Fan',
            '4. Remove Fan',
            '5. Insert PSU',
            '6. Remove PSU',
            '7. Change Rack PUID',
            '8. Change Rack Location.Id',
            '0. Exit'
        ];

        super(prompt, menuOptions);

        this.MockBehavior = this.SetupMockBehavior;
    }

    SetupMockBehavior(line) {
        const rl = this.rl;

        switch (line.trim()) {
            case '1':
                const drawer = encapsulatedMock.AddDrawer(encapsulatedMock.Drawers.length + 1);
                encapsulatedMock.EventTrigger.FireEvent(drawer.GetBaseUri(), 'ResourceAdded');
                console.log(`Drawer added: ${drawer.GetBaseUri()}`);
                break;
            case '2':
                rl.question('Which drawer should I remove? ', id => {
                    const drawerUri = encapsulatedMock.RemoveDrawerWithId(id);
                    if (drawerUri) {
                        encapsulatedMock.EventTrigger.FireEvent(drawerUri, 'ResourceRemoved');
                        console.log(`Drawer removed: ${drawerUri}`);
                    } else {
                        console.log('Invalid Id or chassis type');
                    }
                    rl.prompt();
                });
                break;
            case '3':
                const fan = encapsulatedMock.Rack.GetThermalZoneWithId(1).AddFan();
                encapsulatedMock.EventTrigger.FireEvent(fan.Uri, 'ResourceAdded');
                console.log('Fan added: ' + fan.Uri);
                break;
            case '4':
                rl.question('Which fan should I remove? ', id => {
                    const fanUri = encapsulatedMock.Rack.GetThermalZoneWithId(1).RemoveFan(id);
                    encapsulatedMock.EventTrigger.FireEvent(fanUri, 'ResourceRemoved');
                    console.log(`Fan removed: ${fanUri}`);
                    rl.prompt();
                });
                break;
            case '5':
                const psu = encapsulatedMock.Rack.GetPowerZoneWithId(1).AddPsu();
                encapsulatedMock.EventTrigger.FireEvent(psu.Uri, 'ResourceAdded');
                console.log(`PSU added: ${psu.Uri}`);
                break;
            case '6':
                rl.question('Which psu should I remove? ', id => {
                    const psuUri = encapsulatedMock.Rack.GetPowerZoneWithId(1).RemovePsu(id);
                    encapsulatedMock.EventTrigger.FireEvent(psuUri, 'ResourceRemoved');
                    console.log(`PSU removed: ${psuUri}`);
                    rl.prompt();
                });
                break;
            case '7':
                rl.question('Change RackPUID to: ', id => {
                    const rackPuid = parseInt(id);
                    if (!isNaN(rackPuid)) {
                        encapsulatedMock.Rack.RackPuid = rackPuid;
                        console.log('Changed');
                    } else {
                        console.log('Invalid RackPUID. Should be a number.')
                    }
                    encapsulatedMock.EventTrigger.FireEvent('/redfish/v1/Chassis/Rack', 'ResourceUpdated');
                    rl.prompt();
                });
                break;
            case '8':
                rl.question('Change Location.Id to: ', id => {
                    const locationId = parseInt(id);
                    if (!isNaN(locationId)) {
                        encapsulatedMock.Rack.LocationId = locationId.toString();
                        encapsulatedMock.EventTrigger.FireEvent('/redfish/v1/Chassis/Rack', 'ResourceUpdated');
                        console.log('Changed');
                    } else {
                        console.log('Invalid Location.Id. Should be a number.')
                    }
                    rl.prompt();
                });
                break;
            case '0':
                console.log('rmm service will be removed from /tmp/services.list');
                encapsulatedMock.Stop();
                process.exit();
                break;
        }
        rl.prompt();
    }
}

exports.RmmEventableMock = RmmEventableMock;
