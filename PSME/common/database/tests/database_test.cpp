/*!
 * @brief General database interface test
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file database.cpp
 */

#include "database/file_database.hpp"

#include <gtest/gtest.h>

#include <sys/stat.h>

namespace database {

class DatabaseTest : public ::testing::Test {
public:
    virtual ~DatabaseTest();
};

DatabaseTest::~DatabaseTest() { }

/*!
 * @brief String value trivial implementation
 *
 * It handles any string as a key/value. No additional checking is done here.
 */
class StringValue final : public Serializable {
public:
    /*! @brief Creates empty string */
    StringValue();
    /*! @brief Creates exact string */
    StringValue(const std::string& str);

    /*! @brief Get stored value */
    const std::string& get() const;

    /*! Compares with second value. "Internal" strings compare is only done here. */
    bool operator==(const StringValue& second) const;

private:
    std::string data;

    virtual std::string serialize() const;
    virtual bool unserialize(const std::string& str);
};

StringValue::StringValue(const std::string& str) : data(str) {}

StringValue::StringValue() : data("") { }

const std::string& StringValue::get() const { return data; }

bool StringValue::unserialize(const std::string& str) {
    data = str;
    return true;
}

std::string StringValue::serialize() const { return data; }

bool StringValue::operator==(const StringValue& second) const { return data == second.get(); }

/*! @brief To show proper values in ASSERT_EQ if don't equal */
std::ostream& operator<<(std::ostream& stream, const StringValue& data);
std::ostream& operator<<(std::ostream& stream, const StringValue& data) {
    return stream << "'" << data.get() << "'";
}

/*! @brief "Public" database to give access to private methods */
class DatabaseTester final {
public:
    DatabaseTester(const std::string& name, bool with_policy = true) :
        db{Database::create(name, with_policy)} { fdb = static_cast<FileDatabase*>(db.get()); }

    ~DatabaseTester() {
        db->remove();
        fdb = nullptr;
    }

    const std::string& get_name() const {
        return db->get_name();
    }

    bool start() {
        return fdb->start();
    }

    bool next(Serializable& key, Serializable& value) {
        return fdb->next(key, value);
    }

    void end() {
        fdb->end();
    }

    bool get(const Serializable& key, Serializable& value) {
        return fdb->get(key, value);
    }

    bool put(const Serializable& key, const Serializable& value) {
        return fdb->put(key, value);
    }

    bool remove(const Serializable& key) {
        return fdb->remove(key);
    }

    bool invalidate(const Serializable& key) {
        return fdb->invalidate(key);
    }

    Database::EntityValidity get_validity(const Serializable& key, std::chrono::seconds interval) {
        return fdb->get_validity(key, interval);
    }

    unsigned wipe_outdated(Serializable& key, std::chrono::seconds interval) {
        return fdb->wipe_outdated(key, interval);
    }

    unsigned cleanup(Serializable& key, std::chrono::seconds interval) {
        return fdb->cleanup(key, interval);
    }

    unsigned drop(Serializable& key) {
        return fdb->drop(key);
    }

    std::string full_name(const std::string& key) const {
        return fdb->full_name(key);
    }

    std::string strip_name(const char* d_name) const {
        return fdb->strip_name(d_name);
    }

    static std::string check_directory(const std::string& dir) {
        return FileDatabase::check_directory(dir);
    }

private:
    Database::SPtr db;
    FileDatabase* fdb;
};


TEST_F(DatabaseTest, BasicFileOperations) {
    constexpr unsigned max_len = 128;
    constexpr const char* fname = "/test_XXXXXX";

    /* create test file, in /tmp directory */
    std::string dir = DatabaseTester::check_directory("/tmp");
    ASSERT_NE(0, dir.length());
    ASSERT_GT(max_len - strlen(fname), dir.length());

    char file_name[max_len];
    strcpy(file_name, dir.c_str());
    strcat(file_name, fname);

    errno = 0;
    int fd = mkstemp(file_name);
    ASSERT_NE(-1, fd) << "Cannot create file:: " << strerror(errno) << ", file " << file_name;
    close(fd);

    struct stat creation_stats;
    ASSERT_EQ(0, stat(file_name, &creation_stats));

    /* wait a while.. not too long.. 100ms is enough */
    usleep(100000);

    /* mark as "in use" */
    ASSERT_EQ(0, chmod(file_name, creation_stats.st_mode | S_ISVTX))
                        << "Cannot change file mode";
    struct stat inuse_stats;
    ASSERT_EQ(0, stat(file_name, &inuse_stats));
    ASSERT_EQ(creation_stats.st_mode | S_ISVTX, inuse_stats.st_mode)
                        << "Sticky bin not set";

    /* check if times match: mode is the only changed */
    ASSERT_EQ(creation_stats.st_mtim.tv_nsec, inuse_stats.st_mtim.tv_nsec)
                        << "Modification time changed";
    ASSERT_EQ(creation_stats.st_atim.tv_nsec, inuse_stats.st_atim.tv_nsec)
                        << "Access time changed";
    ASSERT_NE(creation_stats.st_ctim.tv_nsec, inuse_stats.st_ctim.tv_nsec)
                        << "Status time not changed";

    /* wait a while.. not too long.. 100ms should be enough */
    usleep(100000);

    FILE* file = fopen(file_name, "wb");
    ASSERT_NE(nullptr, file) << "Cannot open file " << file;
    fprintf(file, "123");
    fclose(file);

    /* file mode shouldn't be changed */
    struct stat written_stats;
    ASSERT_EQ(0, stat(file_name, &written_stats));
    ASSERT_EQ(inuse_stats.st_mode, written_stats.st_mode);

    /* check if times match, last modified is the only changed */
    ASSERT_NE(inuse_stats.st_mtim.tv_nsec, written_stats.st_mtim.tv_nsec);
    ASSERT_EQ(inuse_stats.st_atim.tv_nsec, written_stats.st_atim.tv_nsec);
    ASSERT_NE(inuse_stats.st_ctim.tv_nsec, written_stats.st_ctim.tv_nsec);

    /* "invalidate", it should be "regular" file, as created */
    ASSERT_EQ(0, chmod(file_name, inuse_stats.st_mode & (~S_ISVTX)));
    struct stat invalid_stats;
    ASSERT_EQ(0, stat(file_name, &invalid_stats));
    ASSERT_EQ(creation_stats.st_mode, invalid_stats.st_mode);

    ASSERT_EQ(0, unlink(file_name));
}

/* Test intended to check if is it possible to create multiple databases whith
 * same name. It throws assertion.. and google tests are not able to catch it
 * properly. Core file is dumped during "OK" execution..
 */
#ifdef OVERRIDE_CORE_DUMPED
void DatabaseTest_SameDatabase_Create(const std::string& DB);
void DatabaseTest_SameDatabase_Create(const std::string& DB) {
    DatabaseTester db{DB};
    ASSERT_EQ(DB, db.get_name());
}

TEST_F(DatabaseTest, SameDatabase) {
    constexpr const char* DB = "database";

    ASSERT_NO_THROW(DatabaseTest_SameDatabase_Create(DB));

    DatabaseTester db{DB};
    ASSERT_EQ(DB, db.get_name());

    /* should throw assertion, two databases with same name are not allowed */
    ASSERT_DEATH(DatabaseTest_SameDatabase_Create(DB), "");
}
#endif

TEST_F(DatabaseTest, PutEntity) {
    DatabaseTester db{"simple_key"};

    StringValue key{"1"};
    StringValue val{"2"};
    StringValue val2{};

    ASSERT_TRUE(db.put(key, val)) << "Cannot write file";

    ASSERT_TRUE(db.get(key, val2)) << "Cannot read written file";
    ASSERT_EQ(val, val2);

    ASSERT_TRUE(db.remove(key)) << "File cannot be removed";
    ASSERT_FALSE(db.get(key, val)) << "Removed file stil exists";
}

TEST_F(DatabaseTest, Iterate) {
    DatabaseTester db{"db"};

    StringValue key{"first_key"};
    StringValue nkey{"second_key"};
    StringValue val{"0"};
    StringValue nval("1");

    StringValue tkey{};
    StringValue tval{};

    db.put(key, val);
    ASSERT_TRUE(db.start());

    /* iterate over the data.. exactly one key is in the database! */
    ASSERT_TRUE(db.next(tkey, tval));
    ASSERT_EQ(key, tkey);
    ASSERT_EQ(val, tval);

    /* is get possible for found key? */
    ASSERT_TRUE(db.get(key, tval));
    ASSERT_EQ(val, tval);

    /* replace key with new value */
    ASSERT_TRUE(db.put(key, nval));
    ASSERT_TRUE(db.get(key, tval));
    ASSERT_EQ(nval, tval);

    /* add second entity, still iterates.. */
    ASSERT_TRUE(db.put(nkey, nval));
    ASSERT_TRUE(db.get(nkey, tval));
    ASSERT_EQ(nval, tval);

    /* second key shouldn't be read! */
    ASSERT_FALSE(db.next(tkey, tval));
    db.end();

    ASSERT_TRUE(db.invalidate(key));
    ASSERT_EQ(2, db.drop(tkey));
}

TEST_F(DatabaseTest, FileOperations) {
    DatabaseTester db{"file_test"};

    constexpr const char* KEY = "KEY";
    constexpr const char* VAL = "VALUE";
    const std::string FILE = db.full_name(KEY);

    struct stat entity_stats;
    StringValue key{KEY};
    StringValue value{VAL};
    StringValue any_key{};

    /* remove all entries if present, test assumes empty DB */
    db.drop(any_key);

    ASSERT_TRUE(db.put(key, value)) << "Cannot store value";

    ASSERT_EQ(0, stat(FILE.c_str(), &entity_stats));
    ASSERT_EQ(S_ISVTX, entity_stats.st_mode & S_ISVTX);

    ASSERT_TRUE(db.invalidate(key));
    ASSERT_EQ(0, db.cleanup(any_key, std::chrono::seconds(10))) << "The only key is just invalidated, nothing to clean up";

    ASSERT_EQ(0, stat(FILE.c_str(), &entity_stats));
    ASSERT_EQ(0, entity_stats.st_mode & S_ISVTX);

    ASSERT_TRUE(db.get(key, value));
    ASSERT_EQ(VAL, value.get());

    /* remove all files (.. created in the past since 0 seconds..) */
    ASSERT_EQ(1, db.cleanup(any_key, std::chrono::seconds(0))) << "Just invalidated entity not removed";

    /* cannot read the file */
    ASSERT_FALSE(db.get(key, value)) << "Removed entity still exists";
    ASSERT_NE(0, stat(FILE.c_str(), &entity_stats)) << "Removed file still exists";

    /* nothing to be removed */
    ASSERT_EQ(0, db.drop(any_key)) << "Any entities found";
}

TEST_F(DatabaseTest, StripName) {
    const std::string DB{"db"};
    const std::string WRONG{"1234/5678"};
    const std::string KEY{"1234.5678-xxx.val"};

    DatabaseTester db{DB};

    ASSERT_EQ("", db.full_name(WRONG)) << "Not allowed characters in the key";

    const std::string F_NAME = db.full_name(KEY);
    ASSERT_NE(std::string::npos, F_NAME.rfind('/'));

    /* directory name doesn't have path included */
    const std::string D_NAME = F_NAME.substr(F_NAME.rfind('/') + 1);
    ASSERT_EQ(KEY, db.strip_name(D_NAME.c_str())) << "d_name is " << D_NAME;

    /* neither NULL nor empty string might be passed! */

    /* variations of file names.. Empty means incorrect */
    ASSERT_EQ("", db.strip_name("any_file.txt")) << "Regular file";
    ASSERT_EQ("", db.strip_name("db.key.txt")) << "Wrong extension";
    ASSERT_EQ("", db.strip_name("database.key.db")) << "Other database";
    ASSERT_EQ("", db.strip_name("db.db")) << "Missing key";
    ASSERT_EQ("", db.strip_name("db..db")) << "Empty key";
    ASSERT_EQ("proper", db.strip_name("db.proper.db")) << "Proper key";
}

TEST_F(DatabaseTest, RemoveAllData) {
    DatabaseTester db{""};
    AlwaysMatchKey key{};
    ASSERT_NO_THROW(db.wipe_outdated(key, std::chrono::seconds(0)));
    ASSERT_EQ(0, db.drop(key)) << "Any data left in the DB";
}

TEST_F(DatabaseTest, NonRetentionPolicy) {
    DatabaseTester persistent("persistent", false);

    database::String key{"KEY"};
    database::String value{"test value"};
    ASSERT_TRUE(persistent.put(key, value)) << "file not created";
    ASSERT_EQ(Database::EntityValidity::VALID, persistent.get_validity(key, std::chrono::seconds(0))) << "is not valid";

    ASSERT_TRUE(persistent.invalidate(key)) << "file not invalidated";
    ASSERT_EQ(Database::EntityValidity::INVALID, persistent.get_validity(key, std::chrono::seconds(1))) << "is not invalid";
    ASSERT_EQ(Database::EntityValidity::OUTDATED, persistent.get_validity(key, std::chrono::seconds(0))) << "is not outdated";

    ASSERT_FALSE(persistent.invalidate(key)) << "file already invalid";
    ASSERT_TRUE(persistent.get(key, value)) << "invalid file removed";


    DatabaseTester retention("*retention");
    database::AlwaysMatchKey all{};
    ASSERT_EQ(0, retention.cleanup(all, std::chrono::seconds(0))) << "invalidated file cleaned up?";
    ASSERT_EQ(0, retention.wipe_outdated(all, std::chrono::seconds(0))) << "any file wiped?";

    ASSERT_TRUE(persistent.get(key, value)) << "non-policed file removed";
    ASSERT_TRUE(persistent.drop(key)) << "file not removed";
}

TEST_F(DatabaseTest, RetentionPolicy) {
    DatabaseTester persistent("persistent");

    database::String key{"KEY"};
    database::String value{"test value"};
    ASSERT_TRUE(persistent.put(key, value)) << "file not created";

    /* file is not ignored by retention policy.. It should be invalidated and wiped cause outdated */
    DatabaseTester retention("*retention");
    database::AlwaysMatchKey all{};
    ASSERT_EQ(1, retention.cleanup(all, std::chrono::seconds(10))) << "invalidated file cleaned up?";
    ASSERT_EQ(1, retention.wipe_outdated(all, std::chrono::seconds(0))) << "any file wiped?";

    ASSERT_FALSE(persistent.get(key, value)) << "non-policed file removed";
}

} // @i{database}
