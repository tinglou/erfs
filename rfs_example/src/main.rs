
extern crate resource_fs;

pub mod rfs_gensrc;


fn main() {
    println!("Hello World!");
}


#[cfg(test)]
mod tests {
    use super::*;
    use resource_fs::open;

    #[test]
    fn test_openfile() {
        let fs = rfs_gensrc::rfs_root();

        let (_handle, size) = open(fs, "/lib.rs").expect("open error");


        println!("content of Cargo.toml: {:?}", size);
        
    }

}