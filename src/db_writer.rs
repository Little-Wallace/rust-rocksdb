use crate::rocksdb_options::WriteOptions;
use crate::write_batch::WriteBatch;
use crocksdb_ffi::{
    crocksdb_db_writer_create, crocksdb_db_writer_destroy, DBWriteBatch, DBWriteBatchWriter,
};
use std::marker::PhantomData;

pub struct WriteBatchWriter<'a> {
    pub(crate) inner: *mut DBWriteBatchWriter,
    _phantom: PhantomData<&'a ()>,
}

impl<'a> WriteBatchWriter<'a> {
    pub fn from_writebatch_vec(
        options: &WriteOptions,
        batches: &'a [WriteBatch],
    ) -> WriteBatchWriter<'a> {
        unsafe {
            let b: Vec<*mut DBWriteBatch> = batches.iter().map(|w| w.inner).collect();
            WriteBatchWriter {
                inner: crocksdb_db_writer_create(options.inner, b.as_ptr(), b.len()),
                _phantom: PhantomData,
            }
        }
    }
    pub fn from_writebatch(options: &WriteOptions, batch: &'a WriteBatch) -> WriteBatchWriter<'a> {
        unsafe {
            let inner = &batch.inner;
            WriteBatchWriter {
                inner: crocksdb_db_writer_create(options.inner, inner, 1),
                _phantom: PhantomData,
            }
        }
    }
}

impl<'a> Drop for WriteBatchWriter<'a> {
    fn drop(&mut self) {
        unsafe {
            crocksdb_db_writer_destroy(self.inner);
        }
    }
}
