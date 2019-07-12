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
const encapsulatedMock = require('../mock_entry_points/storage_service_mock').storageServiceMock;

class StorageServiceEventableMock extends EventableMock {
    constructor() {
        let prompt = 'RSS';
        let menuOptions = [
            '1. Add physical disk',
            '2. Remove physical disk',
            '3. Remove target',
            '4. Change target\'s status',
            '0. Exit'
        ];

        super(prompt, menuOptions);

        this.MockBehavior = this.SetupMockBehavior;
    }

    SetupMockBehavior(line) {
        let rl = this.rl;

        switch (line.trim()) {
            case '1':
                var disk = encapsulatedMock.Service().AddDrive();
                console.log('Adding disk ' + disk.GetBaseUri());
                var sentEvents = encapsulatedMock.FireResourceAddedEvent(disk.GetBaseUri());
                printEventsArray(sentEvents);
                break;
            case '2':
                rl.question('Which disk should be removed? ', function (id) {
                    var diskUri = encapsulatedMock.Service().RemoveDrive(id);

                    if (diskUri === undefined) {
                        console.log('There is no drive with id: ' + id);
                    } else {
                        console.log('Removing disk ' + diskUri);
                        var sentEvents = encapsulatedMock.FireResourceRemovedEvent(diskUri);
                        printEventsArray(sentEvents);
                        rl.prompt();
                    }
                });
                break;
            case '3':
                rl.question('Which target should be removed? ', function (id) {
                    var targetUri = encapsulatedMock.Service().RemoveRemoteTarget(id);

                    if (targetUri === undefined) {
                        console.log('There is no target with id: ' + id);
                    } else {
                        console.log('Removing target ' + targetUri);
                        var sentEvents = encapsulatedMock.FireResourceRemovedEvent(targetUri);
                        printEventsArray(sentEvents);
                        rl.prompt();
                    }
                });
                break;
            case '4':
                rl.question('Which target should be changed? ', function (id) {
                    var target = encapsulatedMock.Service().RemoteTargets.GetById(id);

                    if (target === undefined) {
                        console.log('There is no target with id: ' + id);
                    } else {
                        var currentStatus = target.Status;
                        var newStatus = {State: 'Disabled', Health: 'Critical'};

                        console.log('Changing target\'s status (' + target.GetBaseUri() + ') from: '
                            + JSON.stringify(currentStatus) + ' to: ' + JSON.stringify(newStatus));

                        target.Status = newStatus;

                        var sentEvents = encapsulatedMock.FireStatusChangedEvent(target.GetBaseUri());
                        printEventsArray(sentEvents);
                        rl.prompt();
                    }
                });
                break;
            case '0':
                console.log('storage service will be removed from /tmp/services.list');
                encapsulatedMock.Stop();
                process.exit();
                break;
        }
        rl.prompt();
    }
}

exports.StorageServiceEventableMock = StorageServiceEventableMock;