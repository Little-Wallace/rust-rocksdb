
pub struct DB {
    inner: *mut DBInstance,
    cfs_by_name: BTreeMap<String, usize>,
    cfs: Vec<Option<(String, CFHandle)>>,
    path: String,
    opts: DBOptions,
}