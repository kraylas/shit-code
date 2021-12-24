/// __rec_iter
macro_rules! __rec_iter {
    ( $gen:expr ; $id:ident <- $it:expr, $( $rest:tt )* ) => {
        $it.flat_map(move |$id| __rec_iter!($gen ; $( $rest )* ) )
    };

    ( $gen:expr ;  $id:ident <- $it:expr ) => {
        $it.map(move |$id| $gen)
    };
}
macro_rules! list {
    ( $gen:expr ; $( $id:ident <- $it:expr ),* ) => {
        __rec_iter!( $gen ; $( $id <- $it ),* )
    };

    /* other rules */
}
fn main() {
    let res = list!(x + y + z; x <- 1..=10, y <- 1..=10, z <- 1..=10);
    println!("{:?}", res.collect::<Vec<i32>>());
}
