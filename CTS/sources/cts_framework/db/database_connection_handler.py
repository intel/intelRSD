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

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

from cts_framework.commons.logging_helper import LoggingHelper
from cts_framework.configuration.configuration_file_reader import ConfigurationFileReader
from cts_framework.db.model.base import Base
import cts_framework.db.model

GENERAL = "GENERAL"
SQL_CONNECTION_STRING = "SQL_CONNECTION_STRING"


class DatabaseConnectionHandler:
    engine = None

    def __init__(self):
        session = sessionmaker(bind=DatabaseConnectionHandler.engine, autocommit=True, autoflush=True)
        session.configure(bind=DatabaseConnectionHandler.engine)
        self.session = session()

    @staticmethod
    def initialize_db_connection_handler():
        logger = LoggingHelper(__name__)
        logger.log_debug("initializing database")
        try:
            DatabaseConnectionHandler._connection_string = ConfigurationFileReader().read_file()[GENERAL][
                SQL_CONNECTION_STRING]
            DatabaseConnectionHandler.engine = create_engine(DatabaseConnectionHandler._connection_string, echo=False)
            Base.metadata.create_all(DatabaseConnectionHandler.engine)
            logger.log_debug("database initialization success")
        except KeyError:
            message = "Unable to read %s flag from section %s" % (SQL_CONNECTION_STRING, GENERAL)
            print message
            logger.log_critical(message)
            exit(1)
