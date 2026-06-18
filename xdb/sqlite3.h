#pragma once
#include<winsock2.h>
#define SQLITE_OK           0   /* Successful result */
#define SQLITE_ERROR        1   /* Generic error */
#define SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* Database lock protocol error */
#define SQLITE_EMPTY       16   /* Internal use only */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Not used */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_NOTICE      27   /* Notifications from sqlite3_log() */
#define SQLITE_WARNING     28   /* Warnings from sqlite3_log() */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */

#define SQLITE_UTF8           1    /* IMP: R-37514-35566 */
#define SQLITE_UTF16LE        2    /* IMP: R-03371-37637 */
#define SQLITE_UTF16BE        3    /* IMP: R-51971-34154 */
#define SQLITE_UTF16          4    /* Use native byte order */
#define SQLITE_ANY            5    /* Deprecated */
#define SQLITE_UTF16_ALIGNED  8    /* sqlite3_create_collation only */

#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
#define SQLITE_TEXT     3

#ifdef sleep
#undef sleep
#endif

#define IDA_BASE								0x10000000
#define OFFSET_FROM_IDA(address) (address - IDA_BASE)

typedef void sqlite3;
typedef void sqlite3_context;
typedef void sqlite3_stmt;
typedef long long sqlite_int64;
typedef unsigned long long sqlite_uint64;
typedef long long sqlite3_int64;
typedef unsigned long long sqlite3_uint64;
typedef void sqlite3_value;
typedef void sqlite3_module;
typedef void sqlite3_blob;
typedef void sqlite3_mutex;
typedef void sqlite3_vfs;
typedef void sqlite3_backup;
typedef void sqlite3_index_info;
typedef void sqlite3_str;
typedef void sqlite3_file;

typedef void sqlcipher_provider;

typedef struct sqlcipher_api_routines sqlcipher_api_routines;
typedef struct sqlite3_api_routines sqlite3_api_routines;
typedef int (*sqlite3_callback)(void*, int, char**, char**);

typedef void (*sqlite3CodecGetKey)(sqlite3*, int, void**, int*);

struct sqlite3_api_routines {
    void* (*aggregate_context)(sqlite3_context*, int nBytes);
    int  (*aggregate_count)(sqlite3_context*);
    int  (*bind_blob)(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
    int  (*bind_double)(sqlite3_stmt*, int, double);
    int  (*bind_int)(sqlite3_stmt*, int, int);
    int  (*bind_int64)(sqlite3_stmt*, int, sqlite_int64);
    int  (*bind_null)(sqlite3_stmt*, int);
    int  (*bind_parameter_count)(sqlite3_stmt*);
    int  (*bind_parameter_index)(sqlite3_stmt*, const char* zName);
    const char* (*bind_parameter_name)(sqlite3_stmt*, int);
    int  (*bind_text)(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
    int  (*bind_text16)(sqlite3_stmt*, int, const void*, int, void(*)(void*));
    int  (*bind_value)(sqlite3_stmt*, int, const sqlite3_value*);
    int  (*busy_handler)(sqlite3*, int(*)(void*, int), void*);
    int  (*busy_timeout)(sqlite3*, int ms);
    int  (*changes)(sqlite3*);
    int  (*close)(sqlite3*);
    int  (*collation_needed)(sqlite3*, void*, void(*)(void*, sqlite3*,
        int eTextRep, const char*));
    int  (*collation_needed16)(sqlite3*, void*, void(*)(void*, sqlite3*,
        int eTextRep, const void*));
    const void* (*column_blob)(sqlite3_stmt*, int iCol);
    int  (*column_bytes)(sqlite3_stmt*, int iCol);
    int  (*column_bytes16)(sqlite3_stmt*, int iCol);
    int  (*column_count)(sqlite3_stmt* pStmt);
    const char* (*column_database_name)(sqlite3_stmt*, int);
    const void* (*column_database_name16)(sqlite3_stmt*, int);
    const char* (*column_decltype)(sqlite3_stmt*, int i);
    const void* (*column_decltype16)(sqlite3_stmt*, int);
    double  (*column_double)(sqlite3_stmt*, int iCol);
    int  (*column_int)(sqlite3_stmt*, int iCol);
    sqlite_int64(*column_int64)(sqlite3_stmt*, int iCol);
    const char* (*column_name)(sqlite3_stmt*, int);
    const void* (*column_name16)(sqlite3_stmt*, int);
    const char* (*column_origin_name)(sqlite3_stmt*, int);
    const void* (*column_origin_name16)(sqlite3_stmt*, int);
    const char* (*column_table_name)(sqlite3_stmt*, int);
    const void* (*column_table_name16)(sqlite3_stmt*, int);
    const unsigned char* (*column_text)(sqlite3_stmt*, int iCol);
    const void* (*column_text16)(sqlite3_stmt*, int iCol);
    int  (*column_type)(sqlite3_stmt*, int iCol);
    sqlite3_value* (*column_value)(sqlite3_stmt*, int iCol);
    void* (*commit_hook)(sqlite3*, int(*)(void*), void*);
    int  (*complete)(const char* sql);
    int  (*complete16)(const void* sql);
    int  (*create_collation)(sqlite3*, const char*, int, void*,
        int(*)(void*, int, const void*, int, const void*));
    int  (*create_collation16)(sqlite3*, const void*, int, void*,
        int(*)(void*, int, const void*, int, const void*));
    int  (*create_function)(sqlite3*, const char*, int, int, void*,
        void (*xFunc)(sqlite3_context*, int, sqlite3_value**),
        void (*xStep)(sqlite3_context*, int, sqlite3_value**),
        void (*xFinal)(sqlite3_context*));
    int  (*create_function16)(sqlite3*, const void*, int, int, void*,
        void (*xFunc)(sqlite3_context*, int, sqlite3_value**),
        void (*xStep)(sqlite3_context*, int, sqlite3_value**),
        void (*xFinal)(sqlite3_context*));
    int (*create_module)(sqlite3*, const char*, const sqlite3_module*, void*);
    int  (*data_count)(sqlite3_stmt* pStmt);
    sqlite3* (*db_handle)(sqlite3_stmt*);
    int (*declare_vtab)(sqlite3*, const char*);
    int  (*enable_shared_cache)(int);
    int  (*errcode)(sqlite3* db);
    const char* (*errmsg)(sqlite3*);
    const void* (*errmsg16)(sqlite3*);
    int  (*exec)(sqlite3*, const char*, sqlite3_callback, void*, char**);
    int  (*expired)(sqlite3_stmt*);
    int  (*finalize)(sqlite3_stmt* pStmt);
    void  (*free)(void*);
    void  (*free_table)(char** result);
    int  (*get_autocommit)(sqlite3*);
    void* (*get_auxdata)(sqlite3_context*, int);
    int  (*get_table)(sqlite3*, const char*, char***, int*, int*, char**);
    int  (*global_recover)(void);
    void  (*interruptx)(sqlite3*);
    sqlite_int64(*last_insert_rowid)(sqlite3*);
    const char* (*libversion)(void);
    int  (*libversion_number)(void);
    void* (*malloc)(int);
    char* (*mprintf)(const char*, ...);
    int  (*open)(const char*, sqlite3**);
    int  (*open16)(const void*, sqlite3**);
    int  (*prepare)(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
    int  (*prepare16)(sqlite3*, const void*, int, sqlite3_stmt**, const void**);
    void* (*profile)(sqlite3*, void(*)(void*, const char*, sqlite_uint64), void*);
    void  (*progress_handler)(sqlite3*, int, int(*)(void*), void*);
    void* (*realloc)(void*, int);
    int  (*reset)(sqlite3_stmt* pStmt);
    void  (*result_blob)(sqlite3_context*, const void*, int, void(*)(void*));
    void  (*result_double)(sqlite3_context*, double);
    void  (*result_error)(sqlite3_context*, const char*, int);
    void  (*result_error16)(sqlite3_context*, const void*, int);
    void  (*result_int)(sqlite3_context*, int);
    void  (*result_int64)(sqlite3_context*, sqlite_int64);
    void  (*result_null)(sqlite3_context*);
    void  (*result_text)(sqlite3_context*, const char*, int, void(*)(void*));
    void  (*result_text16)(sqlite3_context*, const void*, int, void(*)(void*));
    void  (*result_text16be)(sqlite3_context*, const void*, int, void(*)(void*));
    void  (*result_text16le)(sqlite3_context*, const void*, int, void(*)(void*));
    void  (*result_value)(sqlite3_context*, sqlite3_value*);
    void* (*rollback_hook)(sqlite3*, void(*)(void*), void*);
    int  (*set_authorizer)(sqlite3*, int(*)(void*, int, const char*, const char*,
        const char*, const char*), void*);
    void  (*set_auxdata)(sqlite3_context*, int, void*, void (*)(void*));
    char* (*xsnprintf)(int, char*, const char*, ...);
    int  (*step)(sqlite3_stmt*);
    int  (*table_column_metadata)(sqlite3*, const char*, const char*, const char*,
        char const**, char const**, int*, int*, int*);
    void  (*thread_cleanup)(void);
    int  (*total_changes)(sqlite3*);
    void* (*trace)(sqlite3*, void(*xTrace)(void*, const char*), void*);
    int  (*transfer_bindings)(sqlite3_stmt*, sqlite3_stmt*);
    void* (*update_hook)(sqlite3*, void(*)(void*, int, char const*, char const*,
        sqlite_int64), void*);
    void* (*user_data)(sqlite3_context*);
    const void* (*value_blob)(sqlite3_value*);
    int  (*value_bytes)(sqlite3_value*);
    int  (*value_bytes16)(sqlite3_value*);
    double  (*value_double)(sqlite3_value*);
    int  (*value_int)(sqlite3_value*);
    sqlite_int64(*value_int64)(sqlite3_value*);
    int  (*value_numeric_type)(sqlite3_value*);
    const unsigned char* (*value_text)(sqlite3_value*);
    const void* (*value_text16)(sqlite3_value*);
    const void* (*value_text16be)(sqlite3_value*);
    const void* (*value_text16le)(sqlite3_value*);
    int  (*value_type)(sqlite3_value*);
    char* (*vmprintf)(const char*, va_list);
    /* Added ??? */
    int (*overload_function)(sqlite3*, const char* zFuncName, int nArg);
    /* Added by 3.3.13 */
    int (*prepare_v2)(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
    int (*prepare16_v2)(sqlite3*, const void*, int, sqlite3_stmt**, const void**);
    int (*clear_bindings)(sqlite3_stmt*);
    /* Added by 3.4.1 */
    int (*create_module_v2)(sqlite3*, const char*, const sqlite3_module*, void*,
        void (*xDestroy)(void*));
    /* Added by 3.5.0 */
    int (*bind_zeroblob)(sqlite3_stmt*, int, int);
    int (*blob_bytes)(sqlite3_blob*);
    int (*blob_close)(sqlite3_blob*);
    int (*blob_open)(sqlite3*, const char*, const char*, const char*, sqlite3_int64,
        int, sqlite3_blob**);
    int (*blob_read)(sqlite3_blob*, void*, int, int);
    int (*blob_write)(sqlite3_blob*, const void*, int, int);
    int (*create_collation_v2)(sqlite3*, const char*, int, void*,
        int(*)(void*, int, const void*, int, const void*),
        void(*)(void*));
    int (*file_control)(sqlite3*, const char*, int, void*);
    sqlite3_int64(*memory_highwater)(int);
    sqlite3_int64(*memory_used)(void);
    sqlite3_mutex* (*mutex_alloc)(int);
    void (*mutex_enter)(sqlite3_mutex*);
    void (*mutex_free)(sqlite3_mutex*);
    void (*mutex_leave)(sqlite3_mutex*);
    int (*mutex_try)(sqlite3_mutex*);
    int (*open_v2)(const char*, sqlite3**, int, const char*);
    int (*release_memory)(int);
    void (*result_error_nomem)(sqlite3_context*);
    void (*result_error_toobig)(sqlite3_context*);
    int (*sleep)(int);
    void (*soft_heap_limit)(int);
    sqlite3_vfs* (*vfs_find)(const char*);
    int (*vfs_register)(sqlite3_vfs*, int);
    int (*vfs_unregister)(sqlite3_vfs*);
    int (*xthreadsafe)(void);
    void (*result_zeroblob)(sqlite3_context*, int);
    void (*result_error_code)(sqlite3_context*, int);
    int (*test_control)(int, ...);
    void (*randomness)(int, void*);
    sqlite3* (*context_db_handle)(sqlite3_context*);
    int (*extended_result_codes)(sqlite3*, int);
    int (*limit)(sqlite3*, int, int);
    sqlite3_stmt* (*next_stmt)(sqlite3*, sqlite3_stmt*);
    const char* (*sql)(sqlite3_stmt*);
    int (*status)(int, int*, int*, int);
    int (*backup_finish)(sqlite3_backup*);
    sqlite3_backup* (*backup_init)(sqlite3*, const char*, sqlite3*, const char*);
    int (*backup_pagecount)(sqlite3_backup*);
    int (*backup_remaining)(sqlite3_backup*);
    int (*backup_step)(sqlite3_backup*, int);
    const char* (*compileoption_get)(int);
    int (*compileoption_used)(const char*);
    int (*create_function_v2)(sqlite3*, const char*, int, int, void*,
        void (*xFunc)(sqlite3_context*, int, sqlite3_value**),
        void (*xStep)(sqlite3_context*, int, sqlite3_value**),
        void (*xFinal)(sqlite3_context*),
        void(*xDestroy)(void*));
    int (*db_config)(sqlite3*, int, ...);
    sqlite3_mutex* (*db_mutex)(sqlite3*);
    int (*db_status)(sqlite3*, int, int*, int*, int);
    int (*extended_errcode)(sqlite3*);
    void (*log)(int, const char*, ...);
    sqlite3_int64(*soft_heap_limit64)(sqlite3_int64);
    const char* (*sourceid)(void);
    int (*stmt_status)(sqlite3_stmt*, int, int);
    int (*strnicmp)(const char*, const char*, int);
    int (*unlock_notify)(sqlite3*, void(*)(void**, int), void*);
    int (*wal_autocheckpoint)(sqlite3*, int);
    int (*wal_checkpoint)(sqlite3*, const char*);
    void* (*wal_hook)(sqlite3*, int(*)(void*, sqlite3*, const char*, int), void*);
    int (*blob_reopen)(sqlite3_blob*, sqlite3_int64);
    int (*vtab_config)(sqlite3*, int op, ...);
    int (*vtab_on_conflict)(sqlite3*);
    /* Version 3.7.16 and later */
    int (*close_v2)(sqlite3*);
    const char* (*db_filename)(sqlite3*, const char*);
    int (*db_readonly)(sqlite3*, const char*);
    int (*db_release_memory)(sqlite3*);
    const char* (*errstr)(int);
    int (*stmt_busy)(sqlite3_stmt*);
    int (*stmt_readonly)(sqlite3_stmt*);
    int (*stricmp)(const char*, const char*);
    int (*uri_boolean)(const char*, const char*, int);
    sqlite3_int64(*uri_int64)(const char*, const char*, sqlite3_int64);
    const char* (*uri_parameter)(const char*, const char*);
    char* (*xvsnprintf)(int, char*, const char*, va_list);
    int (*wal_checkpoint_v2)(sqlite3*, const char*, int, int*, int*);
    /* Version 3.8.7 and later */
    int (*auto_extension)(void(*)(void));
    int (*bind_blob64)(sqlite3_stmt*, int, const void*, sqlite3_uint64,
        void(*)(void*));
    int (*bind_text64)(sqlite3_stmt*, int, const char*, sqlite3_uint64,
        void(*)(void*), unsigned char);
    int (*cancel_auto_extension)(void(*)(void));
    int (*load_extension)(sqlite3*, const char*, const char*, char**);
    void* (*malloc64)(sqlite3_uint64);
    sqlite3_uint64(*msize)(void*);
    void* (*realloc64)(void*, sqlite3_uint64);
    void (*reset_auto_extension)(void);
    void (*result_blob64)(sqlite3_context*, const void*, sqlite3_uint64,
        void(*)(void*));
    void (*result_text64)(sqlite3_context*, const char*, sqlite3_uint64,
        void(*)(void*), unsigned char);
    int (*strglob)(const char*, const char*);
    /* Version 3.8.11 and later */
    sqlite3_value* (*value_dup)(const sqlite3_value*);
    void (*value_free)(sqlite3_value*);
    int (*result_zeroblob64)(sqlite3_context*, sqlite3_uint64);
    int (*bind_zeroblob64)(sqlite3_stmt*, int, sqlite3_uint64);
    /* Version 3.9.0 and later */
    unsigned int (*value_subtype)(sqlite3_value*);
    void (*result_subtype)(sqlite3_context*, unsigned int);
    /* Version 3.10.0 and later */
    int (*status64)(int, sqlite3_int64*, sqlite3_int64*, int);
    int (*strlike)(const char*, const char*, unsigned int);
    int (*db_cacheflush)(sqlite3*);
    /* Version 3.12.0 and later */
    int (*system_errno)(sqlite3*);
    /* Version 3.14.0 and later */
    int (*trace_v2)(sqlite3*, unsigned, int(*)(unsigned, void*, void*, void*), void*);
    char* (*expanded_sql)(sqlite3_stmt*);
    /* Version 3.18.0 and later */
    void (*set_last_insert_rowid)(sqlite3*, sqlite3_int64);
    /* Version 3.20.0 and later */
    int (*prepare_v3)(sqlite3*, const char*, int, unsigned int,
        sqlite3_stmt**, const char**);
    int (*prepare16_v3)(sqlite3*, const void*, int, unsigned int,
        sqlite3_stmt**, const void**);
    int (*bind_pointer)(sqlite3_stmt*, int, void*, const char*, void(*)(void*));
    void (*result_pointer)(sqlite3_context*, void*, const char*, void(*)(void*));
    void* (*value_pointer)(sqlite3_value*, const char*);
    int (*vtab_nochange)(sqlite3_context*);
    int (*value_nochange)(sqlite3_value*);
    const char* (*vtab_collation)(sqlite3_index_info*, int);
    /* Version 3.24.0 and later */
    int (*keyword_count)(void);
    int (*keyword_name)(int, const char**, int*);
    int (*keyword_check)(const char*, int);
    sqlite3_str* (*str_new)(sqlite3*);
    char* (*str_finish)(sqlite3_str*);
    void (*str_appendf)(sqlite3_str*, const char* zFormat, ...);
    void (*str_vappendf)(sqlite3_str*, const char* zFormat, va_list);
    void (*str_append)(sqlite3_str*, const char* zIn, int N);
    void (*str_appendall)(sqlite3_str*, const char* zIn);
    void (*str_appendchar)(sqlite3_str*, int N, char C);
    void (*str_reset)(sqlite3_str*);
    int (*str_errcode)(sqlite3_str*);
    int (*str_length)(sqlite3_str*);
    char* (*str_value)(sqlite3_str*);
    /* Version 3.25.0 and later */
    int (*create_window_function)(sqlite3*, const char*, int, int, void*,
        void (*xStep)(sqlite3_context*, int, sqlite3_value**),
        void (*xFinal)(sqlite3_context*),
        void (*xValue)(sqlite3_context*),
        void (*xInv)(sqlite3_context*, int, sqlite3_value**),
        void(*xDestroy)(void*));
    /* Version 3.26.0 and later */
    const char* (*normalized_sql)(sqlite3_stmt*);
    /* Version 3.28.0 and later */
    int (*stmt_isexplain)(sqlite3_stmt*);
    int (*value_frombind)(sqlite3_value*);
};

struct sqlcipher_api_routines {
    int (*register_provider)(sqlcipher_provider* p);
    sqlcipher_provider* (*get_provider)();
    int (*key)(sqlite3* db, const void* pKey, int nKey);
    int (*key_v2)(sqlite3* db, const char* zDb, const void* pKey, int nKey);
    int (*rekey)(sqlite3* db, const void* pKey, int nKey);
    int (*rekey_v2)(sqlite3* db, const char* zDb, const void* pKey, int nKey);

    int (*register_custom_provider)(const char* name, const sqlcipher_provider* p);
    int (*unregister_custom_provider)(const char* name);
    const sqlcipher_provider* (*get_fallback_provider)();
};