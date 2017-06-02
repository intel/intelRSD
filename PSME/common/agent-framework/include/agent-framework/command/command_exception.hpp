/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 *
 * @file command_exception.hpp
 *
 * @brief Command exception interface
 * */

#pragma once
#include <string>
#include <exception>

namespace agent_framework {
namespace command {
namespace exception {

/*!
 * @brief Command exception interface
 * */
class Exception : public std::exception {
private:
    const char* m_what_message = nullptr;
    Exception& operator=(const Exception&) = delete;
public:
    /*! Default copy constructor */
    Exception(const Exception&) = default;

    /*!
     * @brief Object with what explanatory string
     * @param[in]   what_arg    String explantory for what method
     * */
    explicit Exception(const char* what_arg) : m_what_message(what_arg) { }

    /*!
     * @brief Get exception what explanatory
     *
     * @return  Explanatory string
     * */
    virtual const char* what() const noexcept;

    virtual ~Exception();
};

/*!
 * @brief Command throw exception NotImplemented if execute method is not
 * implemented
 * */
class NotImplemented : public Exception {
public:
    using Exception::Exception;

    /*! Default constructor */
    NotImplemented();

    /*! Default copy constructor */
    NotImplemented(const NotImplemented&) = default;

    virtual ~NotImplemented();
};

/*!
 * @brief Command throw exception NotFound if command is not found in the
 * command list
 * */
class NotFound : public Exception {
public:
    using Exception::Exception;

    /*! Default constructor */
    NotFound();

    /*! Default copy constructor */
    NotFound(const NotFound&) = default;

    virtual ~NotFound();
};

/*!
 * @brief Command throw exception InvalidArgument if execute method has
 * invalid arguments
 * */
class InvalidArgument : public Exception {
public:
    using Exception::Exception;

    /*! Default constructor */
    InvalidArgument();

    /*! Default copy constructor */
    InvalidArgument(const InvalidArgument&) = default;

    virtual ~InvalidArgument();
};

/*!
 * @brief Command throw exception InvalidCommand if execute method has
 * invalid command object
 * */
class InvalidCommand : public Exception {
public:
    using Exception::Exception;

    /*! Default constructor */
    InvalidCommand();

    /*! Default copy constructor */
    InvalidCommand(const InvalidCommand&) = default;

    virtual ~InvalidCommand();
};

/*!
 * @brief Command throw exception OverrideCommand when new command may
 * override existing command in the command list with Register class or
 * add method
 * */
class OverrideCommand : public Exception {
public:
    using Exception::Exception;

    /*! Default constructor */
    OverrideCommand();

    /*! Default copy constructor */
    OverrideCommand(const OverrideCommand&) = default;

    virtual ~OverrideCommand();
};

} /* namespace exception */
} /* namespace command */
} /* namespace agent_framework */

