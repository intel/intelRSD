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

import datetime

from sqlalchemy import Column, Integer, String, DateTime
from sqlalchemy import ForeignKey
from sqlalchemy.orm import relationship

from cts_framework.db.model.base import Base


class MessageModel(Base):
    __tablename__ = "message"

    id = Column("id", Integer, primary_key=True)
    case_execution_id = Column("case_execution_id", Integer, ForeignKey('test_case_execution.id'))
    message_level = Column("message_level", String(20))
    message_text = Column("message_text", String(10000))
    message_datetime = Column("message_datetime", DateTime, default=datetime.datetime.now)

    case_execution = relationship("CaseExecutionModel", back_populates="messages")
