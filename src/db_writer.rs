use crate::rocksdb_options::WriteOptions;
use crate::write_batch::WriteBatch;
use crocksdb_ffi::{
    crocksdb_db_writer_create, crocksdb_db_writer_destroy, DBWriteBatch, DBWriteBatchWriter,
};

pub struct WriteBatchWriter {
    pub(crate) inner: *mut DBWriteBatchWriter,
}

impl WriteBatchWriter {
    pub fn from_writebatch_vec(options: &WriteOptions, batches: &[WriteBatch]) -> WriteBatchWriter {
        unsafe {
            let b: Vec<*mut DBWriteBatch> = batches.iter().map(|w| w.inner).collect();
            WriteBatchWriter {
                inner: crocksdb_db_writer_create(options.inner, b.as_ptr(), b.len()),
            }
        }
    }

    pub fn from_writebatch(options: &WriteOptions, batch: &WriteBatch) -> WriteBatchWriter {
        unsafe {
            let inner = &batch.inner;
            WriteBatchWriter {
                inner: crocksdb_db_writer_create(options.inner, inner, 1),
            }
        }
    }
}

impl Drop for WriteBatchWriter {
    fn drop(&mut self) {
        unsafe {
            crocksdb_db_writer_destroy(self.inner);
        }
    }
}
