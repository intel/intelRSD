/*
 * Copyright (c) 2015-2017 Intel Corporation
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

const BaseAsset = require('../common/baseAsset').BaseAsset;

class LogicalDrive extends BaseAsset {
    constructor(mock, service, id) {
        super(mock, service, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('LogicalDrives');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': `/redfish/v1/$metadata#Services/${this.context.Id}/LogicalDrives/Members/$entity`,
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#LogicalDrive.v1_0_0.LogicalDrive',
            'Id': this.Id,
            'Name': 'Logical Drive',
            'Description': 'Logical drive',
            'Status': this.Status || {
                'State': this.State || 'Starting',
                'Health': 'OK'
            },
            'Type': this.type || 'LVM',
            'Mode': this.mode || 'LVG',
            'Protected': false,
            'CapacityGiB': this.CapacityGiB || 1863,
            'Image': '',
            'Bootable': true,
            'Snapshot': false,
            'Links': {
                'LogicalDrives': this.logicalDrives || [],
                'PhysicalDrives': this.physicalDrives || [],
                'MasterDrive': this.masterDrive || null,
                'UsedBy': this.usedBy || [],
                'Targets': this.targets || [],
                'Oem': {}
            },
            'Oem': {}
        }
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    get Mode() {
        return this.mode;
    }

    set Mode(mode) {
        this.mode = mode;
    }

    get Type() {
        return this.type;
    }

    set Type(type) {
        this.type = type;
    }

    get CapacityGiB() {
        return this.capacityGiB;
    }

    set CapacityGiB(capacityGiB) {
        this.capacityGiB = capacityGiB;
    }

    get State() {
        return this.state;
    }

    set State(state) {
        this.state = state;
    }

    AddPhysicalDrive(element) {
        this.physicalDrives = this.physicalDrives || [];
        this.physicalDrives.push({'@odata.id': element.GetBaseUri()});
        element.AddUsedBy(this);
        return this;
    }

    AddLogicalDrive(element) {
        this.logicalDrives = this.logicalDrives || [];
        this.logicalDrives.push({'@odata.id': element.GetBaseUri()});
        element.AddUsedBy(this);
        return this;
    }

    AddTarget(element) {
        this.targets = this.targets || [];
        this.targets.push({'@odata.id': element.GetBaseUri()});
        element.AddLun(this);
        return this;
    }

    AddUsedBy(element) {
        this.usedBy = this.usedBy || [];
        this.usedBy.push({'@odata.id': element.GetBaseUri()});
        return this;
    }

    set MasterDrive(element) {
        this.masterDrive = {'@odata.id': element.GetBaseUri()};
    }
}

exports.LogicalDrive = LogicalDrive;
