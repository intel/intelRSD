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
from sqlalchemy.orm import relationship

from cts_framework.db.model.base import Base


class RunModel(Base):
    __tablename__ = "test_run"

    id = Column("id", Integer, primary_key=True)
    tag = Column("tag", String())
    cmd = Column("cmd", String())
    run_datetime = Column("run_datetime", DateTime, default=datetime.now, onupdate=datetime.now)

    script_executions = relationship("ScriptExecutionModel", back_populates='run',
                                     cascade="all, delete, delete-orphan")
