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

const OdataIdArray = require('./odataIdArray').OdataIdArray;

class Redundancy {
    constructor(baseUri, id){
        this.baseUri = baseUri;
        this.id = id;
        this.redundancySet = new OdataIdArray();
    }

    Id() {
        return this.id;
    }

    GetBaseUri() {
        return this.baseUri + '#/Redundancy/' + this.Id();
    }

    Json(){
        return {
            '@odata.id': this.GetBaseUri(),
            'MemberId': this.Id(),
            'Name': this.Name || 'Redundancy',
            'RedundancyEnabled': false,
            'RedundancySet': this.redundancySet,
            'Mode': 'N+m',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'MinNumNeeded': 1,
            'MaxNumSupported': 2
        }
    }

    set Status(val) {
        this.status = val;
    }

    get Status() {
        return this.status;
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

    AddToRedundancySet(element) {
        this.redundancySet.AddWithODataId(element.GetBaseUri());
    }
}

exports.Redundancy = Redundancy;
