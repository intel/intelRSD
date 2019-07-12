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

const OdataIdArray = require('../../common/odataIdArray').OdataIdArray;

class Fan {
    constructor(baseUri, id){
        this.baseUri = baseUri;
        this.id = id;
        this.relatedItems = new OdataIdArray();
        this.redundancy = new OdataIdArray();
    }

    Id() {
        return this.id;
    }

    GetBaseUri() {
        return this.baseUri + '#/Fans/' + this.Id();
    }

    Json(){
        return {
            '@odata.id': this.GetBaseUri(),
            'MemberId': this.Id(),
            'Name': this.Name || 'BaseBoard System Fan',
            'PhysicalContext': 'Backplane',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Reading': 2100,
            'ReadingUnits': 'RPM',
            'UpperThresholdNonCritical': 42,
            'UpperThresholdCritical': 4200,
            'UpperThresholdFatal': 42,
            'LowerThresholdNonCritical': 42,
            'LowerThresholdCritical': 5,
            'LowerThresholdFatal': 42,
            'MinReadingRange': 0,
            'MaxReadingRange': 5000,
            'Redundancy' : this.redundancy,
            'RelatedItem' : this.relatedItems
        }
    }

    AddToRelatedItems(element) {
        this.relatedItems.AddWithODataId(element.GetBaseUri());
    }

    AddToRedundancySet(element) {
        this.redundancy.AddWithODataId(element.GetBaseUri());
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

}

exports.Fan = Fan;
