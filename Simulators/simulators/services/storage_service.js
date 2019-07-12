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
const UUID = require('uuid');

class StorageService extends Service {
    constructor(port = '7443', uuid = UUID.v1(), discoveryProtocols = ["ssdp", "dhcp"]) {
        super(uuid, discoveryProtocols, port, 'rss', '/redfish/v1');

        var serviceRoot = this.ServiceRoot;

        serviceRoot.Name = 'RSS Service Root';

        this.service = serviceRoot.AddService();

        var drive = this.service.AddDrive();
        var target = this.service.AddRemoteTarget();

        var logicalDrive1 = this.service.AddLogicalDrive()
            .AddPhysicalDrive(drive)
            .AddTarget(target);
        logicalDrive1.Mode = 'LVG';
        logicalDrive1.CapacityGiB = 5589;
        logicalDrive1.State = 'Enabled';

        var logicalDrive2 = this.service.AddLogicalDrive()
            .AddPhysicalDrive(drive);

        logicalDrive2.Mode = 'PV';
        logicalDrive2.State = 'Enabled';

        logicalDrive2.MasterDrive = logicalDrive1;
        logicalDrive1.AddLogicalDrive(logicalDrive2);

        var logicalDrive3 = this.service.AddLogicalDrive();
        logicalDrive3.Mode = 'LV';
        logicalDrive3.State = 'Enabled';
        logicalDrive3.AddLogicalDrive(logicalDrive1);
        logicalDrive1.AddLogicalDrive(logicalDrive3);

        // TODO: determine if this is correct manager type
        var rssManager = serviceRoot.AddManagementControllerManager()
            .AddManagerForServices(this.service);
        this.service.AddManagedBy(rssManager);
        // TODO: how to list managed drives in manager
    }

    Service() {
        return this.service;
    }

    get Managers() {
        return this.serviceRoot.Managers;
    }
}

exports.StorageService = StorageService;
