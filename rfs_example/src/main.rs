
extern crate resource_fs;

pub mod rfs_gensrc;


fn main() {
    println!("Hello World!");
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_openfile() {
        let fs = rfs_gensrc::rfs_root();
        
    }

}