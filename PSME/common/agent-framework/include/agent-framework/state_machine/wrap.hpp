/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @brief Wrappers for member functions
 * */

#pragma once

namespace agent_framework {
namespace state_machine {

/*!
 * Usually for wrapping member functions std::mem_fn(...) may be used. It crates a function whose first parameter
 * is the object instance used to call provided member function. Later std::bind(...) may be used to bind this parameter
 * with a given object instance (like in the wrap templates below). However all other parameters have to be marked using
 * placeholders... which may be tedious:
 *     for a member function that is called without any parameters:
 *         std::bind(std::mem_fn(pointer-to-member), object_instance)
 *     for a function with one parameter:
 *         std::bind(std::mem_fn(pointer-to-member), object_instnace, std::placeholders::_1)
 *     for a function with two parameters:
 *         std::bind(std::mem_fn(pointer-to-member), object_instnace, std::placeholders::_1, std::placeholders::_2)
 *     etc...
 * To make it work for any arbitrary number of parameters, these 'wrap' functions work with variadic templates.
 * Nothing has to be changed while changing the number of parameters of the wrapped member functions.
 */

/*!
 * @brief Generic wrapper for member functions so they can be used like static functions
 * @param[in] obj 'this' pointer for the object which will be used to call member methods
 * @param[in] pm Pointer to the member function
 * @return Function that performs `return (obj->*pm)(...)` operation
 */
template <typename T, typename... Args, typename R>
constexpr std::function<R(Args...)> wrap(T* obj, R (T::*pm) (Args... args)) {
    return [obj, pm](Args... args) -> R { return (obj->*pm)(args...); };
}

/*!
 * @brief Specialization of the wrapper for methods that are not returning any values
 * @param[in] obj 'this' pointer for the object which will be used to call member methods
 * @param[in] pm Pointer to the member function
 * @return Function that performs `(obj->*pm)(...)` operation
 */
template <typename T, typename... Args>
constexpr std::function<void(Args...)> wrap(T* obj, void (T::*pm) (Args... args)) {
    return [obj, pm](Args... args) { (obj->*pm)(args...); };
}

}
}
