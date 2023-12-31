#[cfg(not(target_pointer_width = "64"))]
compile_error!("compilation is only allowed for 64-bit targets");

pub mod bitwise;
pub mod c_wrapper;
pub mod dense;
pub mod error;
pub mod quantum_execution;
pub mod sparse;
pub mod convert;

pub use dense::Dense;
pub use quantum_execution::*;
pub use sparse::Sparse;
