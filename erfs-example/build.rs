
extern crate cc;

fn generate_rfs_ut() {
    use erfs_gen::erfs_generate;
    {
        erfs_generate("../erfs-gen", "gensrc", 6, "src");
    }
}

fn compile_rfs_source() {
    let src = [
        "src/erfs_gensrc.c",
    ];
    let mut builder = cc::Build::new();
    let build = builder
        .files(src.iter())
        .include("src")
        .include("../erfs-rt/src")
        ;
    build.compile("rfs_gensrc");  
}

fn main() {
    // generate ERFS source files
    generate_rfs_ut();
    // compile ERFS c source files
    compile_rfs_source();
}