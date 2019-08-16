//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

/* Copyright (c) 2011 The LevelDB Authors. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be
  found in the LICENSE file. See the AUTHORS file for names of contributors.

  C bindings for rocksdb.  May be useful as a stable ABI that can be
  used by programs that keep rocksdb in a shared library, or for
  a JNI api.

  Does not support:
  . getters for the option types
  . custom comparators that implement key shortening
  . capturing post-write-snapshot
  . custom iter, db, env, cache implementations using just the C bindings

  Some conventions:

  (1) We expose just opaque struct pointers and functions to clients.
  This allows us to change internal representations without having to
  recompile clients.

  (2) For simplicity, there is no equivalent to the Slice type.  Instead,
  the caller has to pass the pointer and length as separate
  arguments.

  (3) Errors are represented by a null-terminated c string.  NULL
  means no error.  All operations that can raise an error are passed
  a "char** errptr" as the last argument.  One of the following must
  be true on entry:
     *errptr == NULL
     *errptr points to a malloc()ed null-terminated error message
  On success, a leveldb routine leaves *errptr unchanged.
  On failure, leveldb frees the old value of *errptr and
  set *errptr to a malloc()ed error message.

  (4) Bools have the type unsigned char (0 == false; rest == true)

  (5) All of the pointer arguments must be non-NULL.
*/

#pragma once

#ifdef _WIN32
#ifdef C_ROCKSDB_DLL
#ifdef C_ROCKSDB_LIBRARY_EXPORTS
#define C_ROCKSDB_LIBRARY_API __declspec(dllexport)
#else
#define C_ROCKSDB_LIBRARY_API __declspec(dllimport)
#endif
#else
#define C_ROCKSDB_LIBRARY_API
#endif
#else
#define C_ROCKSDB_LIBRARY_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/* Exported types */

typedef struct crocksdb_t                 crocksdb_t;
typedef struct crocksdb_backup_engine_t   crocksdb_backup_engine_t;
typedef struct crocksdb_backup_engine_info_t   crocksdb_backup_engine_info_t;
typedef struct crocksdb_restore_options_t crocksdb_restore_options_t;
typedef struct crocksdb_cache_t           crocksdb_cache_t;
typedef struct crocksdb_compactionfilter_t crocksdb_compactionfilter_t;
typedef struct crocksdb_compactionfiltercontext_t
    crocksdb_compactionfiltercontext_t;
typedef struct crocksdb_compactionfilterfactory_t
    crocksdb_compactionfilterfactory_t;
typedef struct crocksdb_comparator_t      crocksdb_comparator_t;
typedef struct crocksdb_timestamp_comparator_t      crocksdb_timestamp_comparator_t;
typedef struct crocksdb_dbpath_t          crocksdb_dbpath_t;
typedef struct crocksdb_env_t             crocksdb_env_t;
typedef struct crocksdb_fifo_compaction_options_t crocksdb_fifo_compaction_options_t;
typedef struct crocksdb_filelock_t        crocksdb_filelock_t;
typedef struct crocksdb_filterpolicy_t    crocksdb_filterpolicy_t;
typedef struct crocksdb_flushoptions_t    crocksdb_flushoptions_t;
typedef struct crocksdb_iterator_t        crocksdb_iterator_t;
typedef struct crocksdb_logger_t          crocksdb_logger_t;
typedef struct crocksdb_mergeoperator_t   crocksdb_mergeoperator_t;
typedef struct crocksdb_options_t         crocksdb_options_t;
typedef struct crocksdb_compactoptions_t crocksdb_compactoptions_t;
typedef struct crocksdb_block_based_table_options_t
    crocksdb_block_based_table_options_t;
typedef struct crocksdb_cuckoo_table_options_t
    crocksdb_cuckoo_table_options_t;
typedef struct crocksdb_randomfile_t      crocksdb_randomfile_t;
typedef struct crocksdb_readoptions_t     crocksdb_readoptions_t;
typedef struct crocksdb_seqfile_t         crocksdb_seqfile_t;
typedef struct crocksdb_slicetransform_t  crocksdb_slicetransform_t;
typedef struct crocksdb_snapshot_t        crocksdb_snapshot_t;
typedef struct crocksdb_writablefile_t    crocksdb_writablefile_t;
typedef struct crocksdb_writebatch_t      crocksdb_writebatch_t;
typedef struct crocksdb_writebatch_wi_t   crocksdb_writebatch_wi_t;
typedef struct crocksdb_writeoptions_t    crocksdb_writeoptions_t;
typedef struct crocksdb_universal_compaction_options_t crocksdb_universal_compaction_options_t;
typedef struct crocksdb_livefiles_t     crocksdb_livefiles_t;
typedef struct crocksdb_column_family_handle_t crocksdb_column_family_handle_t;
typedef struct crocksdb_envoptions_t      crocksdb_envoptions_t;
typedef struct crocksdb_ingestexternalfileoptions_t crocksdb_ingestexternalfileoptions_t;
typedef struct crocksdb_sstfilewriter_t   crocksdb_sstfilewriter_t;
typedef struct crocksdb_ratelimiter_t     crocksdb_ratelimiter_t;
typedef struct crocksdb_perfcontext_t     crocksdb_perfcontext_t;
typedef struct crocksdb_pinnableslice_t crocksdb_pinnableslice_t;
typedef struct crocksdb_transactiondb_options_t crocksdb_transactiondb_options_t;
typedef struct crocksdb_transactiondb_t crocksdb_transactiondb_t;
typedef struct crocksdb_transaction_options_t crocksdb_transaction_options_t;
typedef struct crocksdb_optimistictransactiondb_t
    crocksdb_optimistictransactiondb_t;
typedef struct crocksdb_optimistictransaction_options_t
    crocksdb_optimistictransaction_options_t;
typedef struct crocksdb_transaction_t crocksdb_transaction_t;
typedef struct crocksdb_checkpoint_t crocksdb_checkpoint_t;
typedef struct crocksdb_wal_iterator_t crocksdb_wal_iterator_t;
typedef struct crocksdb_wal_readoptions_t crocksdb_wal_readoptions_t;
typedef struct crocksdb_memory_consumers_t crocksdb_memory_consumers_t;
typedef struct crocksdb_memory_usage_t crocksdb_memory_usage_t;

/* DB operations */

extern C_ROCKSDB_LIBRARY_API crocksdb_t* crocksdb_open(
    const crocksdb_options_t* options, const char* name, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_t* crocksdb_open_with_ttl(
    const crocksdb_options_t* options, const char* name, int ttl, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_t* crocksdb_open_for_read_only(
    const crocksdb_options_t* options, const char* name,
    unsigned char error_if_log_file_exist, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_t* crocksdb_open_as_secondary(
    const crocksdb_options_t* options, const char* name,
    const char* secondary_path, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_backup_engine_t* crocksdb_backup_engine_open(
    const crocksdb_options_t* options, const char* path, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_backup_engine_create_new_backup(
    crocksdb_backup_engine_t* be, crocksdb_t* db, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_backup_engine_create_new_backup_flush(
    crocksdb_backup_engine_t* be, crocksdb_t* db, unsigned char flush_before_backup,
    char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_backup_engine_purge_old_backups(
    crocksdb_backup_engine_t* be, uint32_t num_backups_to_keep, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_restore_options_t*
crocksdb_restore_options_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_restore_options_destroy(
    crocksdb_restore_options_t* opt);
extern C_ROCKSDB_LIBRARY_API void crocksdb_restore_options_set_keep_log_files(
    crocksdb_restore_options_t* opt, int v);

extern C_ROCKSDB_LIBRARY_API void
crocksdb_backup_engine_verify_backup(crocksdb_backup_engine_t* be,
    uint32_t backup_id, char** errptr);

extern C_ROCKSDB_LIBRARY_API void
crocksdb_backup_engine_restore_db_from_latest_backup(
    crocksdb_backup_engine_t* be, const char* db_dir, const char* wal_dir,
    const crocksdb_restore_options_t* restore_options, char** errptr);

extern C_ROCKSDB_LIBRARY_API const crocksdb_backup_engine_info_t*
crocksdb_backup_engine_get_backup_info(crocksdb_backup_engine_t* be);

extern C_ROCKSDB_LIBRARY_API int crocksdb_backup_engine_info_count(
    const crocksdb_backup_engine_info_t* info);

extern C_ROCKSDB_LIBRARY_API int64_t
crocksdb_backup_engine_info_timestamp(const crocksdb_backup_engine_info_t* info,
                                     int index);

extern C_ROCKSDB_LIBRARY_API uint32_t
crocksdb_backup_engine_info_backup_id(const crocksdb_backup_engine_info_t* info,
                                     int index);

extern C_ROCKSDB_LIBRARY_API uint64_t
crocksdb_backup_engine_info_size(const crocksdb_backup_engine_info_t* info,
                                int index);

extern C_ROCKSDB_LIBRARY_API uint32_t crocksdb_backup_engine_info_number_files(
    const crocksdb_backup_engine_info_t* info, int index);

extern C_ROCKSDB_LIBRARY_API void crocksdb_backup_engine_info_destroy(
    const crocksdb_backup_engine_info_t* info);

extern C_ROCKSDB_LIBRARY_API void crocksdb_backup_engine_close(
    crocksdb_backup_engine_t* be);

extern C_ROCKSDB_LIBRARY_API crocksdb_checkpoint_t*
crocksdb_checkpoint_object_create(crocksdb_t* db, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_checkpoint_create(
    crocksdb_checkpoint_t* checkpoint, const char* checkpoint_dir,
    uint64_t log_size_for_flush, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_checkpoint_object_destroy(
    crocksdb_checkpoint_t* checkpoint);

extern C_ROCKSDB_LIBRARY_API crocksdb_t* crocksdb_open_column_families(
    const crocksdb_options_t* options, const char* name, int num_column_families,
    const char** column_family_names,
    const crocksdb_options_t** column_family_options,
    crocksdb_column_family_handle_t** column_family_handles, char** errptr);

extern C_ROCKSDB_LIBRARY_API uint32_t crocksdb_column_family_handle_id(crocksdb_column_family_handle_t* column_family_handles);

extern C_ROCKSDB_LIBRARY_API crocksdb_t*
crocksdb_open_for_read_only_column_families(
    const crocksdb_options_t* options, const char* name, int num_column_families,
    const char** column_family_names,
    const crocksdb_options_t** column_family_options,
    crocksdb_column_family_handle_t** column_family_handles,
    unsigned char error_if_log_file_exist, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_t* crocksdb_open_as_secondary_column_families(
    const crocksdb_options_t* options, const char* name,
    const char* secondary_path, int num_column_families,
    const char** column_family_names,
    const crocksdb_options_t** column_family_options,
    crocksdb_column_family_handle_t** colummn_family_handles, char** errptr);

extern C_ROCKSDB_LIBRARY_API char** crocksdb_list_column_families(
    const crocksdb_options_t* options, const char* name, size_t* lencf,
    char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_list_column_families_destroy(
    char** list, size_t len);

extern C_ROCKSDB_LIBRARY_API crocksdb_column_family_handle_t*
crocksdb_create_column_family(crocksdb_t* db,
                             const crocksdb_options_t* column_family_options,
                             const char* column_family_name, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_drop_column_family(
    crocksdb_t* db, crocksdb_column_family_handle_t* handle, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_column_family_handle_destroy(
    crocksdb_column_family_handle_t*);

extern C_ROCKSDB_LIBRARY_API void crocksdb_close(crocksdb_t* db);

extern C_ROCKSDB_LIBRARY_API void crocksdb_put(
    crocksdb_t* db, const crocksdb_writeoptions_t* options, const char* key,
    size_t keylen, const char* val, size_t vallen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_put_cf(
    crocksdb_t* db, const crocksdb_writeoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key,
    size_t keylen, const char* val, size_t vallen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_delete(
    crocksdb_t* db, const crocksdb_writeoptions_t* options, const char* key,
    size_t keylen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_delete_cf(
    crocksdb_t* db, const crocksdb_writeoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key,
    size_t keylen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_merge(
    crocksdb_t* db, const crocksdb_writeoptions_t* options, const char* key,
    size_t keylen, const char* val, size_t vallen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_merge_cf(
    crocksdb_t* db, const crocksdb_writeoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key,
    size_t keylen, const char* val, size_t vallen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_write(
    crocksdb_t* db, const crocksdb_writeoptions_t* options,
    crocksdb_writebatch_t* batch, char** errptr);

/* Returns NULL if not found.  A malloc()ed array otherwise.
   Stores the length of the array in *vallen. */
extern C_ROCKSDB_LIBRARY_API char* crocksdb_get(
    crocksdb_t* db, const crocksdb_readoptions_t* options, const char* key,
    size_t keylen, size_t* vallen, char** errptr);

extern C_ROCKSDB_LIBRARY_API char* crocksdb_get_cf(
    crocksdb_t* db, const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key,
    size_t keylen, size_t* vallen, char** errptr);

// if values_list[i] == NULL and errs[i] == NULL,
// then we got status.IsNotFound(), which we will not return.
// all errors except status status.ok() and status.IsNotFound() are returned.
//
// errs, values_list and values_list_sizes must be num_keys in length,
// allocated by the caller.
// errs is a list of strings as opposed to the conventional one error,
// where errs[i] is the status for retrieval of keys_list[i].
// each non-NULL errs entry is a malloc()ed, null terminated string.
// each non-NULL values_list entry is a malloc()ed array, with
// the length for each stored in values_list_sizes[i].
extern C_ROCKSDB_LIBRARY_API void crocksdb_multi_get(
    crocksdb_t* db, const crocksdb_readoptions_t* options, size_t num_keys,
    const char* const* keys_list, const size_t* keys_list_sizes,
    char** values_list, size_t* values_list_sizes, char** errs);

extern C_ROCKSDB_LIBRARY_API void crocksdb_multi_get_cf(
    crocksdb_t* db, const crocksdb_readoptions_t* options,
    const crocksdb_column_family_handle_t* const* column_families,
    size_t num_keys, const char* const* keys_list,
    const size_t* keys_list_sizes, char** values_list,
    size_t* values_list_sizes, char** errs);

extern C_ROCKSDB_LIBRARY_API crocksdb_iterator_t* crocksdb_create_iterator(
    crocksdb_t* db, const crocksdb_readoptions_t* options);

extern C_ROCKSDB_LIBRARY_API crocksdb_wal_iterator_t* crocksdb_get_updates_since(
        crocksdb_t* db, uint64_t seq_number,
        const crocksdb_wal_readoptions_t* options,
        char** errptr
);

extern C_ROCKSDB_LIBRARY_API crocksdb_iterator_t* crocksdb_create_iterator_cf(
    crocksdb_t* db, const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family);

extern C_ROCKSDB_LIBRARY_API void crocksdb_create_iterators(
    crocksdb_t *db, crocksdb_readoptions_t* opts,
    crocksdb_column_family_handle_t** column_families,
    crocksdb_iterator_t** iterators, size_t size, char** errptr);

extern C_ROCKSDB_LIBRARY_API const crocksdb_snapshot_t* crocksdb_create_snapshot(
    crocksdb_t* db);

extern C_ROCKSDB_LIBRARY_API void crocksdb_release_snapshot(
    crocksdb_t* db, const crocksdb_snapshot_t* snapshot);

/* Returns NULL if property name is unknown.
   Else returns a pointer to a malloc()-ed null-terminated value. */
extern C_ROCKSDB_LIBRARY_API char* crocksdb_property_value(crocksdb_t* db,
                                                        const char* propname);
/* returns 0 on success, -1 otherwise */
int crocksdb_property_int(
    crocksdb_t* db,
    const char* propname, uint64_t *out_val);

/* returns 0 on success, -1 otherwise */
int crocksdb_property_int_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* column_family,
    const char* propname, uint64_t *out_val);

extern C_ROCKSDB_LIBRARY_API char* crocksdb_property_value_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* column_family,
    const char* propname);

extern C_ROCKSDB_LIBRARY_API void crocksdb_approximate_sizes(
    crocksdb_t* db, int num_ranges, const char* const* range_start_key,
    const size_t* range_start_key_len, const char* const* range_limit_key,
    const size_t* range_limit_key_len, uint64_t* sizes);

extern C_ROCKSDB_LIBRARY_API void crocksdb_approximate_sizes_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* column_family,
    int num_ranges, const char* const* range_start_key,
    const size_t* range_start_key_len, const char* const* range_limit_key,
    const size_t* range_limit_key_len, uint64_t* sizes);

extern C_ROCKSDB_LIBRARY_API void crocksdb_compact_range(crocksdb_t* db,
                                                      const char* start_key,
                                                      size_t start_key_len,
                                                      const char* limit_key,
                                                      size_t limit_key_len);

extern C_ROCKSDB_LIBRARY_API void crocksdb_compact_range_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* column_family,
    const char* start_key, size_t start_key_len, const char* limit_key,
    size_t limit_key_len);

extern C_ROCKSDB_LIBRARY_API void crocksdb_compact_range_opt(
    crocksdb_t* db, crocksdb_compactoptions_t* opt, const char* start_key,
    size_t start_key_len, const char* limit_key, size_t limit_key_len);

extern C_ROCKSDB_LIBRARY_API void crocksdb_compact_range_cf_opt(
    crocksdb_t* db, crocksdb_column_family_handle_t* column_family,
    crocksdb_compactoptions_t* opt, const char* start_key, size_t start_key_len,
    const char* limit_key, size_t limit_key_len);

extern C_ROCKSDB_LIBRARY_API void crocksdb_delete_file(crocksdb_t* db,
                                                    const char* name);

extern C_ROCKSDB_LIBRARY_API const crocksdb_livefiles_t* crocksdb_livefiles(
    crocksdb_t* db);

extern C_ROCKSDB_LIBRARY_API void crocksdb_flush(
    crocksdb_t* db, const crocksdb_flushoptions_t* options, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_flush_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* column_family,
    const crocksdb_flushoptions_t* options, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_disable_file_deletions(crocksdb_t* db,
                                                               char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_enable_file_deletions(
    crocksdb_t* db, unsigned char force, char** errptr);

/* Management operations */

extern C_ROCKSDB_LIBRARY_API void crocksdb_destroy_db(
    const crocksdb_options_t* options, const char* name, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_repair_db(
    const crocksdb_options_t* options, const char* name, char** errptr);

/* Iterator */

extern C_ROCKSDB_LIBRARY_API void crocksdb_iter_destroy(crocksdb_iterator_t*);
extern C_ROCKSDB_LIBRARY_API unsigned char crocksdb_iter_valid(
    const crocksdb_iterator_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_iter_seek_to_first(crocksdb_iterator_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_iter_seek_to_last(crocksdb_iterator_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_iter_seek(crocksdb_iterator_t*,
                                                  const char* k, size_t klen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_iter_seek_for_prev(crocksdb_iterator_t*,
                                                           const char* k,
                                                           size_t klen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_iter_next(crocksdb_iterator_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_iter_prev(crocksdb_iterator_t*);
extern C_ROCKSDB_LIBRARY_API const char* crocksdb_iter_key(
    const crocksdb_iterator_t*, size_t* klen);
extern C_ROCKSDB_LIBRARY_API const char* crocksdb_iter_value(
    const crocksdb_iterator_t*, size_t* vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_iter_get_error(
    const crocksdb_iterator_t*, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_wal_iter_next(crocksdb_wal_iterator_t* iter);
extern C_ROCKSDB_LIBRARY_API unsigned char crocksdb_wal_iter_valid(
        const crocksdb_wal_iterator_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_wal_iter_status (const crocksdb_wal_iterator_t* iter, char** errptr) ;
extern C_ROCKSDB_LIBRARY_API crocksdb_writebatch_t* crocksdb_wal_iter_get_batch (const crocksdb_wal_iterator_t* iter, uint64_t* seq) ;
extern C_ROCKSDB_LIBRARY_API uint64_t crocksdb_get_latest_sequence_number (crocksdb_t *db);
extern C_ROCKSDB_LIBRARY_API void crocksdb_wal_iter_destroy (const crocksdb_wal_iterator_t* iter) ;

/* Write batch */

extern C_ROCKSDB_LIBRARY_API crocksdb_writebatch_t* crocksdb_writebatch_create(size_t timestamp_size);
extern C_ROCKSDB_LIBRARY_API crocksdb_writebatch_t* crocksdb_writebatch_create_with_capacity(
    size_t reserved_bytes, size_t timestamp_size);

extern C_ROCKSDB_LIBRARY_API crocksdb_writebatch_t* crocksdb_writebatch_create_from(
    const char* rep, size_t size);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_destroy(
    crocksdb_writebatch_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_clear(crocksdb_writebatch_t*);
extern C_ROCKSDB_LIBRARY_API int crocksdb_writebatch_count(crocksdb_writebatch_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_put(crocksdb_writebatch_t*,
                                                       const char* key,
                                                       size_t klen,
                                                       const char* val,
                                                       size_t vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_put_cf(
    crocksdb_writebatch_t*, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen, const char* val, size_t vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_putv(
    crocksdb_writebatch_t* b, int num_keys, const char* const* keys_list,
    const size_t* keys_list_sizes, int num_values,
    const char* const* values_list, const size_t* values_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_putv_cf(
    crocksdb_writebatch_t* b, crocksdb_column_family_handle_t* column_family,
    int num_keys, const char* const* keys_list, const size_t* keys_list_sizes,
    int num_values, const char* const* values_list,
    const size_t* values_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_merge(crocksdb_writebatch_t*,
                                                         const char* key,
                                                         size_t klen,
                                                         const char* val,
                                                         size_t vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_merge_cf(
    crocksdb_writebatch_t*, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen, const char* val, size_t vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_mergev(
    crocksdb_writebatch_t* b, int num_keys, const char* const* keys_list,
    const size_t* keys_list_sizes, int num_values,
    const char* const* values_list, const size_t* values_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_mergev_cf(
    crocksdb_writebatch_t* b, crocksdb_column_family_handle_t* column_family,
    int num_keys, const char* const* keys_list, const size_t* keys_list_sizes,
    int num_values, const char* const* values_list,
    const size_t* values_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_delete(crocksdb_writebatch_t*,
                                                          const char* key,
                                                          size_t klen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_delete_cf(
    crocksdb_writebatch_t*, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_deletev(
    crocksdb_writebatch_t* b, int num_keys, const char* const* keys_list,
    const size_t* keys_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_deletev_cf(
    crocksdb_writebatch_t* b, crocksdb_column_family_handle_t* column_family,
    int num_keys, const char* const* keys_list, const size_t* keys_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_delete_range(
    crocksdb_writebatch_t* b, const char* start_key, size_t start_key_len,
    const char* end_key, size_t end_key_len);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_delete_range_cf(
    crocksdb_writebatch_t* b, crocksdb_column_family_handle_t* column_family,
    const char* start_key, size_t start_key_len, const char* end_key,
    size_t end_key_len);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_delete_rangev(
    crocksdb_writebatch_t* b, int num_keys, const char* const* start_keys_list,
    const size_t* start_keys_list_sizes, const char* const* end_keys_list,
    const size_t* end_keys_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_delete_rangev_cf(
    crocksdb_writebatch_t* b, crocksdb_column_family_handle_t* column_family,
    int num_keys, const char* const* start_keys_list,
    const size_t* start_keys_list_sizes, const char* const* end_keys_list,
    const size_t* end_keys_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_put_log_data(
    crocksdb_writebatch_t*, const char* blob, size_t len);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_iterate(
    crocksdb_writebatch_t*, void* state,
    void (*put)(void*, const char* k, size_t klen, const char* v, size_t vlen),
    void (*deleted)(void*, const char* k, size_t klen));
extern C_ROCKSDB_LIBRARY_API const char* crocksdb_writebatch_data(
    crocksdb_writebatch_t*, size_t* size);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_set_save_point(
    crocksdb_writebatch_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_rollback_to_save_point(
    crocksdb_writebatch_t*, char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_pop_save_point(
    crocksdb_writebatch_t*, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_assign_timestamps(
    crocksdb_writebatch_t*, const char* const* all_ts, size_t tsnum, size_t tslen);

/* Write batch with index */

extern C_ROCKSDB_LIBRARY_API crocksdb_writebatch_wi_t* crocksdb_writebatch_wi_create(
                                                       size_t reserved_bytes,
                                                       unsigned char overwrite_keys);
extern C_ROCKSDB_LIBRARY_API crocksdb_writebatch_wi_t* crocksdb_writebatch_wi_create_from(
    const char* rep, size_t size);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_destroy(
    crocksdb_writebatch_wi_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_clear(crocksdb_writebatch_wi_t*);
extern C_ROCKSDB_LIBRARY_API int crocksdb_writebatch_wi_count(crocksdb_writebatch_wi_t* b);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_put(crocksdb_writebatch_wi_t*,
                                                       const char* key,
                                                       size_t klen,
                                                       const char* val,
                                                       size_t vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_put_cf(
    crocksdb_writebatch_wi_t*, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen, const char* val, size_t vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_putv(
    crocksdb_writebatch_wi_t* b, int num_keys, const char* const* keys_list,
    const size_t* keys_list_sizes, int num_values,
    const char* const* values_list, const size_t* values_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_putv_cf(
    crocksdb_writebatch_wi_t* b, crocksdb_column_family_handle_t* column_family,
    int num_keys, const char* const* keys_list, const size_t* keys_list_sizes,
    int num_values, const char* const* values_list,
    const size_t* values_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_merge(crocksdb_writebatch_wi_t*,
                                                         const char* key,
                                                         size_t klen,
                                                         const char* val,
                                                         size_t vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_merge_cf(
    crocksdb_writebatch_wi_t*, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen, const char* val, size_t vlen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_mergev(
    crocksdb_writebatch_wi_t* b, int num_keys, const char* const* keys_list,
    const size_t* keys_list_sizes, int num_values,
    const char* const* values_list, const size_t* values_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_mergev_cf(
    crocksdb_writebatch_wi_t* b, crocksdb_column_family_handle_t* column_family,
    int num_keys, const char* const* keys_list, const size_t* keys_list_sizes,
    int num_values, const char* const* values_list,
    const size_t* values_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_delete(crocksdb_writebatch_wi_t*,
                                                          const char* key,
                                                          size_t klen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_delete_cf(
    crocksdb_writebatch_wi_t*, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_deletev(
    crocksdb_writebatch_wi_t* b, int num_keys, const char* const* keys_list,
    const size_t* keys_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_deletev_cf(
    crocksdb_writebatch_wi_t* b, crocksdb_column_family_handle_t* column_family,
    int num_keys, const char* const* keys_list, const size_t* keys_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_delete_range(
    crocksdb_writebatch_wi_t* b, const char* start_key, size_t start_key_len,
    const char* end_key, size_t end_key_len);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_delete_range_cf(
    crocksdb_writebatch_wi_t* b, crocksdb_column_family_handle_t* column_family,
    const char* start_key, size_t start_key_len, const char* end_key,
    size_t end_key_len);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_delete_rangev(
    crocksdb_writebatch_wi_t* b, int num_keys, const char* const* start_keys_list,
    const size_t* start_keys_list_sizes, const char* const* end_keys_list,
    const size_t* end_keys_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_delete_rangev_cf(
    crocksdb_writebatch_wi_t* b, crocksdb_column_family_handle_t* column_family,
    int num_keys, const char* const* start_keys_list,
    const size_t* start_keys_list_sizes, const char* const* end_keys_list,
    const size_t* end_keys_list_sizes);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_put_log_data(
    crocksdb_writebatch_wi_t*, const char* blob, size_t len);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_iterate(
    crocksdb_writebatch_wi_t* b,
    void* state,
    void (*put)(void*, const char* k, size_t klen, const char* v, size_t vlen),
    void (*deleted)(void*, const char* k, size_t klen));
extern C_ROCKSDB_LIBRARY_API const char* crocksdb_writebatch_wi_data(
    crocksdb_writebatch_wi_t* b,
    size_t* size);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_set_save_point(
    crocksdb_writebatch_wi_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writebatch_wi_rollback_to_save_point(
    crocksdb_writebatch_wi_t*, char** errptr);
extern C_ROCKSDB_LIBRARY_API char* crocksdb_writebatch_wi_get_from_batch(
    crocksdb_writebatch_wi_t* wbwi,
    const crocksdb_options_t* options,
    const char* key, size_t keylen,
    size_t* vallen,
    char** errptr);
extern C_ROCKSDB_LIBRARY_API char* crocksdb_writebatch_wi_get_from_batch_cf(
    crocksdb_writebatch_wi_t* wbwi,
    const crocksdb_options_t* options,
    crocksdb_column_family_handle_t* column_family,
    const char* key, size_t keylen,
    size_t* vallen,
    char** errptr);
extern C_ROCKSDB_LIBRARY_API char* crocksdb_writebatch_wi_get_from_batch_and_db(
    crocksdb_writebatch_wi_t* wbwi,
    crocksdb_t* db,
    const crocksdb_readoptions_t* options,
    const char* key, size_t keylen,
    size_t* vallen,
    char** errptr);
extern C_ROCKSDB_LIBRARY_API char* crocksdb_writebatch_wi_get_from_batch_and_db_cf(
    crocksdb_writebatch_wi_t* wbwi,
    crocksdb_t* db,
    const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family,
    const char* key, size_t keylen,
    size_t* vallen,
    char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_write_writebatch_wi(
    crocksdb_t* db,
    const crocksdb_writeoptions_t* options,
    crocksdb_writebatch_wi_t* wbwi,
    char** errptr);
extern C_ROCKSDB_LIBRARY_API crocksdb_iterator_t* crocksdb_writebatch_wi_create_iterator_with_base(
    crocksdb_writebatch_wi_t* wbwi,
    crocksdb_iterator_t* base_iterator);
extern C_ROCKSDB_LIBRARY_API crocksdb_iterator_t* crocksdb_writebatch_wi_create_iterator_with_base_cf(
    crocksdb_writebatch_wi_t* wbwi,
    crocksdb_iterator_t* base_iterator,
    crocksdb_column_family_handle_t* cf);

/* Block based table options */

extern C_ROCKSDB_LIBRARY_API crocksdb_block_based_table_options_t*
crocksdb_block_based_options_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_block_based_options_destroy(
    crocksdb_block_based_table_options_t* options);
extern C_ROCKSDB_LIBRARY_API void crocksdb_block_based_options_set_block_size(
    crocksdb_block_based_table_options_t* options, size_t block_size);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_block_size_deviation(
    crocksdb_block_based_table_options_t* options, int block_size_deviation);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_block_restart_interval(
    crocksdb_block_based_table_options_t* options, int block_restart_interval);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_index_block_restart_interval(
    crocksdb_block_based_table_options_t* options, int index_block_restart_interval);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_metadata_block_size(
    crocksdb_block_based_table_options_t* options, uint64_t metadata_block_size);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_partition_filters(
    crocksdb_block_based_table_options_t* options, unsigned char partition_filters);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_use_delta_encoding(
    crocksdb_block_based_table_options_t* options, unsigned char use_delta_encoding);
extern C_ROCKSDB_LIBRARY_API void crocksdb_block_based_options_set_filter_policy(
    crocksdb_block_based_table_options_t* options,
    crocksdb_filterpolicy_t* filter_policy);
extern C_ROCKSDB_LIBRARY_API void crocksdb_block_based_options_set_no_block_cache(
    crocksdb_block_based_table_options_t* options, unsigned char no_block_cache);
extern C_ROCKSDB_LIBRARY_API void crocksdb_block_based_options_set_block_cache(
    crocksdb_block_based_table_options_t* options, crocksdb_cache_t* block_cache);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_block_cache_compressed(
    crocksdb_block_based_table_options_t* options,
    crocksdb_cache_t* block_cache_compressed);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_whole_key_filtering(
    crocksdb_block_based_table_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_block_based_options_set_format_version(
    crocksdb_block_based_table_options_t*, int);
enum {
  crocksdb_block_based_table_index_type_binary_search = 0,
  crocksdb_block_based_table_index_type_hash_search = 1,
  crocksdb_block_based_table_index_type_two_level_index_search = 2,
};
extern C_ROCKSDB_LIBRARY_API void crocksdb_block_based_options_set_index_type(
    crocksdb_block_based_table_options_t*, int);  // uses one of the above enums
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_hash_index_allow_collision(
    crocksdb_block_based_table_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_cache_index_and_filter_blocks(
    crocksdb_block_based_table_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_cache_index_and_filter_blocks_with_high_priority(
    crocksdb_block_based_table_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_pin_l0_filter_and_index_blocks_in_cache(
    crocksdb_block_based_table_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_block_based_options_set_pin_top_level_index_and_filter(
    crocksdb_block_based_table_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_block_based_table_factory(
    crocksdb_options_t* opt, crocksdb_block_based_table_options_t* table_options);

/* Cuckoo table options */

extern C_ROCKSDB_LIBRARY_API crocksdb_cuckoo_table_options_t*
crocksdb_cuckoo_options_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_cuckoo_options_destroy(
    crocksdb_cuckoo_table_options_t* options);
extern C_ROCKSDB_LIBRARY_API void crocksdb_cuckoo_options_set_hash_ratio(
    crocksdb_cuckoo_table_options_t* options, double v);
extern C_ROCKSDB_LIBRARY_API void crocksdb_cuckoo_options_set_max_search_depth(
    crocksdb_cuckoo_table_options_t* options, uint32_t v);
extern C_ROCKSDB_LIBRARY_API void crocksdb_cuckoo_options_set_cuckoo_block_size(
    crocksdb_cuckoo_table_options_t* options, uint32_t v);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_cuckoo_options_set_identity_as_first_hash(
    crocksdb_cuckoo_table_options_t* options, unsigned char v);
extern C_ROCKSDB_LIBRARY_API void crocksdb_cuckoo_options_set_use_module_hash(
    crocksdb_cuckoo_table_options_t* options, unsigned char v);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_cuckoo_table_factory(
    crocksdb_options_t* opt, crocksdb_cuckoo_table_options_t* table_options);

/* Options */
extern C_ROCKSDB_LIBRARY_API void crocksdb_set_options(
    crocksdb_t* db, int count, const char* const keys[], const char* const values[], char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_set_options_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* handle, int count, const char* const keys[], const char* const values[], char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_options_t* crocksdb_options_copy(const crocksdb_options_t *other);

extern C_ROCKSDB_LIBRARY_API crocksdb_options_t* crocksdb_get_options_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* handle);

extern C_ROCKSDB_LIBRARY_API crocksdb_options_t* crocksdb_options_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_destroy(crocksdb_options_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_increase_parallelism(
    crocksdb_options_t* opt, int total_threads);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_optimize_for_point_lookup(
    crocksdb_options_t* opt, uint64_t block_cache_size_mb);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_optimize_level_style_compaction(
    crocksdb_options_t* opt, uint64_t memtable_memory_budget);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_optimize_universal_style_compaction(
    crocksdb_options_t* opt, uint64_t memtable_memory_budget);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_allow_ingest_behind(crocksdb_options_t*,
                                                   unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_compaction_filter(
    crocksdb_options_t*, crocksdb_compactionfilter_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_compaction_filter_factory(
    crocksdb_options_t*, crocksdb_compactionfilterfactory_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_compaction_readahead_size(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_comparator(
    crocksdb_options_t*, crocksdb_comparator_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_timestamp_comparator(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_merge_operator(
    crocksdb_options_t*, crocksdb_mergeoperator_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_uint64add_merge_operator(
    crocksdb_options_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_compression_per_level(
    crocksdb_options_t* opt, int* level_values, size_t num_levels);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_create_if_missing(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_create_missing_column_families(crocksdb_options_t*,
                                                   unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_error_if_exists(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_paranoid_checks(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_db_paths(crocksdb_options_t*,
                                                             const crocksdb_dbpath_t** path_values,
                                                             size_t num_paths);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_env(crocksdb_options_t*,
                                                        crocksdb_env_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_info_log(crocksdb_options_t*,
                                                             crocksdb_logger_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_info_log_level(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_write_buffer_size(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_db_write_buffer_size(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_open_files(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_file_opening_threads(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_total_wal_size(
    crocksdb_options_t* opt, uint64_t n);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_compression_options(
    crocksdb_options_t*, int, int, int, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_prefix_extractor(
    crocksdb_options_t*, crocksdb_slicetransform_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_num_levels(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API int crocksdb_options_get_num_levels(crocksdb_options_t *opt);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_level0_file_num_compaction_trigger(crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_level0_slowdown_writes_trigger(crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_level0_stop_writes_trigger(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_mem_compaction_level(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_target_file_size_base(
    crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_target_file_size_multiplier(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_bytes_for_level_base(
    crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_snap_refresh_nanos(
    crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_level_compaction_dynamic_level_bytes(crocksdb_options_t*,
                                                         unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_max_bytes_for_level_multiplier(crocksdb_options_t*, double);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_max_bytes_for_level_multiplier_additional(
    crocksdb_options_t*, int* level_values, size_t num_levels);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_enable_statistics(
    crocksdb_options_t*);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_skip_stats_update_on_db_open(crocksdb_options_t* opt,
                                                 unsigned char val);

/* returns a pointer to a malloc()-ed, null terminated string */
extern C_ROCKSDB_LIBRARY_API char* crocksdb_options_statistics_get_string(
    crocksdb_options_t* opt);

extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_write_buffer_number(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_min_write_buffer_number_to_merge(crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_max_write_buffer_number_to_maintain(crocksdb_options_t*,
                                                        int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_enable_pipelined_write(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_unordered_write(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_subcompactions(
    crocksdb_options_t*, uint32_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_background_jobs(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_background_compactions(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_base_background_compactions(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_background_flushes(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_log_file_size(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_log_file_time_to_roll(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_keep_log_file_num(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_recycle_log_file_num(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_soft_rate_limit(
    crocksdb_options_t*, double);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_hard_rate_limit(
    crocksdb_options_t*, double);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_soft_pending_compaction_bytes_limit(
    crocksdb_options_t* opt, size_t v);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_hard_pending_compaction_bytes_limit(
    crocksdb_options_t* opt, size_t v);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_rate_limit_delay_max_milliseconds(crocksdb_options_t*,
                                                      unsigned int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_manifest_file_size(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_table_cache_numshardbits(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_table_cache_remove_scan_count_limit(crocksdb_options_t*,
                                                        int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_arena_block_size(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_use_fsync(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_db_log_dir(
    crocksdb_options_t*, const char*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_wal_dir(crocksdb_options_t*,
                                                            const char*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_WAL_ttl_seconds(
    crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_WAL_size_limit_MB(
    crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_manifest_preallocation_size(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_purge_redundant_kvs_while_flush(crocksdb_options_t*,
                                                    unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_allow_mmap_reads(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_allow_mmap_writes(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_use_direct_reads(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_use_direct_io_for_flush_and_compaction(crocksdb_options_t*,
                                                           unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_is_fd_close_on_exec(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_skip_log_error_on_recovery(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_stats_dump_period_sec(
    crocksdb_options_t*, unsigned int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_advise_random_on_open(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_access_hint_on_compaction_start(crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_use_adaptive_mutex(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_bytes_per_sync(
    crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_wal_bytes_per_sync(
        crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_writable_file_max_buffer_size(crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_allow_concurrent_memtable_write(crocksdb_options_t*,
                                                    unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_enable_write_thread_adaptive_yield(crocksdb_options_t*,
                                                       unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_max_sequential_skip_in_iterations(crocksdb_options_t*,
                                                      uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_disable_auto_compactions(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_optimize_filters_for_hits(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_delete_obsolete_files_period_micros(crocksdb_options_t*,
                                                        uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_prepare_for_bulk_load(
    crocksdb_options_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_memtable_vector_rep(
    crocksdb_options_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_memtable_prefix_bloom_size_ratio(
    crocksdb_options_t*, double);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_compaction_bytes(
    crocksdb_options_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_hash_skip_list_rep(
    crocksdb_options_t*, size_t, int32_t, int32_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_hash_link_list_rep(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_plain_table_factory(
    crocksdb_options_t*, uint32_t, int, double, size_t);

extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_min_level_to_compress(
    crocksdb_options_t* opt, int level);

extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_memtable_huge_page_size(
    crocksdb_options_t*, size_t);

extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_max_successive_merges(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_bloom_locality(
    crocksdb_options_t*, uint32_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_inplace_update_support(
    crocksdb_options_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_inplace_update_num_locks(
    crocksdb_options_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_report_bg_io_stats(
    crocksdb_options_t*, int);

enum {
  crocksdb_tolerate_corrupted_tail_records_recovery = 0,
  crocksdb_absolute_consistency_recovery = 1,
  crocksdb_point_in_time_recovery = 2,
  crocksdb_skip_any_corrupted_records_recovery = 3
};
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_wal_recovery_mode(
    crocksdb_options_t*, int);

enum {
  crocksdb_no_compression = 0,
  crocksdb_snappy_compression = 1,
  crocksdb_zlib_compression = 2,
  crocksdb_bz2_compression = 3,
  crocksdb_lz4_compression = 4,
  crocksdb_lz4hc_compression = 5,
  crocksdb_xpress_compression = 6,
  crocksdb_zstd_compression = 7
};
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_compression(
    crocksdb_options_t*, int);

enum {
  crocksdb_level_compaction = 0,
  crocksdb_universal_compaction = 1,
  crocksdb_fifo_compaction = 2
};
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_compaction_style(
    crocksdb_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_options_set_universal_compaction_options(
    crocksdb_options_t*, crocksdb_universal_compaction_options_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_fifo_compaction_options(
    crocksdb_options_t* opt, crocksdb_fifo_compaction_options_t* fifo);
extern C_ROCKSDB_LIBRARY_API void crocksdb_options_set_ratelimiter(
    crocksdb_options_t* opt, crocksdb_ratelimiter_t* limiter);

/* RateLimiter */
extern C_ROCKSDB_LIBRARY_API crocksdb_ratelimiter_t* crocksdb_ratelimiter_create(
    int64_t rate_bytes_per_sec, int64_t refill_period_us, int32_t fairness);
extern C_ROCKSDB_LIBRARY_API void crocksdb_ratelimiter_destroy(crocksdb_ratelimiter_t*);

/* PerfContext */
enum {
  crocksdb_uninitialized = 0,
  crocksdb_disable = 1,
  crocksdb_enable_count = 2,
  crocksdb_enable_time_except_for_mutex = 3,
  crocksdb_enable_time = 4,
  crocksdb_out_of_bounds = 5
};

enum {
  crocksdb_user_key_comparison_count = 0,
  crocksdb_block_cache_hit_count,
  crocksdb_block_read_count,
  crocksdb_block_read_byte,
  crocksdb_block_read_time,
  crocksdb_block_checksum_time,
  crocksdb_block_decompress_time,
  crocksdb_get_read_bytes,
  crocksdb_multiget_read_bytes,
  crocksdb_iter_read_bytes,
  crocksdb_internal_key_skipped_count,
  crocksdb_internal_delete_skipped_count,
  crocksdb_internal_recent_skipped_count,
  crocksdb_internal_merge_count,
  crocksdb_get_snapshot_time,
  crocksdb_get_from_memtable_time,
  crocksdb_get_from_memtable_count,
  crocksdb_get_post_process_time,
  crocksdb_get_from_output_files_time,
  crocksdb_seek_on_memtable_time,
  crocksdb_seek_on_memtable_count,
  crocksdb_next_on_memtable_count,
  crocksdb_prev_on_memtable_count,
  crocksdb_seek_child_seek_time,
  crocksdb_seek_child_seek_count,
  crocksdb_seek_min_heap_time,
  crocksdb_seek_max_heap_time,
  crocksdb_seek_internal_seek_time,
  crocksdb_find_next_user_entry_time,
  crocksdb_write_wal_time,
  crocksdb_write_memtable_time,
  crocksdb_write_delay_time,
  crocksdb_write_pre_and_post_process_time,
  crocksdb_db_mutex_lock_nanos,
  crocksdb_db_condition_wait_nanos,
  crocksdb_merge_operator_time_nanos,
  crocksdb_read_index_block_nanos,
  crocksdb_read_filter_block_nanos,
  crocksdb_new_table_block_iter_nanos,
  crocksdb_new_table_iterator_nanos,
  crocksdb_block_seek_nanos,
  crocksdb_find_table_nanos,
  crocksdb_bloom_memtable_hit_count,
  crocksdb_bloom_memtable_miss_count,
  crocksdb_bloom_sst_hit_count,
  crocksdb_bloom_sst_miss_count,
  crocksdb_key_lock_wait_time,
  crocksdb_key_lock_wait_count,
  crocksdb_env_new_sequential_file_nanos,
  crocksdb_env_new_random_access_file_nanos,
  crocksdb_env_new_writable_file_nanos,
  crocksdb_env_reuse_writable_file_nanos,
  crocksdb_env_new_random_rw_file_nanos,
  crocksdb_env_new_directory_nanos,
  crocksdb_env_file_exists_nanos,
  crocksdb_env_get_children_nanos,
  crocksdb_env_get_children_file_attributes_nanos,
  crocksdb_env_delete_file_nanos,
  crocksdb_env_create_dir_nanos,
  crocksdb_env_create_dir_if_missing_nanos,
  crocksdb_env_delete_dir_nanos,
  crocksdb_env_get_file_size_nanos,
  crocksdb_env_get_file_modification_time_nanos,
  crocksdb_env_rename_file_nanos,
  crocksdb_env_link_file_nanos,
  crocksdb_env_lock_file_nanos,
  crocksdb_env_unlock_file_nanos,
  crocksdb_env_new_logger_nanos,
  crocksdb_total_metric_count = 68
};

extern C_ROCKSDB_LIBRARY_API void crocksdb_set_perf_level(int);
extern C_ROCKSDB_LIBRARY_API crocksdb_perfcontext_t* crocksdb_perfcontext_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_perfcontext_reset(
    crocksdb_perfcontext_t* context);
extern C_ROCKSDB_LIBRARY_API char* crocksdb_perfcontext_report(
    crocksdb_perfcontext_t* context, unsigned char exclude_zero_counters);
extern C_ROCKSDB_LIBRARY_API uint64_t crocksdb_perfcontext_metric(
    crocksdb_perfcontext_t* context, int metric);
extern C_ROCKSDB_LIBRARY_API void crocksdb_perfcontext_destroy(
    crocksdb_perfcontext_t* context);

/* Compaction Filter */

extern C_ROCKSDB_LIBRARY_API crocksdb_compactionfilter_t*
crocksdb_compactionfilter_create(
    void* state, void (*destructor)(void*),
    unsigned char (*filter)(void*, int level, const char* key,
                            size_t key_length, const char* existing_value,
                            size_t value_length, char** new_value,
                            size_t* new_value_length,
                            unsigned char* value_changed),
    const char* (*name)(void*));
extern C_ROCKSDB_LIBRARY_API void crocksdb_compactionfilter_set_ignore_snapshots(
    crocksdb_compactionfilter_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_compactionfilter_destroy(
    crocksdb_compactionfilter_t*);

/* Compaction Filter Context */

extern C_ROCKSDB_LIBRARY_API unsigned char
crocksdb_compactionfiltercontext_is_full_compaction(
    crocksdb_compactionfiltercontext_t* context);

extern C_ROCKSDB_LIBRARY_API unsigned char
crocksdb_compactionfiltercontext_is_manual_compaction(
    crocksdb_compactionfiltercontext_t* context);

/* Compaction Filter Factory */

extern C_ROCKSDB_LIBRARY_API crocksdb_compactionfilterfactory_t*
crocksdb_compactionfilterfactory_create(
    void* state, void (*destructor)(void*),
    crocksdb_compactionfilter_t* (*create_compaction_filter)(
        void*, crocksdb_compactionfiltercontext_t* context),
    const char* (*name)(void*));
extern C_ROCKSDB_LIBRARY_API void crocksdb_compactionfilterfactory_destroy(
    crocksdb_compactionfilterfactory_t*);

/* Comparator */

extern C_ROCKSDB_LIBRARY_API crocksdb_comparator_t* crocksdb_comparator_create(
    void* state, void (*destructor)(void*),
    int (*compare)(void*, const char* a, size_t alen, const char* b,
                   size_t blen),
    const char* (*name)(void*));
extern C_ROCKSDB_LIBRARY_API void crocksdb_comparator_destroy(
    crocksdb_comparator_t*);

/* Filter policy */

extern C_ROCKSDB_LIBRARY_API crocksdb_filterpolicy_t* crocksdb_filterpolicy_create(
    void* state, void (*destructor)(void*),
    char* (*create_filter)(void*, const char* const* key_array,
                           const size_t* key_length_array, int num_keys,
                           size_t* filter_length),
    unsigned char (*key_may_match)(void*, const char* key, size_t length,
                                   const char* filter, size_t filter_length),
    void (*delete_filter)(void*, const char* filter, size_t filter_length),
    const char* (*name)(void*));
extern C_ROCKSDB_LIBRARY_API void crocksdb_filterpolicy_destroy(
    crocksdb_filterpolicy_t*);

extern C_ROCKSDB_LIBRARY_API crocksdb_filterpolicy_t*
crocksdb_filterpolicy_create_bloom(int bits_per_key);
extern C_ROCKSDB_LIBRARY_API crocksdb_filterpolicy_t*
crocksdb_filterpolicy_create_bloom_full(int bits_per_key);

/* Merge Operator */

extern C_ROCKSDB_LIBRARY_API crocksdb_mergeoperator_t*
crocksdb_mergeoperator_create(
    void* state, void (*destructor)(void*),
    char* (*full_merge)(void*, const char* key, size_t key_length,
                        const char* existing_value,
                        size_t existing_value_length,
                        const char* const* operands_list,
                        const size_t* operands_list_length, int num_operands,
                        unsigned char* success, size_t* new_value_length),
    char* (*partial_merge)(void*, const char* key, size_t key_length,
                           const char* const* operands_list,
                           const size_t* operands_list_length, int num_operands,
                           unsigned char* success, size_t* new_value_length),
    void (*delete_value)(void*, const char* value, size_t value_length),
    const char* (*name)(void*));
extern C_ROCKSDB_LIBRARY_API void crocksdb_mergeoperator_destroy(
    crocksdb_mergeoperator_t*);

/* Read options */

extern C_ROCKSDB_LIBRARY_API crocksdb_readoptions_t* crocksdb_readoptions_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_destroy(
    crocksdb_readoptions_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_verify_checksums(
    crocksdb_readoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_fill_cache(
    crocksdb_readoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_snapshot(
    crocksdb_readoptions_t*, const crocksdb_snapshot_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_iterate_upper_bound(
    crocksdb_readoptions_t*, const char* key, size_t keylen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_iterate_lower_bound(
    crocksdb_readoptions_t*, const char* key, size_t keylen);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_read_tier(
    crocksdb_readoptions_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_tailing(
    crocksdb_readoptions_t*, unsigned char);
// The functionality that this option controlled has been removed.
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_managed(
    crocksdb_readoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_readahead_size(
    crocksdb_readoptions_t*, size_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_prefix_same_as_start(
    crocksdb_readoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_pin_data(
    crocksdb_readoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_total_order_seek(
    crocksdb_readoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_max_skippable_internal_keys(
    crocksdb_readoptions_t*, uint64_t);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_background_purge_on_iterator_cleanup(
    crocksdb_readoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_ignore_range_deletions(
    crocksdb_readoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_readoptions_set_timestamp(
    crocksdb_readoptions_t*, const char* ts, size_t tslen);


/* Write options */

extern C_ROCKSDB_LIBRARY_API crocksdb_writeoptions_t*
crocksdb_writeoptions_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_writeoptions_destroy(
    crocksdb_writeoptions_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writeoptions_set_sync(
    crocksdb_writeoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writeoptions_disable_WAL(
    crocksdb_writeoptions_t* opt, int disable);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writeoptions_set_ignore_missing_column_families(
    crocksdb_writeoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writeoptions_set_no_slowdown(
    crocksdb_writeoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writeoptions_set_low_pri(
    crocksdb_writeoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_writeoptions_set_timestamp(
    crocksdb_writeoptions_t*, const char* ts, size_t tslen);
/* Compact range options */

extern C_ROCKSDB_LIBRARY_API crocksdb_compactoptions_t*
crocksdb_compactoptions_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_compactoptions_destroy(
    crocksdb_compactoptions_t*);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_compactoptions_set_exclusive_manual_compaction(
    crocksdb_compactoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_compactoptions_set_bottommost_level_compaction(
    crocksdb_compactoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_compactoptions_set_change_level(
    crocksdb_compactoptions_t*, unsigned char);
extern C_ROCKSDB_LIBRARY_API void crocksdb_compactoptions_set_target_level(
    crocksdb_compactoptions_t*, int);

/* Flush options */

extern C_ROCKSDB_LIBRARY_API crocksdb_flushoptions_t*
crocksdb_flushoptions_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_flushoptions_destroy(
    crocksdb_flushoptions_t*);
extern C_ROCKSDB_LIBRARY_API void crocksdb_flushoptions_set_wait(
    crocksdb_flushoptions_t*, unsigned char);

/* Cache */

extern C_ROCKSDB_LIBRARY_API crocksdb_cache_t* crocksdb_cache_create_lru(
    size_t capacity);
extern C_ROCKSDB_LIBRARY_API void crocksdb_cache_destroy(crocksdb_cache_t* cache);
extern C_ROCKSDB_LIBRARY_API void crocksdb_cache_set_capacity(
    crocksdb_cache_t* cache, size_t capacity);
extern C_ROCKSDB_LIBRARY_API size_t
crocksdb_cache_get_usage(crocksdb_cache_t* cache);
extern C_ROCKSDB_LIBRARY_API size_t
crocksdb_cache_get_pinned_usage(crocksdb_cache_t* cache);

extern C_ROCKSDB_LIBRARY_API size_t crocksdb_options_get_block_cache_usage(crocksdb_options_t *opt);

/* DBPath */

extern C_ROCKSDB_LIBRARY_API crocksdb_dbpath_t* crocksdb_dbpath_create(const char* path, uint64_t target_size);
extern C_ROCKSDB_LIBRARY_API void crocksdb_dbpath_destroy(crocksdb_dbpath_t*);

/* Env */

extern C_ROCKSDB_LIBRARY_API crocksdb_env_t* crocksdb_create_default_env();
extern C_ROCKSDB_LIBRARY_API crocksdb_env_t* crocksdb_create_mem_env();
extern C_ROCKSDB_LIBRARY_API void crocksdb_env_set_background_threads(
    crocksdb_env_t* env, int n);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_env_set_high_priority_background_threads(crocksdb_env_t* env, int n);
extern C_ROCKSDB_LIBRARY_API void crocksdb_env_join_all_threads(
    crocksdb_env_t* env);
extern C_ROCKSDB_LIBRARY_API void crocksdb_env_lower_thread_pool_io_priority(crocksdb_env_t* env);
extern C_ROCKSDB_LIBRARY_API void crocksdb_env_lower_high_priority_thread_pool_io_priority(crocksdb_env_t* env);
extern C_ROCKSDB_LIBRARY_API void crocksdb_env_lower_thread_pool_cpu_priority(crocksdb_env_t* env);
extern C_ROCKSDB_LIBRARY_API void crocksdb_env_lower_high_priority_thread_pool_cpu_priority(crocksdb_env_t* env);

extern C_ROCKSDB_LIBRARY_API void crocksdb_env_destroy(crocksdb_env_t*);

extern C_ROCKSDB_LIBRARY_API crocksdb_envoptions_t* crocksdb_envoptions_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_envoptions_destroy(
    crocksdb_envoptions_t* opt);

/* SstFile */

extern C_ROCKSDB_LIBRARY_API crocksdb_sstfilewriter_t*
crocksdb_sstfilewriter_create(const crocksdb_envoptions_t* env,
                             const crocksdb_options_t* io_options);
extern C_ROCKSDB_LIBRARY_API crocksdb_sstfilewriter_t*
crocksdb_sstfilewriter_create_with_comparator(
    const crocksdb_envoptions_t* env, const crocksdb_options_t* io_options,
    const crocksdb_comparator_t* comparator);
extern C_ROCKSDB_LIBRARY_API void crocksdb_sstfilewriter_open(
    crocksdb_sstfilewriter_t* writer, const char* name, char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_sstfilewriter_add(
    crocksdb_sstfilewriter_t* writer, const char* key, size_t keylen,
    const char* val, size_t vallen, char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_sstfilewriter_put(
    crocksdb_sstfilewriter_t* writer, const char* key, size_t keylen,
    const char* val, size_t vallen, char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_sstfilewriter_merge(
    crocksdb_sstfilewriter_t* writer, const char* key, size_t keylen,
    const char* val, size_t vallen, char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_sstfilewriter_delete(
    crocksdb_sstfilewriter_t* writer, const char* key, size_t keylen,
    char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_sstfilewriter_finish(
    crocksdb_sstfilewriter_t* writer, char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_sstfilewriter_file_size(
    crocksdb_sstfilewriter_t* writer, uint64_t* file_size);
extern C_ROCKSDB_LIBRARY_API void crocksdb_sstfilewriter_destroy(
    crocksdb_sstfilewriter_t* writer);

extern C_ROCKSDB_LIBRARY_API crocksdb_ingestexternalfileoptions_t*
crocksdb_ingestexternalfileoptions_create();
extern C_ROCKSDB_LIBRARY_API void
crocksdb_ingestexternalfileoptions_set_move_files(
    crocksdb_ingestexternalfileoptions_t* opt, unsigned char move_files);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_ingestexternalfileoptions_set_snapshot_consistency(
    crocksdb_ingestexternalfileoptions_t* opt,
    unsigned char snapshot_consistency);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_ingestexternalfileoptions_set_allow_global_seqno(
    crocksdb_ingestexternalfileoptions_t* opt, unsigned char allow_global_seqno);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_ingestexternalfileoptions_set_allow_blocking_flush(
    crocksdb_ingestexternalfileoptions_t* opt,
    unsigned char allow_blocking_flush);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_ingestexternalfileoptions_set_ingest_behind(
    crocksdb_ingestexternalfileoptions_t* opt,
    unsigned char ingest_behind);
extern C_ROCKSDB_LIBRARY_API void crocksdb_ingestexternalfileoptions_destroy(
    crocksdb_ingestexternalfileoptions_t* opt);

extern C_ROCKSDB_LIBRARY_API void crocksdb_ingest_external_file(
    crocksdb_t* db, const char* const* file_list, const size_t list_len,
    const crocksdb_ingestexternalfileoptions_t* opt, char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_ingest_external_file_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* handle,
    const char* const* file_list, const size_t list_len,
    const crocksdb_ingestexternalfileoptions_t* opt, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_try_catch_up_with_primary(
    crocksdb_t* db, char** errptr);

/* SliceTransform */

extern C_ROCKSDB_LIBRARY_API crocksdb_slicetransform_t*
crocksdb_slicetransform_create(
    void* state, void (*destructor)(void*),
    char* (*transform)(void*, const char* key, size_t length,
                       size_t* dst_length),
    unsigned char (*in_domain)(void*, const char* key, size_t length),
    unsigned char (*in_range)(void*, const char* key, size_t length),
    const char* (*name)(void*));
extern C_ROCKSDB_LIBRARY_API crocksdb_slicetransform_t*
    crocksdb_slicetransform_create_fixed_prefix(size_t);
extern C_ROCKSDB_LIBRARY_API crocksdb_slicetransform_t*
crocksdb_slicetransform_create_noop();
extern C_ROCKSDB_LIBRARY_API void crocksdb_slicetransform_destroy(
    crocksdb_slicetransform_t*);

/* Universal Compaction options */

enum {
  crocksdb_similar_size_compaction_stop_style = 0,
  crocksdb_total_size_compaction_stop_style = 1
};

extern C_ROCKSDB_LIBRARY_API crocksdb_universal_compaction_options_t*
crocksdb_universal_compaction_options_create();
extern C_ROCKSDB_LIBRARY_API void
crocksdb_universal_compaction_options_set_size_ratio(
    crocksdb_universal_compaction_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_universal_compaction_options_set_min_merge_width(
    crocksdb_universal_compaction_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_universal_compaction_options_set_max_merge_width(
    crocksdb_universal_compaction_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_universal_compaction_options_set_max_size_amplification_percent(
    crocksdb_universal_compaction_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_universal_compaction_options_set_compression_size_percent(
    crocksdb_universal_compaction_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void
crocksdb_universal_compaction_options_set_stop_style(
    crocksdb_universal_compaction_options_t*, int);
extern C_ROCKSDB_LIBRARY_API void crocksdb_universal_compaction_options_destroy(
    crocksdb_universal_compaction_options_t*);

extern C_ROCKSDB_LIBRARY_API crocksdb_fifo_compaction_options_t*
crocksdb_fifo_compaction_options_create();
extern C_ROCKSDB_LIBRARY_API void
crocksdb_fifo_compaction_options_set_max_table_files_size(
    crocksdb_fifo_compaction_options_t* fifo_opts, uint64_t size);
extern C_ROCKSDB_LIBRARY_API void crocksdb_fifo_compaction_options_destroy(
    crocksdb_fifo_compaction_options_t* fifo_opts);

extern C_ROCKSDB_LIBRARY_API int crocksdb_livefiles_count(
    const crocksdb_livefiles_t*);
extern C_ROCKSDB_LIBRARY_API const char* crocksdb_livefiles_name(
    const crocksdb_livefiles_t*, int index);
extern C_ROCKSDB_LIBRARY_API int crocksdb_livefiles_level(
    const crocksdb_livefiles_t*, int index);
extern C_ROCKSDB_LIBRARY_API size_t
crocksdb_livefiles_size(const crocksdb_livefiles_t*, int index);
extern C_ROCKSDB_LIBRARY_API const char* crocksdb_livefiles_smallestkey(
    const crocksdb_livefiles_t*, int index, size_t* size);
extern C_ROCKSDB_LIBRARY_API const char* crocksdb_livefiles_largestkey(
    const crocksdb_livefiles_t*, int index, size_t* size);
extern C_ROCKSDB_LIBRARY_API uint64_t crocksdb_livefiles_entries(
    const crocksdb_livefiles_t*, int index);
extern C_ROCKSDB_LIBRARY_API uint64_t crocksdb_livefiles_deletions(
    const crocksdb_livefiles_t*, int index);
extern C_ROCKSDB_LIBRARY_API void crocksdb_livefiles_destroy(
    const crocksdb_livefiles_t*);

/* Utility Helpers */

extern C_ROCKSDB_LIBRARY_API void crocksdb_get_options_from_string(
    const crocksdb_options_t* base_options, const char* opts_str,
    crocksdb_options_t* new_options, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_delete_file_in_range(
    crocksdb_t* db, const char* start_key, size_t start_key_len,
    const char* limit_key, size_t limit_key_len, bool include_end, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_delete_file_in_range_cf(
    crocksdb_t* db, crocksdb_column_family_handle_t* column_family,
    const char* start_key, size_t start_key_len, const char* limit_key,
    size_t limit_key_len, bool include_end, char** errptr);

/* Transactions */

extern C_ROCKSDB_LIBRARY_API crocksdb_column_family_handle_t*
crocksdb_transactiondb_create_column_family(
    crocksdb_transactiondb_t* txn_db,
    const crocksdb_options_t* column_family_options,
    const char* column_family_name, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_transactiondb_t* crocksdb_transactiondb_open(
    const crocksdb_options_t* options,
    const crocksdb_transactiondb_options_t* txn_db_options, const char* name,
    char** errptr);

crocksdb_transactiondb_t* crocksdb_transactiondb_open_column_families(
    const crocksdb_options_t* options,
    const crocksdb_transactiondb_options_t* txn_db_options, const char* name,
    int num_column_families, const char** column_family_names,
    const crocksdb_options_t** column_family_options,
    crocksdb_column_family_handle_t** column_family_handles, char** errptr);

extern C_ROCKSDB_LIBRARY_API const crocksdb_snapshot_t*
crocksdb_transactiondb_create_snapshot(crocksdb_transactiondb_t* txn_db);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_release_snapshot(
    crocksdb_transactiondb_t* txn_db, const crocksdb_snapshot_t* snapshot);

extern C_ROCKSDB_LIBRARY_API crocksdb_transaction_t* crocksdb_transaction_begin(
    crocksdb_transactiondb_t* txn_db,
    const crocksdb_writeoptions_t* write_options,
    const crocksdb_transaction_options_t* txn_options,
    crocksdb_transaction_t* old_txn);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_commit(
    crocksdb_transaction_t* txn, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_rollback(
    crocksdb_transaction_t* txn, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_set_savepoint(
    crocksdb_transaction_t* txn);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_rollback_to_savepoint(
    crocksdb_transaction_t* txn, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_destroy(
    crocksdb_transaction_t* txn);

// This snapshot should be freed using crocksdb_free
extern C_ROCKSDB_LIBRARY_API const crocksdb_snapshot_t*
crocksdb_transaction_get_snapshot(crocksdb_transaction_t* txn);

extern C_ROCKSDB_LIBRARY_API char* crocksdb_transaction_get(
    crocksdb_transaction_t* txn, const crocksdb_readoptions_t* options,
    const char* key, size_t klen, size_t* vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API char* crocksdb_transaction_get_cf(
    crocksdb_transaction_t* txn, const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key, size_t klen,
    size_t* vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API char* crocksdb_transaction_get_for_update(
    crocksdb_transaction_t* txn, const crocksdb_readoptions_t* options,
    const char* key, size_t klen, size_t* vlen, unsigned char exclusive,
    char** errptr);

char* crocksdb_transaction_get_for_update_cf(
    crocksdb_transaction_t* txn, const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key, size_t klen,
    size_t* vlen, unsigned char exclusive, char** errptr);

extern C_ROCKSDB_LIBRARY_API char* crocksdb_transactiondb_get(
    crocksdb_transactiondb_t* txn_db, const crocksdb_readoptions_t* options,
    const char* key, size_t klen, size_t* vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API char* crocksdb_transactiondb_get_cf(
    crocksdb_transactiondb_t* txn_db, const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key,
    size_t keylen, size_t* vallen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_put(
    crocksdb_transaction_t* txn, const char* key, size_t klen, const char* val,
    size_t vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_put_cf(
    crocksdb_transaction_t* txn, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen, const char* val, size_t vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_put(
    crocksdb_transactiondb_t* txn_db, const crocksdb_writeoptions_t* options,
    const char* key, size_t klen, const char* val, size_t vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_put_cf(
    crocksdb_transactiondb_t* txn_db, const crocksdb_writeoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key,
    size_t keylen, const char* val, size_t vallen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_write(
    crocksdb_transactiondb_t* txn_db, const crocksdb_writeoptions_t* options,
    crocksdb_writebatch_t *batch, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_merge(
    crocksdb_transaction_t* txn, const char* key, size_t klen, const char* val,
    size_t vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_merge_cf(
    crocksdb_transaction_t* txn, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen, const char* val, size_t vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_merge(
    crocksdb_transactiondb_t* txn_db, const crocksdb_writeoptions_t* options,
    const char* key, size_t klen, const char* val, size_t vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_merge_cf(
    crocksdb_transactiondb_t* txn_db, const crocksdb_writeoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key, size_t klen,
    const char* val, size_t vlen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_delete(
    crocksdb_transaction_t* txn, const char* key, size_t klen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_delete_cf(
    crocksdb_transaction_t* txn, crocksdb_column_family_handle_t* column_family,
    const char* key, size_t klen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_delete(
    crocksdb_transactiondb_t* txn_db, const crocksdb_writeoptions_t* options,
    const char* key, size_t klen, char** errptr);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_delete_cf(
    crocksdb_transactiondb_t* txn_db, const crocksdb_writeoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key,
    size_t keylen, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_iterator_t*
crocksdb_transaction_create_iterator(crocksdb_transaction_t* txn,
                                    const crocksdb_readoptions_t* options);

extern C_ROCKSDB_LIBRARY_API crocksdb_iterator_t*
crocksdb_transaction_create_iterator_cf(
    crocksdb_transaction_t* txn, const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family);

extern C_ROCKSDB_LIBRARY_API crocksdb_iterator_t*
crocksdb_transactiondb_create_iterator(crocksdb_transactiondb_t* txn_db,
                                      const crocksdb_readoptions_t* options);

extern C_ROCKSDB_LIBRARY_API crocksdb_iterator_t*
crocksdb_transactiondb_create_iterator_cf(
    crocksdb_transactiondb_t* txn_db, const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_close(
    crocksdb_transactiondb_t* txn_db);

extern C_ROCKSDB_LIBRARY_API crocksdb_checkpoint_t*
crocksdb_transactiondb_checkpoint_object_create(crocksdb_transactiondb_t* txn_db,
                                               char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_optimistictransactiondb_t*
crocksdb_optimistictransactiondb_open(const crocksdb_options_t* options,
                                     const char* name, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_optimistictransactiondb_t*
crocksdb_optimistictransactiondb_open_column_families(
    const crocksdb_options_t* options, const char* name, int num_column_families,
    const char** column_family_names,
    const crocksdb_options_t** column_family_options,
    crocksdb_column_family_handle_t** column_family_handles, char** errptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_t*
crocksdb_optimistictransactiondb_get_base_db(
    crocksdb_optimistictransactiondb_t* otxn_db);

extern C_ROCKSDB_LIBRARY_API void crocksdb_optimistictransactiondb_close_base_db(
    crocksdb_t* base_db);

extern C_ROCKSDB_LIBRARY_API crocksdb_transaction_t*
crocksdb_optimistictransaction_begin(
    crocksdb_optimistictransactiondb_t* otxn_db,
    const crocksdb_writeoptions_t* write_options,
    const crocksdb_optimistictransaction_options_t* otxn_options,
    crocksdb_transaction_t* old_txn);

extern C_ROCKSDB_LIBRARY_API void crocksdb_optimistictransactiondb_close(
    crocksdb_optimistictransactiondb_t* otxn_db);

/* Transaction Options */

extern C_ROCKSDB_LIBRARY_API crocksdb_transactiondb_options_t*
crocksdb_transactiondb_options_create();

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_options_destroy(
    crocksdb_transactiondb_options_t* opt);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_options_set_max_num_locks(
    crocksdb_transactiondb_options_t* opt, int64_t max_num_locks);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transactiondb_options_set_num_stripes(
    crocksdb_transactiondb_options_t* opt, size_t num_stripes);

extern C_ROCKSDB_LIBRARY_API void
crocksdb_transactiondb_options_set_transaction_lock_timeout(
    crocksdb_transactiondb_options_t* opt, int64_t txn_lock_timeout);

extern C_ROCKSDB_LIBRARY_API void
crocksdb_transactiondb_options_set_default_lock_timeout(
    crocksdb_transactiondb_options_t* opt, int64_t default_lock_timeout);

extern C_ROCKSDB_LIBRARY_API crocksdb_transaction_options_t*
crocksdb_transaction_options_create();

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_options_destroy(
    crocksdb_transaction_options_t* opt);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_options_set_set_snapshot(
    crocksdb_transaction_options_t* opt, unsigned char v);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_options_set_deadlock_detect(
    crocksdb_transaction_options_t* opt, unsigned char v);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_options_set_lock_timeout(
    crocksdb_transaction_options_t* opt, int64_t lock_timeout);

extern C_ROCKSDB_LIBRARY_API void crocksdb_transaction_options_set_expiration(
    crocksdb_transaction_options_t* opt, int64_t expiration);

extern C_ROCKSDB_LIBRARY_API void
crocksdb_transaction_options_set_deadlock_detect_depth(
    crocksdb_transaction_options_t* opt, int64_t depth);

extern C_ROCKSDB_LIBRARY_API void
crocksdb_transaction_options_set_max_write_batch_size(
    crocksdb_transaction_options_t* opt, size_t size);

extern C_ROCKSDB_LIBRARY_API crocksdb_optimistictransaction_options_t*
crocksdb_optimistictransaction_options_create();

extern C_ROCKSDB_LIBRARY_API void crocksdb_optimistictransaction_options_destroy(
    crocksdb_optimistictransaction_options_t* opt);

extern C_ROCKSDB_LIBRARY_API void
crocksdb_optimistictransaction_options_set_set_snapshot(
    crocksdb_optimistictransaction_options_t* opt, unsigned char v);

// referring to convention (3), this should be used by client
// to free memory that was malloc()ed
extern C_ROCKSDB_LIBRARY_API void crocksdb_free(void* ptr);

extern C_ROCKSDB_LIBRARY_API crocksdb_pinnableslice_t* crocksdb_get_pinned(
    crocksdb_t* db, const crocksdb_readoptions_t* options, const char* key,
    size_t keylen, char** errptr);
extern C_ROCKSDB_LIBRARY_API crocksdb_pinnableslice_t* crocksdb_get_pinned_cf(
    crocksdb_t* db, const crocksdb_readoptions_t* options,
    crocksdb_column_family_handle_t* column_family, const char* key,
    size_t keylen, char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_pinnableslice_destroy(
    crocksdb_pinnableslice_t* v);
extern C_ROCKSDB_LIBRARY_API const char* crocksdb_pinnableslice_value(
    const crocksdb_pinnableslice_t* t, size_t* vlen);

extern C_ROCKSDB_LIBRARY_API crocksdb_memory_consumers_t*
    crocksdb_memory_consumers_create();
extern C_ROCKSDB_LIBRARY_API void crocksdb_memory_consumers_add_db(
    crocksdb_memory_consumers_t* consumers, crocksdb_t* db);
extern C_ROCKSDB_LIBRARY_API void crocksdb_memory_consumers_add_cache(
    crocksdb_memory_consumers_t* consumers, crocksdb_cache_t* cache);
extern C_ROCKSDB_LIBRARY_API void crocksdb_memory_consumers_destroy(
    crocksdb_memory_consumers_t* consumers);
extern C_ROCKSDB_LIBRARY_API crocksdb_memory_usage_t*
crocksdb_approximate_memory_usage_create(crocksdb_memory_consumers_t* consumers,
                                       char** errptr);
extern C_ROCKSDB_LIBRARY_API void crocksdb_approximate_memory_usage_destroy(
    crocksdb_memory_usage_t* usage);

extern C_ROCKSDB_LIBRARY_API uint64_t
crocksdb_approximate_memory_usage_get_mem_table_total(
    crocksdb_memory_usage_t* memory_usage);
extern C_ROCKSDB_LIBRARY_API uint64_t
crocksdb_approximate_memory_usage_get_mem_table_unflushed(
    crocksdb_memory_usage_t* memory_usage);
extern C_ROCKSDB_LIBRARY_API uint64_t
crocksdb_approximate_memory_usage_get_mem_table_readers_total(
    crocksdb_memory_usage_t* memory_usage);
extern C_ROCKSDB_LIBRARY_API uint64_t
crocksdb_approximate_memory_usage_get_cache_total(
    crocksdb_memory_usage_t* memory_usage);

extern C_ROCKSDB_LIBRARY_API void crocksdb_sync_wal(
    crocksdb_t* db, char** errptr);

#ifdef __cplusplus
}  /* end extern "C" */
#endif
