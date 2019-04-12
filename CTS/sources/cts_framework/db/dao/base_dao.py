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

from cts_framework.db.database_connection_handler import DatabaseConnectionHandler
from sqlalchemy.exc import OperationalError

class BaseDAO:
    @staticmethod
    def database_operation(func):
        def wrapped_function(*params, **kwargs):
            database_connection_handler = DatabaseConnectionHandler()

            kwargs["database_session"] = database_connection_handler.session

            while True:
                try:
                    return func(*params, **kwargs)
                except OperationalError:
                    continue
                else:
                    database_connection_handler.session.close()

        return wrapped_function
