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

const http = require('http');
const httpStatusCode = require('../utils/httpStatusCode').httpStatusCode;
const debug = require('debug')('simulators');

module.exports = {
    WirelessMock: function (options) {
        var opt = options || {};
        var port = opt.port || 9443;

        var mocks = {};

        var server = {};
        var ip = '127.0.0.1';
        return {
            mockGet: function (url, code, data, callback) {
                createMock(mocks, url, 'GET', code, data, callback);
                return this;
            },
            mockGetWithDynamicBody: function (url, callback) {
                createMock(mocks, url, 'GET', httpStatusCode.OK, undefined, function (req, response) {
                    response.body = callback();
                    return response;
                });
                return this;
            },
            mockPost: function (url, code, data, callback) {
                createMock(mocks, url, 'POST', code, data, callback);
                return this;
            },
            mockDelete: function (url, code, data, callback) {
                createMock(mocks, url, 'DELETE', code, data, callback);
                return this;
            },
            mockPatch: function (url, code, data, callback) {
                createMock(mocks, url, 'PATCH', code, data, callback);
                return this;
            },
            startListening: function (callback) {
                server = CreateServer(mocks, port);
                server.on('listening', function() {
                    //we obtain port from server because it may be selected randomly by OS (if provided port value was 0)
                    port = server.address().port;
                    debug('Mock server running at http://%s:%s/', ip, port);
                    if (typeof callback === 'function') {
                        callback();
                    }
                });
            },
            getBaseUri: function () {
                return `http://${ip}:${port}`;
            },
            stop: function () {
                server.close();
                _openedSockets.forEach(function (socket) {
                    socket.destroy();
                });
            }
        }
    }
};

var _openedSockets = [];

var createMock = function (mocks, url, method, code, body, callback) {
    mocks[url] = mocks[url] || {};

    if (method == 'GET' && mocks[url][method] !== undefined && mocks[url][method].body !== undefined) {
        body = mocks[url][method].body;
    }

    mocks[url][method] = {
        body: body,
        code: code,
        headers: undefined,
        callback: callback
    };
};

var removeLastSlash = function (url) {
    if (url !== '/' && url.slice(-1) === '/') {
        url = url.substring(0, url.length - 1);
    }
    return url;
};

var CreateServer = function (mocks, port) {
    var server = http.createServer(function (req, res) {
        createRequestListener(req, res, mocks);
    });

    server.on('connection', function (socket) {
        _openedSockets.push(socket);

        socket.on('close', function () {
            var index = _openedSockets.indexOf();
            if (index > -1) {
                _openedSockets.splice(index, 1);
            }
        });
    });

    server.listen(port, '0.0.0.0');

    return server;
};

var createRequestListener = function (req, res, mocks) {
    req.url = removeLastSlash(req.url);

    var requested = mocks[req.url];

    if (requested === undefined) {
        res.writeHead(httpStatusCode.NOT_FOUND, {'Content-Type': 'application/json'});
        res.end("{}");
        return;
    }

    var response = requested[req.method];

    req.body = "";
    req.on('data', function (chunk) {
        req.body += chunk;
    });

    req.on('end', function () {
        if (response === undefined) {
            console.log('Undefined behaviour for: ' + req.method + ' ' + req.url + ' ' + req.body);
            res.writeHead(httpStatusCode.METHOD_NOT_ALLOWED, {'Content-Type': 'application/json'});
            res.end("{}");
            return;
        }

        if (response.callback !== undefined) {
            response = response.callback(req, response);
        }

        if (response.headers === undefined) {
            res.writeHead(response.code, {'Content-Type': 'application/json'});
        }
        else {
            response.headers["Content-Type"] = 'application/json';
            res.writeHead(response.code, response.headers);
        }

        res.end(JSON.stringify(response.body, null, 2));
    });
};