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

class Collection {
    constructor(mock, options) {
        this.mock = mock;
        this.options = options;
        this.onElementAddedCallback = options['OnElementAddedCallback'];
        this.onElementRemovedCallback = options['OnElementRemovedCallback'];
        this.InitProperties();
    }

    get Context(){
        return this.options['Context'];
    }

    InitProperties() {
        this.count = 0;
        this.collectionMembers = [];
        this.collectionTemplate = () => {
            return {
                '@odata.context': this.options['@odata.context'] || '',
                '@odata.id': this.options['@odata.id'] || '',
                '@odata.type': this.options['@odata.type'],
                'Name': this.options['Name'] || 'Collection',
                'Description': this.options['Description'],
                'Members@odata.count': this.Size(),
                'Members': this.GetODataIdsCollection(),
                'Oem:': {}
            };
        };
        this.mock.mockGet(this.options['@odata.id'], 200, undefined, (req, response) => {
            response.body = this.collectionTemplate();
            return response;
        });
    }

    GetODataIdsCollection() {
        var oDataIdsCollection = [];

        for (var i = 0; i < this.collectionMembers.length; i++) {
            oDataIdsCollection.push({'@odata.id': this.collectionMembers[i].GetBaseUri()})
        }

        return oDataIdsCollection;
    };

    Add(properties) {
        return this.AddWithId(this.count + 1, properties);
    };

    AddWithPrefix(prefix, properties) {
        return this.AddWithId(prefix + (this.count + 1), properties);
    }

    AddWithId(id, properties) {
        this.count++;
        var Resource = this.options['Resource'];
        var context = this.options['Context'];
        var resource = new Resource(this.mock, context, id, properties);
        this.collectionMembers.push(resource);
        if (this.onElementAddedCallback) {
            this.onElementAddedCallback(resource.GetBaseUri());
        }
        return resource;
    };

    AddWithPrefixAndResource(prefix, resourceType, properties) {
        this.count++;
        var context = this.options['Context'];
        var resource = new resourceType(this.mock, context, `${prefix}${this.count}`, properties);
        this.collectionMembers.push(resource);
        return resource;
    }

    RemoveAll() {
        this.collectionMembers = [];
    };

    GetPositionInMembersCollection(odataId) {
        for (var i = 0; i < this.collectionMembers.length; i++) {
            if (this.collectionMembers[i].GetBaseUri() == odataId) {
                return i;
            }
        }
        return 0;
    };

    GetById(id) {
        for (var i = 0; i < this.collectionMembers.length; i++) {
            if (this.collectionMembers[i].Id == id) {
                return this.collectionMembers[i];
            }
        }
    };

    GetByODataId(oDataId) {
        for (var i = 0; i < this.collectionMembers.length; i++) {
            if (this.collectionMembers[i].GetBaseUri() == oDataId) {
                return this.collectionMembers[i];
            }
        }
    }

    GetByDescription(description) {
        for (var i = 0; i < this.collectionMembers.length; i++) {
            if (this.collectionMembers[i].Description == description) {
                return this.collectionMembers[i];
            }
        }
    };

    RemoveWithOdataId(odataId) {
        if (this.collectionMembers.length > 0) {
            var index = this.GetPositionInMembersCollection(odataId);
            this.collectionMembers.splice(index, 1);
            if (this.onElementRemovedCallback) {
                this.onElementRemovedCallback(odataId);
            }
        }
    };

    Size() {
        return this.collectionMembers.length;
    }

    GetElements() {
        return this.collectionMembers;
    }

    GetAllByProperty(propertyName, propertyValue) {
        return this.collectionMembers.filter(function(element) {
            return element[propertyName] === propertyValue;
        });
    }

    EnablePostAndDelete(postCallback, deleteCallback) {
        this.mock.mockPost(this.options['@odata.id'], 201, undefined, (req, response) => {
            var body = JSON.parse(req.body);
            var resource = postCallback(body);
            response.headers = {'Location': this.mock.getBaseUri() + resource.GetBaseUri()};

            this.mock.mockDelete(resource.GetBaseUri(), 204, undefined, (req, response) => {
                var index = this.collectionMembers.map((e) => e.GetBaseUri()).indexOf(resource.GetBaseUri());

                if (index > -1) {
                    if (deleteCallback !== undefined) {
                        deleteCallback(resource.GetBaseUri());
                    }
                    this.collectionMembers.splice(index, 1);
                }
                return response;
            });

            return response;
        });
    }

    EnableDenyingPost(callback) {
        this.mock.mockPost(this.options['@odata.id'], 400, undefined, (req, response) => {
            return callback(response);
        });
    }

    Clear() {
        this.collectionMembers = [];
    }
}

exports.Collection = Collection;
