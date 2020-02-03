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
    build.compile("rfs_rt");  
}

fn generate_rust_binding() {
    use bindgen::builder;
    {
        // Configure and generate bindings.
        let bindings = builder().header("src/resource_fs.h").generate().unwrap();
        // Write the generated bindings to an output file.
        bindings.write_to_file("src/rfs_binding.rs").unwrap();
    }
}


fn main() {
    build_c_rt();
    generate_rust_binding();
}