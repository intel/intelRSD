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
const Collection = require('./../common/collection').Collection;
const RemoteTarget = require('./remoteTarget').RemoteTarget;
const Drive = require('./drive').Drive;
const LogicalDrive = require('./logicalDrive').LogicalDrive;

class Service extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.eventTrigger = serviceRoot.EventTrigger;
        this.CreateCollections();
        this.MockGetWithDynamicBody();
        this.MockBehaviours();
    }

    GetBaseUri() {
        return super.GetBaseUri('Services');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': `/redfish/v1/$metadata#Services/Members/${this.Id}/$entity`,
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#StorageService.v1_0_0.StorageService',
            'Id': this.Id,
            'Name': this.Name || 'Storage Service',
            'Description' : `Storage Service ${this.Id}`,
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'RemoteTargets': {
                '@odata.id': this.GetBaseUri() + '/Targets'
            },
            'LogicalDrives': {
                '@odata.id': this.GetBaseUri() + '/LogicalDrives'
            },
            'Drives': {
                '@odata.id': this.GetBaseUri() + '/Drives'
            },
            'Oem': {},
            'Links': {
                'ManagedBy': this.ManagedBy || [],
                Oem: {}
            }
        };
    }

    CreateCollections() {
        this.remoteTargetsCollection = new Collection(this.mock, {
            '@odata.context': `/redfish/v1/$metadata#Services/${this.Id}/RemoteTargets`,
            '@odata.id': this.GetBaseUri() + '/Targets',
            '@odata.type': '#RemoteTargetCollection.RemoteTargetCollection',
            'Name': 'Remote Targets Collection',
            'Context': this,
            'Resource': RemoteTarget
        });

        this.logicalDrivesCollection = new Collection(this.mock, {
            '@odata.context': `/redfish/v1/$metadata#Services/${this.Id}/LogicalDrives`,
            '@odata.id': this.GetBaseUri() + '/LogicalDrives',
            '@odata.type': '#LogicalDriveCollection.LogicalDriveCollection',
            'Name': 'Logical Drives Collection',
            'Context': this,
            'Resource': LogicalDrive
        });

        this.drivesCollection = new Collection(this.mock, {
            '@odata.context': `/redfish/v1/$metadata#Services/${this.Id}/Drives`,
            '@odata.id': this.GetBaseUri() + '/Drives',
            '@odata.type': '#PhysicalDriveCollection.PhysicalDriveCollection',
            'Name': 'Physical Drives Collection',
            'Context': this,
            'Resource': Drive,
            'OnElementAddedCallback': (driveUri) => this.eventTrigger.FireResourceAddedEvent(driveUri),
            'OnElementRemovedCallback': (driveUri) => this.eventTrigger.FireResourceRemovedEvent(driveUri)
        });
    }

    MockBehaviours() {
        var self = this;
        this.remoteTargetsCollection.EnablePostAndDelete(function (body) {
            var remoteTarget = self.AddRemoteTarget();

            if (body != undefined
                && body.Addresses != undefined
                && body.Addresses.length != 0
                && body.Addresses[0].iSCSI.TargetLUN.length != 0) {

                var logicalDriveUrl = body.Addresses[0].iSCSI.TargetLUN[0].LogicalDrive;
                remoteTarget.TargetIQN = body.Addresses[0].iSCSI.TargetIQN;
                remoteTarget.LogicalDrives = [logicalDriveUrl];
                var drive = self.LogicalDrives.GetByODataId(logicalDriveUrl["@odata.id"]);
                drive.AddTarget(remoteTarget);
            }
            return remoteTarget;
        });

        this.logicalDrivesCollection.EnablePostAndDelete(function (body) {
            var timeout = 500;
            if (body.Snapshot === false) {
                timeout = 30000;
            }
            var logicalDrive = self.AddLogicalDrive();

            if (body != undefined) {
                logicalDrive.Mode = body.Mode;
                logicalDrive.Type = body.Type;
            }

            setTimeout(() => {
                logicalDrive.State = 'Enabled';
            }, timeout);

            return logicalDrive;
        });
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    set ManagedBy(elements) {
        this.managedBy = elements;
    }

    get ManagedBy() {
        return this.managedBy;
    }

    AddManagedBy(element) {
        this.managedBy = this.managedBy || [];
        this.managedBy.push({'@odata.id': element.GetBaseUri()});
        return this;
    }

    AddRemoteTarget() {
        return this.remoteTargetsCollection.AddWithPrefix('target');
    }

    get RemoteTargets() {
        return this.remoteTargetsCollection;
    }

    AddLogicalDrive() {
        return this.logicalDrivesCollection.AddWithPrefix('lv');
    }

    get LogicalDrives() {
        return this.logicalDrivesCollection;
    }

    AddDrive() {
        return this.drivesCollection.AddWithPrefix('drive');
    }

    get Drives() {
        return this.drivesCollection;
    }

    set Name(name) {
        this.name = name;
    }

    get Name() {
        return this.name;
    }


    RemoveDrive(id) {
        var drive = this.drivesCollection.GetById(id);

        if(drive != undefined) {
            var driveUri = drive.GetBaseUri();
            this.drivesCollection.RemoveWithOdataId(driveUri);
            return driveUri;
        }
    }

    RemoveRemoteTarget(id) {
        var target = this.remoteTargetsCollection.GetById(id);

        if(target != undefined) {
            var targetUri = target.GetBaseUri();
            this.remoteTargetsCollection.RemoveWithOdataId(targetUri);
            return targetUri;
        }
    }
}

exports.Service = Service;
