#pragma once
#include "sqlite3.h"
#include <json/json.hpp>
#include <iostream>
#include <map>
#include <mutex>

namespace xmgr {
    class DatabaseMgr
    {
    public:
        static DatabaseMgr& getInstance() {
            static DatabaseMgr cls;
            return cls;
        }
        const std::map<std::string, LPVOID>& searchDatabases();
        LPVOID getDatabaseHandle(const std::string& dbname);
        std::string codec_get_key(const std::string& dbname);
        std::string codec_get_key(sqlite3* db);
        nlohmann::ordered_json execute(const std::string& dbname, const std::string& sql);
        nlohmann::ordered_json execute(sqlite3* db, const std::string& sql);
        int backup(const std::string& dbname, const std::string& out_path);
        int backup(sqlite3* db, const std::string& out_path);
        void backup_xprogress(int remaining, int pagecount);
        sqlite3_api_routines* getSqlite3Rountines() const {
            return m_sqlite3Rountines;
        }
        sqlcipher_api_routines* getSqlcipherRountines() const {
            return m_sqlcipherRountines;
        }
        nlohmann::ordered_json getDatabaseInfo();
    private:
        DatabaseMgr();
        ~DatabaseMgr();
        DatabaseMgr& operator=(const DatabaseMgr&) = delete;
        DatabaseMgr(const DatabaseMgr&) = delete;
        sqlite3_api_routines* m_sqlite3Rountines = nullptr;
        sqlcipher_api_routines* m_sqlcipherRountines = nullptr;
        sqlite3CodecGetKey m_codecGetKey = nullptr;
        std::map<std::string, LPVOID> m_dbs;
        std::mutex m_searchDbMtx;
        std::vector<BYTE> m_backupAsmCode;
    };
}