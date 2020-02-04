extern crate deflate;
use std::ffi::CString;

#[allow(dead_code)]
#[allow(non_camel_case_types)]
#[allow(non_upper_case_globals)]
mod rfs_gen_binding;

mod gzip_file;

pub fn rfs_generate(path: &str, id: &str, options: i32, target_dir: &str) -> i32 {
    let cpath = CString::new(path.as_bytes()).expect("CString::new failed");
    let cid = CString::new(id.as_bytes()).expect("CString::new failed");
    let ctarget = CString::new(target_dir.as_bytes()).expect("CString::new failed");

    unsafe {
        rfs_gen_binding::rfs_generate(cpath.as_ptr(), cid.as_ptr(), options, ctarget.as_ptr())
    }
}