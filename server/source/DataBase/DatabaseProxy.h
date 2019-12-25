//
// Created by luocf on 2019/6/12.
//

#ifndef LOCALSTORGE_DB_PROXY_H
#define LOCALSTORGE_DB_PROXY_H

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <memory>
#include <sqlite3.h>

namespace micro_service {
    typedef std::lock_guard<std::mutex> MUTEX_LOCKER;
    class DatabaseProxy : std::enable_shared_from_this<DatabaseProxy> {
    public:
        DatabaseProxy();
        virtual ~DatabaseProxy();
        static constexpr const char *TAG = "DatabaseProxy";
        void setValue(const std::string& friendid,
                              const std::string& key,
                              const std::string&value,
                              std::time_t time_stamp);

        std::string getValue(const std::string& friendid,
                                     const std::string& key);
        int createTable(const std::string& friend_id);
        bool startDb(const char *data_dir);
        bool closeDb();
        static int callback(void *context, int argc, char **argv, char **azColName);
    private:
        std::mutex _SyncedKeyValueInfo;
        sqlite3 *mDb;
    };
}
#endif //LOCALSTORGE_DB_PROXY_H
