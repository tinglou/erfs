
extern crate cc;

fn generate_rfs_ut() {
    use rfs_gen::rfs_generate;
    {
        rfs_generate(&String::from("../rfs_gen"), &String::from("gensrc"), 6, &String::from("src"));
    }
}

fn compile_rfs_source() {
    let src = [
        "src/rfs_gensrc.c",
    ];
    let mut builder = cc::Build::new();
    let build = builder
        .files(src.iter())
        .include("src")
        .include("../resource_fs/src")
        ;
    build.compile("rfs_gensrc");  
}

fn main() {
    // generate RFS source files
    generate_rfs_ut();
    // compile RFS c source files
    compile_rfs_source();
}