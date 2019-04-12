/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file last_template_type.hpp
 *
 * @brief Definition of template struct which obtain last type from template parameter pack.
 * */


#pragma once


/*!
 * @brief Helper class to obtain last type from template parameter pack.
 *
 * @tparam T First type of the template parameter pack.
 * @tparam Ts Rest types of the template parameter pack.
 */
template<typename T, typename... Ts>
struct LastOf {
    typedef typename LastOf<Ts...>::type type;
};

/*!
 * @brief LastOf class specialization for the last recursive call. Gives an access to
 * last parameter type of the template parameter pack.
 *
 * @tparam T Last parameter type of the template parameter pack.
 */
template<typename T>
struct LastOf<T> {
    typedef T type;
};
