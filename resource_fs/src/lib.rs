#[allow(dead_code)]
#[allow(non_camel_case_types)]
#[allow(non_upper_case_globals)]
mod rfs_binding;

use rfs_binding::RfsRoot;
use rfs_binding::RfsHandle;
use rfs_binding::rfs_visit;
use std::slice;

pub fn read(fs: RfsRoot, path: &str) -> Result<&[u8], i32> {
    let mut buf = 0 as *const u8;
    let mut size :u32 = 0;

    let pbuf = &mut buf as *mut *const u8;
    let psize = &mut size as *mut u32;

    let ret :i32;
    unsafe { 
        ret = rfs_binding::rfs_read(fs, path.as_ptr(), path.len() as u32, pbuf, psize);
    }
    if ret == 0 {
        unsafe {
            Ok(slice::from_raw_parts(buf, size as usize))
        }
    } else {
        Err(ret)
    }  
}

pub fn open(fs: RfsRoot, path: &str) -> Result<(RfsHandle, u32), i32> {
    let mut handle: RfsHandle = 0 as RfsHandle;
    let mut size :u32 = 0;

    let phandle = &mut handle as *mut RfsHandle;
    let psize = &mut size as *mut u32;

    let ret :i32;
    unsafe { 
        ret = rfs_binding::rfs_open(fs, path.as_ptr(), path.len() as u32, phandle, psize);
    }
    if ret == 0 {
        Ok((handle, size as u32))
    } else {
        Err(ret)
    }  
}


pub fn rfs_entryflags(entry: RfsHandle, flags: *mut u32) -> u32 {
    unsafe { 
        rfs_binding::rfs_entryflags(entry, flags) 
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
