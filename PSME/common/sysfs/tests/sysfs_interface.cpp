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
#include "sysfs/sysfs_interface.hpp"

#include "gtest/gtest.h"

#include <fstream>
#include <algorithm>

using namespace sysfs;

namespace {

std::string execute(const std::string& cmd) {
    static const unsigned SIZE = 100;
    char buffer[SIZE]{};
    FILE* fp = popen(cmd.c_str(), "r");
    std::string ret{};
    while (fgets(buffer, sizeof(buffer), fp)) {
        ret += buffer;
    }
    pclose(fp);
    // this will not work always, but is enough for tests
    if (ret.back() == '\n') {
        ret.pop_back(); // Additional \n at the end of the string
    }
    return ret;
}

bool exists(const Path& path) {
    return execute("test -e " + path.to_string() + "; echo $?") == "0";
}

bool is_dir(const Path& path) {
    return execute("test -d " + path.to_string() + "; echo $?") == "0";
}

bool is_link(const Path& path) {
    return execute("test -L " + path.to_string() + "; echo $?") == "0";
}

bool is_file(const Path& path) {
    return execute("test -f " + path.to_string() + "; echo $?") == "0";
}

void add_dir(const Path& path, const std::string& name) {
    execute("mkdir " + (path / name).to_string());
}

void add_link(const Path& path, const std::string& name, const Path& value) {
    execute("ln -s " + value.to_string() + " " + (path / name).to_string());
}

void add_file(const Path& path, const std::string& name, const std::string& value) {
    std::ofstream file((path / name).to_string(), std::ofstream::out | std::ofstream::trunc);
    file << value;
}

std::string get_link(const Path& path) {
    return execute("readlink " + path.to_string());
}

std::string get_file(const Path& path) {
    return execute("cat " + path.to_string());
}

/* RAII class for test directory creation/removal */
class TestDir final {
public:
    TestDir() {
        m_name = execute("mktemp -d");
        if (m_name.empty()) {
            throw std::runtime_error("Unable to prepare testing directory");
        }
        m_path = Path(m_name);
    }

    ~TestDir() {
        if (!m_name.empty()) {
            execute("rm -R " + m_name + "/");
        }
    }

    const Path& path() const { return m_path; }

private:
    std::string m_name{};
    Path m_path{};
};

#define EXPECT_FILE(path, contents) \
    EXPECT_TRUE(is_file(path)); \
    EXPECT_EQ(get_file(path), contents)

#define EXPECT_LINK(path, link) \
    EXPECT_TRUE(is_link(path)); \
    EXPECT_EQ(get_link(path), link)

#define EXPECT_DIR(path) \
    EXPECT_TRUE(is_dir(path))

}

namespace testing {

TEST(SysfsInterfaceTest, GetEmptyDirectory) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_DIR(dir.path());

    auto data = iface.get_dir(dir.path());

    EXPECT_TRUE(data.path == dir.path());
    EXPECT_TRUE(data.name == dir.path().basename());
    EXPECT_TRUE(data.links.empty());
    EXPECT_TRUE(data.dirs.empty());
    EXPECT_TRUE(data.files.empty());
    EXPECT_DIR(dir.path());
}


TEST(SysfsInterfaceTest, GetNonEmptyDirectoryWithSubDir) {
    TestDir dir{};
    SysfsInterface iface{};

    add_dir(dir.path(), "sub_dir");
    EXPECT_DIR(dir.path() / "sub_dir");

    auto data = iface.get_dir(dir.path());

    EXPECT_TRUE(data.path == dir.path());
    EXPECT_TRUE(data.name == dir.path().basename());
    EXPECT_TRUE(data.links.empty());
    EXPECT_TRUE(data.files.empty());
    ASSERT_EQ(data.dirs.size(), 1);
    ASSERT_TRUE(data.dirs[0] == dir.path() / "sub_dir");
    EXPECT_DIR(dir.path() / "sub_dir");
}

TEST(SysfsInterfaceTest, GetNonEmptyDirectoryWithFile) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "sub_file", "value");
    EXPECT_FILE(dir.path() / "sub_file", "value");

    auto data = iface.get_dir(dir.path());

    EXPECT_TRUE(data.path == dir.path());
    EXPECT_TRUE(data.name == dir.path().basename());
    EXPECT_TRUE(data.dirs.empty());
    EXPECT_TRUE(data.links.empty());
    ASSERT_EQ(data.files.size(), 1);
    ASSERT_TRUE(data.files[0] == dir.path() / "sub_file");
    EXPECT_FILE(dir.path() / "sub_file", "value");
}

TEST(SysfsInterfaceTest, GetNonEmptyDirectoryWithLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_link(dir.path(), "sub_link", dir.path());
    EXPECT_LINK(dir.path() / "sub_link", dir.path().to_string());

    auto data = iface.get_dir(dir.path());
    EXPECT_TRUE(data.path == dir.path());
    EXPECT_TRUE(data.name == dir.path().basename());
    EXPECT_TRUE(data.dirs.empty());
    EXPECT_TRUE(data.files.empty());
    ASSERT_EQ(data.links.size(), 1);
    ASSERT_TRUE(data.links[0] == dir.path() / "sub_link");
    EXPECT_LINK(dir.path() / "sub_link", dir.path().to_string());
}

TEST(SysfsInterfaceTest, GetNonEmptyDirectory) {
    TestDir dir{};
    SysfsInterface iface{};

    add_link(dir.path(), "sub_link1", dir.path());
    add_link(dir.path(), "sub_link2", dir.path());
    add_link(dir.path(), "sub_link3", dir.path());
    add_file(dir.path(), "sub_file1", "1");
    add_file(dir.path(), "sub_file2", "2");
    add_dir(dir.path(), "sub_dir");
    EXPECT_LINK(dir.path() / "sub_link1", dir.path().to_string());
    EXPECT_LINK(dir.path() / "sub_link2", dir.path().to_string());
    EXPECT_LINK(dir.path() / "sub_link3", dir.path().to_string());
    EXPECT_FILE(dir.path() / "sub_file1", "1");
    EXPECT_FILE(dir.path() / "sub_file2", "2");
    EXPECT_DIR(dir.path() / "sub_dir");

    auto data = iface.get_dir(dir.path());

    EXPECT_TRUE(data.path == dir.path());
    EXPECT_TRUE(data.name == dir.path().basename());
    EXPECT_EQ(data.links.size(), 3);
    EXPECT_EQ(data.files.size(), 2);
    EXPECT_EQ(data.dirs.size(), 1);
    EXPECT_LINK(dir.path() / "sub_link1", dir.path().to_string());
    EXPECT_LINK(dir.path() / "sub_link2", dir.path().to_string());
    EXPECT_LINK(dir.path() / "sub_link3", dir.path().to_string());
    EXPECT_FILE(dir.path() / "sub_file1", "1");
    EXPECT_FILE(dir.path() / "sub_file2", "2");
    EXPECT_DIR(dir.path() / "sub_dir");
}

TEST(SysfsInterfaceTest, GetNonEmptyDirectoryWithSubdirectoryTree) {
    TestDir dir{};
    SysfsInterface iface{};

    add_dir(dir.path(), "dir");
    add_dir(dir.path() / "dir", "sub_dir");
    add_file(dir.path() / "dir", "file", "a");
    add_link(dir.path() / "dir", "link", dir.path());
    EXPECT_DIR(dir.path() / "dir");
    EXPECT_DIR(dir.path() / "dir" / "sub_dir");
    EXPECT_FILE(dir.path() / "dir" / "file", "a");
    EXPECT_LINK(dir.path() / "dir" / "link", dir.path().to_string());

    auto data = iface.get_dir(dir.path());
    EXPECT_TRUE(data.path == dir.path());
    EXPECT_TRUE(data.name == dir.path().basename());
    EXPECT_EQ(data.links.size(), 0);
    EXPECT_EQ(data.files.size(), 0);
    ASSERT_EQ(data.dirs.size(), 1);
    EXPECT_TRUE(data.dirs[0] == dir.path() / "dir");
    EXPECT_DIR(dir.path() / "dir");
    EXPECT_DIR(dir.path() / "dir" / "sub_dir");
    EXPECT_FILE(dir.path() / "dir" / "file", "a");
    EXPECT_LINK(dir.path() / "dir" / "link", dir.path().to_string());
}

TEST(SysfsInterfaceTest, GetDirectoryFailOnNonExistingDirectory) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_THROW(iface.get_dir(dir.path() / "dir"), std::runtime_error);

    EXPECT_FALSE(exists(dir.path() / "dir"));
}

TEST(SysfsInterfaceTest, GetDirectoryFailOnExistingFileLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "dir1", "test");
    add_link(dir.path(), "dir2", "some/link");
    EXPECT_FILE(dir.path() / "dir1", "test");
    EXPECT_LINK(dir.path() / "dir2", "some/link");

    EXPECT_THROW(iface.get_dir(dir.path() / "dir1"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir1", "test");
    EXPECT_LINK(dir.path() / "dir2", "some/link");

    EXPECT_THROW(iface.get_dir(dir.path() / "dir2"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir1", "test");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
}

TEST(SysfsInterfaceTest, AddDirectory) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "newdir"));

    iface.add_dir(dir.path(), "newdir");

    EXPECT_DIR(dir.path() / "newdir");
}

TEST(SysfsInterfaceTest, AddDirectoryFailOnExistingFileLinkDir) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "dir", "a");
    add_link(dir.path(), "dir2", "some/link");
    add_dir(dir.path(), "dir3");
    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_dir(dir.path(), "dir"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_dir(dir.path(), "dir2"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_dir(dir.path(), "dir3"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
    EXPECT_DIR(dir.path() / "dir3");
}

TEST(SysfsInterfaceTest, DelDirectory) {
    TestDir dir{};
    SysfsInterface iface{};

    add_dir(dir.path(), "newdir");
    EXPECT_DIR(dir.path() / "newdir");

    iface.del_dir(dir.path() / "newdir");

    EXPECT_FALSE(exists(dir.path() / "newdir"));
}

TEST(SysfsInterfaceTest, DelDirectoryFailOnNonExistingDir) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "newdir"));

    EXPECT_THROW(iface.del_dir(dir.path() / "newdir"), std::runtime_error);

    EXPECT_FALSE(exists(dir.path() / "newdir"));
}

TEST(SysfsInterfaceTest, DelDirectoryFailOnFileLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "file1", "test");
    add_link(dir.path(), "file2", "test/path");
    EXPECT_FILE(dir.path() / "file1", "test");
    EXPECT_LINK(dir.path() / "file2", "test/path");

    EXPECT_THROW(iface.del_dir(dir.path() / "file1"), std::runtime_error);

    EXPECT_FILE(dir.path() / "file1", "test");
    EXPECT_LINK(dir.path() / "file2", "test/path");

    EXPECT_THROW(iface.del_dir(dir.path() / "file2"), std::runtime_error);

    EXPECT_FILE(dir.path() / "file1", "test");
    EXPECT_LINK(dir.path() / "file2", "test/path");
}

TEST(SysfsInterfaceTest, DelDirectoryFailOnNonEmptyDirectory) {
    TestDir dir{};
    SysfsInterface iface{};

    add_dir(dir.path(), "dir");
    add_file(dir.path() / "dir", "file", "contents");
    EXPECT_DIR(dir.path() / "dir");
    EXPECT_FILE(dir.path() / "dir/file", "contents");

    EXPECT_THROW(iface.del_dir(dir.path() / "dir"), std::runtime_error);

    EXPECT_DIR(dir.path() / "dir");
    EXPECT_FILE(dir.path() / "dir/file", "contents");
}

TEST(SysfsInterfaceTest, AddLink) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "link"));

    iface.add_link(dir.path(), "link", Path("some/path"));

    EXPECT_LINK(dir.path() / "link", "some/path");
}

TEST(SysfsInterfaceTest, AddLinkFailOnExistingDirFileLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "dir", "a");
    add_link(dir.path(), "dir2", "some/link");
    add_dir(dir.path(), "dir3");
    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_link(dir.path(), "dir", Path("test/path")), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_link(dir.path(), "dir2", Path("test/path")), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_link(dir.path(), "dir3", Path("test/path")), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/link");
    EXPECT_DIR(dir.path() / "dir3");
}

TEST(SysfsInterfaceTest, DelLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_link(dir.path(), "link", "some/path");
    EXPECT_LINK(dir.path() / "link", "some/path");

    iface.del_link(dir.path() / "link");

    EXPECT_FALSE(exists(dir.path() / "link"));
}

TEST(SysfsInterfaceTest, DelLinkFailOnExistingDirFile) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "dir", "a");
    add_dir(dir.path(), "dir3");
    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.del_link(dir.path() / "dir"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.del_link(dir.path() / "dir3"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_DIR(dir.path() / "dir3");
}

TEST(SysfsInterfaceTest, DelLinkFailsOnNonExistingLink) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "link"));

    EXPECT_THROW(iface.del_link(dir.path() / "link"), std::runtime_error);

    EXPECT_FALSE(exists(dir.path() / "link"));
}

TEST(SysfsInterfaceTest, GetLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_link(dir.path(), "link", "some/path");
    EXPECT_LINK(dir.path() / "link", "some/path");

    auto res = iface.get_link(dir.path() / "link");

    EXPECT_EQ(res.name, "link");
    EXPECT_TRUE(res.path == dir.path() / "link");
    EXPECT_TRUE(res.value == "some/path");
    EXPECT_LINK(dir.path() / "link", "some/path");
}

TEST(SysfsInterfaceTest, GetLinkFailOnExistingDirFile) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "dir", "a");
    add_dir(dir.path(), "dir3");
    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.get_link(dir.path() / "dir"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.get_link(dir.path() / "dir3"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_DIR(dir.path() / "dir3");
}

TEST(SysfsInterfaceTest, GetLinkFailsOnNonExistingLink) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "link"));

    EXPECT_THROW(iface.get_link(dir.path() / "link"), std::runtime_error);

    EXPECT_FALSE(exists(dir.path() / "link"));
}

TEST(SysfsInterfaceTest, AddFile) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "file"));

    iface.add_file(dir.path(), "file", "some values in the file");

    EXPECT_FILE(dir.path() / "file", "some values in the file");
}

TEST(SysfsInterfaceTest, AddFileNewlineCharacters) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "file"));

    iface.add_file(dir.path(), "file", "some values in\n the file");

    EXPECT_FILE(dir.path() / "file", "some values in\n the file");
}

TEST(SysfsInterfaceTest, AddFileFailOnExistingDirFileLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "dir", "a");
    add_link(dir.path(), "dir2", Path("some/path"));
    add_dir(dir.path(), "dir3");
    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/path");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_file(dir.path(), "dir", "new data"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/path");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_file(dir.path(), "dir2", "new data"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/path");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.add_file(dir.path(), "dir3", "new data"), std::runtime_error);

    EXPECT_FILE(dir.path() / "dir", "a");
    EXPECT_LINK(dir.path() / "dir2", "some/path");
    EXPECT_DIR(dir.path() / "dir3");
}

TEST(SysfsInterfaceTest, DelFile) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "file", "some values stored in the file");
    EXPECT_FILE(dir.path() / "file", "some values stored in the file");

    iface.del_file(dir.path() / "file");

    EXPECT_FALSE(exists(dir.path() / "file"));
}

TEST(SysfsInterfaceTest, DelFileFailOnExistingDirLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_dir(dir.path(), "dir");
    add_link(dir.path(), "dir3", Path("some/path"));
    EXPECT_DIR(dir.path() / "dir");
    EXPECT_LINK(dir.path() / "dir3", "some/path");

    EXPECT_THROW(iface.del_file(dir.path() / "dir"), std::runtime_error);

    EXPECT_DIR(dir.path() / "dir");
    EXPECT_LINK(dir.path() / "dir3", "some/path");

    EXPECT_THROW(iface.del_file(dir.path() / "dir3"), std::runtime_error);

    EXPECT_DIR(dir.path() / "dir");
    EXPECT_LINK(dir.path() / "dir3", "some/path");
}

TEST(SysfsInterfaceTest, DelFileFailsOnNonExistingFile) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "file"));

    EXPECT_THROW(iface.del_file(dir.path() / "file"), std::runtime_error);

    EXPECT_FALSE(exists(dir.path() / "file"));
}

TEST(SysfsInterfaceTest, GetFile) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "file", "test value");
    EXPECT_FILE(dir.path() / "file", "test value");

    auto res = iface.get_file(dir.path() / "file");

    EXPECT_EQ(res.name, "file");
    EXPECT_TRUE(res.path == dir.path() / "file");
    EXPECT_EQ(res.value, "test value");
    EXPECT_FILE(dir.path() / "file", "test value");
}

TEST(SysfsInterfaceTest, GetFileNewlineCharacters) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "file", "test\nvalue");
    EXPECT_FILE(dir.path() / "file", "test\nvalue");

    auto res = iface.get_file(dir.path() / "file");

    EXPECT_EQ(res.name, "file");
    EXPECT_TRUE(res.path == dir.path() / "file");
    EXPECT_EQ(res.value, "test\nvalue");
    EXPECT_FILE(dir.path() / "file", "test\nvalue");
}

TEST(SysfsInterfaceTest, GetFileFailOnExistingDirLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_link(dir.path(), "dir", Path("some/path"));
    add_dir(dir.path(), "dir3");
    EXPECT_LINK(dir.path() / "dir", "some/path");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.get_file(dir.path() / "dir"), std::runtime_error);

    EXPECT_LINK(dir.path() / "dir", "some/path");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.get_file(dir.path() / "dir3"), std::runtime_error);

    EXPECT_LINK(dir.path() / "dir", "some/path");
    EXPECT_DIR(dir.path() / "dir3");
}

TEST(SysfsInterfaceTest, GetFileFailsOnNonExistingLink) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "file"));

    EXPECT_THROW(iface.get_file(dir.path() / "file"), std::runtime_error);

    EXPECT_FALSE(exists(dir.path() / "file"));
}

TEST(SysfsInterfaceTest, SetFile) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "file", "test value");
    EXPECT_FILE(dir.path() / "file", "test value");

    iface.set_file(dir.path() / "file", "new value");

    EXPECT_FILE(dir.path() / "file", "new value");
}

TEST(SysfsInterfaceTest, SetFileNewlineCharacters) {
    TestDir dir{};
    SysfsInterface iface{};

    add_file(dir.path(), "file", "test value");
    EXPECT_FILE(dir.path() / "file", "test value");

    iface.set_file(dir.path() / "file", "new\nvalue");

    EXPECT_FILE(dir.path() / "file", "new\nvalue");
}

TEST(SysfsInterfaceTest, SetFileFailOnExistingDirLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_link(dir.path(), "dir", Path("some/path"));
    add_dir(dir.path(), "dir3");
    EXPECT_LINK(dir.path() / "dir", "some/path");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.set_file(dir.path() / "dir", "t"), std::runtime_error);

    EXPECT_LINK(dir.path() / "dir", "some/path");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_THROW(iface.set_file(dir.path() / "dir3", "t"), std::runtime_error);

    EXPECT_LINK(dir.path() / "dir", "some/path");
    EXPECT_DIR(dir.path() / "dir3");
}

TEST(SysfsInterfaceTest, SetFileFailsOnNonExistingFile) {
    TestDir dir{};
    SysfsInterface iface{};

    EXPECT_FALSE(exists(dir.path() / "file"));

    EXPECT_THROW(iface.set_file(dir.path() / "file", "t"), std::runtime_error);

    EXPECT_FALSE(exists(dir.path() / "file"));
}

TEST(SysfsInterfaceTest, Exists) {
    TestDir dir{};
    SysfsInterface iface{};

    add_link(dir.path(), "dir", Path("some/path"));
    add_file(dir.path(), "dir2", "test file");
    add_dir(dir.path(), "dir3");
    EXPECT_LINK(dir.path() / "dir", "some/path");
    EXPECT_FILE(dir.path() / "dir2", "test file");
    EXPECT_DIR(dir.path() / "dir3");

    EXPECT_FALSE(iface.file_exists(dir.path() / "invalid"));
    EXPECT_FALSE(iface.link_exists(dir.path() / "invalid"));
    EXPECT_FALSE(iface.dir_exists(dir.path() / "invalid"));

    EXPECT_TRUE(iface.link_exists(dir.path() / "dir"));
    EXPECT_FALSE(iface.link_exists(dir.path() / "dir2"));
    EXPECT_FALSE(iface.link_exists(dir.path() / "dir3"));

    EXPECT_FALSE(iface.file_exists(dir.path() / "dir"));
    EXPECT_TRUE(iface.file_exists(dir.path() / "dir2"));
    EXPECT_FALSE(iface.file_exists(dir.path() / "dir3"));

    EXPECT_FALSE(iface.dir_exists(dir.path() / "dir"));
    EXPECT_FALSE(iface.dir_exists(dir.path() / "dir2"));
    EXPECT_TRUE(iface.dir_exists(dir.path() / "dir3"));

    EXPECT_LINK(dir.path() / "dir", "some/path");
    EXPECT_FILE(dir.path() / "dir2", "test file");
    EXPECT_DIR(dir.path() / "dir3");
}

TEST(SysfsInterfaceTest, GetMountPoints) {
    TestDir dir{};
    SysfsInterface iface{};
    std::string mounts =
        "sysfs /sys sysfs rw,nosuid,nodev,noexec,relatime 0 0\n"
        "proc /proc proc rw,nosuid,nodev,noexec,relatime 0 0\n"
        "udev /dev devtmpfs rw,nosuid,relatime,size=123k,nr_inodes=123,mode=123 0 0\n"
        "cgroup /sys/fs/cgroup/blkio cgroup rw,nosuid,nodev,noexec,relatime,blkio 0 0\n"
        "/dev/sda1 /boot/efi vfat rw,relatime,fmask=0000,dmask=0000 0 0";

    add_file(dir.path(), "mfile", mounts);
    EXPECT_FILE(dir.path() / "mfile", mounts);

    auto entries = iface.get_mounts(dir.path() / "mfile");

    EXPECT_FILE(dir.path() / "mfile", mounts);

    constexpr unsigned SIZE = 5;
    bool present[SIZE]{false};
    EXPECT_EQ(entries.size(), SIZE);

    for (const auto& entry : entries) {
        if (entry.type == "sysfs") {
            EXPECT_EQ(entry.name, "sysfs");
            EXPECT_EQ(entry.path.to_string(), "/sys");
            present[0] = true;
        }
        else if (entry.type == "proc") {
            EXPECT_EQ(entry.name, "proc");
            EXPECT_EQ(entry.path.to_string(), "/proc");
            present[1] = true;

        }
        else if (entry.type == "devtmpfs") {
            EXPECT_EQ(entry.name, "udev");
            EXPECT_EQ(entry.path.to_string(), "/dev");
            present[2] = true;

        }
        else if (entry.type == "cgroup") {
            EXPECT_EQ(entry.name, "cgroup");
            EXPECT_EQ(entry.path.to_string(), "/sys/fs/cgroup/blkio");
            present[3] = true;

        }
        else if (entry.type == "vfat") {
            EXPECT_EQ(entry.name, "/dev/sda1");
            EXPECT_EQ(entry.path.to_string(), "/boot/efi");
            present[4] = true;
        }
    }
    for (unsigned i = 0; i < SIZE; ++i) {
        EXPECT_TRUE(present[i]);
    }
}

TEST(SysfsInterfaceTest, GetMountPointsExceptionOnNoFile) {
    TestDir dir{};
    SysfsInterface iface{};

    ASSERT_THROW(iface.get_mounts(dir.path() / "mfile"), std::runtime_error);
}

TEST(SysfsInterfaceTest, GetAbsolutePathTest) {
    TestDir dir{};
    SysfsInterface iface{};

    add_dir(dir.path(), "dir1");
    add_dir(dir.path(), "dir2");
    EXPECT_DIR(dir.path() / "dir1");
    EXPECT_DIR(dir.path() / "dir2");
    add_file(dir.path() / "dir1", "file", "test");
    add_link(dir.path() / "dir2", "link", Path("../dir1/file"));
    EXPECT_FILE(dir.path() / "dir1" / "file", "test");
    EXPECT_LINK(dir.path() / "dir2" / "link", "../dir1/file");

    ASSERT_TRUE(iface.get_absolute_path(dir.path() / "dir2" / "link") == dir.path() / "dir1" / "file");
}

TEST(SysfsInterfaceTest, GetAbsolutePathFailsOnNonExistingPath) {
    TestDir dir{};
    SysfsInterface iface{};

    ASSERT_THROW(iface.get_absolute_path(dir.path() / "link"), std::runtime_error);
}

TEST(SysfsInterfaceTest, GetAbsolutePathFailsOnInvalidLink) {
    TestDir dir{};
    SysfsInterface iface{};

    add_link(dir.path(), "link", Path("some/path"));
    EXPECT_LINK(dir.path() / "link", "some/path");

    ASSERT_THROW(iface.get_absolute_path(dir.path() / "link"), std::runtime_error);
}

}
