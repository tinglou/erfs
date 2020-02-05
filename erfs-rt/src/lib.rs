#[allow(dead_code)]
#[allow(non_camel_case_types)]
#[allow(non_upper_case_globals)]
mod erfs_binding;

use erfs_binding::ErfsRoot;
use erfs_binding::ErfsHandle;
use erfs_binding::ErfsVisitFn;
use std::slice;

pub fn read(fs: ErfsRoot, path: &str) -> Result<&[u8], i32> {
    let mut buf = 0 as *const u8;
    let mut size :u32 = 0;

    let pbuf = &mut buf as *mut *const u8;
    let psize = &mut size as *mut u32;

    let ret :i32;
    unsafe { 
        ret = erfs_binding::erfs_read(fs, path.as_ptr(), path.len() as u32, pbuf, psize);
    }
    if ret == 0 {
        unsafe {
            Ok(slice::from_raw_parts(buf, size as usize))
        }
    } else {
        Err(ret)
    }  
}

pub fn open(fs: ErfsRoot, path: &str) -> Result<(ErfsHandle, u32), i32> {
    let mut handle: ErfsHandle = 0 as ErfsHandle;
    let mut size :u32 = 0;

    let phandle = &mut handle as *mut ErfsHandle;
    let psize = &mut size as *mut u32;

    let ret :i32;
    unsafe { 
        ret = erfs_binding::erfs_open(fs, path.as_ptr(), path.len() as u32, phandle, psize);
    }
    if ret == 0 {
        Ok((handle, size as u32))
    } else {
        Err(ret)
    }  
}


pub fn erfs_entryflags(entry: ErfsHandle, flags: *mut u32) -> u32 {
    unsafe { 
        erfs_binding::erfs_entryflags(entry, flags) 
    }
}

pub fn erfs_entryname(fs: ErfsRoot, entry: ErfsHandle, out: *mut *const u8, size: *mut u32) -> i32 {
    unsafe { 
        erfs_binding::erfs_entryname(fs, entry, out, size) 
    }
}

pub fn erfs_readfile(fs: ErfsRoot, entry: ErfsHandle, out: *mut *const u8, size: *mut u32) -> i32 {
    unsafe { 
        erfs_binding::erfs_readfile(fs, entry, out, size) 
    }
}

pub fn erfs_readdir(fs: ErfsRoot, dir: ErfsHandle, index: u32, out: *mut ErfsHandle) -> i32 {
    unsafe { 
        erfs_binding::erfs_readdir(fs, dir, index, out) 
    }
}

pub fn erfs_travel(fs: ErfsRoot, func: ErfsVisitFn, ctx: *mut ::std::os::raw::c_void) -> i32 {
    unsafe { 
        erfs_binding::erfs_travel(fs, func, ctx) 
    }
}
