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

from datetime import datetime
from sqlalchemy import Column, Integer, String, DateTime
from sqlalchemy import ForeignKey
from sqlalchemy.orm import relationship

from cts_framework.db.model.base import Base


class HttpRequestModel(Base):
    __tablename__ = "http_request"

    id = Column("id", Integer, primary_key=True, autoincrement=True)
    script_execution_id = Column("script_execution_id", Integer, ForeignKey('script_execution.id'))
    method = Column("method", String())
    url = Column("url", String())
    request = Column("request", String())
    response = Column("response", String())
    status_code = Column("status_code", Integer)
    script_execution = relationship("ScriptExecutionModel", back_populates="http_requests")
