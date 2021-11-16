use std::{ptr, slice, usize};

use crocksdb_ffi;
use libc::{c_char, c_int, c_uchar, c_void, malloc, memcpy, size_t};
use std::ffi::{CStr, CString};

pub struct CompactionServiceJobInfo {
    db_name: String,
    db_id: String,
    db_session_id: String,
    job_id: u64,
}

pub enum CompactionServiceJobStatus {
    Success(Vec<u8>),
    Failure,
    UseLocal,
}

pub trait RemoteCompactionService: Sync + Send {
    fn start(&self, info: &CompactionServiceJobInfo, input: &[u8]) -> CompactionServiceJobStatus;

    fn wait(&self, info: &CompactionServiceJobInfo) -> CompactionServiceJobStatus;
}

#[repr(C)]
struct CompactionServiceProxy {
    name: CString,
    service: Box<dyn RemoteCompactionService>,
}

extern "C" fn name(proxy: *mut c_void) -> *const c_char {
    unsafe { (*(proxy as *mut CompactionServiceProxy)).name.as_ptr() }
}

extern "C" fn destructor(proxy: *mut c_void) {
    unsafe {
        Box::from_raw(proxy as *mut CompactionServiceProxy);
    }
}

extern "C" fn start_compaction(
    proxy: *mut c_void,
    raw_info: *const crocksdb_ffi::DBCompactionServiceJobInfo,
    input: *const u8,
    input_len: size_t,
) -> i32 {
    unsafe {
        let service = &mut (*(proxy as *mut CompactionServiceProxy)).service;
        let input = slice::from_raw_parts(input, input_len);
        let db_name = crocksdb_ffi::crocksdb_compactionservice_job_info_get_db_name(raw_info);
        let db_id = crocksdb_ffi::crocksdb_compactionservice_job_info_get_db_id(raw_info);
        let db_session_id =
            crocksdb_ffi::crocksdb_compactionservice_job_info_get_db_session_id(raw_info);
        let info = CompactionServiceJobInfo {
            db_name: CStr::from_ptr(db_name).to_str().unwrap().to_owned(),
            db_id: CStr::from_ptr(db_id).to_str().unwrap().to_owned(),
            db_session_id: CStr::from_ptr(db_session_id).to_str().unwrap().to_owned(),
            job_id: crocksdb_ffi::crocksdb_compactionservice_job_info_get_job_id(raw_info),
        };
        match service.start(&info, input) {
            CompactionServiceJobStatus::Success(_) => 0,
            CompactionServiceJobStatus::Failure => 1,
            CompactionServiceJobStatus::UseLocal => 2,
        }
    }
}

extern "C" fn wait_compaction(
    proxy: *mut c_void,
    raw_info: *const crocksdb_ffi::DBCompactionServiceJobInfo,
    output_ptr: *mut *const u8,
    output_len: *mut size_t,
) -> i32 {
    unsafe {
        *output_ptr = ptr::null_mut();
        *output_len = 0;
        let service = &mut (*(proxy as *mut CompactionServiceProxy)).service;
        let db_name = crocksdb_ffi::crocksdb_compactionservice_job_info_get_db_name(raw_info);
        let db_id = crocksdb_ffi::crocksdb_compactionservice_job_info_get_db_id(raw_info);
        let db_session_id =
            crocksdb_ffi::crocksdb_compactionservice_job_info_get_db_session_id(raw_info);
        let info = CompactionServiceJobInfo {
            db_name: CStr::from_ptr(db_name).to_str().unwrap().to_owned(),
            db_id: CStr::from_ptr(db_id).to_str().unwrap().to_owned(),
            db_session_id: CStr::from_ptr(db_session_id).to_str().unwrap().to_owned(),
            job_id: crocksdb_ffi::crocksdb_compactionservice_job_info_get_job_id(raw_info),
        };
        match service.wait(&info) {
            CompactionServiceJobStatus::Success(output) => {
                *output_len = output.len();
                *output_ptr = malloc(*output_len) as *mut u8;
                memcpy(*output_ptr as _, output.as_ptr() as _, *output_len);
                0
            }
            CompactionServiceJobStatus::Failure => 1,
            CompactionServiceJobStatus::UseLocal => 2,
        }
    }
}

pub struct CompactionServiceHandle {
    pub inner: *mut crocksdb_ffi::DBCompactionService,
}

impl Drop for CompactionServiceHandle {
    fn drop(&mut self) {
        unsafe {
            crocksdb_ffi::crocksdb_compactionservice_destroy(self.inner);
        }
    }
}

pub unsafe fn new_compaction_service(
    c_name: CString,
    f: Box<dyn RemoteCompactionService>,
) -> CompactionServiceHandle {
    let filter = new_compaction_service_raw(c_name, f);
    CompactionServiceHandle { inner: filter }
}

/// Just like `new_compaction_service`, but returns a raw pointer instead of a RAII struct.
pub unsafe fn new_compaction_service_raw(
    c_name: CString,
    f: Box<dyn RemoteCompactionService>,
) -> *mut crocksdb_ffi::DBCompactionService {
    let proxy = Box::into_raw(Box::new(CompactionServiceProxy {
        name: c_name,
        service: f,
    }));
    crocksdb_ffi::crocksdb_compactionservice_create(
        proxy as *mut c_void,
        destructor,
        start_compaction,
        wait_compaction,
        name,
    )
}
