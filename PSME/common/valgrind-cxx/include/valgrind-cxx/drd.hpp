/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file valgrind-drd.hpp
 *
 * @brief Workaround for testing software using C++11 std::thread with Valgrind DRD.
 * */
#ifndef VALGRIND_CXX_DRD_HPP
#define VALGRIND_CXX_DRD_HPP

#ifdef ENABLE_VALGRIND_DRD

/*
 * Visit the following link for details.
 * http://valgrind.org/docs/manual/drd-manual.html#drd-manual.C++11
 */

#include <valgrind/drd.h>

/*
 * For clang compiler need to disable reserved-id-macro warning.
 */
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE
#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(addr) ANNOTATE_HAPPENS_BEFORE(addr)

#undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER
#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(addr)  ANNOTATE_HAPPENS_AFTER(addr)

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif /* ENABLE_VALGRIND_DRD */

#endif /* VALGRIND_CXX_DRD_HPP */
