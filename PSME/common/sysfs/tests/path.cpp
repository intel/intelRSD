/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file tests/path.cpp
 */

#include "sysfs/path.hpp"
#include "sysfs/construct_dev_path.hpp"

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
    EXPECT_EQ("", empty.dirname().to_string());
}


TEST(PathTests, EmptyAbsolutePath) {
    Path empty = Path::root();
    check_flags(empty, true, true);
    EXPECT_EQ("/", empty.to_string());
    EXPECT_EQ("", empty.basename());
    EXPECT_EQ("/", empty.dirname().to_string());
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


TEST(PathTests, RelativePathAppend) {
    Path p1{};
    p1.append("this").append("is").append("a").append("path");
    EXPECT_EQ(p1.to_string(), "this/is/a/path");
    check_flags(p1, false, false);

    Path p2{};
    p2 << "this" << "is" << "a" << "second" << "path";
    EXPECT_EQ(p2.to_string(), "this/is/a/second/path");
    check_flags(p2, false, false);

    Path p3{};
    p3.append("test").append(p1).append("end");
    EXPECT_EQ(p3.to_string(), "test/this/is/a/path/end");
    check_flags(p3, false, false);

    Path p4{};
    p4 << "test2" << p2 << "end2";
    EXPECT_EQ(p4.to_string(), "test2/this/is/a/second/path/end2");
    check_flags(p4, false, false);

    Path p5{};
    p5.append(p4);
    EXPECT_EQ(p5.to_string(), "test2/this/is/a/second/path/end2");
    check_flags(p5, false, false);

    Path p6{};
    p6 << p4;
    EXPECT_EQ(p6.to_string(), "test2/this/is/a/second/path/end2");
    check_flags(p6, false, false);

    Path absolute = Path::root();

    Path p7{};
    EXPECT_THROW(p7.append(absolute), std::logic_error);

    Path p8{};
    EXPECT_THROW(p8 << absolute, std::logic_error);

    Path p9{};
    EXPECT_THROW(p9.append("test").append(absolute), std::logic_error);

    Path p10{};
    EXPECT_THROW(p10 << "test" << absolute, std::logic_error);
}


TEST(PathTests, AbsolutePathAppend) {
    Path p1 = Path::root();
    p1.append("this").append("is").append("a").append("path");
    EXPECT_EQ(p1.to_string(), "/this/is/a/path");
    check_flags(p1, true, false);

    Path p2 = Path::root();
    p2 << "this" << "is" << "a" << "second" << "path";
    EXPECT_EQ(p2.to_string(), "/this/is/a/second/path");
    check_flags(p2, true, false);

    Path p3 = Path::root();
    EXPECT_THROW(p3.append("test").append(p1).append("end"), std::logic_error);

    Path p4 = Path::root();
    EXPECT_THROW(p4 << "test2" << p2 << "end2", std::logic_error);

    Path p5 = Path::root();
    EXPECT_THROW(p5.append(p2), std::logic_error);

    Path p6 = Path::root();
    EXPECT_THROW(p6 << p2, std::logic_error);

    Path relative{};
    relative << "relative" << "path";

    Path p7 = Path::root();
    p7.append(relative);
    EXPECT_EQ(p7.to_string(), "/relative/path");
    check_flags(p7, true, false);

    Path p8 = Path::root();
    p8 << relative;
    EXPECT_EQ(p8.to_string(), "/relative/path");
    check_flags(p8, true, false);

    Path p9 = Path::root();
    p9.append("test").append(relative).append("end");
    EXPECT_EQ(p9.to_string(), "/test/relative/path/end");
    check_flags(p9, true, false);

    Path p10 = Path::root();
    p10 << "test" << relative << "end";
    EXPECT_EQ(p10.to_string(), "/test/relative/path/end");
    check_flags(p10, true, false);
}


TEST(PathTests, PathPrepends) {
    Path p1{};
    p1.prepend("last").prepend("middle").prepend("first");
    EXPECT_EQ(p1.to_string(), "first/middle/last");
    check_flags(p1, false, false);

    Path p3{};
    p3.prepend("start").prepend(p1).prepend("/end");
    EXPECT_EQ(p3.to_string(), "/end/first/middle/last/start");
    check_flags(p3, true, false);

    Path p5{};
    p5.prepend(p1);
    EXPECT_EQ(p5.to_string(), "first/middle/last");
    check_flags(p5, false, false);

    Path absolute = Path::root();
    Path p7{"test"};
    p7.prepend(absolute);
    EXPECT_EQ(p7.to_string(), "/test");
    check_flags(p7, true, false);

    absolute.append("first");
    Path p8{"second"};
    p8.prepend(absolute);
    EXPECT_EQ(p8.to_string(), "/first/second");
    check_flags(p8, true, false);

    absolute = Path::root();
    Path p9{};
    EXPECT_EQ(p9.prepend("test").prepend(absolute).to_string(), "/test");

    Path p10 = Path::root();
    EXPECT_THROW(p10.prepend(absolute), std::logic_error);

    Path p11 = Path::root();
    EXPECT_THROW(p10.prepend(""), std::logic_error);
}


TEST(PathTests, AbsoluteTopAndPopRight) {
    Path path = Path::root();
    path << "this" << "is" << "a" << "test";
    EXPECT_EQ(path.to_string(), "/this/is/a/test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path("/this/is/a"));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/this/is/a");
    EXPECT_EQ(path.basename(), "a");
    EXPECT_TRUE(path.dirname() == Path("/this/is"));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/this/is");
    EXPECT_EQ(path.basename(), "is");
    EXPECT_TRUE(path.dirname() == Path("/this"));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/this");
    EXPECT_EQ(path.basename(), "this");
    EXPECT_TRUE(path.dirname() == Path("/"));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/");
    EXPECT_EQ(path.basename(), "");
    EXPECT_TRUE(path.dirname() == Path("/"));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "/");
    EXPECT_EQ(path.basename(), "");
    EXPECT_TRUE(path.dirname() == Path("/"));
}


TEST(PathTests, AbsoluteTopAndPopLeft) {
    Path path = Path::root();
    path << "this" << "is" << "a" << "test";
    EXPECT_EQ(path.to_string(), "/this/is/a/test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path("/this/is/a"));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "is/a/test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path("is/a"));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "a/test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path("a"));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path(""));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "");
    EXPECT_EQ(path.basename(), "");
    EXPECT_TRUE(path.dirname() == Path(""));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "");
    EXPECT_EQ(path.basename(), "");
    EXPECT_TRUE(path.dirname() == Path(""));
}


TEST(PathTests, RelativeTopAndPopRight) {
    Path path;
    path << "this" << "is" << "a" << "test";
    EXPECT_EQ(path.to_string(), "this/is/a/test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path("this/is/a"));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "this/is/a");
    EXPECT_EQ(path.basename(), "a");
    EXPECT_TRUE(path.dirname() == Path("this/is"));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "this/is");
    EXPECT_EQ(path.basename(), "is");
    EXPECT_TRUE(path.dirname() == Path("this"));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "this");
    EXPECT_EQ(path.basename(), "this");
    EXPECT_TRUE(path.dirname() == Path(""));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "");
    EXPECT_EQ(path.basename(), "");
    EXPECT_TRUE(path.dirname() == Path(""));
    path.pop_right();
    EXPECT_EQ(path.to_string(), "");
    EXPECT_EQ(path.basename(), "");
    EXPECT_TRUE(path.dirname() == Path(""));
}


TEST(PathTests, RelativeTopAndPopLeft) {
    Path path{};
    path << "this" << "is" << "a" << "test";
    EXPECT_EQ(path.to_string(), "this/is/a/test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path("this/is/a"));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "is/a/test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path("is/a"));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "a/test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path("a"));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "test");
    EXPECT_EQ(path.basename(), "test");
    EXPECT_TRUE(path.dirname() == Path(""));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "");
    EXPECT_EQ(path.basename(), "");
    EXPECT_TRUE(path.dirname() == Path(""));
    path.pop_left();
    EXPECT_EQ(path.to_string(), "");
    EXPECT_EQ(path.basename(), "");
    EXPECT_TRUE(path.dirname() == Path(""));
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


TEST(PathTests, StartsWithTests) {
    // incompatible types
    EXPECT_TRUE(Path("/test/path").starts_with("/"));
    EXPECT_FALSE(Path("/test/path").starts_with(""));
    EXPECT_FALSE(Path("test/path").starts_with("/"));
    EXPECT_TRUE(Path("test/path").starts_with(""));

    // positive cases
    EXPECT_TRUE(Path("/test/path/to/be/checked").starts_with("/"));
    EXPECT_TRUE(Path("/test/path/to/be/checked").starts_with("/test"));
    EXPECT_TRUE(Path("/test/path/to/be/checked").starts_with("/test/path"));
    EXPECT_TRUE(Path("/test/path/to/be/checked").starts_with("/test/path/to"));
    EXPECT_TRUE(Path("/test/path/to/be/checked").starts_with("/test/path/to/be"));
    EXPECT_TRUE(Path("/test/path/to/be/checked").starts_with("/test/path/to/be/checked"));

    EXPECT_TRUE(Path("test/path/to/be/checked").starts_with(""));
    EXPECT_TRUE(Path("test/path/to/be/checked").starts_with("test"));
    EXPECT_TRUE(Path("test/path/to/be/checked").starts_with("test/path"));
    EXPECT_TRUE(Path("test/path/to/be/checked").starts_with("test/path/to"));
    EXPECT_TRUE(Path("test/path/to/be/checked").starts_with("test/path/to/be"));
    EXPECT_TRUE(Path("test/path/to/be/checked").starts_with("test/path/to/be/checked"));

    EXPECT_TRUE(Path("../test/path/to/be/checked").starts_with("../"));
    EXPECT_TRUE(Path("../test/path/to/be/checked").starts_with("../test"));
    EXPECT_TRUE(Path("../../test/path/to/be/checked").starts_with("../../"));
    EXPECT_TRUE(Path("../../test/path/to/be/checked").starts_with("../../test"));

    // too long test path
    EXPECT_FALSE(Path("short/path").starts_with("short/path/long"));
    EXPECT_TRUE(Path("short/path").starts_with("short/path"));
    EXPECT_FALSE(Path("/path").starts_with("/path/long"));
    EXPECT_TRUE(Path("/path").starts_with("/path"));
    EXPECT_FALSE(Path("/").starts_with("/path"));
    EXPECT_FALSE(Path("").starts_with("path"));

    // negative cases
    EXPECT_FALSE(Path("/test/path/to/be/checked").starts_with("/2test"));
    EXPECT_FALSE(Path("/test/path/to/be/checked").starts_with("/test/2path"));
    EXPECT_FALSE(Path("/test/path/to/be/checked").starts_with("/test/path/2to"));
    EXPECT_FALSE(Path("/test/path/to/be/checked").starts_with("/test/path/to/2be"));
    EXPECT_FALSE(Path("/test/path/to/be/checked").starts_with("/test/path/to/be/2checked"));

    EXPECT_FALSE(Path("test/path/to/be/checked").starts_with("2"));
    EXPECT_FALSE(Path("test/path/to/be/checked").starts_with("2test"));
    EXPECT_FALSE(Path("test/path/to/be/checked").starts_with("test/2path"));
    EXPECT_FALSE(Path("test/path/to/be/checked").starts_with("test/path/2to"));
    EXPECT_FALSE(Path("test/path/to/be/checked").starts_with("test/path/to/2be"));
    EXPECT_FALSE(Path("test/path/to/be/checked").starts_with("test/path/to/be/2checked"));

    EXPECT_FALSE(Path("../test/path/to/be/checked").starts_with("2"));
    EXPECT_FALSE(Path("../test/path/to/be/checked").starts_with("../2test"));
    EXPECT_FALSE(Path("../../test/path/to/be/checked").starts_with("../2/"));
    EXPECT_FALSE(Path("../../test/path/to/be/checked").starts_with("2/../test"));
    EXPECT_FALSE(Path("../../test/path/to/be/checked").starts_with("2/../2"));

}


TEST(PathTests, AtTests) {
    // valid "at" calls
    EXPECT_EQ(Path("/path/to/be/tested").at(0), "path");
    EXPECT_EQ(Path("/path/to/be/tested").at(1), "to");
    EXPECT_EQ(Path("/path/to/be/tested").at(2), "be");
    EXPECT_EQ(Path("/path/to/be/tested").at(3), "tested");
    EXPECT_EQ(Path("../to/be/tested").at(0), "..");
    EXPECT_EQ(Path("../to/be/tested").at(1), "to");
    EXPECT_EQ(Path("../to/be/tested").at(2), "be");
    EXPECT_EQ(Path("../to/be/tested").at(3), "tested");

    // invalid index
    EXPECT_THROW(Path("").at(0), std::out_of_range);
    EXPECT_THROW(Path("/").at(0), std::out_of_range);
    EXPECT_THROW(Path("/path/to/be/tested").at(4), std::out_of_range);
    EXPECT_THROW(Path("../to/be/tested").at(4), std::out_of_range);
}


TEST(PathTests, SizeTests) {
    EXPECT_EQ(Path("/").size(), 0);
    EXPECT_EQ(Path("").size(), 0);
    EXPECT_EQ(Path("/test").size(), 1);
    EXPECT_EQ(Path("test").size(), 1);
    EXPECT_EQ(Path("/rather/long/path").size(), 3);
    EXPECT_EQ(Path("rather/long/path").size(), 3);
    EXPECT_EQ(Path(".././test//").size(), 2);

}


TEST(PathTests, PathVariadicConstructorTests) {
    ASSERT_EQ(Path("/test", "path", "1234"), "/test/path/1234");
    ASSERT_EQ(Path("/", "path", "1234"), "/path/1234");
    ASSERT_EQ(Path("", "path", "1234"), "path/1234");
    ASSERT_EQ(Path("/test", "path"), "/test/path");
    ASSERT_EQ(Path("/test"), "/test");
    ASSERT_EQ(Path("/test", "path1", "path2", "path3", "1234"), "/test/path1/path2/path3/1234");
    ASSERT_EQ(Path("path", "1234"), "path/1234");
    ASSERT_EQ(Path("/"), "/");
    ASSERT_EQ(Path("/", ""), "/");
    ASSERT_EQ(Path("/", "", "test"), "/test");
}


TEST(PathTests, ConstructDevPathTests) {
    ASSERT_EQ(construct_dev_path("nvme1"), "/dev/nvme1");
    ASSERT_EQ(construct_dev_path("vg1", "lv1"), "/dev/vg1/lv1");
}

}
