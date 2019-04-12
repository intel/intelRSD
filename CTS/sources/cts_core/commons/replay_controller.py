"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""
import json
from os import environ, getenv

import sys
from simplejson.scanner import JSONDecodeError

from cts_core.commons.error import cts_error, cts_warning
from cts_framework.db.dao.http_request_dao import HttpRequestDAO


def sanitize_json(d):
    to_ignore = [
        'hooks',
        'headers',
        'cert',
        'auth',
        'verify'
    ]
    return {k: v for k, v in d.iteritems() if k not in to_ignore}

def dictionaries_equal(dict1, dict2):
    dict1_str = json.dumps(sanitize_json(dict1))
    dict2_str = json.dumps(sanitize_json(dict2))

    return dict1_str == dict2_str

class ReplayController:
    _replay_mode_on = False
    _script_execution_id = None
    _http_request_ids = None
    CTS_REPLAY_SCRIPT_EXECUTION_ID = "CTS_REPLAY_SCRIPT_EXECUTION_ID"

    @classmethod
    def initialize(cls):
        cls._replay_mode_on = False
        cls._script_execution_id = None
        cls._http_request_ids = None
        cls._cursor = 0
        if getenv(ReplayController.CTS_REPLAY_SCRIPT_EXECUTION_ID, None):
            cls._script_execution_id = int(environ[ReplayController.CTS_REPLAY_SCRIPT_EXECUTION_ID])
            if cls._script_execution_id is not None:
                cls._http_request_ids = ReplayController._read_request_ids_from_database(cls._script_execution_id)
                if cls._http_request_ids:
                    cls._replay_mode_on = True
                    print "MESSAGE::Replaying script execution #%d" % cls._script_execution_id

    @staticmethod
    def _read_request_ids_from_database(script_execution_id):
        return HttpRequestDAO.requests_for_script_execution_id(script_execution_id)

    @classmethod
    def replay_mode_on(cls):
        return cls._replay_mode_on

    @classmethod
    def request(cls, http_method, url, **kwargs):
        if not cls.replay_mode_on():
            return None
        if cls._cursor < len(cls._http_request_ids):
            _method, _url, _request, _response, _status_code = HttpRequestDAO.retrieve(cls._http_request_ids[cls._cursor])
            if url != _url:
                cts_error("{url:id} requested url={url} is different than url={_url} " +
                          "that is stored in the database. Quitting Replay mode.",
                          **locals())
                sys.exit("Replay Error")

            try:
                request_obj_from_db = json.loads(_request)
            except (JSONDecodeError, ValueError) as err:
                request_obj_from_db = {}

            if not dictionaries_equal(kwargs, request_obj_from_db):
                request = json.dumps(sanitize_json(kwargs))
                cts_warning("{url:id} request {request} is different than request {_request} from database. ",
                            url=url, request=sanitize_json(kwargs), _request=sanitize_json(request_obj_from_db))
                sys.exit("Replay Error")

            cls._cursor += 1
            return _response

        else:
            cts_error("Reached end of recording. Quitting Replay mode.")
            cls._replay_mode_on = False
            return None

