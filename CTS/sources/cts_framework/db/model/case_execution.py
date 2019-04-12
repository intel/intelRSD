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


class CaseExecutionModel(Base):
    __tablename__ = "test_case_execution"

    id = Column("id", Integer, primary_key=True)
    script_execution_id = Column("script_execution_id", Integer, ForeignKey('script_execution.id'))
    name = Column("name", String(100))
    status = Column("status", String(35))

    script_execution = relationship("ScriptExecutionModel", back_populates="case_executions")
    messages = relationship("MessageModel", back_populates='case_execution',
                                     cascade="all, delete, delete-orphan")
