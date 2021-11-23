use libc::{c_char, c_int, c_uchar, c_void, malloc, memcpy, size_t};
use std::ffi::{CStr, CString};
use std::{ptr, slice, usize};

use crocksdb_ffi;

pub struct CompactionServiceJobInfo {
    pub db_name: String,
    pub db_id: String,
    pub db_session_id: String,
    pub job_id: u64,
}

pub enum CompactionServiceJobStatus {
    Success(Vec<u8>),
    Failure,
    UseLocal,
}

pub struct CompactionServiceInput {
    pub(crate) inner: *mut crocksdb_ffi::DBCompactionServiceInput,
}

pub struct CompactionServiceResult {
    pub(crate) inner: *mut crocksdb_ffi::DBCompactionServiceResult,
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

impl Drop for CompactionServiceInput {
    fn drop(&mut self) {
        unsafe {
            crocksdb_ffi::crocksdb_compactionservice_input_destroy(self.inner);
        }
    }
}

impl CompactionServiceInput {
    pub fn new() -> CompactionServiceInput {
        unsafe {
            CompactionServiceInput {
                inner: crocksdb_ffi::crocksdb_compactionservice_input_create(),
            }
        }
    }

    pub fn decode(&mut self, data: &[u8]) -> std::result::Result<(), String> {
        unsafe {
            ffi_try!(crocksdb_compactionservice_input_decode(
                self.inner,
                data.as_ptr(),
                data.len() as size_t
            ));
        }
        Ok(())
    }

    pub fn encode(&self) -> std::result::Result<Vec<u8>, String> {
        unsafe {
            let mut val_len: size_t = 0;
            let value_ptr = ffi_try!(crocksdb_compactionservice_input_encode(
                self.inner,
                &mut val_len
            ));
            let value: *const u8 = std::mem::transmute(value_ptr);
            Ok(slice::from_raw_parts(value, val_len).to_vec())
        }
    }

    pub fn get_file(&self, idx: usize) -> std::result::Result<&str, String> {
        unsafe {
            let f =
                crocksdb_ffi::crocksdb_compactionservice_input_get_file(self.inner, idx as size_t);
            CStr::from_ptr(f).to_str().map_err(|e| e.to_string())
        }
    }

    pub fn get_files_size(&self) -> usize {
        unsafe {
            let n = crocksdb_ffi::crocksdb_compactionservice_input_get_files_size(self.inner);
            n as usize
        }
    }

    pub fn set_files(&self, files: Vec<String>) {
        unsafe {
            crocksdb_ffi::crocksdb_compactionservice_input_clear_files(self.inner);
            for f in files {
                let cpath = CString::new(f).unwrap();
                crocksdb_ffi::crocksdb_compactionservice_input_add_file(self.inner, cpath.as_ptr());
            }
        }
    }

    pub fn get_cf_name(&self) -> &str {
        unsafe {
            let raw_cf_name =
                crocksdb_ffi::crocksdb_compactionservice_input_get_cf_name(self.inner);
            CStr::from_ptr(raw_cf_name).to_str().unwrap()
        }
    }
}

impl Drop for CompactionServiceResult {
    fn drop(&mut self) {
        unsafe {
            crocksdb_ffi::crocksdb_compactionservice_result_destroy(self.inner);
        }
    }
}

impl CompactionServiceResult {
    pub fn new() -> Self {
        unsafe {
            CompactionServiceResult {
                inner: crocksdb_ffi::crocksdb_compactionservice_result_create(),
            }
        }
    }

    pub fn encode(&self) -> std::result::Result<Vec<u8>, String> {
        unsafe {
            let mut val_len: size_t = 0;
            let value_ptr = ffi_try!(crocksdb_compactionservice_result_encode(
                self.inner,
                &mut val_len
            ));
            let value: *const u8 = std::mem::transmute(value_ptr);
            Ok(slice::from_raw_parts(value, val_len).to_vec())
        }
    }

    pub fn set_output_file(&self, path: &str) -> std::result::Result<(), String> {
        let cpath = match CString::new(path.as_bytes()) {
            Ok(c) => c,
            Err(_) => {
                return Err(
                    "Failed to convert path to CString when creating rocksdb info log".to_owned(),
                );
            }
        };
        unsafe {
            crocksdb_ffi::crocksdb_compactionservice_result_set_output_path(
                self.inner,
                cpath.as_ptr(),
            );
        }
        Ok(())
    }
}
