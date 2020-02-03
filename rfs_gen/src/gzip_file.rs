
#[allow(unused_variables)]
#[no_mangle]
pub extern "C" fn gzip_file(source_path: *const ::std::os::raw::c_char, 
    dest_path: *const ::std::os::raw::c_char) -> ::std::os::raw::c_int {
    -3
}
/*
use std::io::Write;

use deflate::Compression;
use deflate::write::ZlibEncoder;

let data = b"This is some test data";
let mut encoder = ZlibEncoder::new(Vec::new(), Compression::Default);
encoder.write_all(data).expect("Write error!");
let compressed_data = encoder.finish().expect("Failed to finish compression!");
*/