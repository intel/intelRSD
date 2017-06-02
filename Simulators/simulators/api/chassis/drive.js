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

const BaseAsset = require('../common/baseAsset').BaseAsset;
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;
const OdataIdArray = require('../common/odataIdArray').OdataIdArray;

const secureEraseTimeInSeconds = 10000;

class Drive extends BaseAsset {
    constructor(mock, chassis, id) {
        super(mock, chassis, id);
        this.MockGetWithDynamicBody();

        this.endpoints = new OdataIdArray();
        this.volumes = new OdataIdArray();

        mock.mockPost(this.GetBaseUri() + '/Actions/Drive.SecureErase',
            httpStatusCode.NO_CONTENT,
            undefined,
            (req, response) => {
                this.SecureErase();
                return response;
            }
        );

        mock.mockPatch(this.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            (req, response) => {
                var jsonRequest = JSON.parse(req.body);
                this.AssetTag = jsonRequest.AssetTag || this.AssetTag;

                if (jsonRequest.Oem != undefined && jsonRequest.Oem.Intel_RackScale != undefined) {
                    var newDriveErased = jsonRequest.Oem.Intel_RackScale.DriveErased;
                    if (newDriveErased !== undefined) {
                        this.DriveErased = newDriveErased;
                    }
                }

                response.body = this.CreateJsonTemplate();
                return response;
            }
        );
    }

    GetBaseUri() {
        return super.GetBaseUri('Drives');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Members/Drives/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Drive.v1_0_0.Drive',
            'Name': 'Drive name',
            'Description': 'Drive description',
            'Id': this.Id,
            'IndicatorLED': this.IndicatorLED || null,
            'Model': this.Model || null,
            'Status': this.Status || {'State': 'Enabled', 'Health': 'OK'},
            'CapacityBytes': this.CapacityBytes || null,
            'Protocol': this.Protocol || null,
            'MediaType': this.MediaType || null,
            'Manufacturer': this.Manufacturer || null,
            'SerialNumber': this.SerialNumber || null,
            'PartNumber': this.PartNumber || null,
            'SKU': this.SKU || null,
            'StatusIndicator': this.StatusIndicator || null,
            'Revision': this.Revision || null,
            'FailurePredicted': this.FailurePredicted || false,
            'AssetTag': this.AssetTag || null,
            'CapableSpeedGbs': this.CapableSpeedGbs || null,
            'NegotiatedSpeedGbs': this.NegotiatedSpeedGbs || null,
            'Location': this.Location || [ ],
            'Identifiers': this.Identifiers || [ ],
            'HotspareType': this.HotspareType || null,
            'EncryptionAbility': this.EncryptionAbility || null,
            'EncryptionStatus': this.EncryptionStatus || null,
            'RotationSpeedRPM': this.RotationSpeedRPM || null,
            'BlockSizeBytes': this.BlockSizeBytes || null,
            'PredictedMediaLifeLeftPercent': this.PredictedMediaLifeLeftPercent || null,
            'Links': {
                '@odata.type': '#Drive.v1_1_0.Links',
                'Volumes': this.AddVolumes || [],
                'Endpoints': this.Endpoints || [],
                'Oem': {
                    'Intel_RackScale': {
                        '@odata.type': '#Intel.Oem.Drive',
                        'PCIeEndpoint': this.PCIeEndpoint || null
                    }
                }
            },
            'Actions': {
                '#Drive.SecureErase': {
                    'Target': this.GetBaseUri() + '/Actions/Drive.SecureErase'
                }
            },
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.Drive',
                    'EraseOnDetach': null,
                    'FirmwareVersion': '1.17',
                    'DriveErased': this.DriveErased,
                    'PCIeFunction': this.PcieFunction || null,
                    'Storage': this.Storage || null
                }
            }
        };
    }

    get Location() {
        return this.location;
    }

    set Location(location) {
        this.location = location;
    }

    get BlockSizeBytes() {
        return this.blockSizeBytes;
    }

    set BlockSizeBytes(blockSizeBytes) {
        this.blockSizeBytes = blockSizeBytes;
    }

    get HotspareType() {
        return this.hotspareType;
    }

    set HotspareType(hotspareType) {
        this.hotspareType = hotspareType;
    }

    get PcieFunction() {
        return this.pcieFunction;
    }

    AddPcieFunction(pcieFunction) {
        this.pcieFunction = {'@odata.id': pcieFunction.GetBaseUri()};
    }

    get Storage() {
        return this.storage;
    }

    AddStorage(storage) {
        this.storage = {'@odata.id': storage.GetBaseUri()};
    }

    get EncryptionAbility() {
        return this.encryptionAbility;
    }

    set EncryptionAbility(encryptionAbility) {
        this.encryptionAbility = encryptionAbility;
    }

    get EncryptionStatus() {
        return this.encryptionStatus;
    }

    set EncryptionStatus(encryptionStatus) {
        this.encryptionStatus = encryptionStatus;
    }

    get RotationSpeedRPM() {
        return this.rotationSpeedRPM;
    }

    set RotationSpeedRPM(rotationSpeedRPM) {
        this.rotationSpeedRPM = rotationSpeedRPM;
    }

    get PredictedMediaLifeLeftPercent() {
        return this.predictedMediaLifeLeftPercent;
    }

    set PredictedMediaLifeLeftPercent(predictedMediaLifeLeftPercent) {
        this.predictedMediaLifeLeftPercent = predictedMediaLifeLeftPercent;
    }

    get Identifiers() {
        return this.identifiers;
    }

    set Identifiers(identifiers) {
        this.identifiers = identifiers;
    }

    get Model() {
        return this.model;
    }

    set Model(val) {
        this.model = val;
    }

    get IndicatorLED() {
        return this.indicatorLED;
    }

    set IndicatorLED(val) {
        this.indicatorLED = val;
    }

    get Protocol() {
        return this.protocol;
    }

    set Protocol(val) {
        this.protocol = val;
    }

    get MediaType() {
        return this.mediaType;
    }

    set MediaType(val) {
        this.mediaType = val;
    }

    get Manufacturer() {
        return this.manufacturer;
    }

    set Manufacturer(val) {
        this.manufacturer = val;
    }

    get SerialNumber() {
        return this.serialNumber;
    }

    set SerialNumber(val) {
        this.serialNumber = val;
    }

    get PartNumber() {
        return this.partNumber;
    }

    set PartNumber(val) {
        this.partNumber = val;
    }

    get SKU() {
        return this.sku;
    }

    set SKU(val) {
        this.sku = val;
    }

    get StatusIndicator() {
        return this.statusIndicator;
    }

    set StatusIndicator(val) {
        this.statusIndicator = val;
    }

    get Revision() {
        return this.revision;
    }

    set Revision(val) {
        this.revision = val;
    }

    get FailurePredicted() {
        return this.failurePredicted;
    }

    set FailurePredicted(val) {
        this.failurePredicted = val;
    }

    get AssetTag() {
        return this.assetTag;
    }

    set AssetTag(val) {
        this.assetTag = val;
    }

    get CapableSpeedGbs() {
        return this.capableSpeedGbs;
    }

    set CapableSpeedGbs(val) {
        this.capableSpeedGbs = val;
    }

    get NegotiatedSpeedGbs() {
        return this.negotiatedSpeedGbs;
    }

    set NegotiatedSpeedGbs(val) {
        this.negotiatedSpeedGbs = val;
    }

    get CapacityBytes() {
        return this.sizeBytes;
    }

    set CapacityBytes(sizeBytes) {
        this.sizeBytes = sizeBytes;
    }

    get PCIeEndpoint() {
        return this.pcieEndpoint;
    }

    set PCIeEndpoint(endpoint) {
        this.pcieEndpoint = {'@odata.id': endpoint.GetBaseUri()};
    }

    get Status() {
        return this.status;
    }

    set Status(status) {
        this.status = status;
        this.Parent.Parent.EventTrigger.FireStatusChangedEvent(this.GetBaseUri()); //FIXME: dirty hack
    }

    get DriveErased() {
        return this.driveErased;
    }

    set DriveErased(driveErased) {
        this.driveErased = driveErased;
    }

    SecureErase() {
        this.Status = {'State': 'Starting', 'Health': 'OK'};
        setTimeout(() => {
            this.DriveErased = true;
            this.Status = {'State': 'Enabled', 'Health': 'OK'};
        }, secureEraseTimeInSeconds);
    }

    get Volumes () {
        return this.volumes;
    }

    AddVolumes (elements) {
        this.volumes.Add(elements);
    }

    get Endpoints () {
        return this.endpoints;
    }

    AddEndpoints (elements) {
        this.endpoints.Add(elements);
    }
}

exports.Drive = Drive;