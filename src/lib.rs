#[cfg(not(target_pointer_width = "64"))]
compile_error!("compilation is only allowed for 64-bit targets");

use quantum_execution::QuantumExecution;

mod bitwise;
mod c_wrapper;
mod dense;
mod quantum_execution;
mod sparse;

pub fn run_dense_from_json(quantum_code: &str, metrics: &str) -> Result<String, String> {
    Ok(serde_json::to_string(&dense::Dense::new().run_from_json(quantum_code, metrics)?).unwrap())
}

pub fn run_dense_from_bin(quantum_code: &[u8], metrics: &[u8]) -> Result<Vec<u8>, String> {
    Ok(bincode::serialize(&dense::Dense::new().run_from_bin(quantum_code, metrics)?).unwrap())
}

pub fn run_dense_from_process(process: &mut ket::Process) -> Result<(), String> {
    let result = dense::Dense::new().run_from_process(process)?;
    process.set_quantum_result(result)?;
    Ok(())
}

pub fn run_sparse_from_json(quantum_code: &str, metrics: &str) -> Result<String, String> {
    Ok(
        serde_json::to_string(&sparse::Sparse::new().run_from_json(quantum_code, metrics)?)
            .unwrap(),
    )
}

pub fn run_sparse_from_bin(quantum_code: &[u8], metrics: &[u8]) -> Result<Vec<u8>, String> {
    Ok(bincode::serialize(&sparse::Sparse::new().run_from_bin(quantum_code, metrics)?).unwrap())
}

pub fn run_sparse_from_process(process: &mut ket::Process) -> Result<(), String> {
    let result = sparse::Sparse::new().run_from_process(process)?;
    process.set_quantum_result(result)?;
    Ok(())
}
