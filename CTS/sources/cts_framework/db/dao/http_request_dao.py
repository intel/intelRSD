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
import pickle
from collections import OrderedDict

from cts_core.commons.error import cts_warning
from cts_framework.db.dao.base_dao import BaseDAO
from cts_framework.db.model.http_request import HttpRequestModel
from cts_framework.commons.enums import RequestStatus, ReturnCodes


class HttpRequestDAO(BaseDAO):
    @staticmethod
    @BaseDAO.database_operation
    def register_request(script_execution_id, method, url, request, response, status_code, database_session):
        """
        :type script_execution_id: int
        :type method: string
        :type url: string
        :type request: str
        :type response: str
        """
        req = HttpRequestModel()
        req.script_execution_id = script_execution_id
        req.method = method
        req.url = url
        req.request = request
        req.response = response
        req.status_code = status_code

        database_session.add(req)
        database_session.flush()
        database_session.refresh(req)

        return req.id

    @staticmethod
    @BaseDAO.database_operation
    def retrieve(request_id, database_session):
        """
        :type request_id: int
        :rtype: HttpRequestModel
        """
        record = database_session.query(HttpRequestModel).filter(HttpRequestModel.id == request_id).first()

        if record:
            method, url, request, response, status_code = \
                record.method, record.url, record.request, pickle.loads(record.response), record.status_code
            if not request:
                request = None
            return method, url, request, response, status_code

        return None, None, None, None, None

    @staticmethod
    def get_code_request_response(request_id):
        """
        :type request_id: int
        :rtype: HttpRequestModel
        """

        method, url, request, response, status_code = HttpRequestDAO.retrieve(request_id)
        if response is not None:
            req = json.dumps(json.loads(request, object_pairs_hook=OrderedDict), indent=4)

            if not response.text or response.text == str():
                response_text = "{}"
            else:
                response_text = response.text

            if response.status_code in [ReturnCodes.NO_CONTENT, ReturnCodes.CREATED]:
                response_body = dict()
            else:
                response_body = json.loads(response_text, object_pairs_hook=OrderedDict)
            rsp = json.dumps(response_body, indent=4) \
                if response.status_code != ReturnCodes.NO_CONTENT else "No content"

            return status_code, req, rsp
        else:
            return None, None, None

    @staticmethod
    @BaseDAO.database_operation
    def num_of_registered_requests(script_execution_id, database_session):
        num = database_session.query(HttpRequestModel).\
            filter(HttpRequestModel.script_execution_id == script_execution_id).count()
        return num

    @staticmethod
    @BaseDAO.database_operation
    def requests_for_script_execution_id(script_execution_id, database_session):
        #in the http model definition there is an autoincrement attribute associated with 'id' column
        #so we can sort by id to order request from the oldest to the newest
        return sorted([req.id for req in \
                database_session.query(HttpRequestModel).\
                filter(HttpRequestModel.script_execution_id == script_execution_id).all()])




