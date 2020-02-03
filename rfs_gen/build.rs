extern crate cc;

fn build_cpp_gen() {
    let src = [
        "src/rfs_generator.cpp",
        "src/gzip_file.cpp",
    ];
    let mut builder = cc::Build::new();
    let build = builder
        .cpp(true) // Switch to C++ library compilation.
        .flag("-std=c++17")     // enable c++17
        .files(src.iter())
        .include("src")
        ;
    build.compile("rfs_gen_cpp");  
}

fn generate_rust_binding() {
    use bindgen::builder;
    {
        // Configure and generate bindings.
        let bindings = builder().header("src/rfs_generator.h").generate().unwrap();
        // Write the generated bindings to an output file.
        bindings.write_to_file("src/rfs_gen_binding.rs").unwrap();
    }
}

fn main() {
        /*
    pkg_config::Config::new()
        .atleast_version("1.2")
        .probe("z")
        .unwrap();
        */
    build_cpp_gen();

    generate_rust_binding();
}