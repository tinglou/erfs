fn main() {
    pkg_config::Config::new()
        .atleast_version("1.2")
        .probe("z")
        .unwrap();
    let src = [
        "src/resource_fs.c",
    ];
    let mut builder = cc::Build::new();
    let build = builder
        .files(src.iter())
        .include("src")
        .flag("-Wno-unused-parameter")
        .define("USE_ZLIB", None)
        ;
    build.compile("foo");
}