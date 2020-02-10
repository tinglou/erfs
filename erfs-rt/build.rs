extern crate cc;

fn build_c_rt() {
    let src = [
        "src/resource_fs.c",
    ];
    let mut builder = cc::Build::new();
    let build = builder
        .files(src.iter())
        .include("src")
        ;
    build.compile("erfs_c_rt");  
}

fn generate_rust_binding() {
    use bindgen::builder;
    {
        // Configure and generate bindings.
        let bindings = builder().header("src/resource_fs.h").generate().unwrap();
        // Write the generated bindings to an output file.
        bindings.write_to_file("src/erfs_binding.rs").unwrap();
    }
}


fn main() {
    build_c_rt();

    // commented for crates.io, because src directory is read-only
    // generate_rust_binding();
}