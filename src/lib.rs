#[allow(dead_code)]
#[allow(non_camel_case_types)]
#[allow(non_upper_case_globals)]
mod rfs_binding;

use rfs_binding::RfsRoot;
use rfs_binding::RfsHandle;
use rfs_binding::rfs_visit;

pub fn rfs_read(fs: RfsRoot, path: *const u8, out: *mut *const u8, size: *mut u32) -> i32 {
    unsafe { 
        rfs_binding::rfs_read(fs, path, out, size) 
    }
}

pub fn rfs_open(fs: RfsRoot, path: *const u8, out: *mut RfsHandle, size: *mut u32) -> i32 {
    unsafe { 
        rfs_binding::rfs_open(fs, path, out, size) 
    }
}

pub fn rfs_entryflags(entry: RfsHandle) -> u32 {
    unsafe { 
        rfs_binding::rfs_entryflags(entry) 
    }
}

pub fn rfs_entryname(fs: RfsRoot, entry: RfsHandle, out: *mut *const u8, size: *mut u32) -> i32 {
    unsafe { 
        rfs_binding::rfs_entryname(fs, entry, out, size) 
    }
}

pub fn rfs_readfile(fs: RfsRoot, entry: RfsHandle, out: *mut *const u8, size: *mut u32) -> i32 {
    unsafe { 
        rfs_binding::rfs_readfile(fs, entry, out, size) 
    }
}

pub fn rfs_readdir(fs: RfsRoot, dir: RfsHandle, index: u32, out: *mut RfsHandle) -> i32 {
    unsafe { 
        rfs_binding::rfs_readdir(fs, dir, index, out) 
    }
}

pub fn rfs_travel(fs: RfsRoot, func: rfs_visit, ctx: *mut ::std::os::raw::c_void) -> i32 {
    unsafe { 
        rfs_binding::rfs_travel(fs, func, ctx) 
    }
}
