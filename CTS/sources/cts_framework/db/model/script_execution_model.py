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

from sqlalchemy import Column, Integer, String
from sqlalchemy import ForeignKey
from sqlalchemy.orm import relationship

from cts_framework.db.model.base import Base


class ScriptExecutionModel(Base):
    __tablename__ = "script_execution"

    id = Column("id", Integer, primary_key=True)
    run_id = Column("run_id", Integer, ForeignKey('test_run.id'))
    script_path = Column("script_path", String(100))
    configuration = Column("configuration", String(1000))
    control_sum = Column("control_sum", String(130))
    pid = Column("pid", Integer)

    run = relationship("RunModel", back_populates="script_executions")

    case_executions = relationship("CaseExecutionModel", back_populates='script_execution',
                                     cascade="all, delete, delete-orphan")
    http_requests = relationship("HttpRequestModel", back_populates='script_execution',
                                     cascade="all, delete, delete-orphan")

