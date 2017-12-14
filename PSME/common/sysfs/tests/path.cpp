/*!
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file tests/path.cpp
 */

#include "sysfs/path.hpp"

#include "gtest/gtest.h"

using namespace sysfs;

namespace {
    void check_flags(const Path& p, bool should_be_absolute, bool should_be_empty) {
        EXPECT_EQ(should_be_absolute, p.is_absolute());
        EXPECT_EQ(!should_be_absolute, p.is_relative());
        EXPECT_EQ(should_be_empty, p.is_empty());
    }
}

namespace testing {

TEST(PathTests, EmptyRelativePath) {
    Path empty{};
    check_flags(empty, false, true);
    EXPECT_EQ("", empty.to_string());
    EXPECT_EQ("", empty.basename());
}

TEST(PathTests, EmptyAbsolutePath) {
    Path empty = Path::root();
    check_flags(empty, true, true);
    EXPECT_EQ("/", empty.to_string());
    EXPECT_EQ("", empty.basename());
}

TEST(PathTests, RelativePathDeserialization) {
    Path path{};

    path = Path("");
    EXPECT_EQ(path.to_string(), "");
    check_flags(path, false, true);

    path = Path("test");
    EXPECT_EQ(path.to_string(), "test");
    check_flags(path, false, false);

    path = Path("test/");
    EXPECT_EQ(path.to_string(), "test");
    check_flags(path, false, false);

    path = Path("test/path");
    EXPECT_EQ(path.to_string(), "test/path");
    check_flags(path, false, false);

    path = Path("test/path/");
    EXPECT_EQ(path.to_string(), "test/path");
    check_flags(path, false, false);

    path = Path("test//path/");
    EXPECT_EQ(path.to_string(), "test/path");
    check_flags(path, false, false);

    path = Path("test//path//");
    EXPECT_EQ(path.to_string(), "test/path");
    check_flags(path, false, false);

    path = Path("test/./path//");
    EXPECT_EQ(path.to_string(), "test/path");
    check_flags(path, false, false);

    path = Path("test/./path//.");
    EXPECT_EQ(path.to_string(), "test/path");
    check_flags(path, false, false);
}

TEST(PathTests, AbsolutePathDeserialization) {
    Path path{};

    path = Path("/");
    EXPECT_EQ(path.to_string(), "/");
    check_flags(path, true, true);

    path = Path("/test");
    EXPECT_EQ(path.to_string(), "/test");
    check_flags(path, true, false);

    path = Path("/test/");
    EXPECT_EQ(path.to_string(), "/test");
    check_flags(path, true, false);

    path = Path("/test/path");
    EXPECT_EQ(path.to_string(), "/test/path");
    check_flags(path, true, false);

    path = Path("/test/path/");
    EXPECT_EQ(path.to_string(), "/test/path");
    check_flags(path, true, false);

    path = Path("/test//path/");
    EXPECT_EQ(path.to_string(), "/test/path");
    check_flags(path, true, false);

    path = Path("/test//path//");
    EXPECT_EQ(path.to_string(), "/test/path");
    check_flags(path, true, false);

    path = Path("/test/./path//");
    EXPECT_EQ(path.to_string(), "/test/path");
    check_flags(path, true, false);

    path = Path("/test/./path//.");
    EXPECT_EQ(path.to_string(), "/test/path");
    check_flags(path, true, false);
}

TEST(PathTests, ComplexPathsDeserialization) {
    Path path{};

    path = Path("/test/1////../../");
    EXPECT_EQ(path.to_string(), "/");
    check_flags(path, true, true);

    path = Path("/./././././");
    EXPECT_EQ(path.to_string(), "/");
    check_flags(path, true, true);

    path = Path("/////test/.");
    EXPECT_EQ(path.to_string(), "/test");
    check_flags(path, true, false);

    path = Path("/test/..");
    EXPECT_EQ(path.to_string(), "/");
    check_flags(path, true, true);

    EXPECT_THROW(Path("/test/../.."), std::logic_error);

    path = Path("test/../..");
    EXPECT_EQ(path.to_string(), "..");
    check_flags(path, false, false);
}

TEST(PathTests, RelativePathPush) {
    Path p1{};
    p1.push_right("this").push_right("is").push_right("a").push_right("path");
    EXPECT_EQ(p1.to_string(), "this/is/a/path");
    check_flags(p1, false, false);

    Path p2{};
    p2 << "this" << "is" << "a" << "second" << "path";
    EXPECT_EQ(p2.to_string(), "this/is/a/second/path");
    check_flags(p2, false, false);

    Path p3{};
    p3.push_right("test").push_right(p1).push_right("end");
    EXPECT_EQ(p3.to_string(), "test/this/is/a/path/end");
    check_flags(p3, false, false);

    Path p4{};
    p4 << "test2" << p2 << "end2";
    EXPECT_EQ(p4.to_string(), "test2/this/is/a/second/path/end2");
    check_flags(p4, false, false);

    Path p5{};
    p5.push_right(p4);
    EXPECT_EQ(p5.to_string(), "test2/this/is/a/second/path/end2");
    check_flags(p5, false, false);

    Path p6{};
    p6 << p4;
    EXPECT_EQ(p6.to_string(), "test2/this/is/a/second/path/end2");
    check_flags(p6, false, false);

    Path absolute = Path::root();

    Path p7{};
    EXPECT_THROW(p7.push_right(absolute), std::logic_error);

    Path p8{};
    EXPECT_THROW(p8 << absolute, std::logic_error);

    Path p9{};
    EXPECT_THROW(p9.push_right("test").push_right(absolute), std::logic_error);

    Path p10{};
    EXPECT_THROW(p10 << "test" << absolute, std::logic_error);
}

TEST(PathTests, AbsolutePathPush) {
    Path p1 = Path::root();
    p1.push_right("this").push_right("is").push_right("a").push_right("path");
    EXPECT_EQ(p1.to_string(), "/this/is/a/path");
    check_flags(p1, true, false);

    Path p2 = Path::root();
    p2 << "this" << "is" << "a" << "second" << "path";
    EXPECT_EQ(p2.to_string(), "/this/is/a/second/path");
    check_flags(p2, true, false);

    Path p3 = Path::root();
    EXPECT_THROW(p3.push_right("test").push_right(p1).push_right("end"), std::logic_error);

    Path p4 = Path::root();
    EXPECT_THROW(p4 << "test2" << p2 << "end2", std::logic_error);

    Path p5 = Path::root();
    EXPECT_THROW(p5.push_right(p2), std::logic_error);

    Path p6 = Path::root();
    EXPECT_THROW(p6 << p2, std::logic_error);

    Path relative{};
    relative << "relative" << "path";

    Path p7 = Path::root();
    p7.push_right(relative);
    EXPECT_EQ(p7.to_string(), "/relative/path");
    check_flags(p7, true, false);

    Path p8 = Path::root();
    p8 << relative;
    EXPECT_EQ(p8.to_string(), "/relative/path");
    check_flags(p8, true, false);

    Path p9 = Path::root();
    p9.push_right("test").push_right(relative).push_right("end");
    EXPECT_EQ(p9.to_string(), "/test/relative/path/end");
    check_flags(p9, true, false);

    Path p10 = Path::root();
    p10 << "test" << relative << "end";
    EXPECT_EQ(p10.to_string(), "/test/relative/path/end");
    check_flags(p10, true, false);
}

TEST(PathTests, AbsoluteTopAndPop) {
    Path path = Path::root();
    path << "this" << "is" << "a" << "test";
    EXPECT_EQ(path.to_string(), "/this/is/a/test");
    EXPECT_EQ(path.basename(), "test");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/this/is/a");
    EXPECT_EQ(path.basename(), "a");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/this/is");
    EXPECT_EQ(path.basename(), "is");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/this");
    EXPECT_EQ(path.basename(), "this");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/");
    EXPECT_EQ(path.basename(), "");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/");
    EXPECT_EQ(path.basename(), "");
}

TEST(PathTests, RelativeTopAndPop) {
    Path path;
    path << "this" << "is" << "a" << "test";
    EXPECT_EQ(path.to_string(), "this/is/a/test");
    EXPECT_EQ(path.basename(), "test");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "this/is/a");
    EXPECT_EQ(path.basename(), "a");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "this/is");
    EXPECT_EQ(path.basename(), "is");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "this");
    EXPECT_EQ(path.basename(), "this");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "");
    EXPECT_EQ(path.basename(), "");
    path.pop_right();
    EXPECT_EQ(path.to_string(), "");
    EXPECT_EQ(path.basename(), "");
}

TEST(PathTests, RelativePathNormalization) {
    Path path;
    path << "test" << "path";
    EXPECT_EQ(path.to_string(), "test/path");
    path << "..";
    EXPECT_EQ(path.to_string(), "test");
    path << ".";
    EXPECT_EQ(path.to_string(), "test");
    path << "";
    EXPECT_EQ(path.to_string(), "test");
    path << "..";
    EXPECT_EQ(path.to_string(), "");
    path << "..";
    EXPECT_EQ(path.to_string(), "..");
    path << "path";
    EXPECT_EQ(path.to_string(), "../path");
    path << "..";
    EXPECT_EQ(path.to_string(), "..");
    path << "..";
    EXPECT_EQ(path.to_string(), "../..");
    path << ".";
    EXPECT_EQ(path.to_string(), "../..");
    path << "";
    EXPECT_EQ(path.to_string(), "../..");
}

TEST(PathTests, PathSumsWithConstructors) {
    Path p{};
    p = Path("/test/path") / Path("some/more");
    EXPECT_EQ(p.to_string(), "/test/path/some/more");

    p = Path("test/path") / Path("some/more");
    EXPECT_EQ(p.to_string(), "test/path/some/more");

    p = Path("/test/path") / Path("../new_path");
    EXPECT_EQ(p.to_string(), "/test/new_path");

    p = Path("test/path") / Path("../new_path");
    EXPECT_EQ(p.to_string(), "test/new_path");

    p = Path("/test/path") / Path(".././");
    EXPECT_EQ(p.to_string(), "/test");

    p = Path("test/path") / Path("../.");
    EXPECT_EQ(p.to_string(), "test");

    EXPECT_THROW(Path("test/path") / Path("/absolute"), std::logic_error);
    EXPECT_THROW(Path("/test/path") / Path("/absolute"), std::logic_error);
}

TEST(PathTests, PathSumsWithRightSideCasting) {
    Path p{};
    p = Path("/test/path") / "some/more";
    EXPECT_EQ(p.to_string(), "/test/path/some/more");

    p = Path("test/path") / "some/more";
    EXPECT_EQ(p.to_string(), "test/path/some/more");

    p = Path("/test/path") / "../new_path";
    EXPECT_EQ(p.to_string(), "/test/new_path");

    p = Path("test/path") / "../new_path";
    EXPECT_EQ(p.to_string(), "test/new_path");

    p = Path("/test/path") / ".././";
    EXPECT_EQ(p.to_string(), "/test");

    p = Path("test/path") / "../.";
    EXPECT_EQ(p.to_string(), "test");

    EXPECT_THROW(Path("test/path") / "/absolute", std::logic_error);
    EXPECT_THROW(Path("/test/path") / "/absolute", std::logic_error);
}

TEST(PathTests, PathSumsWithLeftSideCasting) {
    Path p{};
    p = "/test/path" / Path("some/more");
    EXPECT_EQ(p.to_string(), "/test/path/some/more");

    p = "test/path" / Path("some/more");
    EXPECT_EQ(p.to_string(), "test/path/some/more");

    p = "/test/path" / Path("../new_path");
    EXPECT_EQ(p.to_string(), "/test/new_path");

    p = "test/path" / Path("../new_path");
    EXPECT_EQ(p.to_string(), "test/new_path");

    p = "/test/path" / Path(".././");
    EXPECT_EQ(p.to_string(), "/test");

    p = "test/path" / Path("../.");
    EXPECT_EQ(p.to_string(), "test");

    EXPECT_THROW("test/path" / Path("/absolute"), std::logic_error);
    EXPECT_THROW("/test/path" / Path("/absolute"), std::logic_error);
}

TEST(PathTests, PathCompares) {
    EXPECT_TRUE(Path("/test/path") == "/test/path");
    EXPECT_TRUE(Path("/test/path") == "/test/path/");
    EXPECT_TRUE(Path("/test/path") == "/test/a/../path");
    EXPECT_TRUE(Path("/test/path") == "/test/./path");
    EXPECT_TRUE(Path("/test/path") == "/test/./////path");
    EXPECT_TRUE(Path("/test/path") == "/test/a/../path/../path/");

    EXPECT_TRUE(Path("test/path") != "/test/path");
    EXPECT_TRUE(Path("test/path") != "/test/path/");
    EXPECT_TRUE(Path("test/path") != "/test/a/../path");
    EXPECT_TRUE(Path("test/path") != "/test/./path");
    EXPECT_TRUE(Path("test/path") != "/test/./////path");
    EXPECT_TRUE(Path("test/path") != "/test/a/../path/../path/");
    EXPECT_TRUE(Path("test/path") != "/test/../path");
    EXPECT_TRUE(Path("test/path") != "/teest/path");
}

}
