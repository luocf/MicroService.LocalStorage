//
// Created by luocf on 2019/6/12.
//
#include <map>
#include <iostream>
#include <string.h>
#include <Tools/Log.hpp>
#include "DatabaseProxy.h"
namespace micro_service {
    DatabaseProxy::DatabaseProxy(){
    }
    DatabaseProxy::~DatabaseProxy() {
    }
    void DatabaseProxy::setValue(const std::string& friendid,
                  const std::string& key,
                  const std::string&value,
                  std::time_t time_stamp) {
        std::string value_ori = getValue(friendid, key);
        MUTEX_LOCKER locker_sync_data(_SyncedKeyValueInfo);
        //查询一条记录
        char *errMsg = NULL;
        std::string t_strSql;
        if (value_ori.empty()) {
            t_strSql = "insert into table_"+friendid+" values(NULL,'"+key+"'";
            t_strSql += ",'"+value+"'";
            t_strSql += ","+std::to_string(time_stamp)+"";
            t_strSql += ");";
        } else {
            t_strSql = "update table_"+friendid+" set value='"+value+"'";
            t_strSql += " where key='"+key+"';";
        }

        //消息直接入库
        int rv = sqlite3_exec(mDb, t_strSql.c_str(), callback, this, &errMsg);
        if (rv != SQLITE_OK) {
            Log::I(DatabaseProxy::TAG, "SQLite setValue error: %s\n",
                   errMsg);
        } else {
            Log::I(TAG, "setValue, successful sql:%s", t_strSql.c_str());
        }
    }

    std::string DatabaseProxy::getValue(const std::string& friendid,
                         const std::string& key) {
        MUTEX_LOCKER locker_sync_data(_SyncedKeyValueInfo);
        //查询一条记录
        char **azResult;   //二维数组存放结果
        char *errMsg = NULL;
        int nrow;          /* Number of result rows written here */
        int ncolumn;
        std::string t_strSql;
        t_strSql = "select * from table_"+friendid+" where key=='"+key+"';";
        /*step 2: sql语句对象。*/
        sqlite3_stmt *pStmt;
        int rc = sqlite3_prepare_v2(
                mDb, //数据库连接对象
                t_strSql.c_str(), //指向原始sql语句字符串
                strlen(t_strSql.c_str()), //
                &pStmt,
                NULL
        );
        if (rc != SQLITE_OK) {
            Log::I(TAG, "sqlite3_prepare_v2 error:");
            return "";
        }

        rc = sqlite3_get_table(mDb, t_strSql.c_str(), &azResult, &nrow, &ncolumn, &errMsg);
        if (rc == SQLITE_OK) {
            Log::I(TAG, "getValue, successful sql:%s", t_strSql.c_str());
        } else {
            Log::I(TAG, "getValue, Can't get table: %s", sqlite3_errmsg(mDb));
            return "";
        }
        std::string value = "";
        if (nrow != 0 && ncolumn != 0) {     //有查询结果,不包含表头所占行数
            for (int i = nrow; i >=1; i--) {        // 第0行为数据表头
                value = std::string(azResult[4*i + 2]);
                Log::I(TAG, "getValue, key:%s, value :%s", key.c_str(),value.c_str());
                //break;
            }
        }
        sqlite3_free_table(azResult);
        sqlite3_finalize(pStmt);     //销毁一个SQL语句对象
        return value;
    }

    int  DatabaseProxy::createTable(const std::string& friend_id) {
        MUTEX_LOCKER locker_sync_data(_SyncedKeyValueInfo);
        std::string create_table = "CREATE TABLE IF NOT EXISTS table_"+friend_id+"(id INTEGER PRIMARY KEY AUTOINCREMENT,key TEXT NOT NULL, value TEXT, timestamp INTEGER)";
        char *errMsg = NULL;
        int rv = sqlite3_exec(mDb, create_table.c_str(), callback, this, &errMsg);
        if (rv != SQLITE_OK) {
            Log::I(DatabaseProxy::TAG, "SQLite create_table statement execution error: %s\n",
                   errMsg);
            return 1;
        }
        return 0;
    }

    int DatabaseProxy::callback(void *context, int argc, char **argv, char **azColName) {
        auto database_proxy = reinterpret_cast<DatabaseProxy *>(context);
        int i;
        for (i = 0; i < argc; ++i) {
            Log::I(DatabaseProxy::TAG, "database %s = %s\n", azColName[i],
                   argv[i] ? argv[i] : "NULL");
        }
        return 0;
    }

    bool DatabaseProxy::closeDb() {
        if (mDb != nullptr) {
            sqlite3_close(mDb);
        }
        return true;
    }

    bool DatabaseProxy::startDb(const char *data_dir) {
        std::string strConn = std::string(data_dir) + "/local_storage.db";
        char *errMsg;
        //打开一个数据库，如果改数据库不存在，则创建一个名字为databaseName的数据库文件
        int rv;
        rv = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        if (rv != SQLITE_OK) {
            Log::I(DatabaseProxy::TAG, "sqlite3_config error: %d\n", rv);
            return 1;
        }
        rv = sqlite3_open(strConn.c_str(), &mDb);
        if (rv != SQLITE_OK) {
            Log::I(DatabaseProxy::TAG, "Cannot open database: %s\n", sqlite3_errmsg(mDb));
            sqlite3_close(mDb);
            return 1;
        }
        return 0;
    }
}
