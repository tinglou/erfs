use std::env;

fn usage () {
    let args: Vec<String> = env::args().collect();
    println!("Usage: {} [options] <src_dir> <id> <dest_dir>", args[0]);
    println!("Options:");
    println!("  --gzip      compress file if needed.");   
    println!("  --rust      generate rust binding codes.");     
}


fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 4  {
        usage();
    }

    let mut real_args: Vec<String> = Vec::new();
    let mut index = 1;
    let mut option = 0;

    while index < args.len() {
        let arg = &args[index];
        if arg.bytes().next() == Some(b'-') {
            if arg == "--gzip" {
                option |= 2;
            } else if arg == ("--rust") {
                option |= 4;
            } else {
                println!("Unknown option: {}", arg);
                usage();
                return;
            }
        } else {
            real_args.push(arg.to_string());
        }

        index = index + 1;
    }

    println!("{:?}, option: {}", real_args, option);
    
}
