"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

from cts_framework.configuration.configuration_file_reader import ConfigurationFileReader

from cts_framework.db.model.base import Base

GENERAL = "GENERAL"
SQL_CONNECTION_STRING = "SQL_CONNECTION_STRING"


class DatabaseConnectionHandler:
    def __init__(self):
        self.engine = create_engine(DatabaseConnectionHandler._connection_string, echo=False)
        Base.metadata.create_all(self.engine)
        self.session = sessionmaker(bind=self.engine)
        self.session.configure(bind=self.engine)
        self.session = self.session()


    @staticmethod
    def initialize_db_connection_handler():
        DatabaseConnectionHandler._connection_string = ConfigurationFileReader().read_file()[GENERAL][
            SQL_CONNECTION_STRING]
