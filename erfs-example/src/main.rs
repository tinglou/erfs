
extern crate erfs_rt;

pub mod erfs_gensrc;


fn main() {
    println!("Hello World!");
}


#[cfg(test)]
mod tests {
    use super::*;
    use erfs_rt::open;

    #[test]
    fn test_openfile() {
        let fs = erfs_gensrc::rfs_root();

        let (_handle, size) = open(fs, "/lib.rs").expect("open error");


        println!("content of Cargo.toml: {:?}", size);
        
    }

}