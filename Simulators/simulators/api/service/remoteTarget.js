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
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;

class RemoteTarget extends BaseAsset {
    constructor(mock, service, id) {
        super(mock, service, id);
        this.Status = {State: 'Enabled', Health: 'OK'};
        this.lunsCount = 0;
        this.InitiatorIQN = 'ALL';
        this.MockGetWithDynamicBody();
        this.MockPatch();
        this.initAddresses();
    }

    initAddresses() {
        this.patchableChap = new Chap();
        this.luns = [];
        this.addresses = [];
        this.addresses.push(
            {
                'iSCSI': {
                    'TargetLUN': this.luns,
                    'TargetIQN': 'base_logical_volume_target',
                    'TargetPortalIP': this.TargetPortalIP || '10.2.0.4',
                    'TargetPortalPort': this.TargetPortalPort || 3260,
                    'CHAP': this.patchableChap
                }
            }
        );
    }

    MockPatch() {
        this.mock.mockPatch(this.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            (req, response) => {
                var jsonRequest = JSON.parse(req.body);
                if (jsonRequest.Addresses !== undefined && jsonRequest.Addresses[0].iSCSI !== undefined) {
                    jsonRequest.Addresses[0].iSCSI.CHAP == undefined ? this.clearChapData() : this.patchChapData(jsonRequest);
                }

                response.body = this.CreateJsonTemplate();
                return response;
            }
        );
    }

    patchChapData(jsonRequest) {
        var chap = jsonRequest.Addresses[0].iSCSI.CHAP;
        this.patchableChap.Type = chap.Type === undefined ? this.patchableChap.Type : chap.Type;
        this.patchableChap.Username = chap.Username === undefined ? this.patchableChap.Username : chap.Username;
        this.patchableChap.Secret = chap.Secret === undefined ? this.patchableChap.Secret : chap.Secret;
        this.patchableChap.MutualUsername = chap.MutualUsername === undefined ? this.patchableChap.MutualUsername : chap.MutualUsername;
        this.patchableChap.MutualSecret = chap.MutualSecret === undefined ? this.patchableChap.MutualSecret : chap.MutualSecret;

        if (jsonRequest.Initiator !== undefined && jsonRequest.Initiator[0].iSCSI !== undefined) {
            this.InitiatorIQN = jsonRequest.Initiator[0].iSCSI.InitiatorIQN === undefined ? this.InitiatorIQN : jsonRequest.Initiator[0].iSCSI.InitiatorIQN;
        }
    }

    clearChapData() {
        this.patchableChap.Type = null;
        this.patchableChap.Username = null;
        this.patchableChap.Secret = null;
        this.patchableChap.MutualUsername = null;
        this.patchableChap.MutualSecret = null;
    }

    GetBaseUri() {
        return super.GetBaseUri('Targets');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': `/redfish/v1/$metadata#Services/${this.context.Id}/RemoteTargetsMembers/$entity`,
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#RemoteTarget.v1_0_0.RemoteTarget',
            'Id': this.Id,
            'Name': this.Name || 'Remote Target',
            'Description': 'Remote Target',
            'Status': {
                'Health': this.Status.Health,
                'State': this.Status.State
            },
            'Type': 'iSCSITargets',
            'Addresses': this.addresses || [],
            'Initiator': [
                {
                    'iSCSI': {
                        'InitiatorIQN': this.InitiatorIQN
                    }
                }
            ],
            'Oem': {},
            'Links': {
                'ManagedBy': [],
                'Oem': {}
            }
        }
    }

    get LogicalDrives() {
        return this.logicalDrives;
    }

    set LogicalDrives(logicalDrives) {
        this.logicalDrives = logicalDrives;
    }

    get Status() {
        return this.status;
    }

    set Status(status) {
        this.status = status;
    }

    set Name(name) {
        this.name = name;
    }

    get Name() {
        return this.name;
    }

    get Addresses() {
        return this.addresses;
    }

    get TargetIQN() {
        return this.targetIQN;
    }

    set TargetIQN(targetIQN) {
        this.targetIQN = targetIQN;
    }

    get TargetPortalIP() {
        return this.targetPortalIP;
    }

    set TargetPortalIP(targetPortalIP) {
        this.targetPortalIP = targetPortalIP;
    }

    get TargetPortalPort() {
        return this.targetPortalPort;
    }

    set TargetPortalPort(targetPortalPort) {
        this.targetPortalPort = targetPortalPort;
    }

    get ChapType() {
        return this.chapType;
    }

    set ChapType(chapType) {
        this.chapType = chapType;
    }

    get ChapUsername() {
        return this.chapUsername;
    }

    set ChapUsername(chapUsername) {
        this.chapUsername = chapUsername;
    }

    get ChapSecret() {
        return this.chapSecret;
    }

    set ChapSecret(chapSecret) {
        this.chapSecret = chapSecret;
    }

    get ChapMutualUsername() {
        return this.chapMutualUsername;
    }

    set ChapMutualUsername(chapMutualUsername) {
        this.chapMutualUsername = chapMutualUsername;
    }

    get ChapMutualSecret() {
        return this.chapMutualSecret;
    }

    set ChapMutualSecret(chapMutualSecret) {
        this.chapMutualSecret = chapMutualSecret;
    }

    get InitiatorIQN() {
        return this.initiatorIQN;
    }

    set InitiatorIQN(initiatorIQN) {
        this.initiatorIQN = initiatorIQN;
    }

    AddLun(lun) {
        this.lunsCount++;
        this.luns = this.luns || [];
        this.luns.push(
            {
                'LUN': this.lunsCount,
                'LogicalDrive': {
                    '@odata.id': lun.GetBaseUri()
                }
            });
        return this;
    }
}

function Chap() {
    this.Type = 'Mutual';
    this.Username = 'CHAP username';
    this.Secret = null;
    this.MutualUsername = 'MutualUsername username';
    this.MutualSecret = null;
}

exports.RemoteTarget = RemoteTarget;
